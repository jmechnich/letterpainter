/**************************************************************************
**       Title: LetterPainter.h
**    $RCSfile: $
**   $Revision: $$Name:  $
**       $Date: $
**   Copyright: GPL $Author:$
** Description:
**
**
**
**-------------------------------------------------------------------------
**
**  $Log: $
**
**
**************************************************************************/
#ifndef LETTERPAINTER_H
#define LETTERPAINTER_H

#include <qwidget.h>
#include <qevent.h>
#include <qpen.h>
#include <qpointarray.h>
#include <qtoolbutton.h>
#include <qpoint.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>

#include <iostream>
#include <vector>

using namespace std;

/*======================================================================*/
/*!
 *  \class Canvas LetterPainter.hh
 *  \brief The Canvas class provides a paint canvas with basic functions
 *         for IO
 */
/*======================================================================*/

class Canvas : public QWidget
{
  Q_OBJECT

public:
/*======================================================================*/
/*!
 *   Constructor
 */
/*======================================================================*/
  Canvas( QWidget *parent = 0, const char *name = 0 );

/*======================================================================*/
/*!
 *   Clears the painting canvas
 */
/*======================================================================*/
  void clearScreen();

/*======================================================================*/
/*!
 *   The read function reads a picture in acd-format from an istream
 *   \param is Inputstream which should be read
 */
/*======================================================================*/
  void read( istream &is);

/*======================================================================*/
/*!
 *   The write function writes a picture in acd-format to an ostream
 *   \param os Outputstream to which should be written
 */
/*======================================================================*/
  void write( ostream &os);

/*======================================================================*/
/*!
 *   Returns the picture's label
 *   \return The label of the picture
 */
/*======================================================================*/
  QString getLabel() const;

/*======================================================================*/
/*!
 *   Sets the picture's label
 *   \param label The label of the picture
*/
/*======================================================================*/
  void setLabel( QString label);

/*======================================================================*/
/*!
 *   (description)
 *
 *   \param
 *
 *   \exception
 *
 *   \return
 */
/*======================================================================*/
  bool isNotEmpty() const;

signals:
/*======================================================================*/
/*!
 *   Is emitted when the label of the picture changed
 */
/*======================================================================*/
  void labelChanged( const QString&);

protected:
/*======================================================================*/
/*!
 *   Overloading the mousePressEvent function inherited by the QWidget
 *   \param e The QMouseEvent
 */
/*======================================================================*/
  void mousePressEvent( QMouseEvent* e);

/*======================================================================*/
/*!
 *   Overloading the mouseReleaseEvent function inherited by the QWidget
 *   \param e The QMouseEvent
 */
/*======================================================================*/
  void mouseReleaseEvent( QMouseEvent* e);

/*======================================================================*/
/*!
 *   Overloading the mouseMoveEvent function inherited by the QWidget
 *   \param e The QMouseEvent
 */
/*======================================================================*/
  void mouseMoveEvent( QMouseEvent* e);

/*======================================================================*/
/*!
 *   Overloading the paintEvent function inherited by the QWidget
 *   \param e The QPaintEvent
 */
/*======================================================================*/
  void paintEvent( QPaintEvent* e);

/*=========================================================================
 *  Modul global Variables
 *========================================================================*/
  QWidget* p_parent;

  QPen pen;
  QPointArray polyline;

  bool mousePressed;

  vector<vector<QPoint> > p_buffer;
  vector<QPoint> p_lineBuffer;

  QString p_label;
};

/*======================================================================*/
/*!
 *  \class LetterPainter LetterPainter.hh
 *  \brief The class creates the application's main window
 */
/*======================================================================*/
class LetterPainter : public QWidget
{
  Q_OBJECT

public:

/*======================================================================*/
/*!
 *   Constructor
 *   \param parent The parent widget of the LetterPainter object
 *   \param name The object's name
 */
/*======================================================================*/
  LetterPainter( QWidget* parent = 0, const char *name = 0 );

protected:
/*=========================================================================
 *  Modul global Variables
 *========================================================================*/
  Canvas* canvas;
  QHBoxLayout* layout;
  QPushButton* bLoad;
  QPushButton* bSave;
  QPushButton* bClear;
  QLineEdit* labelLine;

protected slots:

/*======================================================================*/
/*!
 *   Slot for reading from stdin
 */
/*======================================================================*/
  void slotLoad();

/*======================================================================*/
/*!
 *   Slot for reading from a file
 */
/*======================================================================*/
  void slotLoadFromFile();

/*======================================================================*/
/*!
 *   Slot for writing to stdout
 */
/*======================================================================*/
  void slotSave();

/*======================================================================*/
/*!
 *   Slot for writing to a file
 */
/*======================================================================*/
  void slotSaveToFile();

/*======================================================================*/
/*!
 *   Slot for clearing the painting canvas
 */
/*======================================================================*/
  void slotClear();

/*======================================================================*/
/*!
 *   Slot for invoking an QInputDialog to set the picture's label
 */
/*======================================================================*/
  void slotSetLabel();

/*======================================================================*/
/*!
 *   Slot for invoking the about messagebox
 */
/*======================================================================*/
  void slotAbout();
};

#include "LetterPainter.icc"

#endif
