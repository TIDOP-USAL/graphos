/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
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

#ifndef GRAPHOS_FEATURE_EXTRACTOR_VIEW_H
#define GRAPHOS_FEATURE_EXTRACTOR_VIEW_H

#include "graphos/components/featextract/FeatureExtractorView.h"

class QGridLayout;
class QComboBox;
class QDialogButtonBox;
class QSpinBox;
class QCheckBox;
class QLabel;

namespace graphos
{

class FeatureExtractorViewImp
  : public FeatureExtractorView
{

  Q_OBJECT

public:

  FeatureExtractorViewImp(QWidget *parent = nullptr);
  ~FeatureExtractorViewImp() override;

protected slots:

    void onCheckBoxFullImageChange();

// FeatureExtractorView interface

public:

  void addDetectorDescriptor(QWidget *detectorDescriptor) override;
  QString currentDetectorDescriptor() const override;
  int maxImageSize() const override;
  bool fullImageSize() const override;

public slots:

  void setCurrentDetectorDescriptor(const QString &detectorDescriptor) override;
  void setMaxImageSize(int imageSize) override;
  void setFullImageSize(bool fullImageSize) override;

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QGridLayout *mGridLayoutDetectorDescriptor;
  QCheckBox *mCheckBoxFullImage;
  QLabel *mLabelMaxImageSize;
  QSpinBox *mSpinBoxMaxImageSize;
  QLabel *mLabelDetectorDescriptor;
  QComboBox *mComboBoxDetectorDescriptor;
  QDialogButtonBox *mButtonBox;
};

} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_VIEW_H
