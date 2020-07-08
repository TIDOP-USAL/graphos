#include "CamerasView.h"

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QPushButton>

namespace inspector
{

namespace ui
{


CamerasViewImp::CamerasViewImp(QWidget *parent)
  : CamerasView(parent)
{
  this->initUI();
  this->initSignalAndSlots();
}

void CamerasViewImp::initUI()
{
  this->setObjectName(QString("CamerasView"));

  this->resize(800, 660);

  //this->setWindowIcon(QIcon(":/ico/img/InspectorIcon.ico"));

  QGridLayout *layout = new QGridLayout();
  this->setLayout(layout);

  mLabelCameras = new QLabel(this);
  layout->addWidget(mLabelCameras, 0, 0, 1, 1);

  mListWidgetCameras = new QListWidget(this);
  mListWidgetCameras->setMaximumSize(QSize(250, 16777215));
  layout->addWidget(mListWidgetCameras, 1, 0, 4, 1);

  mGroupBoxCamera = new QGroupBox(this);
  QGridLayout *gridLayout3 = new QGridLayout(mGroupBoxCamera);

  mLabelMake = new QLabel(mGroupBoxCamera);
  gridLayout3->addWidget(mLabelMake, 1, 0, 1, 1);
  mLineEditMake = new QLineEdit(mGroupBoxCamera);
  gridLayout3->addWidget(mLineEditMake, 1, 1, 1, 1);

  mLabelModel = new QLabel(mGroupBoxCamera);
  gridLayout3->addWidget(mLabelModel, 1, 3, 1, 1);
  mLineEditModel = new QLineEdit(mGroupBoxCamera);
  gridLayout3->addWidget(mLineEditModel, 1, 4, 1, 1);

  mLabelWidth = new QLabel(mGroupBoxCamera);
  gridLayout3->addWidget(mLabelWidth, 2, 0, 1, 1);
  mSpinBoxWidth = new QSpinBox(mGroupBoxCamera);
  mSpinBoxWidth->setMaximum(100000);
  gridLayout3->addWidget(mSpinBoxWidth, 2, 1, 1, 1);

  mLabelHeight = new QLabel(mGroupBoxCamera);
  gridLayout3->addWidget(mLabelHeight, 2, 3, 1, 1);
  mSpinBoxHeight = new QSpinBox(mGroupBoxCamera);
  mSpinBoxHeight->setMaximum(100000);
  gridLayout3->addWidget(mSpinBoxHeight, 2, 4, 1, 1);

  mLabelSensorSize = new QLabel(mGroupBoxCamera);
  gridLayout3->addWidget(mLabelSensorSize, 5, 0, 1, 1);
  mLineEditSensorSize = new QLineEdit(mGroupBoxCamera);
  gridLayout3->addWidget(mLineEditSensorSize, 5, 1, 1, 5);

  mLabelFocal = new QLabel(mGroupBoxCamera);
  gridLayout3->addWidget(mLabelFocal, 6, 0, 1, 1);
  mLineEditFocal = new QLineEdit(mGroupBoxCamera);
  gridLayout3->addWidget(mLineEditFocal, 6, 1, 1, 5);

  mLabelType = new QLabel(mGroupBoxCamera);
  gridLayout3->addWidget(mLabelType, 7, 0, 1, 1);
  mComboBoxType = new QComboBox(mGroupBoxCamera);
  mComboBoxType->addItem(QString());
  mComboBoxType->addItem(QString());
  mComboBoxType->addItem(QString());
  gridLayout3->addWidget(mComboBoxType, 7, 1, 1, 5);

  layout->addWidget(mGroupBoxCamera, 1, 1, 2, 4);

  mGroupBoxCalibrationParameters = new QGroupBox(this);
  QGridLayout *gridLayout2 = new QGridLayout(mGroupBoxCalibrationParameters);

  mLabelF = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelF, 0, 0, 1, 1);
  mLineEditF = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditF, 0, 1, 1, 3);
  
  mLabelFx = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelFx, 0, 0, 1, 1);
  mLineEditFx = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditFx, 0, 1, 1, 3);
    
  mLabelFy = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelFy, 0, 2, 1, 1);
  mLineEditFy = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditFy, 0, 3, 1, 3);

  mLabelCx = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelCx, 1, 0, 1, 1);
  mLineEditCx = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditCx, 1, 1, 2, 1);

  mLabelCy = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelCy, 1, 2, 1, 1);
  mLineEditCy = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditCy, 1, 3, 1, 1);

  mLabelK1 = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelK1, 4, 0, 1, 1);
  mLineEditK1 = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditK1, 4, 1, 1, 1);

  mLabelK2 = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelK2, 4, 2, 1, 1);
  mLineEditK2 = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditK2, 4, 3, 1, 1);

  mLabelK3 = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelK3, 6, 0, 1, 1);
  mLineEditK3 = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditK3, 6, 1, 1, 1);

  mLabelK4 = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelK4, 6, 2, 1, 1);
  mLineEditK4 = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditK4, 6, 3, 1, 1);

  mLabelK5 = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelK5, 7, 0, 1, 1);
  mLineEditK5 = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditK5, 7, 1, 1, 1);

  mLabelK6 = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelK6, 7, 2, 1, 1);
  mLineEditK6 = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditK6, 7, 3, 1, 1);

  mLabelP1 = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelP1, 8, 0, 1, 1);
  mLineEditP1 = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditP1, 8, 1, 1, 1);

  mLabelP2 = new QLabel(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLabelP2, 8, 2, 1, 1);
  mLineEditP2 = new QLineEdit(mGroupBoxCalibrationParameters);
  gridLayout2->addWidget(mLineEditP2, 8, 3, 1, 1);

  layout->addWidget(mGroupBoxCalibrationParameters, 4, 1, 1, 4);

  mLabelImages = new QLabel(this);
  layout->addWidget(mLabelImages, 5, 0, 1, 1);

  mListWidgetImages = new QListWidget(this);
  layout->addWidget(mListWidgetImages, 6, 0, 1, 5);

  mButtonBox = new QDialogButtonBox(this);
  mButtonBox->setObjectName(QStringLiteral("buttonBox"));
  mButtonBox->setOrientation(Qt::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Help|QDialogButtonBox::Ok);

  layout->addWidget(mButtonBox, 7, 0, 1, 5);


  this->retranslate();
  this->update();
  this->clear();
}

void CamerasViewImp::initSignalAndSlots()
{
  connect(mListWidgetCameras,   &QListWidget::itemSelectionChanged,  this,  &CamerasViewImp::onSelectionChanged);
  connect(mLineEditMake,        &QLineEdit::textEdited,              this,  &CamerasView::makeChanged);
  connect(mLineEditModel,       &QLineEdit::textEdited,              this,  &CamerasView::modelChanged);
  //connect(mSpinBoxWidth,        SIGNAL(valueChanged(int)),           this,  SIGNAL(widthChange(int)));
  //connect(mSpinBoxHeight,       SIGNAL(valueChanged(int)),           this,  SIGNAL(heightChange(int)));
  connect(mLineEditSensorSize,  &QLineEdit::textEdited,              this,  &CamerasView::sensorSizeChange);
  connect(mLineEditFocal,       &QLineEdit::textEdited,              this,  &CamerasView::focalChange);
  connect(mComboBoxType,        &QComboBox::currentTextChanged,      this,  &CamerasView::typeChange);
  connect(mComboBoxType,        &QComboBox::currentTextChanged,      this,  &CamerasViewImp::update);

  connect(mLineEditCx, SIGNAL(textEdited(QString)),  this,  SIGNAL(calibCxChange(QString)));
  connect(mLineEditCy, SIGNAL(textEdited(QString)),  this,  SIGNAL(calibCyChange(QString)));
  connect(mLineEditF,  SIGNAL(textEdited(QString)),  this,  SIGNAL(calibFChange(QString)));
  connect(mLineEditFx,  SIGNAL(textEdited(QString)),  this,  SIGNAL(calibFxChange(QString)));
  connect(mLineEditFy,  SIGNAL(textEdited(QString)),  this,  SIGNAL(calibFyChange(QString)));
  connect(mLineEditK1, SIGNAL(textEdited(QString)),  this,  SIGNAL(calibK1Change(QString)));
  connect(mLineEditK2, SIGNAL(textEdited(QString)),  this,  SIGNAL(calibK2Change(QString)));
  connect(mLineEditK3, SIGNAL(textEdited(QString)),  this,  SIGNAL(calibK3Change(QString)));
  connect(mLineEditK4, SIGNAL(textEdited(QString)),  this,  SIGNAL(calibK4Change(QString)));
  connect(mLineEditK5, SIGNAL(textEdited(QString)),  this,  SIGNAL(calibK5Change(QString)));
  connect(mLineEditK6, SIGNAL(textEdited(QString)),  this,  SIGNAL(calibK6Change(QString)));
  connect(mLineEditP1, SIGNAL(textEdited(QString)),  this,  SIGNAL(calibP1Change(QString)));
  connect(mLineEditP2, SIGNAL(textEdited(QString)),  this,  SIGNAL(calibP2Change(QString)));

  connect(mButtonBox->button(QDialogButtonBox::Ok),     &QAbstractButton::clicked, this, &QDialog::accept);
  connect(mButtonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Help),   &QAbstractButton::clicked, this, &IDialogView::help);
}

void CamerasViewImp::clear()
{
  const QSignalBlocker blocker(mListWidgetCameras);
  mListWidgetCameras->clear();
  mListWidgetImages->clear();

  mLineEditMake->setText("");
  mLineEditModel->setText("");
  mSpinBoxWidth->setValue(0);
  mSpinBoxHeight->setValue(0);
  mLineEditSensorSize->setText("");
  mLineEditFocal->setText("");
  mSpinBoxWidth->setEnabled(false);
  mSpinBoxHeight->setEnabled(false);
  this->enableCameraEdition(false);
  mLineEditCx->setText("");
  mLineEditCy->setText("");
  mLineEditF->setText("");
  mLineEditFx->setText("");
  mLineEditFy->setText("");
  mLineEditK1->setText("");
  mLineEditK2->setText("");
  mLineEditK3->setText("");
  mLineEditK4->setText("");
  mLineEditK5->setText("");
  mLineEditK6->setText("");
  mLineEditP1->setText("");
  mLineEditP2->setText("");
}

void CamerasViewImp::update()
{
  mLineEditCx->setEnabled(mComboBoxType->currentIndex() == 0 || mComboBoxType->currentIndex() == 1 || mComboBoxType->currentIndex() == 2);
  mLineEditCy->setEnabled(mComboBoxType->currentIndex() == 0 || mComboBoxType->currentIndex() == 1 || mComboBoxType->currentIndex() == 2);
  mLineEditF->setEnabled(mComboBoxType->currentIndex() == 0 || mComboBoxType->currentIndex() == 1 || mComboBoxType->currentIndex() == 2);
  mLineEditK1->setEnabled(mComboBoxType->currentIndex() == 0 || mComboBoxType->currentIndex() == 1 || mComboBoxType->currentIndex() == 2);
  mLineEditK2->setEnabled(mComboBoxType->currentIndex() == 1 || mComboBoxType->currentIndex() == 2);
  mLineEditK3->setEnabled(mComboBoxType->currentIndex() == 2);
  mLineEditP1->setEnabled(mComboBoxType->currentIndex() == 2);
  mLineEditP2->setEnabled(mComboBoxType->currentIndex() == 2);

  if (mComboBoxType->currentIndex() <= 2) {
    TL_TODO("Añadir el resto de cámaras")
    mLabelFx->setVisible(false);
    mLabelFy->setVisible(false);
    mLineEditFx->setVisible(false);
    mLineEditFy->setVisible(false);
    mLineEditK4->setEnabled(false);
    mLineEditK5->setEnabled(false);
    mLineEditK6->setEnabled(false);
  }

}

void CamerasViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("CamerasView", "Cameras", nullptr));
  mGroupBoxCalibrationParameters->setTitle(QApplication::translate("CamerasView", "Calibration parameters", nullptr));
  mLabelF->setText(QApplication::translate("CamerasView", "f", nullptr));
  mLabelFx->setText(QApplication::translate("CamerasView", "fx", nullptr));
  mLabelFy->setText(QApplication::translate("CamerasView", "fy", nullptr));
  mLabelCx->setText(QApplication::translate("CamerasView", "cx", nullptr));
  mLabelCy->setText(QApplication::translate("CamerasView", "cy", nullptr));
  mLabelK1->setText(QApplication::translate("CamerasView", "k1", nullptr));
  mLabelK2->setText(QApplication::translate("CamerasView", "k2", nullptr));
  mLabelK3->setText(QApplication::translate("CamerasView", "k3", nullptr));
  mLabelK4->setText(QApplication::translate("CamerasView", "k4", nullptr));
  mLabelK5->setText(QApplication::translate("CamerasView", "k5", nullptr));
  mLabelK6->setText(QApplication::translate("CamerasView", "k6", nullptr));
  mLabelP1->setText(QApplication::translate("CamerasView", "p1", nullptr));
  mLabelP2->setText(QApplication::translate("CamerasView", "p2", nullptr));
  mLabelImages->setText(QApplication::translate("CamerasView", "Images:", nullptr));
  mLabelCameras->setText(QApplication::translate("CamerasView", "Cameras:", nullptr));
  mGroupBoxCamera->setTitle(QApplication::translate("CamerasView", "Camera", nullptr));
  mLabelType->setText(QApplication::translate("CamerasView", "Type", nullptr));
  mLabelWidth->setText(QApplication::translate("CamerasView", "Width (pixels)", nullptr));
  mLabelSensorSize->setText(QApplication::translate("CamerasView", "Sensor size (mm)", nullptr));
  mLabelFocal->setText(QApplication::translate("CamerasView", "Focal", nullptr));
  mComboBoxType->setItemText(0, QApplication::translate("CamerasView", "Simple Radial", nullptr));
  mComboBoxType->setItemText(1, QApplication::translate("CamerasView", "Radial", nullptr));
  mComboBoxType->setItemText(2, QApplication::translate("CamerasView", "Full Radial", nullptr));

  mLabelMake->setText(QApplication::translate("CamerasView", "Make", nullptr));
  mLabelModel->setText(QApplication::translate("CamerasView", "Model", nullptr));
  mLabelHeight->setText(QApplication::translate("CamerasView", "Height (pixels)", nullptr));

  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("CamerasView", "Cancel"));
  mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("CamerasView", "Ok"));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("CamerasView", "Help"));
}

int CamerasViewImp::activeCamera() const
{
  QList<QListWidgetItem*> itemsCameras = mListWidgetCameras->selectedItems();
  if (itemsCameras.size() == 1)
    return itemsCameras.at(0)->data(Qt::UserRole).toInt();
  else
    return -1;
}

void CamerasViewImp::setActiveCamera(int id)
{
  const QSignalBlocker blocker(mListWidgetCameras);
  for (int i = 0; i < mListWidgetCameras->count(); i++){
    if (mListWidgetCameras->item(i)->data(Qt::UserRole).toInt() == id){
      mListWidgetCameras->item(i)->setSelected(true);
      break;
    }
  }
}

void CamerasViewImp::enableCameraEdition(bool enable)
{
  mLineEditMake->setEnabled(enable);
  mLineEditModel->setEnabled(enable);
  mLineEditSensorSize->setEnabled(enable);
  mLineEditFocal->setEnabled(enable);
}

void CamerasViewImp::addCamera(int cameraId, const QString &cameraName)
{
  const QSignalBlocker blocker(mListWidgetCameras);
  QListWidgetItem *item = new QListWidgetItem(cameraName);
  item->setData(Qt::UserRole, QVariant(cameraId));
  mListWidgetCameras->addItem(item);
}

void CamerasViewImp::setMake(const QString &make)
{
  mLineEditMake->setText(make);
}

void CamerasViewImp::setModel(const QString &model)
{
  mLineEditModel->setText(model);
}

void CamerasViewImp::setWidth(int width)
{
  mSpinBoxWidth->setValue(width);
}

void CamerasViewImp::setHeight(int height)
{
  mSpinBoxHeight->setValue(height);
}

void CamerasViewImp::setSensorSize(const QString &sensorSize)
{
  mLineEditSensorSize->setText(sensorSize);
}

void CamerasViewImp::setFocal(const QString &focal)
{
  mLineEditFocal->setText(focal);
}

void CamerasViewImp::setType(const QString &type)
{
  const QSignalBlocker blockerType(mComboBoxType);

  mComboBoxType->setCurrentText(type);

  update();
}

void CamerasViewImp::setImages(const QStringList &images)
{
  mListWidgetImages->clear();
  mListWidgetImages->addItems(images);
}

void CamerasViewImp::setCalibCx(const QString &cx)
{
  mLineEditCx->setText(cx);
}

void CamerasViewImp::setCalibCy(const QString &cy)
{
  mLineEditCy->setText(cy);
}

void CamerasViewImp::setCalibF(const QString &f)
{
  mLineEditF->setText(f);
}

void CamerasViewImp::setCalibFx(const QString &fx)
{
  mLineEditFx->setText(fx);
}

void CamerasViewImp::setCalibFy(const QString &fy)
{
  mLineEditFy->setText(fy);
}

void CamerasViewImp::setCalibK1(const QString &k1)
{
  mLineEditK1->setText(k1);
}

void CamerasViewImp::setCalibK2(const QString &k2)
{
  mLineEditK2->setText(k2);
}

void CamerasViewImp::setCalibK3(const QString &k3)
{
  mLineEditK3->setText(k3);
}

void CamerasViewImp::setCalibK4(const QString &k4)
{
  mLineEditK4->setText(k4);
}

void CamerasViewImp::setCalibK5(const QString &k5)
{
  mLineEditK5->setText(k5);
}

void CamerasViewImp::setCalibK6(const QString &k6)
{
  mLineEditK6->setText(k6);
}

void CamerasViewImp::setCalibP1(const QString &p1)
{
  mLineEditP1->setText(p1);
}

void CamerasViewImp::setCalibP2(const QString &p2)
{
  mLineEditP2->setText(p2);
}

void CamerasViewImp::onSelectionChanged()
{
  QList<QListWidgetItem*> itemsCameras = mListWidgetCameras->selectedItems();
  if (itemsCameras.size() == 1) {
    emit cameraChange(itemsCameras.at(0)->data(Qt::UserRole).toInt());
  }
  /*mListWidgetImages->clear();*/
}

} // namespace ui

} // namespace inspector
