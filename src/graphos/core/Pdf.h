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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_UTILS_PDF_H
#define GRAPHOS_UTILS_PDF_H

#include <functional>
#include <memory>

#include <QFont>
#include <QRect>
#include <QBrush>
#include <QPen>

class QPdfWriter;
class QPainter;

namespace graphos
{


//TODO: esta clase debería ser mucho mas completa.
// ¿Utilizar GraphicStyle?
class PdfStyle
{

public:

  /*!
   * \brief Constructor por defecto
   */
  PdfStyle();

  /*!
   * \brief Constructor
   * \param[in] font Fuente
   * \param[in] brush Pincel
   * \param[in] pen Pluma
   * \param[in] options Opciones
   */
  PdfStyle(const QFont &font, 
           const QBrush &brush = QBrush(), 
           const QPen &pen = QPen(), 
           int options = 0);

  /*!
   * \brief Constructor de copia
   */
  PdfStyle(const PdfStyle &pdfStyle);

  /*!
   * \brief Destructora
   */
  ~PdfStyle();

  const QBrush brush() const;
  const QFont font() const;
  int options() const;
  const QPen pen() const;

  void setBrush(const QBrush &brush);
  void setFont(const QFont &font);
  void setFont(const QString &family, 
               int pointSize, 
               int weight, 
               bool italic);
  void setOptions(int options);
  void setPen(const QPen &pen);

protected:

  /*!
   * \brief Fuente
   */
  QFont mFont;

  /*!
   * \brief Pincel
   */
  QBrush mBrush;

  /*!
   * \brief Pluma
   */
  QPen mPen;

  /*!
   * \brief Opciones
   */
  int mOptions;

};


class TablePdf
{

  public:
    
    enum class Border
    {
      none   = 1 << 0,
      left   = 1 << 1,
      right  = 1 << 2,
      top    = 1 << 3,
      botton = 1 << 4,
      all    = left | right | top | botton
    };

  class Cell
  {

  public:

    Cell(TablePdf *parent = nullptr);
    Cell(QString value, 
         int colspan = 1, 
         int rowspan = 1);
    Cell(const Cell &cell);
    //Cell(const Cell &cell) = delete;
    //Cell(Cell &&cell) = delete;


    QPen border(Border border) const;
    int colspan() const;
    int rowspan() const;
    int spacing(Border border) const;
    PdfStyle style() const;
    QString text() const;

    bool isSpan() const;

    void setBorder(Border border, const QPen &pen);
    void setColspan(int colspan);
    void setRowspan(int rowspan);
    void setSpacing(Border border, int spacing);
    void setSpan(int r, int c);
    void setSpanRef(Cell &ref);
    void setStyle(const PdfStyle &style);
    void setText(const QString &text);

  public:

    //TODO: incluir un espaciado del texto al borde

  protected:

    /*!
     * \brief Texto de la celda
     */
    QString mText;

    /*!
     * \brief Número de celdas que se combinan en horizontal
     */
    int mColspan;

    /*!
     * \brief Número de celdas que se combinan en vertical
     */
    int mRowspan;

    /*!
     * \brief Estilo
     */
    PdfStyle mStyle;

    /*!
     * \brief Pluma de borde
     */
    std::map<Border, QPen> mBorder;

    /*!
     * \brief Referencia a la celda a la que esta unida
     */
    Cell *mSpanRef;

    /*!
     * \brief Espaciado de borde
     */
    std::map<Border, int> mSpacing;

  private:

    /*!
     * \brief Puntero a la tabla a la que pertenece
     */
    TablePdf *mParent;

  };

protected:

  /*!
   * \brief Número de columnas
   */
  int mCols;

  /*!
   * \brief Número de filas
   */
  int mRows;

  /*!
   * \brief Celdas de la tabla
   */
  std::vector<std::vector<Cell>> mValues;

  /*!
   * \brief Estilo de la tabla 
   */
  PdfStyle mStyleTable;

  /*!
   * \brief Bordes de la tabla 
   */
  std::map<Border, QPen> mBorders;

public:

  /*!
   * \brief Constructor por defecto
   */
  TablePdf();

  /*!
   * \brief Constructor
   * \param[in] cols Número de columnas
   * \param[in] rows Número de filas
   */
  TablePdf(int cols, int rows);

  /*!
   * \brief Constructor de copia
   * \param[in] TablePdf Objeto TablePdf que se copia
   */
  TablePdf(const TablePdf &table);

  /*!
   * \brief Destructor
   */
  ~TablePdf();

  /*!
   * \brief Dibuja la tabla
   */
  //void draw(QPainter *painter, int x, int y);

  /*!
   * \brief Devuelve la pluma del borde indicado
   * \param[in] border Borde
   * \return Pluma del borde indicado
   * \see Border
   */
  QPen borderTable(Border border) const;

  /*!
   * \brief Devuelve la celda en la posición r, c
   * \param[in] r Fila
   * \param[in] c Columna
   * \return Celda
   * \see Cell
   */
  const Cell *cell(int r, int c) const;

  /*!
   * \brief Devuelve el número de columnas de la tabla
   * \return Número de columnas
   */
  int cols() const;

  /*!
   * \brief Devuelve el número de filas de la tabla
   * \return Número de filas
   */
  int rows() const;

  /*!
   * \brief Devuelve el estilo de la tabla
   * \return Estilo de la tabla
   */
  PdfStyle styleTable() const;

  /*!
   * \brief Devuelve el valor de una celda
   * \param[in] r Fila
   * \param[in] c Columna
   * \return Valor de la celda
   */
  QString value(int r, int c) const;

  /*!
   * \brief Establece el borde de una celda
   * \param[in] r Fila
   * \param[in] c Columna
   * \param[in] border Borde que se modifica
   * \param[in] pen Pluma
   * \see Border
   */
  void setBorderCell(int r, 
                     int c, 
                     Border border,
                     const QPen &pen);
 
  /*!
   * \brief Establece el borde de una columna
   * \param[in] c Columna
   * \param[in] border Borde que se modifica
   * \param[in] pen Pluma
   * \see Border
   */
  void setBorderCol(int c,
                    Border border, 
                    const QPen &pen);

  /*!
   * \brief Establece el borde en un rango de celdas
   * \param[in] r1 Fila inicial
   * \param[in] c1 Columna inicial
   * \param[in] r2 Fila final
   * \param[in] c2 Columna final
   * \param[in] border Borde que se modifica
   * \param[in] pen Pluma
   * \see Border
   */
  void setBorderRange(int r1, 
                      int c1,
                      int r2,
                      int c2, 
                      Border border,
                      const QPen &pen);
  
  /*!
   * \brief Establece el borde de una fila
   * \param[in] r Fila
   * \param[in] border Borde que se modifica
   * \param[in] pen Pluma
   * \see Border
   */
  void setBorderRow(int r, 
                    Border border, 
                    const QPen &pen);

  /*!
   * \brief Establece el borde de la tabla
   * \param[in] border Borde que se modifica
   * \param[in] pen Pluma
   * \see Border
   */
  void setBorderTable(Border border,
                      const QPen &pen);

  /*!
   * \brief Establece el número de columnas
   * \param[in] cols Número de columnas
   */
  void setCols(int cols);

  /*!
   * \brief Establece el número de columnas
   * \param[in] rows Número de filas
   */
  void setRows(int rows);
 
  /*!
   * \brief Establece el espaciado de borde de una columna
   * \param[in] c Columna
   * \param[in] border Borde que se modifica
   * \param[in] int Espaciado
   * \see Border
   */
  void setSpacingCol(int c, 
                     Border border, 
                     int spacing);

  /*!
   * \brief Establece el espaciado de borde de una fila
   * \param[in] r Fila
   * \param[in] border Borde que se modifica
   * \param[in] int Espaciado
   * \see Border
   */
  void setSpacingRow(int r, 
                     Border border, 
                     int spacing);

  /*!
   * \brief Establece el espaciado de borde de las celdas de una tabla
   * \param[in] r Fila
   * \param[in] border Borde que se modifica
   * \param[in] int Espaciado
   * \see Border
   */
  void setSpacingTable(Border border, 
                       int spacing);

  /*!
   * \brief Aplica un estilo a una celda
   * \param[in] r Fila
   * \param[in] c Columna
   * \param[in] style Estilo
   */
  void setStyleCell(int r,
                    int c, 
                    const PdfStyle &style);

  /*!
   * \brief Aplica un estilo a una columna
   * \param[in] c Columna
   * \param[in] style Estilo
   */
  void setStyleCol(int c, 
                   const PdfStyle &style);

  /*!
   * \brief Aplica un estilo a un rango de celdas
   * \param[in] r1 Fila inicial
   * \param[in] c1 Columna inicial
   * \param[in] r2 Fila final
   * \param[in] c2 Columna final
   * \param[in] style Estilo
   */
  void setStyleRange(int r1, 
                     int c1, 
                     int r2, 
                     int c2, 
                     const PdfStyle &style);

  /*!
   * \brief Aplica un estilo a una fila
   * \param[in] r Fila
   * \param[in] style Estilo
   */
  void setStyleRow(int r, const PdfStyle &style);
  
  /*!
   * \brief Aplica un estilo a una tabla
   * \param[in] style Estilo
   */  
  void setStyleTable(const PdfStyle &style);

  /*!
   * \brief Establece el valor de una celda
   * \param[in] r Fila
   * \param[in] c Columna
   * \param[in] value Valor de la celda
   * \param[in] colspan Expansión de columnas
   * \param[in] rowspan Expansión de filas
   */
  void setValue(int r, 
                int c, 
                const QString &value, 
                int colspan = 1, 
                int rowspan = 1);

  /*!
   * \brief Establece el valor de una celda
   * \param[in] r Fila
   * \param[in] c Columna
   * \param[in] value Valor de la celda
   * \param[in] style Estilo de la celda
   * \param[in] colspan Expansión de columnas
   * \param[in] rowspan Expansión de filas
   */
  void setValue(int r, 
                int c, 
                const QString &value, 
                const PdfStyle *style, 
                int colspan = 1, 
                int rowspan = 1);

private:

  /*!
   * \brief Actualiza la tabla ajustando el tamaño al númer de filas y columnas
   */
  void updateTable();

};



//TODO: listas númeradas
//TODO: Encabezados númerados

/*!
 * \brief Clase para generar ficheros pdf
 */
class Pdf
{
public:

  enum class Style
  {
    default_text,
    title,
    heading1,
    heading2,
    heading3,
    textbody
  };

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

public:

  Pdf(const QString &file);
  ~Pdf();

  //void print(const std::string &fileName, const std::string &tmpl, const std::map<std::string, std::string> &tag_values);
  void show();

  // TODO: Por ahora solo texto. Se deberia devolver un Rect para la
  // cabecera y el pie de página que permitan una configuración mas avanzada
  /*!
   * \brief Encabezado de página
   * \param[in] text Texto plano o plantilla definida con campos como el titulo del documento, autor, etc. 
   * \param[in] PdfStyle Estilo de texto
   */
  void addHeader(const QString &text);
  void addHeader(std::function<void(QRect *)> f);

  /*!
   * \brief Pie de página
   * \param[in] text Texto plano o plantilla definida con campos como el número de página. 
   * \param[in] PdfStyle Estilo de texto
   */
  void addFooter(const QString &text);

  void drawTitle(const QString &text);
  void drawHeading1(const QString &text);
  void drawHeading2(const QString &text);
  void drawHeading3(const QString &text);
  void drawText(const QString &text);
  //void drawText(Style style, const char *text, ...);
  void drawTextBody(const QString &text);
  QRect drawImage(const QString &img,
                  const QString &caption = "");
  QRect drawImage(const QImage &image,
                  const QString &caption = "");
  void drawList(const QString &text);
  //void drawNumberedList(const char *text);
  QRect drawTable(const TablePdf &table,
                  const QString &caption = "");

  void endDraw();
  void initDraw();
  void newLine();
  void newPage();

  /*!
   * \brief Establece el programa creador del documento pdf
   * \param[in] creator 
   */
  void setCreator(const QString &creator);

  /*!
   * \brief Establece la fuente actual
   * \param[in] family 
   */
  void setFont(const QString &family,
               int pointSize = -1, 
               int weight = -1, 
               bool italic = false);
 
  /*!
   * \brief Establece la fuente por defecto
   */
  void setFontDefault();

  /*!
   * \brief Establece los margenes de página
   * \param[in] left Margen izquierdo
   * \param[in] right Margen derecho
   * \param[in] top Margen superior
   * \param[in] bottom Margen inferior
   */
  void setMargins(int left, 
                  int right, 
                  int top, 
                  int bottom);

  /*!
   * \brief Establece el tamaño de página
   * \param[in] pageSize Id tamaño de pagina (QPdfWriter::PageSize)
   */
  void setPageSize(PageSize pageSize);

  /*!
   * \brief Establece el titulo del documento pdf
   * \param[in] title Titulo del pdf
   */
  void setTitle(const QString &title);

  PdfStyle currentStyle() const;

  PdfStyle titleStyle() const;
  void setTitleStyle(const PdfStyle &titleStyle);

  PdfStyle heading1Style() const;
  void setHeading1Style(const PdfStyle &heading1Style);

  PdfStyle heading2Style() const;
  void setHeading2Style(const PdfStyle &heading2Style);

  PdfStyle heading3Style() const;
  void setHeading3Style(const PdfStyle &heading3Style);

  PdfStyle textBodyStyle() const;
  void setTextBodyStyle(const PdfStyle &textBodyStyle);

  QRect rectBody() const;
  QPoint currentPosition() const;
  void setPaintRect(const QRect &rect);

private:

  void update();
  void drawTextBlock(const QString &text,
                     const PdfStyle &style);
  void drawHeader();
  void drawFooter();
  QString replace(const QString &input) const;
  void setPageNumber(int n);

protected:

  /*!
   * \brief Fichero pdf
   */
  QString mFile;

  /*!
   * \brief Clase de Qt para generar archivos pdf
   */
  QPdfWriter *pPdfWriter;

  QPainter *pPainter;

  //QFont *mDefFont;
  //QFont *mCurrentFont; //TODO: Sustituir por un estilo

  /*!
   * \brief Estilo de texto por defecto
   */
  PdfStyle mDefStyle;

  /*!
   * \brief Estilo actual
   */
  PdfStyle mCurrentStyle;

  /*!
   * \brief Estilo de titulo
   */
  PdfStyle mTitleStyle;

  /*!
   * \brief Estilo de encabezado 1
   */
  PdfStyle mHeading1Style;

  /*!
   * \brief Estilo de encabezado 2
   */
  PdfStyle mHeading2Style;

  /*!
   * \brief Estilo de encabezado 3
   */
  PdfStyle mHeading3Style;

  /*!
   * \brief Estilo de cuerpo de texto
   */
  PdfStyle mTextBodyStyle;

  //TODO: Estilos definidos por el usuario
  std::map<QString, PdfStyle> mUserStyles;

  /*!
   * \brief espaciado de lineas
   */
  int mSpace;

  PageSize mPageSize;

  /*!
   * \brief Una vez que se empieza a dibujar no se pueden modificar las opciones de página.
   */
  bool bIniDraw;

  /*!
   * \brief Cabecera activada
   */
  bool bHeader;

  /*!
   * \brief Pie de página activado
   */
  bool bFooter;

  /*!
   * \brief Texto del encabezado de página
   */
  QString mHeaderText;

  std::function<void(QRect *)> *mHeaderFunction;

  /*!
   * \brief Texto del pie de página
   */
  QString mFooterText;

  /*!
   * \brief Estilo del encabezado de página
   */
  PdfStyle mHeaderStyle;

  /*!
   * \brief Estilo del pie de página
   */
  PdfStyle mFooterStyle;

  QRect mRectHeader;
  QRect mRectBody;
  QRect mRectFooter;

  int mImgCaption;
  int mTableCaption;

private:

  /*!
   * \brief Posición x actual
   * Esta variable se va modificando según nos desplazamos por el texto
   */
  int mX;

  /*!
   * \brief Posición y actual
   * Esta variable se va modificando según nos desplazamos por el texto
   */
  int mY;

  int mWidth;
  int mHeight;

  // valores para las templates
  std::map<std::string, std::string> mTagValues;
  int mPageNumber;


};

} // namespace graphos

#endif // GRAPHOS_UTILS_PDF_H

