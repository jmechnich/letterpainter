/**************************************************************************
**       Title: LetterPainter.cc
**    $RCSfile: $
**   $Revision: $$Name:  $
**       $Date: $
**   Copyright: GPL $Author: $
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

#include "LetterPainter.h"

#include <qapplication.h>
#include <qpainter.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qmessagebox.h>

#include <iostream>
#include <fstream>
#include <sstream>

#ifndef WIN32
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif	// #ifndef WIN32

/*-------------------------------------------------------------------------
 *  This function eats comments and whitespaces in istreams
 *-------------------------------------------------------------------------*/
istream& eatWsAndComments( istream& s)
{
  s >> ws; // eat whitespaces
  
  while( s.peek() == '#')
  { 
    s.ignore( 10000, '\n');  // ignore characters until line end
    s >> ws;                 // eat whitespaces
  }
  return s;
}

/*-------------------------------------------------------------------------
 *  The Canvas class
 *-------------------------------------------------------------------------*/
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:Canvas::Canvas( QWidget* parent, const char* name)
 *  ==> see headerfile
 *=======================================================================*/
Canvas::Canvas( QWidget* parent, const char* name)
        : QWidget( parent, name, WNorthWestGravity ),
          p_parent( parent), pen( Qt::red, 1 ), polyline(3),
          mousePressed( FALSE ), p_label("-")
{
  setBackgroundMode( QWidget::PaletteBase);
#ifndef QT_NO_CURSOR
  setCursor( Qt::crossCursor );
#endif
  setFixedSize( 200, 200);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:Canvas::mouseMoveEvent( QMouseEvent* e)
 *  ==> see headerfile
 *=======================================================================*/
void
Canvas::mouseMoveEvent( QMouseEvent* e)
{
  if ( mousePressed ) {
    QPoint tmp = e->pos();
    int x = tmp.x() < 200 ?
        tmp.x() : 199;
    x = x < 0 ?
        0 : x;
    int y = tmp.y() < 200 ?
        tmp.y() : 199;
    y = y < 0 ?
        0 : y;
    tmp = QPoint( x, y);
    
    if( !( polyline[0] == tmp))
    {
      QPainter painter;
      painter.begin( this);
      painter.setPen( pen );
      polyline[2] = polyline[1];
      polyline[1] = polyline[0];
      polyline[0] = tmp;
      painter.drawPolyline( polyline);
      painter.end();
      p_lineBuffer.push_back( tmp);
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:Canvas::paintEvent( QPaintEvent* e)
 *  ==> see headerfile
 *=======================================================================*/
void
Canvas::paintEvent( QPaintEvent* e)
{
  QWidget::paintEvent( e );

  if( p_buffer.size() != 0){
    QPointArray tmp_polyline(3);
    vector<vector<QPoint> >::iterator buffer_it = p_buffer.begin();
    
    for( ; buffer_it != p_buffer.end(); buffer_it++){
      vector<QPoint>::iterator line_it = (*buffer_it).begin();
      tmp_polyline[2] = tmp_polyline[1] = tmp_polyline[0] = *line_it;
      line_it++;
    
      for( ; line_it != (*buffer_it).end(); line_it++){
        QPainter painter;
        painter.begin( this);
        painter.setPen( pen );
        tmp_polyline[2] = tmp_polyline[1];
        tmp_polyline[1] = tmp_polyline[0];
        tmp_polyline[0] = *line_it;
        painter.drawPolyline( tmp_polyline);
        painter.end();
      }
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:Canvas::read( istream& is)
 *  ==> see headerfile
 *=======================================================================*/
void
Canvas::read( istream& is)
{
  if( !is.eof())
  {
    char label;
    is >> eatWsAndComments >> label;
    p_label = label;
    emit labelChanged( p_label);
    
    unsigned int numberOfLines;
    is >> eatWsAndComments >> numberOfLines;
      
    p_buffer.clear();
      
    for( unsigned int lineCount = 0; lineCount < numberOfLines;
         lineCount++){
      
      p_lineBuffer.clear();
      
      unsigned int numberOfPoints;
      is >> eatWsAndComments >> numberOfPoints;
      
      for( unsigned int pointCount = 0;
           pointCount < numberOfPoints; pointCount++){
        
        int x, y;
        char trash;
        
        is >> eatWsAndComments >> trash;
        is >> eatWsAndComments >> x;
        is >> eatWsAndComments >> y;
        is >> eatWsAndComments >> trash;
        p_lineBuffer.push_back( QPoint( x, 199-y));
      }
      p_buffer.push_back( p_lineBuffer);
    }
    is >> eatWsAndComments;
    repaint(TRUE);
  }
  else
  {
    QMessageBox::warning( this, "LetterPainter", "EOF reached !");
  }
}   
  
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
Canvas::write( ostream &os)
{
  if( p_buffer.size() != 0)
  {
    os << p_label.latin1() << " " << p_buffer.size() << endl;
    vector<vector<QPoint> >::iterator buffer_it = p_buffer.begin();
    
    for( ; buffer_it != p_buffer.end(); buffer_it++)
    {
      vector<QPoint>::iterator line_it = (*buffer_it).begin();
      os << (*buffer_it).size() << " ";
      
      for( ; line_it != (*buffer_it).end(); line_it++)
      {
        os << "(" << (*line_it).x() << " " << (199 - (*line_it).y()) << ") ";
      }
      os << endl;
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
Canvas::mousePressEvent( QMouseEvent* e)
{
  mousePressed = TRUE;
  polyline[2] = polyline[1] = polyline[0] = e->pos();
  p_lineBuffer.push_back( e->pos());
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:Canvas::mouseReleaseEvent( QMouseEvent*)
 *  ==> see headerfile
 *=======================================================================*/
void
Canvas::mouseReleaseEvent( QMouseEvent*)
{
  mousePressed = FALSE;
  if( p_lineBuffer.size() > 1)
  {
    p_buffer.push_back( p_lineBuffer);
    p_lineBuffer.clear();
  }
  p_lineBuffer.clear();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:Canvas::clearScreen()
 *  ==> see headerfile
 *=======================================================================*/
void
Canvas::clearScreen()
{
  p_buffer.clear();
  repaint( TRUE);
  p_label = "-";
  emit labelChanged( "-");
}

/*-------------------------------------------------------------------------
 *  The LetterPainter class
 *-------------------------------------------------------------------------*/
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LetterPainter::LetterPainter( QWidget* parent,
 *                                                        const char* name)
 *  ==> see headerfile
 *=======================================================================*/
LetterPainter::LetterPainter( QWidget* parent, const char* name)
        :QWidget( parent, name)
{
  canvas = new Canvas( this );

  if( qApp->argc() == 2)
  {
    ifstream file( qApp->argv()[1]);
    canvas->read( file);
    file.close();
  }
  
  QPopupMenu *file = new QPopupMenu( this);
  CHECK_PTR( file);
  
  file->insertItem( "&Open File"    , this, SLOT( slotLoadFromFile()),
                    CTRL+Key_O);
  file->insertItem( "&Save File"    , this, SLOT( slotSaveToFile()  ),
                    CTRL+Key_S);
  file->insertSeparator();
  file->insertItem( "E&xit"         , qApp, SLOT( quit()), CTRL+Key_Q);
  
  QPopupMenu *help = new QPopupMenu( this);
  CHECK_PTR( help);

  help->insertItem( "&About", this, SLOT( slotAbout()), CTRL+Key_H);
  
  QMenuBar *menu = new QMenuBar( this);
  CHECK_PTR( menu);

  menu->insertItem( "&File"   , file);
  menu->insertSeparator();
  menu->insertItem( "&Help"   , help);
  menu->setSeparator( QMenuBar::InWindowsStyle);
  
  layout = new QHBoxLayout( this);
  layout->setMenuBar( menu);
  layout->addWidget( canvas);
  
  bLoad         = new QPushButton( "Read stdin"    , this);
  bSave         = new QPushButton( "Write stdout"  , this);
  bClear        = new QPushButton( "Clear Screen"  , this);
  
  connect( bLoad, SIGNAL( clicked()), this, SLOT( slotLoad()));
  connect( bSave, SIGNAL( clicked()), this, SLOT( slotSave()));
  connect( bClear, SIGNAL( clicked()), this, SLOT( slotClear()));
    
  QVBoxLayout* buttons = new QVBoxLayout( layout);
  buttons->addWidget( bLoad);
  buttons->addWidget( bSave);
  buttons->addWidget( bClear);
  
  QHBoxLayout* labelLayout = new QHBoxLayout( buttons);
  
  QLabel* label = new QLabel( "Label:", this);
  
  labelLine     = new QLineEdit( canvas->getLabel(), this);
  labelLine->setMaxLength( 1);
  connect( canvas, SIGNAL( labelChanged( const QString &)),
           labelLine, SLOT( setText( const QString &)));

  labelLayout->addWidget( label);
  labelLayout->addWidget( labelLine);
  
  setFixedSize( 280, 200);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LetterPainter::slotLoad()
 *  ==> see headerfile
 *=======================================================================*/
void
LetterPainter::slotLoad()
{

#ifndef WIN32
  fd_set rfds;
  struct timeval tv;
  int retval;

  FD_ZERO(&rfds);
  FD_SET(0, &rfds);

  tv.tv_sec = 2;
  tv.tv_usec = 0;
  
  retval = select(1, &rfds, NULL, NULL, &tv);
  
  if (retval)
  {
    canvas->read( cin);
  }
  else
  {
    QMessageBox::warning( this, "LetterPainter", "Stdin timed out !");
  }

#endif	// #ifndef WIN32

#ifdef WIN32
  canvas->read( cin);
#endif	// #ifdef WIN32
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LetterPainter::slotLoadFromFile()
 *  ==> see headerfile
 *=======================================================================*/
void
LetterPainter::slotLoadFromFile()
{
  QString filename = QFileDialog::getOpenFileName( QString::null,
                                                   "*.acd", this );
  if( !filename.isNull())
  {
    ifstream file( filename.latin1());
    canvas->read( file);
    file.close();
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LetterPainter::slotSave()
 *  ==> see headerfile
 *=======================================================================*/
void
LetterPainter::slotSave()
{
  slotSetLabel();
  canvas->write( std::cout);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LetterPainter::slotSaveToFile()
 *  ==> see headerfile
 *=======================================================================*/
void
LetterPainter::slotSaveToFile()
{
  if( canvas->isNotEmpty())
  {
    QString filename = QFileDialog::getSaveFileName( QString::null,
                                                     "*.acd", this );
    if( !filename.isNull())
    {
      slotSetLabel();
      ofstream file( filename.latin1());
      canvas->write( file);
      file.close();
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LetterPainter::slotClear()
 *  ==> see headerfile
 *=======================================================================*/
void 
LetterPainter::slotClear()
{
  canvas->clearScreen();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LetterPainter::slotSetLabel()
 *  ==> see headerfile
 *=======================================================================*/
void
LetterPainter::slotSetLabel()
{
  labelLine->selectAll();
  canvas->setLabel( labelLine->text());
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LetterPainter::slotAbout()
 *  ==> see headerfile
 *=======================================================================*/
void
LetterPainter::slotAbout()
{
  QMessageBox::about( this, "LetterPainter",
                      "LetterPainter V1.0 by Joerg Mechnich");
}
