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

#include "TiffFormatWidget.h"

#include <QApplication>
#include <QGridLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>

namespace graphos
{

TiffFormatWidget::TiffFormatWidget(QWidget *parent)
  : GraphosWidgetView(parent)
{
    TiffFormatWidget::initUI();
    TiffFormatWidget::initSignalAndSlots();
}

TiffFormatWidget::~TiffFormatWidget()
{
}

void TiffFormatWidget::initUI()
{
    setObjectName(QString("TiffFormatWidget"));
    resize(380, 250);

    QGridLayout *gridLayout = new QGridLayout();
    setLayout(gridLayout);

    mLabelBigtiff = new QLabel(this);
    gridLayout->addWidget(mLabelBigtiff, 0, 0);
    mBigtiff = new QComboBox(this);
    gridLayout->addWidget(mBigtiff, 0, 1);
    mBigtiff->setToolTip(QString::fromUtf8("Specifies whether to use BigTIFF based on the file size or other criteria"));

    mLabelCompress = new QLabel(this);
    gridLayout->addWidget(mLabelCompress, 1, 0);
    mCompress = new QComboBox(this);
    gridLayout->addWidget(mCompress, 1, 1);
    mCompress->setToolTip(QString::fromUtf8("Specifies the compression method to be used"));

    mTiled = new QCheckBox(this);
    gridLayout->addWidget(mTiled, 2, 0, 1, 2);
    mLabelBlockxsize = new QLabel(this);
    gridLayout->addWidget(mLabelBlockxsize, 3, 0);
    mBlockxsize = new QSpinBox(this);
    gridLayout->addWidget(mBlockxsize, 3, 1);
    mBlockxsize->setToolTip(QString::fromUtf8("Tile width in pixels"));

    mLabelBlockysize = new QLabel(this);
    gridLayout->addWidget(mLabelBlockysize, 4, 0);
    mBlockysize = new QSpinBox(this);
    gridLayout->addWidget(mBlockysize, 4, 1);
    mBlockysize->setToolTip(QString::fromUtf8("Tile height in pixels"));

    mLabelJpegquality = new QLabel(this);
    gridLayout->addWidget(mLabelJpegquality, 5, 0);
    mJpegquality = new QSpinBox(this);
    gridLayout->addWidget(mJpegquality, 5, 1);
    mJpegquality->setToolTip(QString::fromUtf8("JPEG quality value"));

    mBigtiff->addItem("yes");
    mBigtiff->addItem("no");
    mBigtiff->addItem("if_needed");
    mBigtiff->addItem("if_safer");
    mBigtiff->setCurrentText("if_needed");
    mCompress->addItem("jpeg");
    mCompress->addItem("lzw");
    mCompress->addItem("packbits");
    mCompress->addItem("deflate");
    mCompress->addItem("ccittrle");
    mCompress->addItem("ccittfax3");
    mCompress->addItem("ccittfax4");
    mCompress->addItem("lzma");
    mCompress->addItem("zstd");
    mCompress->addItem("lerc");
    mCompress->addItem("lerc_deflate");
    mCompress->addItem("lerc_zstd");
    mCompress->addItem("webp");
    mCompress->addItem("none");
    mCompress->setCurrentText("if_needed");
    mBlockxsize->setMinimum(16);
    mBlockxsize->setMaximum(1024);
    mBlockxsize->setValue(256);
    mBlockysize->setMinimum(16);
    mBlockysize->setMaximum(1024);
    mBlockysize->setValue(15);
    mJpegquality->setMinimum(1);
    mJpegquality->setMaximum(100);
    mJpegquality->setValue(75);

    retranslate();
    clear();
    update();
}

void TiffFormatWidget::initSignalAndSlots()
{
    connect(mBigtiff, &QComboBox::currentTextChanged, this, &TiffFormatWidget::update);
    connect(mCompress, &QComboBox::currentTextChanged, this, &TiffFormatWidget::update);
    connect(mBlockxsize, QOverload<int>::of(&QSpinBox::valueChanged), this, &TiffFormatWidget::update);
    connect(mBlockysize, QOverload<int>::of(&QSpinBox::valueChanged), this, &TiffFormatWidget::update);
    connect(mJpegquality, QOverload<int>::of(&QSpinBox::valueChanged), this, &TiffFormatWidget::update);
}

void TiffFormatWidget::retranslate()
{
    mLabelBigtiff->setText(QApplication::translate("TiffFormatWidget", "Big Tiff", nullptr));
    mLabelCompress->setText(QApplication::translate("TiffFormatWidget", "Compress", nullptr));
    mTiled->setText(QApplication::translate("TiffFormatWidget", "Tiled", nullptr));
    mLabelBlockxsize->setText(QApplication::translate("TiffFormatWidget", "Block X Size", nullptr));
    mLabelBlockysize->setText(QApplication::translate("TiffFormatWidget", "Block Y Size", nullptr));
    mLabelJpegquality->setText(QApplication::translate("TiffFormatWidget", "Jpeg Quality", nullptr));
}

void TiffFormatWidget::clear()
{
    mBigtiff->setCurrentText("if_needed");
    mCompress->setCurrentText("if_needed");
    mTiled->setChecked(true);
    mBlockxsize->setValue(256);
    mBlockysize->setValue(256);
    mJpegquality->setValue(75);	
}

void TiffFormatWidget::update()
{
}

QString TiffFormatWidget::bigTiff() const
{
    return mBigtiff->currentText();
}

QString TiffFormatWidget::compress() const
{
    return mCompress->currentText();
}

bool TiffFormatWidget::tiled() const
{
    return mTiled->isChecked();
}

int TiffFormatWidget::blockXSize() const
{
    return mBlockxsize->value();
}

int TiffFormatWidget::blockYSize() const
{
    return mBlockysize->value();
}

int TiffFormatWidget::jpegQuality() const
{
    return mJpegquality->value();
}

void TiffFormatWidget::setBigtiff(const QString& value)
{
    mBigtiff->setCurrentText(value);
}

void TiffFormatWidget::setCompress(const QString& value)
{
    mCompress->setCurrentText(value);
}

void TiffFormatWidget::setTiled(bool value)
{
    mTiled->setChecked(value);
}

void TiffFormatWidget::setBlockxsize(int value)
{
    mBlockxsize->setValue(value);
}

void TiffFormatWidget::setBlockysize(int value)
{
    mBlockysize->setValue(value);
}

void TiffFormatWidget::setJpegquality(int value)
{
    mJpegquality->setValue(value);
}

} // namespace graphos
