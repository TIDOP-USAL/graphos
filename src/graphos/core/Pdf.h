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

#ifndef GRAPHOS_UTILS_PDF_H
#define GRAPHOS_UTILS_PDF_H

#include <functional>

#include <QFont>
#include <QPen>

class QPdfWriter;
class QPainter;

namespace graphos
{


//TODO: esta clase debería ser mucho mas completa.
// ¿Utilizar GraphicStyle?

/*!
 * \brief Class representing the style of a PDF element.
 */
class PdfStyle
{

public:

    /*!
     * \brief Default constructor for the PdfStyle class.
     */
    PdfStyle();

    /*!
     * \brief Constructor for the PdfStyle class with specified font, brush, pen, and options.
     * \param[in] font QFont object representing the font.
     * \param[in] brush QBrush object representing the brush.
     * \param[in] pen QPen object representing the pen.
     * \param[in] options Integer representing additional options.
     */
    PdfStyle(const QFont &font,
             const QBrush &brush = QBrush(),
             const QPen &pen = QPen(),
             int options = 0);

    /*!
     * \brief Copy constructor for the PdfStyle class.
     * \param[in] pdfStyle Another PdfStyle object to copy.
     */
    PdfStyle(const PdfStyle &pdfStyle);

    /*!
     * \brief Default destructor for the PdfStyle class.
     */
    ~PdfStyle() = default;

    /*!
     * \brief Copy assignment operator for the PdfStyle class.
     * \param[in] pdfStyle Another PdfStyle object to copy.
     * \return Reference to the PdfStyle object after assignment.
     */
    auto operator =(const PdfStyle &pdfStyle) -> PdfStyle& = default;

    /*!
     * \brief Get the brush of the PdfStyle.
     * \return QBrush object representing the brush.
     */
    auto brush() const -> QBrush;

    /*!
     * \brief Get the font of the PdfStyle.
     * \return QFont object representing the font.
     */
    auto font() const -> QFont;

    /*!
     * \brief Get the options of the PdfStyle.
     * \return Integer representing additional options.
     */
    auto options() const -> int;

    /*!
     * \brief Get the pen of the PdfStyle.
     * \return QPen object representing the pen.
     */
    auto pen() const -> QPen;

    /*!
     * \brief Set the brush of the PdfStyle.
     * \param[in] brush QBrush object representing the brush to set.
     */
    void setBrush(const QBrush &brush);

    /*!
     * \brief Set the font of the PdfStyle.
     * \param[in] font QFont object representing the font to set.
     */
    void setFont(const QFont &font);

    /*!
     * \brief Set the font of the PdfStyle using specified parameters.
     * \param[in] family Font family.
     * \param[in] pointSize Point size.
     * \param[in] weight Font weight.
     * \param[in] italic True if italic, false otherwise.
     */
    void setFont(const QString &family,
                 int pointSize,
                 int weight,
                 bool italic);

    /*!
     * \brief Set the options of the PdfStyle.
     * \param[in] options Integer representing the additional options to set.
     */
    void setOptions(int options);

    /*!
     * \brief Set the pen of the PdfStyle.
     * \param[in] pen QPen object representing the pen to set.
     */
    void setPen(const QPen &pen);

protected:

    /*!
     * \brief Font of the PdfStyle.
     */
    QFont mFont;

    /*!
     * \brief Brush of the PdfStyle.
     */
    QBrush mBrush;

    /*!
     * \brief Pen of the PdfStyle.
     */
    QPen mPen;

    /*!
     * \brief Additional options of the PdfStyle.
     */
    int mOptions;

};



/*!
 * \brief Class representing a PDF table.
 */
class TablePdf
{

public:

    /*!
     * \brief Enumeration defining different border types for the table.
     */
    enum class Border
    {
        none = 1 << 0,                    /*!< No border. */
        left = 1 << 1,                    /*!< Left border. */
        right = 1 << 2,                   /*!< Right border. */
        top = 1 << 3,                     /*!< Top border. */
        bottom = 1 << 4,                  /*!< Bottom border. */
        all = left | right | top | bottom /*!< All borders. */
    };

    /*!
     * \brief Class representing a cell in a PDF table.
     */
    class Cell
    {

    public:

        /*!
         * \brief Default constructor for the Cell class.
         * \param[in] parent Pointer to the parent TablePdf object.
         */
        Cell(TablePdf *parent = nullptr);

        /*!
         * \brief Constructor for the Cell class with specified value, colspan, and rowspan.
         * \param[in] value Text value of the cell.
         * \param[in] colspan Number of columns spanned by the cell.
         * \param[in] rowspan Number of rows spanned by the cell.
         */
        Cell(QString value, int colspan = 1, int rowspan = 1);

        /*!
         * \brief Copy constructor for the Cell class.
         * \param[in] cell Another Cell object to copy.
         */
        Cell(const Cell &cell);

        /*!
         * \brief Get the pen for the specified border of the cell.
         * \param[in] border Border type.
         * \return QPen object representing the pen for the specified border.
         * \see Border
         */
        auto border(Border border) const -> QPen;

        /*!
         * \brief Get the number of columns spanned by the cell.
         * \return Number of columns spanned by the cell.
         */
        auto colspan() const -> int;

        /*!
         * \brief Get the number of rows spanned by the cell.
         * \return Number of rows spanned by the cell.
         */
        auto rowspan() const -> int;

        /*!
         * \brief Get the spacing of the cell border.
         * \param[in] border Border type.
         * \return Spacing value.
         * \see Border
         */
        auto spacing(Border border) const -> int;

        /*!
         * \brief Get the style of the cell.
         * \return PdfStyle object representing the style of the cell.
         */
        auto style() const -> PdfStyle;

        /*!
         * \brief Get the text value of the cell.
         * \return Text value of the cell.
         */
        auto text() const -> QString;

        /*!
         * \brief Check if the cell is part of a span.
         * \return True if the cell is part of a span, false otherwise.
         */
        bool isSpan() const;

        /*!
         * \brief Set the border of the cell for the specified border type.
         * \param[in] border Border type.
         * \param[in] pen QPen object representing the pen for the border.
         * \see Border
         */
        void setBorder(Border border, const QPen &pen);

        /*!
         * \brief Set the number of columns spanned by the cell.
         * \param[in] colspan Number of columns to span.
         */
        void setColspan(int colspan);

        /*!
         * \brief Set the number of rows spanned by the cell.
         * \param[in] rowspan Number of rows to span.
         */
        void setRowspan(int rowspan);

        /*!
         * \brief Set the spacing of the cell border for the specified border type.
         * \param[in] border Border type.
         * \param[in] spacing Spacing value.
         * \see Border
         */
        void setSpacing(Border border, int spacing);

        /*!
         * \brief Set the cell to be part of a span starting at the specified row and column.
         * \param[in] r Row index.
         * \param[in] c Column index.
         */
        void setSpan(int r, int c);

        /*!
         * \brief Set the reference cell to which this cell is spanned.
         * \param[in] ref Reference to the cell to which this cell is spanned.
         */
        void setSpanRef(Cell &ref);

        /*!
         * \brief Set the style of the cell.
         * \param[in] style PdfStyle object representing the style to apply.
         */
        void setStyle(const PdfStyle &style);

        /*!
         * \brief Set the text value of the cell.
         * \param[in] text Text value to set in the cell.
         */
        void setText(const QString &text);

    public:

        //TODO: incluir un espaciado del texto al borde
    private:

        /*!
         * \brief Pointer to the parent TablePdf object to which the cell belongs.
         */
        TablePdf *mParent;

    protected:

        /*!
         * \brief Text value of the cell.
         */
        QString mText;

        /*!
         * \brief Number of columns spanned by the cell.
         */
        int mColspan;

        /*!
         * \brief Number of rows spanned by the cell.
         */
        int mRowspan;

        /*!
         * \brief Style of the cell.
         */
        PdfStyle mStyle;

        /*!
         * \brief Border pens for different border types of the cell.
         */
        std::map<Border, QPen> mBorder;

        /*!
         * \brief Reference to the cell to which this cell is spanned.
         */
        Cell *mSpanRef;

        /*!
         * \brief Spacing values for different border types of the cell.
         */
        std::map<Border, int> mSpacing;

    };

protected:

    /*!
     * \brief Number of columns in the table.
     */
    int mCols;

    /*!
     * \brief Number of rows in the table.
     */
    int mRows;

    /*!
     * \brief Cells of the table.
     */
    std::vector<std::vector<Cell>> mValues;

    /*!
     * \brief Style of the table.
     */
    PdfStyle mStyleTable;

    /*!
     * \brief Borders of the table.
     */
    std::map<Border, QPen> mBorders;

public:

    /*!
     * \brief Default constructor for the TablePdf class.
     */
    TablePdf();

    /*!
     * \brief Constructor for the TablePdf class with specified number of columns and rows.
     * \param[in] cols Number of columns in the table.
     * \param[in] rows Number of rows in the table.
     */
    TablePdf(int cols, int rows);

    /*!
     * \brief Copy constructor for the TablePdf class.
     * \param[in] table Another TablePdf object to copy.
     */
    TablePdf(const TablePdf &table);

    /*!
     * \brief Destructor for the TablePdf class.
     */
    ~TablePdf();

    /*!
     * \brief Get the pen for the specified border of the table.
     * \param[in] border Border type.
     * \return QPen object representing the pen for the specified border.
     * \see Border
     */
    auto borderTable(Border border) const -> QPen;

    /*!
     * \brief Get the cell at the specified row and column.
     * \param[in] r Row index.
     * \param[in] c Column index.
     * \return Pointer to the Cell object.
     * \see Cell
     */
    auto cell(int r, int c) const -> const Cell*;

    /*!
     * \brief Get the number of columns in the table.
     * \return Number of columns.
     */
    auto cols() const -> int;

    /*!
     * \brief Get the number of rows in the table.
     * \return Number of rows.
     */
    auto rows() const -> int;

    /*!
     * \brief Get the style of the table.
     * \return PdfStyle object representing the style of the table.
     */
    auto styleTable() const -> PdfStyle;

    /*!
     * \brief Get the value of the cell at the specified row and column.
     * \param[in] r Row index.
     * \param[in] c Column index.
     * \return Value of the cell.
     */
    auto value(int r, int c) const -> QString;

    /*!
     * \brief Set the border of a cell at the specified row and column.
     * \param[in] r Row index.
     * \param[in] c Column index.
     * \param[in] border Border type.
     * \param[in] pen QPen object representing the pen for the border.
     * \see Border
     */
    void setBorderCell(int r,
                       int c,
                       Border border,
                       const QPen &pen);

    /*!
     * \brief Set the border of a column.
     * \param[in] c Column index.
     * \param[in] border Border type.
     * \param[in] pen QPen object representing the pen for the border.
     * \see Border
     */
    void setBorderCol(int c,
                      Border border,
                      const QPen &pen);

    /*!
     * \brief Set the border of a range of cells.
     * \param[in] r1 Starting row index.
     * \param[in] c1 Starting column index.
     * \param[in] r2 Ending row index.
     * \param[in] c2 Ending column index.
     * \param[in] border Border type.
     * \param[in] pen QPen object representing the pen for the border.
     * \see Border
     */
    void setBorderRange(int r1,
                        int c1,
                        int r2,
                        int c2,
                        Border border,
                        const QPen &pen);

    /*!
     * \brief Set the border of a row.
     * \param[in] r Row index.
     * \param[in] border Border type.
     * \param[in] pen QPen object representing the pen for the border.
     * \see Border
     */
    void setBorderRow(int r,
                      Border border,
                      const QPen &pen);

    /*!
     * \brief Set the border of the entire table.
     * \param[in] border Border type.
     * \param[in] pen QPen object representing the pen for the border.
     * \see Border
     */
    void setBorderTable(Border border,
                        const QPen &pen);

    /*!
     * \brief Set the number of columns in the table.
     * \param[in] cols Number of columns.
     */
    void setCols(int cols);

    /*!
     * \brief Set the number of rows in the table.
     * \param[in] rows Number of rows.
     */
    void setRows(int rows);

    /*!
     * \brief Set the spacing of a column border.
     * \param[in] c Column index.
     * \param[in] border Border type.
     * \param[in] spacing Spacing value.
     * \see Border
     */
    void setSpacingCol(int c,
                       Border border,
                       int spacing);

    /*!
     * \brief Set the spacing of a row border.
     * \param[in] r Row index.
     * \param[in] border Border type.
     * \param[in] spacing Spacing value.
     * \see Border
     */
    void setSpacingRow(int r,
                       Border border,
                       int spacing);

    /*!
     * \brief Set the spacing of borders for all cells in the table.
     * \param[in] border Border type.
     * \param[in] spacing Spacing value.
     * \see Border
     */
    void setSpacingTable(Border border,
                         int spacing);

    /*!
     * \brief Apply a style to a cell at the specified row and column.
     * \param[in] r Row index.
     * \param[in] c Column index.
     * \param[in] style PdfStyle object representing the style to apply.
     */
    void setStyleCell(int r,
                      int c,
                      const PdfStyle &style);

    /*!
     * \brief Apply a style to a column.
     * \param[in] c Column index.
     * \param[in] style PdfStyle object representing the style to apply.
     */
    void setStyleCol(int c,
                     const PdfStyle &style);

    /*!
     * \brief Apply a style to a range of cells.
     * \param[in] r1 Starting row index.
     * \param[in] c1 Starting column index.
     * \param[in] r2 Ending row index.
     * \param[in] c2 Ending column index.
     * \param[in] style PdfStyle object representing the style to apply.
     */
    void setStyleRange(int r1,
                       int c1,
                       int r2,
                       int c2,
                       const PdfStyle &style);

    /*!
     * \brief Apply a style to a row.
     * \param[in] r Row index.
     * \param[in] style PdfStyle object representing the style to apply.
     */
    void setStyleRow(int r, const PdfStyle &style);

    /*!
     * \brief Apply a style to the entire table.
     * \param[in] style PdfStyle object representing the style to apply.
     */
    void setStyleTable(const PdfStyle &style);

    /*!
     * \brief Set the value of a cell at the specified row and column.
     * \param[in] r Row index.
     * \param[in] c Column index.
     * \param[in] value Value to set in the cell.
     * \param[in] colspan Number of columns the cell spans.
     * \param[in] rowspan Number of rows the cell spans.
     */
    void setValue(int r,
                  int c,
                  const QString &value,
                  int colspan = 1,
                  int rowspan = 1);

    /*!
     * \brief Set the value of a cell at the specified row and column along with its style.
     * \param[in] r Row index.
     * \param[in] c Column index.
     * \param[in] value Value to set in the cell.
     * \param[in] style Pointer to the PdfStyle object representing the style of the cell.
     * \param[in] colspan Number of columns the cell spans.
     * \param[in] rowspan Number of rows the cell spans.
     */
    void setValue(int r,
                  int c,
                  const QString &value,
                  const PdfStyle *style,
                  int colspan = 1,
                  int rowspan = 1);

private:

    /*!
     * \brief Update the table by adjusting its size according to the number of rows and columns.
     */
    void updateTable();

};




/*!
 * \brief Class representing a header for PDF documents.
 */
class PdfHeader
{

public:

    /*!
     * \brief Default constructor.
     */
    PdfHeader()
      : bImage(false),
        bText(false)
    {
    }

    /*!
     * \brief Adds an image to the header.
     * \param[in] image QImage object representing the image.
     * \param[in] rect QRect object defining the rectangle for the image.
     */
    void addImage(const QImage &image, const QRect &rect)
    {
        bImage = true;
        mImage = image;
        mImageRect = rect;
    }

    /*!
     * \brief Adds text to the header.
     * \param[in] text QString object representing the text.
     * \param[in] rect QRect object defining the rectangle for the text.
     */
    auto addText(const QString& text, const QRect& rect) -> void
    {
        bText = true;
        mText = text;
        mTextRect = rect;
    }

    /*!
     * \brief Checks if the header has an image.
     * \return True if the header has an image, false otherwise.
     */
    auto hasImage() const -> bool
    {
        return bImage;
    }

    /*!
     * \brief Checks if the header has text.
     * \return True if the header has text, false otherwise.
     */
    auto hasText() const -> bool
    {
        return bText;
    }

    /*!
     * \brief Retrieves the image of the header.
     * \return QImage object representing the image of the header.
     */
    auto image() const -> QImage
    {
        return mImage;
    }

    /*!
     * \brief Retrieves the rectangle of the image in the header.
     * \return QRect object defining the rectangle of the image in the header.
     */
    auto imageRect() const -> QRect
    {
        return mImageRect;
    }

    /*!
     * \brief Retrieves the text of the header.
     * \return QString object representing the text of the header.
     */
    auto text() const -> QString
    {
        return mText;
    }

    /*!
     * \brief Retrieves the rectangle of the text in the header.
     * \return QRect object defining the rectangle of the text in the header.
     */
    auto textRect() const -> QRect
    {
        return mTextRect;
    }

    /*!
     * \brief Retrieves the style of the header.
     * \return PdfStyle object representing the style of the header.
     */
    auto style() const -> PdfStyle
    {
        return mStyle;
    }

    /*!
     * \brief Sets the style of the header.
     * \param[in] style PdfStyle object representing the style to set.
     */
    void setStyle(const PdfStyle &style)
    {
        mStyle = style;
    }

private:

    bool bImage;
    bool bText;
    QImage mImage;
    QRect mImageRect;
    QString mText;
    QRect mTextRect;
    PdfStyle mStyle;
};


//TODO: listas númeradas
//TODO: Encabezados númerados

/*!
 * \brief Class for generating PDF files.
 */
class Pdf
{
public:

    /*!
     * \brief Enum defining different text styles.
     */
    enum class Style
    {
        default_text,
        title,
        heading1,
        heading2,
        heading3,
        textbody
    };

    /*!
     * \brief Enum defining standard page sizes.
     */
    enum class PageSize
    {
        // ISO - DIN serie A
        A0,
        A1,
        A2,
        A3,
        A4,
        A5,
        A6,
        A7,
        A8,
        A9,
        A10,

        // ISO - DIN serie B
        B0,
        B1,
        B2,
        B3,
        B4,
        B5,
        B6,
        B7,
        B8,
        B9,
        B10,

        //ISO - DIN serie C
        C0,
        C1,
        C2,
        C3,
        C4,
        C5,
        C6,
        C7,
        C8,
        C9,
        C10,

        // North America
        letter,
        legal,
        executive

    };

    /*!
     * \brief Enum defining page orientations.
     */
    enum class PageOrientation
    {
        portrait,
        landscape
    };

public:

    /*!
     * \brief Constructor to initialize PDF with a file.
     * \param[in] file File path for the PDF.
     */
    explicit Pdf(const QString &file);

    /*!
     * \brief Destructor to clean up resources.
     */
    ~Pdf();

// Header and Footer Methods

    /*!
     * \brief Method to display the PDF.
     */
    void show() const;

    /*!
     * \brief Adds a header to the page.
     * \param[in] text Text or template for the header.
     */
    void addHeader(const QString &text);

    /*!
     * \brief Adds a custom header.
     * \param[in] f Function defining the header.
     */
    void addHeader(std::function<void(QRect *)> f);

    /*!
     * \brief Adds a predefined header.
     * \param[in] header Predefined header object.
     * \see PdfHeader
     */
    void addHeader(const PdfHeader &header);

    /*!
     * \brief Adds a footer to the page.
     * \param[in] text Text or template for the footer.
     */
    void addFooter(const QString &text);

// Drawing Methods

    /*!
     * \brief Draws a title with specified text.
     * \param[in] text Text for the title.
     */
    void drawTitle(const QString &text);

    /*!
     * \brief Draws a heading 1 with specified text.
     * \param[in] text Text for heading 1.
     */
    void drawHeading1(const QString &text);

    /*!
     * \brief Draws a heading 2 with specified text.
     * \param[in] text Text for heading 2.
     */
    void drawHeading2(const QString &text);

    /*!
     * \brief Draws a heading 3 with specified text.
     * \param[in] text Text for heading 3.
     */
    void drawHeading3(const QString &text);

    /*!
     * \brief Draws text with specified style.
     * \param[in] text Text to draw.
     */
    void drawText(const QString &text);

    /*!
     * \brief Draws text body with specified style.
     * \param[in] text Text for text body.
     */
    void drawTextBody(const QString &text);

    /*!
     * \brief Draws an image on the page.
     * \param[in] img Path to the image file.
     * \param[in] caption Caption for the image.
     * \param[in] options Alignment options for the image.
     * \return The rectangle occupied by the drawn image.
     */
    auto drawImage(const QString &img,
                   const QString &caption = "",
                   int options = Qt::AlignCenter) -> QRect;

    /*!
     * \brief Draws an image on the page.
     * \param[in] image QImage object representing the image.
     * \param[in] caption Caption for the image.
     * \param[in] options Alignment options for the image.
     * \return The rectangle occupied by the drawn image.
     */
    auto drawImage(const QImage &image,
                   const QString &caption = "",
                   int options = Qt::AlignCenter) -> QRect;

    /*!
     * \brief Draws a list on the page.
     * \param[in] text Text representing the list.
     */
    void drawList(const QString &text);

    /*!
     * \brief Draws a table on the page.
     * \param[in] table TablePdf object representing the table.
     * \param[in] caption Caption for the table.
     * \return The rectangle occupied by the drawn table.
     */
    auto drawTable(const TablePdf &table,
                   const QString &caption = "") -> QRect;

    /*!
     * \brief Draws a line on the page.
     * \param[in] pt1 Starting point of the line.
     * \param[in] pt2 Ending point of the line.
     * \param[in] style Style of the line.
     */
    void drawLine(const QPoint &pt1, 
                  const QPoint &pt2, 
                  const PdfStyle &style) const;


    /*!
     * \brief Draws a rectangle on the page.
     * \param[in] rect Rectangle to draw.
     * \param[in] style Style of the rectangle.
     */
    void drawRect(const QRect &rect,
                  const PdfStyle &style) const;

    /*!
     * \brief Ends the drawing process.
     */
    void endDraw() const;

    /*!
     * \brief Initializes the drawing process.
     */
    void initDraw();

    /*!
     * \brief Moves to a new line for drawing.
     */
    void newLine();

    /*!
     * \brief Moves to a new page for drawing.
     */
    void newPage();

// Setter Methods

    /*!
     * \brief Sets the creator of the PDF.
     * \param[in] creator Name of the creator.
     */
    void setCreator(const QString &creator);

    /*!
     * \brief Sets the current font for text.
     * \param[in] family Font family.
     * \param[in] pointSize Font size.
     * \param[in] weight Font weight.
     * \param[in] italic Whether the font is italic or not.
     */
    void setFont(const QString &family, 
                 int pointSize = -1, 
                 int weight = -1, 
                 bool italic = false);

    /*!
     * \brief Sets the default font.
     */
    void setFontDefault();

    /*!
     * \brief Sets the page margins.
     * \param[in] left Left margin.
     * \param[in] right Right margin.
     * \param[in] top Top margin.
     * \param[in] bottom Bottom margin.
     */
    void setMargins(int left,
                    int right,
                    int top,
                    int bottom) const;

    /*!
     * \brief Sets the page size.
     * \param[in] pageSize Size of the page.
     */
    void setPageSize(PageSize pageSize);

    /*!
     * \brief Sets the page orientation.
     * \param[in] pageOrientation Orientation of the page.
     */
    void setPageOrientation(PageOrientation pageOrientation);

    /*!
     * \brief Sets the title of the PDF.
     * \param[in] title Title of the PDF.
     */
    void setTitle(const QString &title);

// Getter Methods

    /*!
     * \brief Retrieves the current text style.
     * \return Current text style.
     */
    auto currentStyle() const -> PdfStyle;

    /*!
     * \brief Retrieves the title style.
     * \return Title style.
     */
    auto titleStyle() const -> PdfStyle;

    /*!
     * \brief Sets the title style.
     * \param[in] titleStyle Style for the title.
     */
    void setTitleStyle(const PdfStyle &titleStyle);

    /*!
     * \brief Retrieves the heading 1 style.
     * \return Heading 1 style.
     */
    auto heading1Style() const -> PdfStyle;

    /*!
     * \brief Sets the heading 1 style.
     * \param[in] heading1Style Style for heading 1.
     */
    void setHeading1Style(const PdfStyle &heading1Style);

    /*!
     * \brief Retrieves the heading 2 style.
     * \return Heading 2 style.
     */
    auto heading2Style() const -> PdfStyle;

    /*!
     * \brief Sets the heading 2 style.
     * \param[in] heading2Style Style for heading 2.
     */
    void setHeading2Style(const PdfStyle &heading2Style);

    /*!
     * \brief Retrieves the heading 3 style.
     * \return Heading 3 style.
     */
    auto heading3Style() const -> PdfStyle;

    /*!
     * \brief Sets the heading 3 style.
     * \param[in] heading3Style Style for heading 3.
     */
    void setHeading3Style(const PdfStyle &heading3Style);

    /*!
     * \brief Retrieves the text body style.
     * \return Text body style.
     */
    auto textBodyStyle() const -> PdfStyle;

    /*!
     * \brief Sets the text body style.
     * \param[in] textBodyStyle Style for text body.
     */
    void setTextBodyStyle(const PdfStyle &textBodyStyle);

    /*!
     * \brief Retrieves the body rectangle.
     * \return Rectangle representing the body.
     */
    auto rectBody() const -> QRect;

    /*!
     * \brief Retrieves the current position.
     * \return Current position.
     */
    auto currentPosition() const -> QPoint;

    /*!
     * \brief Sets the rectangle for painting.
     * \param[in] rect Rectangle for painting.
     */
    void setPaintRect(const QRect &rect);

    /*!
     * \brief Retrieves the header rectangle.
     * \return Rectangle representing the header.
     */
    auto rectHeader() const -> QRect;

private:

    static void update();
    void drawTextBlock(const QString &text,
                       const PdfStyle &style);
    void drawHeader();
    void drawFooter() const;
    auto replace(const QString& input) const -> QString;
    void setPageNumber(int n);

protected:

    QString mFile;
    QPdfWriter *mPdfWriter;
    QPainter *mPainter;
    PdfStyle mDefStyle;
    PdfStyle mCurrentStyle;
    PdfStyle mTitleStyle;
    PdfStyle mHeading1Style;
    PdfStyle mHeading2Style;
    PdfStyle mHeading3Style;
    PdfStyle mTextBodyStyle;
    std::map<QString, PdfStyle> mUserStyles;
    int mSpace;
    PageSize mPageSize;
    PageOrientation mPageOrientation;
    bool mBIniDraw;
    bool mBHeader;
    bool mBFooter;
    QString mHeaderText;
    PdfHeader mHeader;
    std::function<void(QRect *)> mHeaderFunction;
    QString mFooterText;
    PdfStyle mHeaderStyle;
    PdfStyle mFooterStyle;
    QRect mRectHeader;
    QRect mRectBody;
    QRect mRectFooter;
    int mImgCaption;
    int mTableCaption;

private:

    int mX;
    int mY;
    int mWidth;
    int mHeight;
    std::map<std::string, std::string> mTagValues;
    int mPageNumber;


};

} // namespace graphos

#endif // GRAPHOS_UTILS_PDF_H

