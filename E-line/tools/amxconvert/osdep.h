#ifndef _OSDEP_H_
#define _OSDEP_H_


#ifdef QTGUI
#  include <qmessagebox.h>
#  include <qstring.h>
#endif

/*
 * This file declares various functions that are dependent on
 * the OS or other environment differences.
 *
 */

// Function display_error()
//
// Display an error message
//
void display_error( const char* );

// Function display_warning()
//
// Display an warning message
//
void display_warning( const char* );

#ifdef QTGUI
void display_error_win( const QString& );
#endif  // QTGUI



#endif
