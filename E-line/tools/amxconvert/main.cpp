#include <qapplication.h>
#include "amxconverterdialog.h"


int main( int argc, char** argv )
{
	QApplication app( argc, argv );

	amxConverterDialog dialog( 0, 0, TRUE );
	app.setMainWidget(&dialog);


 
	dialog.exec();

	return 0;
}

