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

#include "graphos/core/Pdf.h"

#include <QTextDocument>
#include <QPrinter>
#include <QFileInfo>
#include <QDesktopServices>
#include <QPdfWriter>
#include <QPainter>

#include <math.h> 

namespace graphos
{

PdfStyle::PdfStyle()
  : mFont(QFont()),
    mBrush(QBrush()),
    mPen(QPen()),
    mOptions(1)
{
}

PdfStyle::PdfStyle(const QFont &font,
                   const QBrush &brush, 
                   const QPen &pen, 
                   int options)
  : mFont(font),
    mBrush(brush),
    mPen(pen),
    mOptions(options)
{
}

PdfStyle::PdfStyle(const PdfStyle &pdfStyle)
  : mFont(pdfStyle.mFont),
    mBrush(pdfStyle.mBrush),
    mPen(pdfStyle.mPen),
    mOptions(pdfStyle.mOptions)
{

}

const QBrush PdfStyle::brush() const
{ 
    return mBrush; 
}

const QFont PdfStyle::font() const
{
    return mFont;
}

int PdfStyle::options() const
{
    return mOptions;
}

const QPen PdfStyle::pen() const
{
    return mPen;
}

void PdfStyle::setBrush(const QBrush &brush)
{
    mBrush = brush;
}

void PdfStyle::setFont(const QFont &font)
{
    mFont = font;
}

void PdfStyle::setFont(const QString &family,
                       int pointSize, 
                       int weight,
                       bool italic)
{
    mFont = QFont(family, pointSize, weight, italic);
}

void PdfStyle::setPen(const QPen &pen)
{
    mPen = pen;
}

void PdfStyle::setOptions(int options)
{
    mOptions = options;
}




TablePdf::TablePdf()
  : mCols(0),
    mRows(0),
    mValues(0),
    mStyleTable(QFont("Times", 9, QFont::Normal), QBrush(), QPen(), Qt::AlignCenter)
{
    setBorderTable(Border::all, QPen());
    updateTable();
}

TablePdf::TablePdf(int cols, int rows)
  : mCols(cols),
    mRows(rows),
    mValues(0),
    mStyleTable(QFont("Times", 9, QFont::Normal), QBrush(), QPen(), Qt::AlignCenter)
{
    setBorderTable(Border::all, QPen());
    updateTable();
}

TablePdf::TablePdf(const TablePdf &table)
  : mCols(table.mCols),
    mRows(table.mRows),
    mValues(table.mValues),
    mStyleTable(table.mStyleTable),
    mBorders(table.mBorders)
{

}

TablePdf::~TablePdf()
{
}

QPen TablePdf::borderTable(Border border) const
{
    return mBorders.at(border);
}

const TablePdf::Cell *TablePdf::cell(int r, int c) const
{
    if (r < mRows && c < mCols) {
        if (static_cast<size_t>(c) < mValues[r].size())
            return &mValues[r][c];
    }
    return nullptr;
}

int TablePdf::cols() const
{
    return mCols;
}

int TablePdf::rows() const
{
    return mRows;
}

PdfStyle TablePdf::styleTable() const
{
    return mStyleTable;
}

QString TablePdf::value(int r, int c) const
{
    if (r < mRows && c < mCols) {
        if (static_cast<size_t>(c) < mValues[r].size())
            return mValues[r][c].text();
    }
    return nullptr;
}

void TablePdf::setBorderCell(int r,
                             int c,
                             TablePdf::Border border,
                             const QPen &pen)
{
    if (r >= 0 && r < mRows && c >= 0 && c < mCols) {
        mValues[r][c].setBorder(border, pen);
    }
}

void TablePdf::setBorderCol(int c,
                            TablePdf::Border border,
                            const QPen &pen)
{
    if (c >= 0 && c < mCols) {
        for (int r = 0; r < mRows; r++) {
            mValues[r][c].setBorder(border, pen);
        }
    }
}

void TablePdf::setBorderRange(int r1,
                              int c1,
                              int r2,
                              int c2,
                              TablePdf::Border border,
                              const QPen &pen)
{
    if (r1 >= 0 && r1 < mRows && c1 >= 0 && c1 < mCols &&
        r2 >= 0 && r2 < mRows && c2 >= 0 && c2 < mCols) {
        int r_max = std::max(r1, r2);
        int r_min = std::min(r1, r2);
        int c_max = std::max(c1, c2);
        int c_min = std::min(c1, c2);
        for (int r = r_min; r <= r_max; r++) {
            for (int c = c_min; c <= c_max; c++) {
                mValues[r][c].setBorder(border, pen);
            }
        }
    }
}

void TablePdf::setBorderRow(int r,
                            TablePdf::Border border,
                            const QPen &pen)
{
    if (r >= 0 && r < mRows) {
        for (int c = 0; c < mCols; c++) {
            mValues[r][c].setBorder(border, pen);
        }
    }
}

void TablePdf::setBorderTable(Border border,
                              const QPen &pen)
{
    switch (border) {
    case Border::none:
        break;
    case Border::left:
        mBorders[Border::left] = pen;
        break;
    case Border::right:
        mBorders[Border::right] = pen;
        break;
    case Border::top:
        mBorders[Border::top] = pen;
        break;
    case Border::botton:
        mBorders[Border::botton] = pen;
        break;
    case Border::all:
        mBorders[Border::left] = pen;
        mBorders[Border::right] = pen;
        mBorders[Border::top] = pen;
        mBorders[Border::botton] = pen;
        break;
    default:
        break;
    }

}

void TablePdf::setCols(int cols)
{
    mCols = cols;
    updateTable();
}
  
void TablePdf::setRows(int rows)
{
    mRows = rows;
    updateTable();
}

void TablePdf::setSpacingCol(int c,
                             TablePdf::Border border,
                             int spacing)
{
    if (c >= 0 && c < mCols) {
        for (int r = 0; r < mRows; r++) {
            mValues[r][c].setSpacing(border, spacing);
        }
    }
}

void TablePdf::setSpacingRow(int r,
                             TablePdf::Border border,
                             int spacing)
{
    if (r >= 0 && r < mRows) {
        for (int c = 0; c < mCols; c++) {
            mValues[r][c].setSpacing(border, spacing);
        }
    }
}

void TablePdf::setSpacingTable(TablePdf::Border border,
                               int spacing)
{
    for (int r = 0; r < mRows; r++) {
        for (int c = 0; c < mCols; c++) {
            mValues[r][c].setSpacing(border, spacing);
        }
    }
}


void TablePdf::setStyleCell(int r,
                            int c,
                            const PdfStyle &style)
{
    if (r >= 0 && r < mRows && c >= 0 && c < mCols) {
        mValues[r][c].setStyle(style);
    }
}

void TablePdf::setStyleCol(int c,
                           const PdfStyle &style)
{
    if (c >= 0 && c < mCols) {
        for (int r = 0; r < mRows; r++) {
            mValues[r][c].setStyle(style);
        }
    }
}

void TablePdf::setStyleRange(int r1,
                             int c1,
                             int r2,
                             int c2,
                             const PdfStyle &style)
{
    //TODO: Ver si utilizo esta expresiÃ³n o relleno lo que se pueda
    if (r1 >= 0 && r1 < mRows && c1 >= 0 && c1 < mCols &&
        r2 >= 0 && r2 < mRows && c2 >= 0 && c2 < mCols) {
        int r_max = std::max(r1, r2);
        int r_min = std::min(r1, r2);
        int c_max = std::max(c1, c2);
        int c_min = std::min(c1, c2);
        for (int r = r_min; r <= r_max; r++) {
            for (int c = c_min; c <= c_max; c++) {
                mValues[r][c].setStyle(style);
            }
        }
    }
}

void TablePdf::setStyleRow(int r,
                           const PdfStyle &style)
{
    if (r >= 0 && r < mRows) {
        for (int c = 0; c < mCols; c++) {
            mValues[r][c].setStyle(style);
        }
    }
}

void TablePdf::setStyleTable(const PdfStyle &style)
{
    mStyleTable = style;
}

void TablePdf::setValue(int r,
                        int c, 
                        const QString &value,
                        int colspan,
                        int rowspan)
{
    setValue(r, c, value, nullptr, colspan, rowspan);
}
  
void TablePdf::setValue(int r,
                        int c,
                        const QString &value,
                        const PdfStyle *style,
                        int colspan,
                        int rowspan)
{
    if (rowspan < 1) rowspan = 1;
    if (colspan < 1) colspan = 1;
    if (r >= 0 && r < mRows && c >= 0 && c < mCols) {
        if (mValues[r][c].isSpan() == false) {
            mValues[r][c].setText(value);
            mValues[r][c].setSpan(rowspan, colspan);
            if (style) mValues[r][c].setStyle(*style);
            if (colspan > 1 || rowspan > 1) {
                for (int _r = 0; _r < rowspan; _r++) {
                    for (int _c = 0; _c < colspan; _c++) {
                        if (_r == 0 && _c == 0) continue;
                        mValues[_r + r][_c + c].setSpanRef(mValues[r][c]);
                        if (style) mValues[_r + r][_c + c].setStyle(*style);
                    }
                }
            }
        }
    }
}

void TablePdf::updateTable()
{
    if (mValues.size() != static_cast<size_t>(mRows))
        mValues.resize(mRows);

    if (mValues[0].size() != static_cast<size_t>(mCols)) {
        for (int r = 0; r < mRows; r++) {
            mValues[r].resize(mCols, TablePdf::Cell(this));
        }
    }
}


TablePdf::Cell::Cell(TablePdf *parent)
  : mParent(parent),
    mText(""),
    mColspan(1),
    mRowspan(1)
{
    // Valores por defecto
    if (mParent) {
        mStyle = mParent->styleTable();
    } else {
        mStyle = PdfStyle();
    }
    setBorder(Border::all, QPen());
    setSpacing(Border::all, 15);
    mSpanRef = nullptr;
}

TablePdf::Cell::Cell(QString value,
                     int colspan,
                     int rowspan)
  : mParent(nullptr),
    mText(std::move(value)),
    mColspan(colspan),
    mRowspan(rowspan),
    mSpanRef(0)
{
    mStyle = PdfStyle();
    setBorder(Border::all, QPen());
    setSpacing(Border::all, 15);
}

TablePdf::Cell::Cell(const Cell &cell) 
  : mParent(cell.mParent),
    mText(cell.mText), 
    mColspan(cell.mColspan),
    mRowspan(cell.mRowspan),
    mSpanRef(cell.mSpanRef),
    mStyle(cell.mStyle),
    mBorder(cell.mBorder),
    mSpacing(cell.mSpacing)
{
}

QPen TablePdf::Cell::border(Border border) const
{
    return mBorder.at(border);
}

int TablePdf::Cell::colspan() const
{
    return mColspan;
}

int TablePdf::Cell::rowspan() const
{
    return mRowspan;
}

int TablePdf::Cell::spacing(Border border) const
{
    return mSpacing.at(border);
}

PdfStyle TablePdf::Cell::style() const
{
    return mStyle;
}

QString TablePdf::Cell::text() const
{
    return mText;
}

bool TablePdf::Cell::isSpan() const
{
    return mSpanRef;
}



void TablePdf::Cell::setBorder(Border border, const QPen &pen)
{
    switch (border) {
    case Border::none:
        break;
    case Border::left:
        mBorder[Border::left] = pen;
        break;
    case Border::right:
        mBorder[Border::right] = pen;
        break;
    case Border::top:
        mBorder[Border::top] = pen;
        break;
    case Border::botton:
        mBorder[Border::botton] = pen;
        break;
    case Border::all:
        mBorder[Border::left] = pen;
        mBorder[Border::right] = pen;
        mBorder[Border::top] = pen;
        mBorder[Border::botton] = pen;
        break;
    default:
        break;
    }
}

void TablePdf::Cell::setColspan(int colspan)
{
    mColspan = colspan;
}

void TablePdf::Cell::setRowspan(int rowspan)
{
    mRowspan = rowspan;
}

void TablePdf::Cell::setSpacing(Border border, int spacing)
{
    switch (border) {
    case Border::none:
        break;
    case Border::left:
        mSpacing[Border::left] = spacing;
        break;
    case Border::right:
        mSpacing[Border::right] = spacing;
        break;
    case Border::top:
        mSpacing[Border::top] = spacing;
        break;
    case Border::botton:
        mSpacing[Border::botton] = spacing;
        break;
    case Border::all:
        mSpacing[Border::left] = spacing;
        mSpacing[Border::right] = spacing;
        mSpacing[Border::top] = spacing;
        mSpacing[Border::botton] = spacing;
        break;
    default:
        break;
    }
}

void TablePdf::Cell::setSpan(int r, int c)
{
    mColspan = c;
    mRowspan = r;
}

void TablePdf::Cell::setSpanRef(Cell &ref)
{
    mSpanRef = &ref;
}

void TablePdf::Cell::setStyle(const PdfStyle &style)
{
    mStyle = style;
}

void TablePdf::Cell::setText(const QString &text)
{
    mText = text;
}



Pdf::Pdf(const QString &file)
  : mFile(file),
    pPdfWriter(new QPdfWriter(file)),
    pPainter(0),
    mTitleStyle(QFont("Helvetica", 16, QFont::Bold), QBrush(), QPen(), Qt::TextWordWrap | Qt::AlignJustify),
    mHeading1Style(QFont("Times", 14, QFont::Bold), QBrush(), QPen(), Qt::TextWordWrap),
    mHeading2Style(QFont("Times", 13, QFont::DemiBold), QBrush(), QPen(), Qt::TextWordWrap),
    mHeading3Style(QFont("Times", 12, QFont::DemiBold), QBrush(), QPen(), Qt::TextWordWrap),
    mTextBodyStyle(QFont("Times", 9, QFont::Normal), QBrush(), QPen(), Qt::TextWordWrap | Qt::AlignJustify),
    mSpace(100),
    mPageSize(PageSize::A4),
    mPageOrientation(PageOrientation::portrait),
    bIniDraw(false),
    bHeader(false),
    bFooter(false),
    mHeaderStyle(QFont("Times", 8, QFont::Normal), QBrush(), QPen(), Qt::AlignTop),
    mFooterStyle(QFont("Times", 8, QFont::Normal), QBrush(), QPen(), Qt::AlignBottom | Qt::AlignHCenter),
    mPageNumber(1),
    mImgCaption(0),
    mTableCaption(0)
{

}

Pdf::~Pdf()
{
    if (pPdfWriter) delete pPdfWriter;
    if (pPainter) delete pPainter;
    //if (mTextBodyFont) delete mTextBodyFont;
}

void Pdf::addHeader(const QString &text)
{
    bHeader = true;
    mHeaderText = text;
}

void Pdf::addHeader(std::function<void(QRect *)> f)
{
    bHeader = true;
    mHeaderFunction = f;
}

void Pdf::addHeader(const PdfHeader &header)
{
    bHeader = true;
    mHeader = header;
}

void Pdf::addFooter(const QString &text)
{
    bFooter = true;
    mFooterText = text;
}

void Pdf::drawTitle(const  QString &text)
{
    drawTextBlock(text, mTitleStyle);
}

void Pdf::drawHeading1(const  QString &text)
{
    drawTextBlock(text, mHeading1Style);
}

void Pdf::drawHeading2(const QString &text)
{
    drawTextBlock(text, mHeading2Style);
}

void Pdf::drawHeading3(const QString &text)
{
    drawTextBlock(text, mHeading3Style);
}

void Pdf::drawText(const QString &text)
{
    drawTextBlock(text, mDefStyle);
}

void Pdf::drawTextBody(const QString &text)
{
    drawTextBlock(text, mTextBodyStyle);
}

QRect Pdf::drawImage(const QString &img, const QString &caption, int options)
{
    return drawImage(QImage(img), caption, options);
}

QRect Pdf::drawImage(const QImage &image, const QString &caption, int options)
{
    QRect used_rect(mX, mY, mWidth, 0);

    if (!image.isNull()) {

        QImage _image = image;

        double scale = pPdfWriter->logicalDpiX() / _image.logicalDpiX();

        double scale_x = mWidth / _image.width();
        double scale_y = mHeight / _image.height();
        if (scale_x < scale_y) {
            if (_image.width() * scale > mWidth) {
                _image = _image.scaledToWidth(mWidth);
            } else {
                _image = _image.scaledToWidth(_image.width() * scale);
            }
        } else {
            if (_image.height() * scale > mHeight) {
                _image = _image.scaledToHeight(mHeight);
            } else {
                _image = _image.scaledToHeight(_image.height() * scale);
            }
        }


        int height = _image.height();
        int captionHeight = 0;
        if (caption.isEmpty() == false) {
            PdfStyle style(QFont("Helvetica", 6, QFont::StyleItalic));
            QFontMetrics fontMetrics(style.font(), pPainter->device());
            QRect checkRect = fontMetrics.boundingRect(QRect(mX, mY, mWidth, mHeight), style.options(), caption);
            captionHeight = checkRect.height();
            height += checkRect.height() * 2;
        }

        if (mY + height > mRectBody.height()) {
            QRect rect(mX, mRectBody.y(), mWidth, mRectBody.height());
            newPage();
            setPaintRect(rect);
            used_rect.setY(rect.y());
        }

        int x{};
        if (options & Qt::AlignLeft) {
            x = mX;
        } else if (options & Qt::AlignRight) {
            x = mX + (mWidth - _image.width());
        } else {
            x = mX + (mWidth - _image.width()) / 2;
        }

        if (x < 0) x = 0;

        QRect rect(x, mY, _image.width(), _image.height());
        pPainter->drawImage(rect, _image);

        mY += _image.height();
        mHeight -= _image.height();

        if (caption.isEmpty() == false) {
            QString legend("Imagen ");
            legend.append(QString::number(++mImgCaption));
            legend.append(": ");
            legend.append(caption);
            PdfStyle style(QFont("Helvetica", 6, QFont::Normal, true));
            pPainter->setFont(style.font());
            QRect usedRect;
            pPainter->drawText(QRect(x, mY + captionHeight / 2, mWidth, mHeight), style.options(), legend, &usedRect);
            pPainter->setFont(mCurrentStyle.font());
        }

        mY += captionHeight + mSpace;
        mHeight -= (captionHeight + mSpace);

        used_rect.setHeight(mY - used_rect.y());
    }

    return used_rect;
}

void Pdf::drawList(const QString &text)
{
    if (pPainter) {
        QRect usedRect;

        QFontMetrics fontMetrics(mCurrentStyle.font(), pPainter->device());
        QRect checkRect = fontMetrics.boundingRect(QRect(mX, mY, mWidth, mHeight), mCurrentStyle.options(), text);

        if (checkRect.height() > mHeight)
            newPage();

        QPen linepen(Qt::black);
        linepen.setCapStyle(Qt::RoundCap);
        linepen.setWidth(20);
        pPainter->setPen(linepen);

        fontMetrics = QFontMetrics(mCurrentStyle.font());
        int height = fontMetrics.height();

        pPainter->drawPoint(QPoint(mX + 100, mY + height / 2));

        pPainter->drawText(QRect(mX + 200, mY, mWidth, mHeight), Qt::TextWordWrap | Qt::AlignJustify, text, &usedRect);
        mY += usedRect.height() + mSpace;
        mHeight -= (usedRect.height() + mSpace);
    }
}

QRect Pdf::drawTable(const TablePdf &table,
                     const QString &caption)
{
    QRect rect(mX, mY, mWidth, 0);

    if (pPainter) {
        //TODO: calcular tamaño y ver si hay que cambiar de pÃ¡gina
        int rows = table.rows();
        int cols = table.cols();
        std::vector<int> maxHeight(rows, -std::numeric_limits<int>().max());
        std::vector<int> maxWidth(cols, -std::numeric_limits<int>().max());

        for (int r = 0; r < rows; r++) {

            for (int c = 0; c < cols; c++) {
                const TablePdf::Cell *cell = table.cell(r, c);
                PdfStyle style = cell->style();
                QFontMetrics fontMetrics(style.font(), pPainter->device());

                QString value = cell->text();
                if (value.isEmpty() == false) {
                    QRect checkRect = fontMetrics.boundingRect(QRect(mX, mY, mWidth, mHeight), style.options(), value);

                    if (cell->colspan() <= 1) {
                        int space_w = cell->spacing(TablePdf::Border::left) + cell->spacing(TablePdf::Border::right);
                        maxWidth[c] = std::max(static_cast<int>(round(((checkRect.width() + space_w) * 1.25))), maxWidth[c]);
                    }

                    if (cell->rowspan() <= 1) {
                        int space_h = cell->spacing(TablePdf::Border::top) + cell->spacing(TablePdf::Border::botton);
                        maxHeight[r] = std::max(static_cast<int>(round(((checkRect.height() + space_h) * 1.25))), maxHeight[r]);
                    }
                }
            }

            // Se vuelve a recorrer para ajustar con las celdas unidas
            for (int c = 0; c < cols; c++) {
                const TablePdf::Cell *cell = table.cell(r, c);
                PdfStyle style = cell->style();
                QFontMetrics fontMetrics(style.font(), pPainter->device());
                if (cell->colspan() > 1) {
                    QString value = cell->text();
                    if (value.isEmpty() == false) {
                        QRect checkRect = fontMetrics.boundingRect(QRect(mX, mY, mWidth, mHeight), style.options(), value);
                        int w = 0;
                        for (int i = 0; i < cell->colspan(); i++) {
                            w += maxWidth[c + i];
                        }
                        if (checkRect.width() * 1.25 > w) {
                            double scale = checkRect.width() * 1.25 / w;
                            for (int i = 0; i < cell->colspan(); i++) {
                                maxWidth[c + i] *= scale;
                            }
                        }
                    }
                }
            }

        }

        int total_width = 0;
        for (int c = 0; c < cols; c++) {
            total_width += maxWidth[c];
        }

        if (total_width > mWidth) {
            // TODO: En este caso hay que ajustar el ancho de columnas de otra forma o 
        }

        int total_height = 0;
        for (int r = 0; r < rows; r++) {
            total_height += maxHeight[r];
        }

        int captionHeight = 0;
        if (caption.isEmpty() == false) {
            PdfStyle style(QFont("Helvetica", 6, QFont::StyleItalic));
            QFontMetrics fontMetrics(style.font(), pPainter->device());
            QRect checkRect = fontMetrics.boundingRect(QRect(mX, mY, mWidth, mHeight), style.options(), caption);
            captionHeight = checkRect.height();
            total_height += checkRect.height() * 2;
        }

        if (total_height > mHeight) {
            QRect rect(mX, mRectBody.y(), mWidth, mRectBody.height());
            newPage();
            setPaintRect(rect);
        }

        //int height = table_height / rows;
        int x = mX + (mWidth - total_width) / 2;

        QRect rectCell;
        QRect rectText;
        for (int r = 0; r < rows; r++) {
            mX = x;
            for (int c = 0; c < cols; c++) {
                const TablePdf::Cell *cell = table.cell(r, c);
                if (cell->isSpan() == false) {
                    int _height = cell->rowspan() <= 1 ? maxHeight[r] : maxHeight[r] * cell->rowspan();
                    int width = maxWidth[c];
                    if (cell->colspan() > 1) {
                        for (int i = c + 1; i < c + cell->colspan(); i++)
                            width += maxWidth[i];
                    }
                    rectCell = QRect(mX, mY, width, _height);
                    rectText = QRect(mX + cell->spacing(TablePdf::Border::left), mY + cell->spacing(TablePdf::Border::top),
                                     width - (cell->spacing(TablePdf::Border::left) + cell->spacing(TablePdf::Border::right)),
                                     _height - (cell->spacing(TablePdf::Border::top) + cell->spacing(TablePdf::Border::botton)));
                    QString value = table.value(r, c);

                    //....
                    // Estilos de la tabla

                    PdfStyle style_text = cell->style();
                    pPainter->setBrush(style_text.brush());
                    pPainter->setPen(QPen(Qt::NoPen));
                    pPainter->drawRect(rectCell);
                    pPainter->setPen(style_text.pen());
                    pPainter->setFont(style_text.font());
                    pPainter->drawText(rectText, style_text.options(), value);

                    //.....
                    // TODO: En lugar de drawRect hay que utilizar lineas y color de fondo
                    //pPainter->setPen( QPen(Qt::black, 5));
                    //pPainter->drawRect(rect);

                    std::map<TablePdf::Border, QPen>::const_iterator it;

                    pPainter->setPen(cell->border(TablePdf::Border::top));
                    pPainter->drawLine(rectCell.topLeft(), rectCell.topRight());

                    pPainter->setPen(cell->border(TablePdf::Border::botton));
                    pPainter->drawLine(rectCell.bottomLeft(), rectCell.bottomRight());

                    pPainter->setPen(cell->border(TablePdf::Border::left));
                    pPainter->drawLine(rectCell.topLeft(), rectCell.bottomLeft());

                    pPainter->setPen(cell->border(TablePdf::Border::right));
                    pPainter->drawLine(rectCell.topRight(), rectCell.bottomRight());

                    /*it = cell->border.find(TablePdf::Border::TOP);
                    if (it != cell->border.end()) {
                      pPainter->setPen(it->second);
                      pPainter->drawLine(rect.topLeft(), rect.topRight());
                    }

                    it = cell->border.find(TablePdf::Border::BOTTON);
                    if (it != cell->border.end()) {
                      pPainter->setPen(it->second);
                      pPainter->drawLine(rect.bottomLeft(), rect.bottomRight());
                    }

                    it = cell->border.find(TablePdf::Border::LEFT);
                    if (it != cell->border.end()) {
                      pPainter->setPen(it->second);
                      pPainter->drawLine(rect.topLeft(), rect.bottomLeft());
                    }

                    it = cell->border.find(TablePdf::Border::RIGHT);
                    if (it != cell->border.end()) {
                      pPainter->setPen(it->second);
                      pPainter->drawLine(rect.topRight(), rect.bottomRight());
                    }*/

                    //TODO: recuperar todas las opciones
                    pPainter->setFont(mCurrentStyle.font());
                    pPainter->setBrush(mCurrentStyle.brush());
                    pPainter->setPen(mCurrentStyle.pen());
                }

                mX += maxWidth[c];//rect.width();
            }
            mY += maxHeight[r];//rect.height();
        }

        mHeight -= total_height;


        if (caption.isEmpty() == false) {
            QString legend("Tabla ");
            legend.append(QString::number(++mTableCaption));
            legend.append(": ");
            legend.append(caption);
            PdfStyle style(QFont("Helvetica", 6, QFont::Normal, true));
            pPainter->setFont(style.font());
            QRect usedRect;
            pPainter->drawText(QRect(x, mY + captionHeight / 2, mWidth, mHeight), style.options(), legend, &usedRect);
            pPainter->setFont(mCurrentStyle.font());

            mY += usedRect.height();
            mHeight -= usedRect.height();
        }

        newLine();

        rect.setHeight(mY - rect.y());

    }

    return rect;
}

void Pdf::drawLine(const QPoint &pt1, const QPoint &pt2, const PdfStyle &style)
{
    pPainter->setBrush(style.brush());
    pPainter->setPen(style.pen());

    pPainter->drawLine(pt1, pt2);
}

void Pdf::endDraw()
{
    if (pPainter) {
        pPainter->end();
    }
}



void Pdf::initDraw()
{
    if (pPainter == NULL) {
        pPdfWriter->setResolution(600);
        pPainter = new QPainter(pPdfWriter);
        pPainter->setRenderHint(QPainter::Antialiasing);

        mDefStyle.setFont(pPainter->font());
        mDefStyle.setOptions(Qt::TextWordWrap);

        QRect rect = pPainter->viewport();

        mRectBody = rect;

        // Primera página
        setPageNumber(1);

        // Se reserva espacio para la cabecera
        if (bHeader) {
            //QFontMetrics fontMetrics(mHeaderStyle.font(), pPainter->device());
            //QRect aux_rect = fontMetrics.boundingRect(rect, mHeaderStyle.options(), mHeaderText);
            mRectHeader.setX(rect.x());
            mRectHeader.setY(rect.y());
            mRectHeader.setWidth(rect.width());
            mRectHeader.setHeight(300 /*aux_rect.height() * 3*/);

            //drawHeader();

            mRectBody.setY(mRectHeader.height() + 200);
        }

        // Se reserva espacio para el pie de página

        if (bFooter) {

            int height = 0;
            if (mHeaderText.isEmpty()) {

                QRect text_rect;
                if (mHeader.hasText()) {
                    text_rect = mHeader.textRect();
                }

                QRect image_rect;
                if (mHeader.hasImage()) {
                    image_rect = mHeader.imageRect();
                }

                height = std::max(text_rect.height(), image_rect.height());


            } else {
                QFontMetrics fontMetrics(mFooterStyle.font(), pPainter->device());
                QRect aux_rect = fontMetrics.boundingRect(rect, mFooterStyle.options(), mFooterText);
                height = aux_rect.height() * 3;
            }

            mRectFooter.setX(rect.x());
            mRectFooter.setY(rect.height() - height);
            mRectFooter.setWidth(rect.width());
            mRectFooter.setHeight(height);

            mRectBody.setHeight(mRectBody.height() - mRectFooter.height());

            //drawFooter();
        }

        mX = mRectBody.x();
        mY = mRectBody.y();
        mWidth = mRectBody.width();
        mHeight = mRectBody.height();

        bIniDraw = true;

        setFontDefault();
    }
}

void Pdf::newLine()
{
    if (pPainter) {
        mX = 0;
        mY += mSpace;
        mHeight -= mSpace;
        if (mHeight < 0) {
            pPdfWriter->newPage();
            mY = 0;
        }
    }
}

void Pdf::newPage()
{
    if (pPainter) {
        pPdfWriter->newPage();
        setPageNumber(++mPageNumber);
        //TODO: Controlar aqui el encabezado y pie de pÃ¡gina
        if (bHeader) drawHeader();
        if (bFooter) drawFooter();
        mX = mRectBody.x();
        mY = mRectBody.y();
        mWidth = mRectBody.width();
        mHeight = mRectBody.height();
    }
}

void Pdf::setCreator(const QString &creator)
{
    pPdfWriter->setCreator(creator);
    mTagValues["Creator"] = creator.toStdString();
}

void Pdf::setFont(const QString &family,
                  int pointSize,
                  int weight,
                  bool italic)
{
    mCurrentStyle.setFont(family, pointSize, weight, italic);
    if (pPainter) {
        pPainter->setFont(mCurrentStyle.font());
    }
}

void Pdf::setFontDefault()
{
    mCurrentStyle.setFont(mDefStyle.font());
    if (pPainter) {
        pPainter->setFont(mCurrentStyle.font());
    }
}

void Pdf::setMargins(int left,
                     int right,
                     int top,
                     int bottom)
{
    if (!bIniDraw) {
        //QPagedPaintDevice::Margins margins;
        //margins.left = left;
        //margins.right = right;
        //margins.top = top;
        //margins.bottom = bottom;
        //pPdfWriter->setMargins(margins);
        pPdfWriter->setPageMargins(QMarginsF(left, top, right, bottom), QPageLayout::Millimeter);
    }

}

void Pdf::setPageNumber(int n)
{
    mTagValues["PageNumber"] = std::to_string(n);
}

void Pdf::setPageSize(Pdf::PageSize pageSize)
{
    if (!bIniDraw) {
        mPageSize = pageSize;

        //QPdfWriter::PageSize qPageSize;
        QPageSize::PageSizeId page_size_id = QPageSize::PageSizeId::A4;

        switch (pageSize) {
        case Pdf::PageSize::A0:
            //qPageSize = QPdfWriter::PageSize::A0;
            page_size_id = QPageSize::PageSizeId::A0;
            break;
        case Pdf::PageSize::A1:
            //qPageSize = QPdfWriter::PageSize::A1;
            page_size_id = QPageSize::PageSizeId::A1;
            break;
        case Pdf::PageSize::A2:
            //qPageSize = QPdfWriter::PageSize::A2;
            page_size_id = QPageSize::PageSizeId::A2;
            break;
        case Pdf::PageSize::A3:
            //qPageSize = QPdfWriter::PageSize::A3;
            page_size_id = QPageSize::PageSizeId::A3;
            break;
        case Pdf::PageSize::A4:
            //qPageSize = QPdfWriter::PageSize::A4;
            page_size_id = QPageSize::PageSizeId::A4;
            break;
        case Pdf::PageSize::A5:
            //qPageSize = QPdfWriter::PageSize::A5;
            page_size_id = QPageSize::PageSizeId::A5;
            break;
        case Pdf::PageSize::A6:
            //qPageSize = QPdfWriter::PageSize::A6;
            page_size_id = QPageSize::PageSizeId::A6;
            break;
        case Pdf::PageSize::A7:
            //qPageSize = QPdfWriter::PageSize::A7;
            page_size_id = QPageSize::PageSizeId::A7;
            break;
        case Pdf::PageSize::A8:
            //qPageSize = QPdfWriter::PageSize::A8;
            page_size_id = QPageSize::PageSizeId::A8;
            break;
        case Pdf::PageSize::A9:
            //qPageSize = QPdfWriter::PageSize::A9;
            page_size_id = QPageSize::PageSizeId::A9;
            break;
        case Pdf::PageSize::A10:
            //qPageSize = QPdfWriter::PageSize::A10;
            page_size_id = QPageSize::PageSizeId::A10;
            break;
        case Pdf::PageSize::B0:
            //qPageSize = QPdfWriter::PageSize::B0;
            page_size_id = QPageSize::PageSizeId::B0;
            break;
        case Pdf::PageSize::B1:
            //qPageSize = QPdfWriter::PageSize::B1;
            page_size_id = QPageSize::PageSizeId::B1;
            break;
        case Pdf::PageSize::B2:
            //qPageSize = QPdfWriter::PageSize::B2;
            page_size_id = QPageSize::PageSizeId::B2;
            break;
        case Pdf::PageSize::B3:
            //qPageSize = QPdfWriter::PageSize::B3;
            page_size_id = QPageSize::PageSizeId::B3;
            break;
        case Pdf::PageSize::B4:
            //qPageSize = QPdfWriter::PageSize::B4;
            page_size_id = QPageSize::PageSizeId::B4;
            break;
        case Pdf::PageSize::B5:
            //qPageSize = QPdfWriter::PageSize::B5;
            page_size_id = QPageSize::PageSizeId::B5;
            break;
        case Pdf::PageSize::B6:
            //qPageSize = QPdfWriter::PageSize::B6;
            page_size_id = QPageSize::PageSizeId::B6;
            break;
        case Pdf::PageSize::B7:
            //qPageSize = QPdfWriter::PageSize::B7;
            page_size_id = QPageSize::PageSizeId::B7;
            break;
        case Pdf::PageSize::B8:
            //qPageSize = QPdfWriter::PageSize::B8;
            page_size_id = QPageSize::PageSizeId::B8;
            break;
        case Pdf::PageSize::B9:
            //qPageSize = QPdfWriter::PageSize::B9;
            page_size_id = QPageSize::PageSizeId::B9;
            break;
        case Pdf::PageSize::B10:
            //qPageSize = QPdfWriter::PageSize::B10;
            page_size_id = QPageSize::PageSizeId::B10;
            break;
            //case Pdf::PageSize::C0:
            //  break;
            //case Pdf::PageSize::C1:
            //  break;
            //case Pdf::PageSize::C2:
            //  break;
            //case Pdf::PageSize::C3:
            //  break;
            //case Pdf::PageSize::C4:
            //  break;
            //case Pdf::PageSize::C5:
            //  break;
            //case Pdf::PageSize::C6:
            //  break;
            //case Pdf::PageSize::C7:
            //  break;
            //case Pdf::PageSize::C8:
            //  break;
            //case Pdf::PageSize::C9:
            //  break;
            //case Pdf::PageSize::C10:
            //  break;
        case Pdf::PageSize::letter:
            //qPageSize = QPdfWriter::PageSize::Letter;
            page_size_id = QPageSize::PageSizeId::Letter;
            break;
        case Pdf::PageSize::legal:
            //qPageSize = QPdfWriter::PageSize::Legal;
            page_size_id = QPageSize::PageSizeId::Legal;
            break;
        case Pdf::PageSize::executive:
            //qPageSize = QPdfWriter::PageSize::Executive;
            page_size_id = QPageSize::PageSizeId::Executive;
            break;
        default:
            //qPageSize = QPdfWriter::PageSize::A4;
            page_size_id = QPageSize::PageSizeId::A4;
            break;
        }

        //pPdfWriter->setPageSize(qPageSize);
        pPdfWriter->setPageSize(QPageSize());
        update();
    }
}

void Pdf::setPageOrientation(PageOrientation pageOrientation)
{
    if (!bIniDraw) {

        mPageOrientation = pageOrientation;

        QPageLayout::Orientation orientation;

        switch (pageOrientation) {
        case graphos::Pdf::PageOrientation::portrait:
            orientation = QPageLayout::Orientation::Portrait;
            break;
        case graphos::Pdf::PageOrientation::landscape:
            orientation = QPageLayout::Orientation::Landscape;
            break;
        default:
            orientation = QPageLayout::Orientation::Portrait;
            break;
        }

        pPdfWriter->setPageOrientation(orientation);
        update();
    }
}

void Pdf::setTitle(const QString &title)
{
    pPdfWriter->setTitle(title);
    mTagValues["Title"] = title.toStdString();
}

void Pdf::show()
{
    QFileInfo fileInfo(mFile);
    if (fileInfo.exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(mFile));
    }
}

void Pdf::update()
{

}

void Pdf::drawTextBlock(const QString &text,
                        const PdfStyle &style)
{
    if (pPainter) {
        pPainter->setFont(style.font());

        QFontMetrics fontMetrics(style.font(), pPainter->device());
        QRect rect = fontMetrics.boundingRect(QRect(mX, mY, mWidth, mHeight), style.options(), text);

        if (rect.height() > mHeight)
            newPage();

        rect = QRect(mX, mY, mWidth, rect.height());

        pPainter->setBrush(style.brush());
        pPainter->setPen(QPen(Qt::NoPen));
        pPainter->drawRect(rect);

        pPainter->setPen(style.pen());
        pPainter->drawText(QRect(mX, mY, mWidth, mHeight), style.options(), text, &rect);
        //QFontMetrics fontMetrics(style.font());
        //int w = fontMetrics.width(text);

        mY += rect.height() + mSpace;
        mHeight -= (rect.height() + mSpace);

        pPainter->setFont(mCurrentStyle.font());
    }
}

void Pdf::drawHeader()
{
    if (pPainter) {
        /*if (mHeaderText.isEmpty()) {

          setPaintRect(mHeader.imageRect());
          drawImage(mHeader.image());
          PdfStyle style = mHeader.style();
          pPainter->setFont(style.font());
          pPainter->setPen(QPen(Qt::darkGray));
          pPainter->drawText(mHeader.textRect(), style.options(), mHeader.text());
          pPainter->setFont(mCurrentStyle.font());
          pPainter->setPen(QPen(Qt::black));

        } */
        if (mHeaderText.isEmpty()) {

            mHeaderFunction(&mRectHeader);

        } else {

            pPainter->setFont(mHeaderStyle.font());
            pPainter->setPen(QPen(Qt::darkGray));
            QString header_text = replace(mHeaderText);
            pPainter->drawText(mRectHeader, mHeaderStyle.options(), header_text);
            pPainter->setFont(mCurrentStyle.font());
            pPainter->setPen(QPen(Qt::black));

        }
    }
}

void Pdf::drawFooter()
{
    if (pPainter) {
        pPainter->setFont(mFooterStyle.font());
        pPainter->setPen(QPen(Qt::darkGray));
        QString footer_text = replace(mFooterText);
        pPainter->drawText(mRectFooter, mFooterStyle.options(), footer_text);
        pPainter->setFont(mCurrentStyle.font());
        pPainter->setPen(QPen(Qt::black));
    }
}

QString Pdf::replace(const QString &input) const
{
    std::string output = input.toStdString();
    std::size_t ini = output.find("#");
    std::size_t end;
    while (ini != std::string::npos) {
        end = output.find("#", ini + 1);
        size_t size = end - ini + 1;
        std::string tagString = output.substr(ini + 1, end - (ini + 1));
        std::string replaceText = "";
        std::map<std::string, std::string>::const_iterator it = mTagValues.find(tagString);
        if (it != mTagValues.end())
            replaceText.append(it->second);

        output.replace(ini, size, replaceText);
        ini = output.find("#", replaceText.size());
    }
    return QString::fromStdString(output);
}



PdfStyle Pdf::currentStyle() const
{
    return mCurrentStyle;
}

PdfStyle Pdf::titleStyle() const
{
    return mTitleStyle;
}

void Pdf::setTitleStyle(const PdfStyle &titleStyle)
{
    mTitleStyle = titleStyle;
}

PdfStyle Pdf::heading1Style() const
{
    return mHeading1Style;
}

void Pdf::setHeading1Style(const PdfStyle &heading1Style)
{
    mHeading1Style = heading1Style;
}

PdfStyle Pdf::heading2Style() const
{
    return mHeading2Style;
}

void Pdf::setHeading2Style(const PdfStyle &heading2Style)
{
    mHeading2Style = heading2Style;
}

PdfStyle Pdf::heading3Style() const
{
    return mHeading3Style;
}

void Pdf::setHeading3Style(const PdfStyle &heading3Style)
{
    mHeading3Style = heading3Style;
}

PdfStyle Pdf::textBodyStyle() const
{
    return mTextBodyStyle;
}

void Pdf::setTextBodyStyle(const PdfStyle &textBodyStyle)
{
    mTextBodyStyle = textBodyStyle;
}

QRect Pdf::rectBody() const
{
    return mRectBody;
}

QPoint Pdf::currentPosition() const
{
    return QPoint(mX, mY);
}

void Pdf::setPaintRect(const QRect &rect)
{
    /// TODO: setPaintRect debería ser privada
    mX = rect.x();
    mY = rect.y();
    mWidth = rect.width();
    mHeight = rect.height();
}


} // namespace graphos
