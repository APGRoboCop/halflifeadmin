
#include "osdep.h"



#ifdef QTGUI

void display_error( const char* _psMessage ) {

}  // display_error()

void display_error_win( const QString& _string ) {
    QMessageBox::critical( 0, "Error", _string, 1, 2 );
}  // display_error()

#else // 

#include <stdio.h>


void display_error( const char* _psMessage ) {
  fprintf( stderr, "%s\n", _psMessage );
}  // display_error()

#endif //if QTGUI
