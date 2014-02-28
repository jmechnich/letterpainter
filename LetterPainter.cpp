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

#include <QApplication>
#include <QPainter>
#include <QFileDialog>
#include <QInputDialog>
#include <QColor>
#include <QMenu>
#include <QMenuBar>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>

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
Canvas::Canvas( QWidget* parent)
        : QWidget( parent),
          p_parent( parent), pen( Qt::red, 1 ),
          mousePressed( FALSE ), p_label("-")
{
  setAttribute( Qt::WA_StaticContents);
  setBackgroundRole( QPalette::Base);
#ifndef QT_NO_CURSOR
  setCursor( Qt::CrossCursor );
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
    
    p_lineBuffer.push_back( tmp);
    update();
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
    QPolygon tmp_polyline(3);
    vector<vector<QPoint> >::iterator buffer_it = p_buffer.begin();
    
    for( ; buffer_it != p_buffer.end(); buffer_it++){
      vector<QPoint>::iterator line_it = (*buffer_it).begin();
      tmp_polyline[2] = tmp_polyline[1] = tmp_polyline[0] = *line_it;
      line_it++;
      for( ; line_it != (*buffer_it).end(); line_it++){
        QPainter painter(this);
        painter.setPen( pen );
        tmp_polyline[2] = tmp_polyline[1];
        tmp_polyline[1] = tmp_polyline[0];
        tmp_polyline[0] = *line_it;
        painter.drawPolyline( tmp_polyline);
        painter.end();
      }
    }
  }
  if( p_lineBuffer.size() != 0){
    QPolygon tmp_polyline(3);
    vector<QPoint >::iterator line_it = p_lineBuffer.begin();
    tmp_polyline[2] = tmp_polyline[1] = tmp_polyline[0] = *line_it;
    line_it++;
    for( ; line_it != p_lineBuffer.end(); line_it++){
      QPainter painter(this);
      painter.setPen( pen );
      tmp_polyline[2] = tmp_polyline[1];
      tmp_polyline[1] = tmp_polyline[0];
      tmp_polyline[0] = *line_it;
      painter.drawPolyline( tmp_polyline);
      painter.end();
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
    repaint();
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
    os << p_label.toLatin1().constData() << " " << p_buffer.size() << endl;
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
  p_lineBuffer.push_back( e->pos());
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:Canvas::mouseReleaseEvent( QMouseEvent*)
 *  ==> see headerfile
 *=======================================================================*/
void
Canvas::mouseReleaseEvent( QMouseEvent*)
{
  update();
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
  repaint();
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
LetterPainter::LetterPainter( QWidget* parent)
        :QWidget( parent)
{
  // left window Pane
  canvas = new Canvas( this );
  if( qApp->argc() == 2)
  {
    ifstream file( qApp->argv()[1]);
    canvas->read( file);
    file.close();
  }
  
  // right window pane
  QWidget* rightPane = new QWidget;

  // buttons
  bLoad         = new QPushButton( "Read stdin"  );
  bSave         = new QPushButton( "Write stdout");
  bClear        = new QPushButton( "Clear Screen");
  connect( bLoad,  SIGNAL( clicked()), this, SLOT( slotLoad()));
  connect( bSave,  SIGNAL( clicked()), this, SLOT( slotSave()));
  connect( bClear, SIGNAL( clicked()), this, SLOT( slotClear()));
  
  // label
  QWidget* wlabel = new QWidget;
  QLabel* label = new QLabel( "Label:");
  labelLine     = new QLineEdit( canvas->getLabel());
  labelLine->setMaxLength( 1);
  connect( canvas, SIGNAL( labelChanged( const QString &)),
           labelLine, SLOT( setText( const QString &)));

  QHBoxLayout* labelLayout = new QHBoxLayout();
  labelLayout->addWidget( label);
  labelLayout->addWidget( labelLine);
  wlabel->setLayout( labelLayout);
  
  QVBoxLayout* buttonLayout = new QVBoxLayout;
  buttonLayout->addWidget( bLoad);
  buttonLayout->addWidget( bSave);
  buttonLayout->addWidget( bClear);
  buttonLayout->addWidget( wlabel);
  rightPane->setLayout( buttonLayout);
  
  // menu bar
  QMenu *file = new QMenu( "&File"   , this);
  file->addAction( "&Open File"    , this, SLOT( slotLoadFromFile()),
                   Qt::CTRL+Qt::Key_O);
  file->addAction( "&Save File"    , this, SLOT( slotSaveToFile()  ),
                   Qt::CTRL+Qt::Key_S);
  file->addSeparator();
  file->addAction( "E&xit"         , qApp, SLOT( quit()), Qt::CTRL+Qt::Key_Q);
  
  QMenu *help = new QMenu( "&Help"   , this);
  help->addAction( "&About", this, SLOT( slotAbout()), Qt::CTRL+Qt::Key_H);
  
  QMenuBar *menu = new QMenuBar( this);
  menu->addMenu( file);
  menu->addSeparator();
  menu->addMenu( help);
  
  layout = new QHBoxLayout();
  layout->setMenuBar( menu);
  layout->addWidget( canvas);
  layout->addWidget( rightPane);
  setLayout( layout);
  
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
  QString filename = QFileDialog::getOpenFileName( this, "Open File...",
                                                   QString(), "*.acd");
  if( !filename.isNull())
  {
    ifstream file( filename.toLatin1().constData());
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
    QString filename = QFileDialog::getSaveFileName( this, "Save File...",
                                                     QString(), "*.acd" );
    if( !filename.isNull())
    {
      slotSetLabel();
      ofstream file( filename.toLatin1().constData());
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
