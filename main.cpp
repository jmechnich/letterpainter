/**************************************************************************
**       Title: letterpainter.cc
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

/*-------------------------------------------------------------------------
 * Creatinng the main application window 
 *-------------------------------------------------------------------------*/
using namespace std;

int main( int argc, char **argv )
{
  QApplication a( argc, argv );
  
  LetterPainter mainWindow;
  mainWindow.setCaption( "LetterPainter V1.0");
  a.setMainWidget( &mainWindow );
  mainWindow.show();
  return a.exec();
}
