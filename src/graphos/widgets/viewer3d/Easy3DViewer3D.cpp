#include "Easy3DViewer3D.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QWheelEvent>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <memory>

#include <easy3d/core/point_cloud.h>
#include <easy3d/core/surface_mesh.h>
#include <easy3d/core/poly_mesh.h>
#include <easy3d/algo/point_cloud_normals.h>
#include <easy3d/fileio/point_cloud_io.h>
#include <easy3d/fileio/surface_mesh_io.h>
#include <easy3d/fileio/ply_reader_writer.h>
#include <easy3d/fileio/poly_mesh_io.h>
#include <easy3d/fileio/point_cloud_io_ptx.h>
#include <easy3d/renderer/clipping_plane.h>
#include <easy3d/renderer/drawable.h>
#include <easy3d/renderer/drawable_lines.h>
#include <easy3d/renderer/drawable_points.h>
#include <easy3d/renderer/drawable_triangles.h>
#include <easy3d/renderer/manipulated_camera_frame.h>
#include <easy3d/renderer/opengl_util.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/renderer/shader_program.h>
#include <easy3d/renderer/shader_manager.h>
#include <easy3d/renderer/shape.h>
#include <easy3d/renderer/transform.h>
#include <easy3d/renderer/read_pixel.h>
#include <easy3d/util/signal.h>
#include <easy3d/util/setting.h>

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Exception.h>

namespace graphos
{

#ifdef HAVE_EASY3D

namespace
{

auto readModels(const QString &file) -> std::vector<easy3d::Model *>
{
    const auto extension = QFileInfo(file).suffix().toLower();
    const std::string native_file = QFile::encodeName(file).toStdString();
    std::vector<easy3d::Model *> models;

    bool is_ply_mesh = false;
    if (extension == "ply")
        is_ply_mesh = easy3d::io::PlyReader::num_instances(native_file, "face") > 0;

    easy3d::Model *model = nullptr;
    if ((extension == "ply" && is_ply_mesh) ||
        extension == "obj" || extension == "off" ||
        extension == "stl" || extension == "sm" ||
        extension == "geojson" || extension == "trilist") {
        model = easy3d::SurfaceMeshIO::load(native_file);
    } else if (extension == "ply" &&
               easy3d::io::PlyReader::num_instances(native_file, "edge") > 0) {
        model = easy3d::SurfaceMeshIO::load(native_file);
    } else if (extension == "plm" || extension == "pm" || extension == "mesh") {
        model = easy3d::PolyMeshIO::load(native_file);
    } else if (extension == "ptx") {
        easy3d::io::PointCloudIO_ptx serializer(native_file);
        while (auto cloud = serializer.load_next())
            models.push_back(cloud);
        return models;
    } else {
        model = easy3d::PointCloudIO::load(native_file);
    }

    if (model)
        models.push_back(model);

    return models;
}

}

Easy3DViewer3D::Easy3DViewer3D(QWidget *parent)
  : QOpenGLWidget(parent),
    mSelectedObject(nullptr),
    mCamera(new easy3d::Camera),
    mEdl(std::make_unique<easy3d::EyeDomeLighting>(mCamera)),
    mPickingMode(PickingMode::none),
    mMeasurementComplete(false),
    mAreaClosed(false),
    mContextMenu(new Viewer3DContextMenu(this)),
    mBackgroundColor(50, 50, 50),
    mPressedButton(Qt::NoButton),
    mEdlEnabled(false),
    mDrawableAxes(nullptr),
    mDrawableManipSphere(nullptr),
    mDrawableMeasurementLines(nullptr),
    mDrawableMeasurementPoints(nullptr),
    mShowManipSphere(false)
{
    mCamera->setType(easy3d::Camera::PERSPECTIVE);
    mCamera->setUpVector(easy3d::vec3(0, 0, 1));
    mCamera->setViewDirection(easy3d::vec3(-1, 0, 0));
    mCamera->showEntireScene();

    init();
    initSignalsAndSlots();
}

Easy3DViewer3D::~Easy3DViewer3D()
{
    makeCurrent();

    for (auto model : mModels)
        delete model;

    mModels.clear();
    delete mDrawableManipSphere;
    delete mDrawableMeasurementLines;
    delete mDrawableMeasurementPoints;
    delete mDrawableAxes;
    mEdl.reset();
    delete mCamera;
    doneCurrent();
}

void Easy3DViewer3D::clear()
{
    makeCurrent();

    for (auto model : mModels)
        delete model;

    mModels.clear();
    mSelectedObject = nullptr;
    mPickedPoints.clear();
    mMeasurementPoints.clear();
    mMeasurementComplete = false;
    mAreaClosed = false;
    updateMeasurementDrawables();
    doneCurrent();
    update();
}

void Easy3DViewer3D::deleteEntity(const QString &id)
{
    for (auto it = mModels.begin(); it != mModels.end(); ++it) {
        if (QString::fromStdString((*it)->name()) == id) {
            makeCurrent();
            delete *it;
            mModels.erase(it);
            doneCurrent();
            mSelectedObject = nullptr;
            update();
            break;
        }
    }
}

void Easy3DViewer3D::loadFromFile(const QString &file, const QString &)
{
    try {
        for (auto *model : readModels(file)) {
            model->set_name(file.toStdString());
            addModel(model);
        }
        update();
    } catch (const std::exception &e) {
        tl::printException(e);
    }
}

void Easy3DViewer3D::loadFromFileAsync(const QString &file,
                                       std::function<void(bool)> finished)
{
    auto *watcher = new QFutureWatcher<std::vector<easy3d::Model *>>(this);
    connect(watcher, &QFutureWatcher<std::vector<easy3d::Model *>>::finished,
            this, [this, watcher, finished = std::move(finished), file]() mutable {
        bool success = false;
        try {
                auto models = watcher->result();
                for (auto *model : models) {
                    model->set_name(file.toStdString());
                    addModel(model);
                }
                update();
                success = !models.empty();
        } catch (const std::exception &e) {
            tl::printException(e);
        }
        watcher->deleteLater();
        if (finished)
            finished(success);
    });
    watcher->setFuture(QtConcurrent::run([file] {
        return readModels(file);
    }));
}

void Easy3DViewer3D::loadFromFiles(const QStringList &files, const QString &parent)
{
    for (const auto &file : files)
        loadFromFile(file, parent);
}

void Easy3DViewer3D::addCamera(const QString &id,
                               double x,
                               double y,
                               double z,
                               const tl::RotationMatrix<double> &rotation)
{
    float scene_radius = 0.f;
    for (const auto *model : mModels)
        scene_radius = std::max(scene_radius, model->bounding_box(true).radius());

    const float scale = scene_radius > 0.f ? scene_radius * 0.04f : 1.f;
    const float half_width = scale;
    const float half_height = scale * 0.7f;
    const float depth = scale * 1.4f;

    const auto transform = [&rotation, x, y, z](const easy3d::vec3 &point) {
        return easy3d::vec3(
            static_cast<float>(x + rotation.at(0, 0) * point.x +
                               rotation.at(0, 1) * point.y +
                               rotation.at(0, 2) * point.z),
            static_cast<float>(y + rotation.at(1, 0) * point.x +
                               rotation.at(1, 1) * point.y +
                               rotation.at(1, 2) * point.z),
            static_cast<float>(z + rotation.at(2, 0) * point.x +
                               rotation.at(2, 1) * point.y +
                               rotation.at(2, 2) * point.z));
    };

    auto *camera = new easy3d::SurfaceMesh();
    //camera->read(id.toStdString());
    const auto center = camera->add_vertex(transform(easy3d::vec3(0.f, 0.f, 0.f)));
    const auto top_left = camera->add_vertex(transform(easy3d::vec3(-half_width, half_height, depth)));
    const auto top_right = camera->add_vertex(transform(easy3d::vec3(half_width, half_height, depth)));
    const auto bottom_right = camera->add_vertex(transform(easy3d::vec3(half_width, -half_height, depth)));
    const auto bottom_left = camera->add_vertex(transform(easy3d::vec3(-half_width, -half_height, depth)));

    const auto add_face = [camera](std::initializer_list<easy3d::SurfaceMesh::Vertex> vertices) {
        camera->add_face(std::vector<easy3d::SurfaceMesh::Vertex>(vertices));
    };

    add_face({center, top_left, top_right});
    add_face({center, top_right, bottom_right});
    add_face({center, bottom_right, bottom_left});
    add_face({center, bottom_left, top_left});
    add_face({top_left, bottom_left, bottom_right});
    add_face({top_left, bottom_right, top_right});

    addModel(camera);

    auto *faces = camera->renderer()->get_triangles_drawable("faces", false);
    if (faces)
        faces->set_visible(false);

    auto *edges = camera->renderer()->get_lines_drawable("edges", false);
    if (edges) {
        edges->set_visible(true);
        edges->set_line_width(2.f);
        edges->state().set_uniform_coloring(easy3d::vec4(0.1f, 0.8f, 1.f, 1.f));
        edges->state().set_lighting(false);
    }

    auto *base = new easy3d::SurfaceMesh(/*id.toStdString() + "_base"*/);
    const auto base_top_left = base->add_vertex(transform(easy3d::vec3(-half_width, half_height, depth)));
    const auto base_top_right = base->add_vertex(transform(easy3d::vec3(half_width, half_height, depth)));
    const auto base_bottom_right = base->add_vertex(transform(easy3d::vec3(half_width, -half_height, depth)));
    const auto base_bottom_left = base->add_vertex(transform(easy3d::vec3(-half_width, -half_height, depth)));
    base->add_face(std::vector<easy3d::SurfaceMesh::Vertex>{base_top_left, base_bottom_left, base_bottom_right});
    base->add_face(std::vector<easy3d::SurfaceMesh::Vertex>{base_top_left, base_bottom_right, base_top_right});

    addModel(base);

    if (auto *base_faces = base->renderer()->get_triangles_drawable("faces", false)) {
        base_faces->state().set_uniform_coloring(easy3d::vec4(0.05f, 0.35f, 0.55f, 1.f));
        base_faces->state().set_lighting(false);
    }
    if (auto *base_edges = base->renderer()->get_lines_drawable("edges", false))
        base_edges->set_visible(false);
}

void Easy3DViewer3D::setPickingMode(PickingMode mode)
{
    mPickingMode = mode;
    mPickedPoints.clear();
    mMeasurementPoints.clear();
    mMeasurementComplete = false;
    mAreaClosed = false;
    updateMeasurementDrawables();

    switch (mode) {
    case PickingMode::pointInfo:
        emit measurementMessage(tr("Click a point to display its coordinates."));
        break;
    case PickingMode::distance:
        emit measurementMessage(tr("Select two points to measure the distance."));
        break;
    case PickingMode::angle:
        emit measurementMessage(tr("Select three points to measure the angle."));
        break;
    case PickingMode::area:
        emit measurementMessage(tr("Select area vertices and right-click to finish."));
        break;
    case PickingMode::scale:
        emit measurementMessage(tr("Select two points to set the model scale."));
        break;
    case PickingMode::none:
        emit measurementMessage(QString());
        break;
    }
}

void Easy3DViewer3D::applyScale(double scale)
{
    tl::Matrix<double, 4, 4> transform = tl::Matrix<double, 4, 4>::identity();
    transform[0][0] = scale;
    transform[1][1] = scale;
    transform[2][2] = scale;
    applyTransform(transform);
}

void Easy3DViewer3D::applyTransform(const tl::Matrix<double, 4, 4> &transform)
{
    makeCurrent();
    for (auto *model : mModels) {
        if (!model)
            continue;
        for (auto &point : model->points()) {
            const double x = point.x;
            const double y = point.y;
            const double z = point.z;
            point.x = static_cast<float>(transform[0][0] * x + transform[0][1] * y +
                                         transform[0][2] * z + transform[0][3]);
            point.y = static_cast<float>(transform[1][0] * x + transform[1][1] * y +
                                         transform[1][2] * z + transform[1][3]);
            point.z = static_cast<float>(transform[2][0] * x + transform[2][1] * y +
                                         transform[2][2] * z + transform[2][3]);
        }
        model->invalidate_bounding_box();
        if (model->renderer())
            model->renderer()->update();
    }
    mPickedPoints.clear();
    mMeasurementPoints.clear();
    mMeasurementComplete = false;
    mAreaClosed = false;
    updateMeasurementDrawables();
    doneCurrent();
    setGlobalZoom();
}

void Easy3DViewer3D::updateMeasurementDrawables()
{
    const bool owns_context = QOpenGLContext::currentContext() != context();
    if (owns_context)
        makeCurrent();

    if (!mDrawableMeasurementPoints)
        mDrawableMeasurementPoints = new easy3d::PointsDrawable("measurement_points");
    if (!mDrawableMeasurementLines)
        mDrawableMeasurementLines = new easy3d::LinesDrawable("measurement_lines");

    std::vector<easy3d::vec3> points;
    points.reserve(mMeasurementPoints.size());
    for (const auto &point : mMeasurementPoints)
        points.emplace_back(point.x(), point.y(), point.z());

    std::vector<easy3d::vec3> lines;
    if (mMeasurementPoints.size() > 1) {
        lines.reserve((mMeasurementPoints.size() - 1 + (mAreaClosed ? 1 : 0)) * 2);
        for (size_t i = 1; i < mMeasurementPoints.size(); ++i) {
            const auto &first = mMeasurementPoints[i - 1];
            const auto &second = mMeasurementPoints[i];
            lines.emplace_back(first.x(), first.y(), first.z());
            lines.emplace_back(second.x(), second.y(), second.z());
        }
        if (mAreaClosed) {
            const auto &last = mMeasurementPoints.back();
            const auto &first = mMeasurementPoints.front();
            lines.emplace_back(last.x(), last.y(), last.z());
            lines.emplace_back(first.x(), first.y(), first.z());
        }
    }

    mDrawableMeasurementPoints->update_vertex_buffer(points);
    mDrawableMeasurementPoints->state().set_uniform_coloring(easy3d::vec4(1.f, 0.1f, 0.05f, 1.f));
    mDrawableMeasurementPoints->state().set_lighting(false);
    mDrawableMeasurementPoints->set_point_size(12.f);

    mDrawableMeasurementLines->update_vertex_buffer(lines);
    mDrawableMeasurementLines->state().set_uniform_coloring(easy3d::vec4(1.f, 0.9f, 0.05f, 1.f));
    mDrawableMeasurementLines->state().set_lighting(false);
    mDrawableMeasurementLines->set_line_width(3.f);

    if (owns_context)
        doneCurrent();
}

void Easy3DViewer3D::drawMeasurements()
{
    if (mMeasurementPoints.empty() || !mDrawableMeasurementPoints ||
        !mDrawableMeasurementLines)
        return;

    const bool depth_test_enabled = glIsEnabled(GL_DEPTH_TEST) == GL_TRUE;
    glDisable(GL_DEPTH_TEST);
    if (mMeasurementPoints.size() > 1)
        mDrawableMeasurementLines->draw(mCamera);
    mDrawableMeasurementPoints->draw(mCamera);
    if (depth_test_enabled)
        glEnable(GL_DEPTH_TEST);
}

void Easy3DViewer3D::processPickedPoint(const easy3d::vec3 &point)
{
    if (mMeasurementComplete) {
        mPickedPoints.clear();
        mMeasurementPoints.clear();
        mMeasurementComplete = false;
        mAreaClosed = false;
    }

    const QVector3D picked(point.x, point.y, point.z);
    mPickedPoints.push_back(picked);
    mMeasurementPoints.push_back(picked);
    updateMeasurementDrawables();
    emit mouseClicked(picked);

    switch (mPickingMode) {
    case PickingMode::pointInfo:
        emit measurementMessage(tr("Point: X=%1, Y=%2, Z=%3")
            .arg(picked.x(), 0, 'f', 3)
            .arg(picked.y(), 0, 'f', 3)
            .arg(picked.z(), 0, 'f', 3));
        mPickedPoints.clear();
        mMeasurementComplete = true;
        break;
    case PickingMode::distance:
    case PickingMode::scale:
        if (mPickedPoints.size() == 1) {
            emit measurementMessage(tr("First point selected. Select the second point."));
        } else if (mPickedPoints.size() == 2) {
            const double distance = mPickedPoints[0].distanceToPoint(mPickedPoints[1]);
            if (mPickingMode == PickingMode::scale)
                emit measurementMessage(tr("Measured length: %1 m").arg(distance, 0, 'f', 3));
            else
                emit measurementMessage(tr("Distance: %1 m").arg(distance, 0, 'f', 3));
            mPickedPoints.clear();
            mMeasurementComplete = true;
        }
        break;
    case PickingMode::angle:
        if (mPickedPoints.size() < 3) {
            emit measurementMessage(tr("Select point %1 of 3.")
                .arg(mPickedPoints.size() + 1));
        } else {
            const QVector3D first = mPickedPoints[0] - mPickedPoints[1];
            const QVector3D second = mPickedPoints[2] - mPickedPoints[1];
            const double denominator = static_cast<double>(first.length()) * second.length();
            if (denominator > 0.) {
                const double cosine = std::clamp(
                    static_cast<double>(QVector3D::dotProduct(first, second)) / denominator,
                    -1., 1.);
                const double angle = std::acos(cosine) * 180. / 3.14159265358979323846;
                emit measurementMessage(tr("Angle: %1°").arg(angle, 0, 'f', 2));
            } else {
                emit measurementMessage(tr("Angle cannot be measured from coincident points."));
            }
            mPickedPoints.clear();
            mMeasurementComplete = true;
        }
        break;
    case PickingMode::area:
        emit measurementMessage(tr("%1 area vertices selected. Right-click to finish.")
            .arg(mPickedPoints.size()));
        break;
    case PickingMode::none:
        mPickedPoints.clear();
        break;
    }
}

void Easy3DViewer3D::finishAreaMeasurement()
{
    if (mPickedPoints.size() < 3) {
        emit measurementMessage(tr("Select at least three points to measure an area."));
        mPickedPoints.clear();
        mMeasurementPoints.clear();
        updateMeasurementDrawables();
        return;
    }

    double area = 0.;
    const QVector3D origin = mPickedPoints.front();
    for (size_t i = 1; i + 1 < mPickedPoints.size(); ++i) {
        const QVector3D first = mPickedPoints[i] - origin;
        const QVector3D second = mPickedPoints[i + 1] - origin;
        area += static_cast<double>(QVector3D::crossProduct(first, second).length()) * 0.5;
    }

    emit measurementMessage(tr("Area: %1 m²").arg(area, 0, 'f', 3));
    mPickedPoints.clear();
    mAreaClosed = true;
    mMeasurementComplete = true;
    updateMeasurementDrawables();
}

auto Easy3DViewer3D::captureModel() -> QImage
{ 
    return grabFramebuffer(); 
}

void Easy3DViewer3D::update()
{
    QOpenGLWidget::update();
}

void Easy3DViewer3D::deleteSelectEntity()
{
    if (mSelectedObject)
        deleteEntity(QString::fromStdString(mSelectedObject->name()));
}

void Easy3DViewer3D::setFrontView()
{
    setStandardView(easy3d::vec3(0, 1, 0), easy3d::vec3(0, 0, 1));
}

void Easy3DViewer3D::setBottomView()
{
    setStandardView(easy3d::vec3(0, 0, 1), easy3d::vec3(0, 1, 0));
}

void Easy3DViewer3D::setTopView()
{
    setStandardView(easy3d::vec3(0, 0, -1), easy3d::vec3(0, 1, 0));
}

void Easy3DViewer3D::setBackView()
{
    setStandardView(easy3d::vec3(0, -1, 0), easy3d::vec3(0, 0, 1));
}

void Easy3DViewer3D::setLeftView()
{
    setStandardView(easy3d::vec3(1, 0, 0), easy3d::vec3(0, 0, 1));
}

void Easy3DViewer3D::setRightView()
{
    setStandardView(easy3d::vec3(-1, 0, 0), easy3d::vec3(0, 0, 1));
}

void Easy3DViewer3D::setIsoView()
{
    setStandardView(easy3d::vec3(1, 1, -1).normalize(), easy3d::vec3(1, 1, 1));
}

void Easy3DViewer3D::setStandardView(const easy3d::vec3 &direction,
                                     const easy3d::vec3 &up)
{
    if (mModels.empty())
        return;

    mCamera->setUpVector(up);
    mCamera->setViewDirection(direction);
    setGlobalZoom();
}

void Easy3DViewer3D::setGlobalZoom()
{
    if (!mModels.empty()) {

        easy3d::Box3 box;
        for (auto model : mModels) {
            if (model)
                box.grow(model->bounding_box(true));
        }

        if (!box.is_valid())
            return;

        mCamera->setSceneBoundingBox(box.min_point(), box.max_point());
        mCamera->showEntireScene();
        mCamera->setPivotPoint(mCamera->sceneCenter());
        easy3d::ClippingPlane::instance()->fit_scene(mCamera->sceneCenter(), mCamera->sceneRadius());

        update();
    }
}

void Easy3DViewer3D::enableEDL()
{
    mEdlEnabled = true;
    update();
}

void Easy3DViewer3D::disableEDL()
{
    mEdlEnabled = false;
    update();
}

void Easy3DViewer3D::setPerspectiveProjection(bool enabled)
{
    mCamera->setType(enabled ? easy3d::Camera::PERSPECTIVE : easy3d::Camera::ORTHOGRAPHIC);
    update();
}

void Easy3DViewer3D::setPointLighting(bool enabled)
{
    for (auto model : mModels) {

        if (!model->renderer()) continue;

        auto *cloud = dynamic_cast<easy3d::PointCloud *>(model);
        auto *mesh = dynamic_cast<easy3d::SurfaceMesh *>(model);

        if (cloud) {

            if (!cloud->get_vertex_property<easy3d::vec3>("v:normal"))
                continue;

            for (const auto &drawable : cloud->renderer()->points_drawables()) {
                drawable->set_lighting(enabled);
            }

        } else if (mesh) {

            if (!mesh->get_vertex_property<easy3d::vec3>("v:normal"))
                continue;

            for (const auto &drawable : mesh->renderer()->triangles_drawables()) {
                drawable->set_lighting(enabled);
            }
        }
    }

    update();
}

void Easy3DViewer3D::setPointSize(int size)
{
    for (auto model : mModels) {
        if (!model->renderer())
            continue;
        for (const auto &drawable : model->renderer()->points_drawables())
            drawable->set_point_size(static_cast<float>(size));
    }

    update();
}

void Easy3DViewer3D::changePointSize()
{
    float currentSize = 1.f;
    for (auto model : mModels) {
        if (model->renderer() && !model->renderer()->points_drawables().empty()) {
            currentSize = model->renderer()->points_drawables().front()->point_size();
            break;
        }
    }

    bool accepted = false;
    const double size = QInputDialog::getDouble(this, tr("Point Size"), tr("Size:"),
                                                currentSize, 1.0, 100.0, 1, &accepted);
    if (accepted) {
        //for (auto model : mModels) {
        //    if (!model->renderer())
        //        continue;
        //    for (const auto &drawable : model->renderer()->points_drawables())
        //        drawable->set_point_size(static_cast<float>(size));
        //}
        //update();
        setPointSize(size);
    }
}

void Easy3DViewer3D::setBackgroundColor(const QColor &color)
{
    mBackgroundColor = color;

    makeCurrent();
    context()->functions()->glClearColor(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF(), 1.f);
    doneCurrent();
}

auto Easy3DViewer3D::backGroundColor() const -> QColor
{
    return mBackgroundColor;
}

void Easy3DViewer3D::selectEntity(easy3d::Model *entity)
{
    for (auto model : mModels)
        if (model->renderer())
            model->renderer()->set_selected(false);
    mSelectedObject = entity;
    if (entity && entity->renderer())
        entity->renderer()->set_selected(true);
    update();
}

void Easy3DViewer3D::showContextMenu(const QPoint &position)
{
    bool hasNormals = false;
    bool pointLighting = false;
    bool hasPointCloud = false;

    for (auto model : mModels) {

        if (!model || !model->renderer())
            continue;

        bool modelHasNormals = false;
        auto *cloud = dynamic_cast<easy3d::PointCloud *>(model);
        auto *mesh = dynamic_cast<easy3d::SurfaceMesh *>(model);

        if (cloud) {
            hasPointCloud = true;
        }

        if (cloud && cloud->get_vertex_property<easy3d::vec3>("v:normal")) {
            modelHasNormals = true;
        } else if (mesh && mesh->get_vertex_property<easy3d::vec3>("v:normal")) {
            modelHasNormals = true;
        }

        if (modelHasNormals) {
            hasNormals = true;

            if (cloud && !cloud->renderer()->points_drawables().empty() &&
                cloud->renderer()->points_drawables().front()->lighting()) {
                pointLighting = true;
            } else if (mesh && !mesh->renderer()->triangles_drawables().empty() &&
                mesh->renderer()->triangles_drawables().front()->lighting()) {
                pointLighting = true;
            }
        }

        //auto *cloud = dynamic_cast<easy3d::PointCloud *>(model);
        //if (!cloud || !cloud->renderer() ||
        //    !cloud->get_vertex_property<easy3d::vec3>("v:normal"))
        //    continue;
        //hasNormals = true;
        //if (!cloud->renderer()->points_drawables().empty() &&
        //    cloud->renderer()->points_drawables().front()->lighting())
        //    pointLighting = true;
    }

    mContextMenu->setPerspectiveProjection(mCamera->type() == easy3d::Camera::PERSPECTIVE);
    mContextMenu->setPointLightingAvailable(hasNormals);
    mContextMenu->setPointLighting(pointLighting);

    if (mMousePressed == position)
        mContextMenu->exec(mapToGlobal(position));
}

void Easy3DViewer3D::drawCornerAxes()
{
    easy3d::ShaderProgram *program = easy3d::ShaderManager::get_program("surface/surface");

    if (!program) {
        std::vector<easy3d::ShaderProgram::Attribute> attributes;
        attributes.emplace_back(easy3d::ShaderProgram::Attribute(easy3d::ShaderProgram::POSITION, "vtx_position"));
        attributes.emplace_back(easy3d::ShaderProgram::Attribute(easy3d::ShaderProgram::TEXCOORD, "vtx_texcoord"));
        attributes.emplace_back(easy3d::ShaderProgram::Attribute(easy3d::ShaderProgram::COLOR, "vtx_color"));
        attributes.emplace_back(easy3d::ShaderProgram::Attribute(easy3d::ShaderProgram::NORMAL, "vtx_normal"));
        program = easy3d::ShaderManager::create_program_from_files("surface/surface", attributes);
    }
    if (!program)
        return;

    if (!mDrawableAxes) {
        const float base = 0.5f;
        const float head = 0.2f;
        std::vector<easy3d::vec3> points, normals, colors;
        easy3d::shape::create_cylinder(0.03, 10, easy3d::vec3(0, 0, 0), easy3d::vec3(base, 0, 0), easy3d::vec3(1, 0, 0), points, normals, colors);
        easy3d::shape::create_cylinder(0.03, 10, easy3d::vec3(0, 0, 0), easy3d::vec3(0, base, 0), easy3d::vec3(0, 1, 0), points, normals, colors);
        easy3d::shape::create_cylinder(0.03, 10, easy3d::vec3(0, 0, 0), easy3d::vec3(0, 0, base), easy3d::vec3(0, 0, 1), points, normals, colors);
        easy3d::shape::create_cone(0.06, 20, easy3d::vec3(base, 0, 0), easy3d::vec3(base + head, 0, 0), easy3d::vec3(1, 0, 0), points, normals, colors);
        easy3d::shape::create_cone(0.06, 20, easy3d::vec3(0, base, 0), easy3d::vec3(0, base + head, 0), easy3d::vec3(0, 1, 0), points, normals, colors);
        easy3d::shape::create_cone(0.06, 20, easy3d::vec3(0, 0, base), easy3d::vec3(0, 0, base + head), easy3d::vec3(0, 0, 1), points, normals, colors);
        easy3d::shape::create_sphere(easy3d::vec3(0, 0, 0), 0.06, 20, 20, easy3d::vec3(0, 1, 1), points, normals, colors);
        mDrawableAxes = new easy3d::TrianglesDrawable("corner_axes");
        mDrawableAxes->update_vertex_buffer(points);
        mDrawableAxes->update_normal_buffer(normals);
        mDrawableAxes->update_color_buffer(colors);
        mDrawableAxes->set_property_coloring(easy3d::State::VERTEX);
    }
    if (!mDrawableAxes->is_visible())
        return;

    // The viewport is changed to fit the lower left corner.
    std::array<int, 4> viewport{};
    auto *functions = context()->functions();
    functions->glGetIntegerv(GL_VIEWPORT, viewport.data());

    const int corner_frame_size = static_cast<int>(100.0 * devicePixelRatioF());
    functions->glViewport(0, 0, corner_frame_size, corner_frame_size);
    functions->glDepthRangef(0, 0.01f);

    const easy3d::mat4 &proj = easy3d::transform::ortho(-1, 1, -1, 1, -1, 1);
    const easy3d::mat4 &view = mCamera->orientation().inverse().matrix();
    const easy3d::mat4 &MVP = proj * view;

    // camera position is defined in world coordinate system.
    const easy3d::vec3 &wCamPos = mCamera->position();
    const easy3d::mat4 &MV = mCamera->modelViewMatrix();
    const easy3d::vec4 &wLightPos = inverse(MV) * easy3d::setting::light_position;

    program->bind();
    program->set_uniform("MVP", MVP)
        ->set_uniform("MANIP", easy3d::mat4::identity())
        ->set_uniform("NORMAL", easy3d::mat3::identity())   // needs be padded when using uniform blocks
        ->set_uniform("lighting", true)
        ->set_uniform("two_sides_lighting", false)
        ->set_uniform("smooth_shading", true)
        ->set_uniform("wLightPos", wLightPos)
        ->set_uniform("wCamPos", wCamPos)
        ->set_uniform("ssaoEnabled", false)
        ->set_uniform("per_vertex_color", true)
        ->set_uniform("distinct_back_color", false)
        ->set_block_uniform("Material", "ambient", easy3d::setting::material_ambient)
        ->set_block_uniform("Material", "specular", easy3d::setting::material_specular)
        ->set_block_uniform("Material", "shininess", &easy3d::setting::material_shininess)
        ->set_uniform("highlight", false)
        ->set_uniform("clippingPlaneEnabled", false)
        ->set_uniform("selected", false)
        ->set_uniform("highlight_color", easy3d::setting::highlight_color)
        ->set_uniform("use_texture", false);
    mDrawableAxes->gl_draw();
    program->release();

    // restore the viewport
    functions->glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    functions->glDepthRangef(0.0f, 1.0f);
}

auto Easy3DViewer3D::pointUnderPixel(const QPoint &position, bool &found) const -> easy3d::vec3
{
    const_cast<Easy3DViewer3D *>(this)->makeCurrent();

    int glx = position.x();
    int gly = height() - 1 - position.y();

    float device_pixel_ratio = devicePixelRatioF();

    glx = static_cast<int>(static_cast<float>(glx) * device_pixel_ratio);
    gly = static_cast<int>(static_cast<float>(gly) * device_pixel_ratio);

    int samples = 0;
    context()->functions()->glGetIntegerv(GL_SAMPLES, &samples);

        float depth = 1.0f;
    if (samples > 0) {
        easy3d::opengl::read_depth_ms(depth, glx, gly);
    } else {
        easy3d::opengl::read_depth(depth, glx, gly);
    }

    const_cast<Easy3DViewer3D *>(this)->doneCurrent();

    found = depth < 1.0f;
    if (found) {
        // The input to unprojectedCoordinatesOf() is defined in the screen coordinate system
        easy3d::vec3 point(static_cast<float>(position.x()), static_cast<float>(position.y()), depth);
        point = mCamera->unprojectedCoordinatesOf(point);
        return point;
    }

    return {};
}

void Easy3DViewer3D::init()
{
    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void Easy3DViewer3D::initSignalsAndSlots()
{
    connect(this, &QWidget::customContextMenuRequested, this, &Easy3DViewer3D::showContextMenu);
    connect(mContextMenu, &Viewer3DContextMenu::globalZoom, this, &Easy3DViewer3D::setGlobalZoom);
    connect(mContextMenu, &Viewer3DContextMenu::viewFront, this, &Easy3DViewer3D::setFrontView);
    connect(mContextMenu, &Viewer3DContextMenu::viewTop, this, &Easy3DViewer3D::setTopView);
    connect(mContextMenu, &Viewer3DContextMenu::viewLeft, this, &Easy3DViewer3D::setLeftView);
    connect(mContextMenu, &Viewer3DContextMenu::viewRight, this, &Easy3DViewer3D::setRightView);
    connect(mContextMenu, &Viewer3DContextMenu::viewBack, this, &Easy3DViewer3D::setBackView);
    connect(mContextMenu, &Viewer3DContextMenu::viewBottom, this, &Easy3DViewer3D::setBottomView);
    connect(mContextMenu, &Viewer3DContextMenu::filterEDL, this,
            [this](bool active) { active ? enableEDL() : disableEDL(); });
    connect(mContextMenu, &Viewer3DContextMenu::perspectiveProjection,
            this, &Easy3DViewer3D::setPerspectiveProjection);
    connect(mContextMenu, &Viewer3DContextMenu::pointLighting,
            this, &Easy3DViewer3D::setPointLighting);
    connect(mContextMenu, &Viewer3DContextMenu::pointSize,
            this, &Easy3DViewer3D::changePointSize);

    easy3d::connect(&mCamera->frame_modified, this, &Easy3DViewer3D::update);
}

void Easy3DViewer3D::addModel(easy3d::Model *entity)
{
    if (!entity)
        return;

    for (auto model : mModels) {
        if (entity == model) {
            tl::Message::warning("Model has already been added to the viewer: {}", model->name());
            return;
        }
    }

    if (entity->empty()) {
        delete entity;
        return;
    }

    makeCurrent();
    entity->set_renderer(std::make_shared<easy3d::Renderer>(entity));
    doneCurrent();

    mModels.push_back(entity);
}

auto Easy3DViewer3D::findChild(const QString &name, easy3d::Model *) -> easy3d::Model *
{
    for (auto model : mModels)
        if (QString::fromStdString(model->name()) == name)
            return model;
    return nullptr;
}

void Easy3DViewer3D::preDraw()
{
    context()->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Easy3DViewer3D::draw()
{
    if (mEdlEnabled)
        mEdl->begin();

    for (auto model : mModels) {

        if (!model->renderer() || !model->renderer()->is_visible())
            continue;

        glDepthRange(0.001, 1.0);
        for (const auto &drawable : model->renderer()->triangles_drawables()) {
            if (drawable->is_visible()) 
                drawable->draw(mCamera);
        }

        glDepthRange(0.0, 1.0);
        for (const auto &drawable : model->renderer()->lines_drawables()) {
            if (drawable->is_visible()) 
                drawable->draw(mCamera);
        }

        for (const auto &drawable : model->renderer()->points_drawables()) {
            if (drawable->is_visible()) 
                drawable->draw(mCamera);
        }
    }

    if (mEdlEnabled)
        mEdl->end();
}

void Easy3DViewer3D::postDraw()
{
    if (mShowManipSphere) {
        if (!mDrawableManipSphere)
            mDrawableManipSphere = new easy3d::LinesDrawable("manipulation_sphere");

        const int radius = 150;
        const float ratio = mCamera->pixelGLRatio(mCamera->pivotPoint());
        const easy3d::mat4 manipulation =
            easy3d::mat4::translation(mCamera->pivotPoint()) *
            easy3d::mat4::scale(radius * ratio);
        easy3d::shape::draw_sphere_big_circles(
            mDrawableManipSphere,
            mCamera->modelViewProjectionMatrix(),
            manipulation);
    }

    drawMeasurements();
    drawCornerAxes();
}


void Easy3DViewer3D::initializeGL()
{
    QOpenGLWidget::initializeGL();

    auto *functions = context()->functions();
    functions->initializeOpenGLFunctions();
    functions->glEnable(GL_DEPTH_TEST);

    easy3d::OpenglUtil::init();

    TL_ASSERT(functions->hasOpenGLFeature(QOpenGLFunctions::Multisample), "Multisample not supported on this machine!!! Viewer may not run properly");
    TL_ASSERT(functions->hasOpenGLFeature(QOpenGLFunctions::Framebuffers), "Framebuffer Object is not supported on this machine!!! Viewer may not run properly");

    functions->glEnable(GL_DEPTH_TEST);
    functions->glClearDepthf(1.0f);
    functions->glClearColor(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF(), 1.f);

    int major = 0;
    functions->glGetIntegerv(GL_MAJOR_VERSION, &major);
    int minor = 0;
    functions->glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (major * 10 + minor < 32) {
        throw std::runtime_error("Viewer requires at least OpenGL 3.2");
    }
}

void Easy3DViewer3D::resizeGL(int width, int height)
{
    QOpenGLWidget::resizeGL(width, height);

    mCamera->setScreenWidthAndHeight(width, height);
}

void Easy3DViewer3D::paintGL()
{
    //auto *functions = context()->functions();
    //functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //for (auto model : mModels) {
    //    if (!model->renderer() || !model->renderer()->is_visible())
    //        continue;
    //    for (const auto &drawable : model->renderer()->triangles_drawables())
    //        if (drawable->is_visible()) drawable->draw(mCamera);
    //    for (const auto &drawable : model->renderer()->lines_drawables())
    //        if (drawable->is_visible()) drawable->draw(mCamera);
    //    for (const auto &drawable : model->renderer()->points_drawables())
    //        if (drawable->is_visible()) drawable->draw(mCamera);
    //}

    preDraw();
    draw();
    postDraw();
}

void Easy3DViewer3D::mousePressEvent(QMouseEvent *event)
{
    mPressedButton = event->button();
    mMousePrevious = event->pos();
    mMousePressed = event->pos();

    if (mPickingMode == PickingMode::none)
        mCamera->frame()->action_start();

    if (mPickingMode == PickingMode::none &&
        (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) &&
        event->modifiers() == Qt::NoModifier) {
        mShowManipSphere = true;
    }
    
    if (event->modifiers() == Qt::ShiftModifier) {
        if (event->button() == Qt::LeftButton) {
            bool found = false;
            const easy3d::vec3 &p = pointUnderPixel(event->pos(), found);
            if (found) {
                mCamera->interpolateToLookAt(p);
                mCamera->setPivotPoint(p);

                // show, but hide the visual hint of pivot point after \p delay milliseconds.
                //show_pivot_point_ = true;
                //Timer<>::single_shot(10000, [&]() {
                //    show_pivot_point_ = false;
                //    update();
                //    });
            } else {
                mCamera->setPivotPoint(mCamera->sceneCenter());
                //show_pivot_point_ = false;
            }
        } else if (event->button() == Qt::RightButton) {
            //TODO: Ocultar menú contextual en este caso
            mCamera->interpolateToFitScene();
            mCamera->setPivotPoint(mCamera->sceneCenter());
            //show_pivot_point_ = false;
            
        }
    }

    QOpenGLWidget::mousePressEvent(event);
    update();
}

void Easy3DViewer3D::mouseReleaseEvent(QMouseEvent *event)
{
    if (mPickingMode != PickingMode::none &&
        event->button() == Qt::RightButton &&
        mPickingMode == PickingMode::area) {
        finishAreaMeasurement();
        mPressedButton = Qt::NoButton;
        event->accept();
        update();
        return;
    }

    if (mPickingMode != PickingMode::none &&
        event->button() == Qt::LeftButton &&
        (event->pos() - mMousePressed).manhattanLength() <= 4) {
        bool found = false;
        const easy3d::vec3 point = pointUnderPixel(event->pos(), found);
        if (found)
            processPickedPoint(point);
        else
            emit measurementMessage(tr("No model point was selected."));
        mPressedButton = Qt::NoButton;
        event->accept();
        update();
        return;
    }

    if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
        int xmin = std::min(mMousePressed.x(), event->pos().x());
        int xmax = std::max(mMousePressed.x(), event->pos().x());
        int ymin = std::min(mMousePressed.y(), event->pos().y());
        int ymax = std::max(mMousePressed.y(), event->pos().y());
        mCamera->fitScreenRegion(xmin, ymin, xmax, ymax);
    } else if (mPickingMode == PickingMode::none) {
        mCamera->frame()->action_end();
    }

    if (event->button() == Qt::LeftButton ||
        event->button() == Qt::RightButton)
        mShowManipSphere = false;

    mPressedButton = Qt::NoButton;

    QOpenGLWidget::mouseReleaseEvent(event);
    update();
}

void Easy3DViewer3D::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint position = event->pos();

    bool found = false;
    const easy3d::vec3 &point = pointUnderPixel(position, found);
    if (found) {
        mCamera->setPivotPoint(point);
    } else {
        mCamera->setPivotPoint(mCamera->sceneCenter());
    }

    QOpenGLWidget::mouseDoubleClickEvent(event);
}

void Easy3DViewer3D::mouseMoveEvent(QMouseEvent *event)
{
    QPoint position = event->pos();

    if (position.x() < 0 || position.y() < 0 || position.x() >= width() || position.y() >= height()) {
        event->ignore();
        return;
    }

    if (mPressedButton != Qt::NoButton) {
        if (event->modifiers() == Qt::NoModifier &&
            mPickingMode == PickingMode::none) {

            int dx = position.x() - mMousePrevious.x();
            int dy = position.y() - mMousePrevious.y();

            if (mPressedButton == Qt::LeftButton) {
                mCamera->frame()->action_rotate(position.x(), position.y(), dx, dy,
                                                mCamera, easy3d::ManipulatedFrame::NONE);
            } else if (mPressedButton == Qt::RightButton) {
                mCamera->frame()->action_translate(position.x(), position.y(), dx, dy,
                                                   mCamera, easy3d::ManipulatedFrame::NONE);
            } else if (mPressedButton == Qt::MiddleButton && dy != 0) {
                mCamera->frame()->action_zoom(dy > 0 ? 1 : -1, mCamera);
            }
        }
    }

    mMousePrevious = position;
    QOpenGLWidget::mouseMoveEvent(event);
    update();
}

void Easy3DViewer3D::wheelEvent(QWheelEvent *event)
{
    mCamera->frame()->action_zoom(event->angleDelta().y() > 0 ? 1 : -1, mCamera);

    QOpenGLWidget::wheelEvent(event);
    update();
}

#endif
}
