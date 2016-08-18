#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qabstractlayout.h>
#include "amxconverterdialog.h"
#include "osdep.h"
#include <string.h>
#include <errno.h>

amxConverterDialog::amxConverterDialog( QWidget* parent, const char* name, bool modal, WFlags f )
	: amxConverterDialogBase( parent, name, modal, f )
{

	// Add your code
    connect( QuitButton, SIGNAL( clicked() ), this, SLOT( close() ) );
    HideConvInfo();
	labelSuccess->hide();
	bSuccessVisible = false;
    psInFilename = 0;
    psOutFilename = 0;
    FileType = INVAL_AMX;
}


void amxConverterDialog::HideConvInfo( void ) {
    QLayoutIterator it = ConvertInfo->iterator();
    QLayoutItem* item = 0;
    while ( (item = it.current()) ) {
        if ( item->widget() ) item->widget()->hide();
        ++it;
    }  // while
}


void amxConverterDialog::ShowConvInfo( void ) {
    QLayoutIterator it = ConvertInfo->iterator();
    QLayoutItem* item = 0;
    while ( (item = it.current()) ) {
        if ( item->widget() ) item->widget()->show();
        ++it;
    }  // while
}




void amxConverterDialog::SetInFile( void ) {

    QString filename = QFileDialog::getOpenFileName( "", "*.amx;;*.*", this);
    if ( !filename.isEmpty() ) {
        InputFile->setText( filename );
    }  // if
}


void amxConverterDialog::SetOutFile( void ) {

    QString filename = QFileDialog::getSaveFileName( QString::null, "*.amx;;*.*", this);
    if ( !filename.isEmpty() ) {
        OutputFile->setText( filename );
    }  // if
}


void amxConverterDialog::CheckAmxFile( const QString& _Filename ) {

    FileInfo.setFile( _Filename );
	if ( bSuccessVisible ) labelSuccess->hide();
    if ( FileInfo.exists() ) {
       
        InFilename = FileInfo.absFilePath().latin1();
        psInFilename = InFilename;
		
		
        FileType = check_file_type( psInFilename, FileSize );
		
        
        if ( FileType == INVAL_AMX ) {
            SetIsConvertable( false, FileType );
            psInFilename = 0;
		} else if ( FileType == INVAL_ERR ) {
	        display_error_win( QString("Could not open input file %1.").arg(psInFilename) );
            psInFilename = 0;
        } else {
            SetIsConvertable( true, FileType );
            FileSize = FileInfo.size();
        }  // if-else
		
    }  // if
	
}


void amxConverterDialog::SetIsConvertable( bool _bIsConvertable, int _iFileType ) {

    if ( _bIsConvertable ) {
        if ( _iFileType == LINUX_AMX ) {
            labelFrom->setText( "<font color=\"purple\" size=\"+2\">Linux</font>" );
            labelTo->setText( "<font color=\"blue\" size=\"+2\">Windows</font>" );
        } else {
            labelFrom->setText( "<font color=\"blue\" size=\"+2\">Windows</font>" );
            labelTo->setText( "<font color=\"purple\" size=\"+2\">Linux</font>" );
        }  // if-else
        ShowConvInfo();
        ConvertButton->setEnabled( true );
    } else {
        HideConvInfo();
        ConvertButton->setEnabled( false );
    }  // if-else

}


void amxConverterDialog::ConvertFile( void ) {

    if ( !FileInfo.exists()
        || ( psInFilename == 0 )
        || ( FileType == INVAL_AMX )
        || ( FileSize <= 50 ) ) {
        return;
    }  // if


    if ( OutputFile->text().isEmpty() ) {
        psOutFilename = 0;
    } else {
        QString path = OutputFile->text();
        psOutFilename = path.latin1();
    }  // if-else

    int result = convert_file( psInFilename, psOutFilename, FileType, FileSize );


    if ( result == 0 ) {
        ClearAll();
		labelSuccess->show();
		bSuccessVisible = true;
    } else if ( result == CONV_ERR_OUT ) {
        display_error_win( QString( "Output file %1 could not be written." ).arg(psOutFilename) );
        return;
    } else if ( result == CONV_ERR_INF ) {
        display_error_win( QString("Could not open input file %1.").arg(psInFilename) );
        return;
    } else if ( result == CONV_ERR_MEM ) {
        display_error_win( "You have severer memory problems.\nBuy a new computer." );
        return;
     } else { 
        display_error_win( "An unhandled error occured. Dump this stupid program." );
     }  // if-else


}


void amxConverterDialog::ClearAll( void ) {

    InputFile->clear();
    OutputFile->clear();
    ConvertButton->setDisabled( true );
    HideConvInfo();

}
