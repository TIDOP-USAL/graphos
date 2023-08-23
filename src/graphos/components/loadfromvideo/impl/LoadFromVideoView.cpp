/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "LoadFromVideoView.h"

#include <QApplication>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QFileDialog>
#include <QToolBar>
#include <QSlider>


namespace graphos
{

LoadFromVideoViewImp::LoadFromVideoViewImp(QWidget *parent)
    : LoadFromVideoView(parent)
{
    this->initUI();
    this->initSignalAndSlots();
}

LoadFromVideoViewImp::~LoadFromVideoViewImp()
{

}

void LoadFromVideoViewImp::initUI()
{
    this->setObjectName(QString("LoadFromVideoView"));
    this->resize(750, 450);

    QGridLayout *gridLayout = new QGridLayout();
    this->setLayout(gridLayout);

    QToolBar *toolBar = new QToolBar(this);
    mActionLoadVideo = new QAction(this);
    mActionLoadVideo->setIcon(QIcon::fromTheme("video-folder"));
    toolBar->addAction(mActionLoadVideo);
    toolBar->addSeparator();
    mActionPlayVideo = new QAction(this);
    mActionPlayVideo->setIcon(QIcon::fromTheme("play"));
    toolBar->addAction(mActionPlayVideo);
    mActionPauseVideo = new QAction(this);
    mActionPauseVideo->setIcon(QIcon::fromTheme("pause"));
    toolBar->addAction(mActionPauseVideo);
    mActionStopVideo = new QAction(this);
    mActionStopVideo->setIcon(QIcon::fromTheme("stop"));
    toolBar->addAction(mActionStopVideo);
    gridLayout->setMargin(0);
    gridLayout->addWidget(toolBar, 0, 1);

    QGridLayout *gridLayout2 = new QGridLayout();
    gridLayout2->setContentsMargins(11, 0, 11, 11);
    gridLayout->addLayout(gridLayout2, 1, 1);

    //mLabelVideo = new QLabel(this);
    //gridLayout->addWidget(mLabelVideo, 0, 0, 1, 1);
    //mLineEditVideo = new QLineEdit(this);
    //gridLayout->addWidget(mLineEditVideo, 0, 1, 1, 1);
    //mPushButtonVideo = new QPushButton(this);
    //mPushButtonVideo->setMaximumSize(31, 28);
    //mPushButtonVideo->setText("...");
    //gridLayout->addWidget(mPushButtonVideo, 0, 2, 1, 1);

    mLabelVideoViewer = new QLabel(this);
    mLabelVideoViewer->setMinimumHeight(400);
    mLabelVideoViewer->setStyleSheet("background-color: #FF000000");
    gridLayout2->addWidget(mLabelVideoViewer, 0, 0, 1, 3);

    mSliderVideo = new QSlider(Qt::Orientation::Horizontal, this);
    gridLayout2->addWidget(mSliderVideo, 1, 0, 1, 3);

    mLabelSkipFrames = new QLabel(this);
    gridLayout2->addWidget(mLabelSkipFrames, 2, 0, 1, 1);
    mSpinBoxSkipFrames = new QSpinBox(this);
    mSpinBoxSkipFrames->setValue(20);
    gridLayout2->addWidget(mSpinBoxSkipFrames, 2, 1, 1, 2);

    mLabelVideoIni = new QLabel(this);
    gridLayout2->addWidget(mLabelVideoIni, 3, 0, 1, 1);
    mSpinBoxVideoIni = new QSpinBox(this);
    gridLayout2->addWidget(mSpinBoxVideoIni, 3, 1, 1, 1);
    mPushButtonVideoIni = new QPushButton(this);
    gridLayout2->addWidget(mPushButtonVideoIni, 3, 2, 1, 1);

    mLabelVideoEnd = new QLabel(this);
    gridLayout2->addWidget(mLabelVideoEnd, 4, 0, 1, 1);
    mSpinBoxVideoEnd = new QSpinBox(this);
    gridLayout2->addWidget(mSpinBoxVideoEnd, 4, 1, 1, 1);
    mPushButtonVideoEnd = new QPushButton(this);
    gridLayout2->addWidget(mPushButtonVideoEnd, 4, 2, 1, 1);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    gridLayout2->addWidget(mButtonBox, 5, 0, 1, 3);

    this->retranslate();
    this->clear();
    this->update();
}

void LoadFromVideoViewImp::initSignalAndSlots()
{
    //connect(mLineEditVideo, &QLineEdit::textChanged, this, &LoadFromVideoViewImp::update);
    connect(mActionLoadVideo, &QAction::triggered, [&]() {

        mVideo = QFileDialog::getOpenFileName(nullptr, tr("Open video"),
        QDir::homePath(),
        tr("Video file (*.*)"));

    if (!mVideo.isEmpty()) {
        if (capture.open(mVideo.toStdString())) {
            mFlags.activeFlag(Flag::video_load, true);
            int frames = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_COUNT));
            mSpinBoxVideoIni->setMaximum(frames);
            mSpinBoxVideoIni->setValue(0);
            mSpinBoxVideoEnd->setMaximum(frames);
            mSpinBoxVideoEnd->setValue(frames);
            mSliderVideo->setMaximum(frames);
            //importFrame();
            update();
        }
    }
            });

    connect(mActionPlayVideo, &QAction::triggered, [&]() {
        if (!capture.isOpened()) {
            capture.open(mVideo.toStdString());
        }

        mFlags.activeFlag(Flag::video_run, true);

        int frame_per_seconds = static_cast<int>(capture.get(cv::CAP_PROP_FPS));
        timer->start(frame_per_seconds);

        update();
            });

    connect(mActionPauseVideo, &QAction::triggered, [&]() {

        timer->stop();

        mFlags.activeFlag(Flag::video_run, false);

        update();
            });

    connect(mActionStopVideo, &QAction::triggered, [&]() {

        timer->stop();
        capture.release();
        mFlags.activeFlag(Flag::video_run, false);
        mLabelVideoViewer->clear();
        update();

            });

    connect(mSpinBoxSkipFrames, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &LoadFromVideoView::skip_frames_changed);
    connect(mSpinBoxVideoIni, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &LoadFromVideoView::start_video_changed);
    connect(mSpinBoxVideoEnd, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &LoadFromVideoView::end_video_changed);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(importFrame()));

    connect(mPushButtonVideoIni, &QPushButton::clicked, [&]() {

        int frame = static_cast<int>(capture.get(cv::CAP_PROP_POS_FRAMES));
        mSpinBoxVideoIni->setValue(frame);
        if (mSpinBoxVideoEnd->value() < frame)
            mSpinBoxVideoEnd->setValue(frame);
            });

    connect(mPushButtonVideoEnd, &QPushButton::clicked, [&]() {

        int frame = static_cast<int>(capture.get(cv::CAP_PROP_POS_FRAMES));
        mSpinBoxVideoEnd->setValue(frame);
        if (mSpinBoxVideoIni->value() > frame)
            mSpinBoxVideoIni->setValue(frame);

            });

    connect(mSliderVideo, &QSlider::valueChanged, [&](int value) {

        capture.set(cv::CAP_PROP_POS_FRAMES, static_cast<double>(value));

            });

    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &LoadFromVideoView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void LoadFromVideoViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("LoadFromVideoView", "Load from Video"));
    mActionLoadVideo->setText(QApplication::translate("LoadFromVideoView", "Load video"));
    mActionPlayVideo->setText(QApplication::translate("LoadFromVideoView", "Play video"));
    mActionPauseVideo->setText(QApplication::translate("LoadFromVideoView", "Pause video"));
    mActionStopVideo->setText(QApplication::translate("LoadFromVideoView", "Stop video"));
    //mLabelVideo->setText(QApplication::translate("VideoView", "Video:"));
    mLabelSkipFrames->setText(QApplication::translate("SkipFramesView", "Skip Frames:"));
    mLabelVideoIni->setText(QApplication::translate("SkipFramesView", "Video ini:"));
    mPushButtonVideoIni->setText(QApplication::translate("SkipFramesView", "Set video ini"));
    mLabelVideoEnd->setText(QApplication::translate("SkipFramesView", "Video end:"));
    mPushButtonVideoEnd->setText(QApplication::translate("SkipFramesView", "Set video end"));

    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("LoadFromVideoView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("LoadFromVideoView", "Run"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("LoadFromVideoView", "Help"));
}

void LoadFromVideoViewImp::clear()
{

}

void LoadFromVideoViewImp::update()
{
    bool video_opened = capture.isOpened();
    mActionPlayVideo->setEnabled(video_opened);
    mActionPlayVideo->setVisible(!mFlags.isEnabled(Flag::video_run));
    mActionPauseVideo->setVisible(mFlags.isEnabled(Flag::video_run));
    mActionStopVideo->setEnabled(video_opened);
    mSpinBoxVideoIni->setEnabled(video_opened);
    mSpinBoxVideoEnd->setEnabled(video_opened);
    mPushButtonVideoIni->setEnabled(video_opened);
    mPushButtonVideoEnd->setEnabled(video_opened);
    mSliderVideo->setEnabled(video_opened);

    mButtonBox->button(QDialogButtonBox::Apply)->setEnabled(video_opened);
}

QString LoadFromVideoViewImp::video() const
{
    return mVideo;
}

int LoadFromVideoViewImp::skipFrames() const
{
    return mSpinBoxSkipFrames->value();
}

int LoadFromVideoViewImp::videoIni() const
{
    return mSpinBoxVideoIni->value();
}

int LoadFromVideoViewImp::videoEnd() const
{
    return mSpinBoxVideoEnd->value();
}


//void LoadFromVideoViewImp::setVideo(const QString &video) 
//{
//  mVideo = video;
//}

void LoadFromVideoViewImp::setSkipFrames(int SkipFrames)
{
    const QSignalBlocker blocker(mSpinBoxSkipFrames);
    mSpinBoxSkipFrames->setValue(SkipFrames);
}

/**/

void LoadFromVideoViewImp::importFrame()
{
    capture >> frame;
    if (!frame.empty()) {

        int position = static_cast<int>(capture.get(cv::CAP_PROP_POS_FRAMES));
        mSliderVideo->setValue(position);

        cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage srcQImage = QImage((uchar *)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
        int w = mLabelVideoViewer->width();
        int h = mLabelVideoViewer->height();
        QPixmap pixmap = QPixmap::fromImage(srcQImage);
        mLabelVideoViewer->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
        mLabelVideoViewer->show();

    } else {
        mSliderVideo->setValue(0);
        mFlags.activeFlag(Flag::video_run, false);
        update();
        timer->stop();
    }
}

//void LoadFromVideoViewImp::onLoadVideo()
//{
//
//  //int frame_per_seconds = static_cast<int>(capture.get(cv::CAP_PROP_FPS));
//  //timer->start(33);
//}

//void LoadFromVideoViewImp::on_stopButton_clicked()
//{
//  timer->stop();
//  capture.release();
//}
/**/

} // namespace graphos

