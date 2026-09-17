#include "Easy3DViewer3D.h"

#include <QApplication>
#include <QFileInfo>
#include <QInputDialog>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QWheelEvent>

#include <memory>

#include <easy3d/core/point_cloud.h>
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
#include <easy3d/util/signal.h>
#include <easy3d/util/setting.h>

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Exception.h>

namespace graphos
{

#ifdef HAVE_EASY3D

Easy3DViewer3D::Easy3DViewer3D(QWidget *parent)
  : QOpenGLWidget(parent),
    mSelectedObject(nullptr),
    mCamera(new easy3d::Camera),
    mContextMenu(new Viewer3DContextMenu(this)),
    mBackgroundColor(50, 50, 50),
    mPressedButton(Qt::NoButton),
    edl_enabled_(false),
    mPickingMode(PickingMode::point_info),
    mDrawableAxes(nullptr),
    mDrawableManipSphere(nullptr),
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
    delete mDrawableAxes;
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
        const auto extension = QFileInfo(file).suffix().toLower();
        

        bool is_ply_mesh = false;
        if (extension == "ply")
            is_ply_mesh = (easy3d::io::PlyReader::num_instances(file.toStdString(), "face") > 0);

        easy3d::Model *model = nullptr;
        if ((extension == "ply" && is_ply_mesh) || 
            extension == "obj" || extension == "off" || 
            extension == "stl" || extension == "sm" || 
            extension == "geojson" || extension == "trilist") {
            model = easy3d::SurfaceMeshIO::load(file.toStdString());
        } else if (extension == "ply" && easy3d::io::PlyReader::num_instances(file.toStdString(), "edge") > 0){
            model = easy3d::SurfaceMeshIO::load(file.toStdString());
        } else if (extension == "plm" || extension == "pm" || extension == "mesh") {
            model = easy3d::PolyMeshIO::load(file.toStdString());
        } else {
            if (extension == "ptx") {
                easy3d::io::PointCloudIO_ptx serializer(file.toStdString());
                while (auto cloud = serializer.load_next()) {
                    //viewer_->addModel(cloud);
                    addModel(cloud);
                    update();
                }
                return;
            } else
                model = easy3d::PointCloudIO::load(file.toStdString());
        }

        if (model) {
            model->set_name(file.toStdString());
            addModel(model);
            //setGlobalZoom();
            update();
        } else {
            tl::Message::error("[Load] Failed to load file: {}", file.toStdString());
        }
    } catch (const std::exception &e) {
        tl::printException(e);
    }
}

void Easy3DViewer3D::loadFromFiles(const QStringList &files, const QString &parent)
{
    for (const auto &file : files)
        loadFromFile(file, parent);
}

void Easy3DViewer3D::activatePicker(PickingMode pickerMode)
{ 
    mPickingMode = pickerMode;
}

void Easy3DViewer3D::deactivatePicker()
{
    mPickingMode = PickingMode::point_info;
}

QImage Easy3DViewer3D::captureModel() 
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
    mCamera->setViewDirection(easy3d::vec3(0, 0, -1)); 
    update();
}

void Easy3DViewer3D::setBottomView()
{
    mCamera->setViewDirection(easy3d::vec3(0, -1, 0)); 
    update();
}

void Easy3DViewer3D::setTopView()
{
    mCamera->setViewDirection(easy3d::vec3(0, 1, 0));
    update();
}

void Easy3DViewer3D::setBackView()
{
    mCamera->setViewDirection(easy3d::vec3(0, 0, 1)); 
    update();
}

void Easy3DViewer3D::setLeftView()
{
    mCamera->setViewDirection(easy3d::vec3(-1, 0, 0));
    update();
}

void Easy3DViewer3D::setRightView()
{
    mCamera->setViewDirection(easy3d::vec3(1, 0, 0));
    update();
}

void Easy3DViewer3D::setIsoView1()
{
    mCamera->setViewDirection(easy3d::vec3(1, 1, 1).normalize());
    update();
}

void Easy3DViewer3D::setIsoView2()
{
    mCamera->setViewDirection(easy3d::vec3(-1, 1, 1).normalize());
    update();
}

void Easy3DViewer3D::setGlobalZoom()
{
    if (!mModels.empty()) {
        easy3d::Box3 box;
        for (auto model : mModels) {
            //const auto &modelBox = model->bounding_box(true);
            //box.grow(modelBox.min_point());
            //box.grow(modelBox.max_point());
            box.grow(model->bounding_box());
        }
        mCamera->setSceneBoundingBox(box.min_point(), box.max_point());
        mCamera->showEntireScene();
        mCamera->setPivotPoint(mCamera->sceneCenter());
        easy3d::ClippingPlane::instance()->fit_scene(mCamera->sceneCenter(), mCamera->sceneRadius());
        update();
    }
}

void Easy3DViewer3D::enableEDL()
{
    edl_enabled_ = true;
    update();
}

void Easy3DViewer3D::disableEDL()
{
    edl_enabled_ = false; update();
}

void Easy3DViewer3D::setPerspectiveProjection(bool enabled)
{
    mCamera->setType(enabled ? easy3d::Camera::PERSPECTIVE : easy3d::Camera::ORTHOGRAPHIC);
    update();
}

void Easy3DViewer3D::setPointLighting(bool enabled)
{
    for (auto model : mModels) {

        auto *cloud = dynamic_cast<easy3d::PointCloud *>(model);
        if (!cloud || !cloud->renderer())
            continue;

        if (!cloud->get_vertex_property<easy3d::vec3>("v:normal"))
            continue;

        for (const auto &drawable : cloud->renderer()->points_drawables()) {
            drawable->set_lighting(enabled);
        }
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
        for (auto model : mModels) {
            if (!model->renderer())
                continue;
            for (const auto &drawable : model->renderer()->points_drawables())
                drawable->set_point_size(static_cast<float>(size));
        }
        update();
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

void Easy3DViewer3D::processPickedPoint(easy3d::Model *entity, unsigned, int, int,
                                        const easy3d::vec3 &point)
{
    if (entity)
        emit mouseClicked(QVector3D(point.x, point.y, point.z));
}

void Easy3DViewer3D::showContextMenu(const QPoint &position)
{
    bool hasNormals = false;
    bool pointLighting = false;
    for (auto model : mModels) {
        auto *cloud = dynamic_cast<easy3d::PointCloud *>(model);
        if (!cloud || !cloud->renderer() ||
            !cloud->get_vertex_property<easy3d::vec3>("v:normal"))
            continue;
        hasNormals = true;
        if (!cloud->renderer()->points_drawables().empty() &&
            cloud->renderer()->points_drawables().front()->lighting())
            pointLighting = true;
    }
    mContextMenu->setPerspectiveProjection(mCamera->type() == easy3d::Camera::PERSPECTIVE);
    mContextMenu->setPointLightingAvailable(hasNormals);
    mContextMenu->setPointLighting(pointLighting);
    if (mMousePressed == position)
        mContextMenu->exec(mapToGlobal(position));
}

void Easy3DViewer3D::mousePressEvent(QMouseEvent *event)
{
    mPressedButton = event->button();
    mMousePrevious = event->pos();
    mMousePressed = event->pos();

    if ((event->button() == Qt::LeftButton || event->button() == Qt::RightButton) &&
        event->modifiers() == Qt::NoModifier) {
        mShowManipSphere = true;
    }

    mCamera->frame()->action_start();
    QOpenGLWidget::mousePressEvent(event);
    update();
}

void Easy3DViewer3D::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint position = event->pos();

    if (position.x() < 0 || position.y() < 0 || position.x() >= width() || position.y() >= height()) {
        event->ignore();
        return;
    }

    if (mPressedButton != Qt::NoButton) {
        if (event->modifiers() == Qt::NoModifier) {

            const int dx = position.x() - mMousePrevious.x();
            const int dy = position.y() - mMousePrevious.y();

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

void Easy3DViewer3D::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
        int xmin = std::min(mMousePressed.x(), event->pos().x());
        int xmax = std::max(mMousePressed.x(), event->pos().x());
        int ymin = std::min(mMousePressed.y(), event->pos().y());
        int ymax = std::max(mMousePressed.y(), event->pos().y());
        mCamera->fitScreenRegion(xmin, ymin, xmax, ymax);
    } else
        mCamera->frame()->action_end();

    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
        mShowManipSphere = false;
    mPressedButton = Qt::NoButton;
    //mMousePressed = QPoint(0, 0);

    QOpenGLWidget::mouseReleaseEvent(event);
    update();
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
    // it can also be computed as follows:
    //const vec3& wCamPos = invMV * vec4(0, 0, 0, 1);
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

void Easy3DViewer3D::wheelEvent(QWheelEvent *event)
{
    if (mCamera)
        mCamera->frame()->action_zoom(event->angleDelta().y() > 0 ? 1 : -1, mCamera);

    QOpenGLWidget::wheelEvent(event);
    update();
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

easy3d::Model *Easy3DViewer3D::findChild(const QString &name, easy3d::Model *)
{
    for (auto model : mModels)
        if (QString::fromStdString(model->name()) == name)
            return model;
    return nullptr;
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
    //functions->glClearColor(0.f, 0.f, 0.f, 1.f);
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

void Easy3DViewer3D::preDraw()
{
    context()->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Easy3DViewer3D::draw()
{
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

    drawCornerAxes();
}

#endif
}
