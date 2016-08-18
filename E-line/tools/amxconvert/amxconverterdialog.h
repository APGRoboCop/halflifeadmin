#include <qfileinfo.h>
#include <qstring.h>
#include <stdio.h>
#include "amxconverterdialogbase.h"
#include "amxconv.h"


class amxConverterDialog : public amxConverterDialogBase
{
	Q_OBJECT
public:
	amxConverterDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags f = 0 );


public slots:
    virtual void CheckAmxFile( const QString& );
    virtual void SetInFile();
    virtual void SetIsConvertable(bool,int);
    virtual void SetOutFile();
    virtual void ConvertFile();
    virtual void ClearAll();

    
protected:
    virtual void HideConvInfo( void );
    virtual void ShowConvInfo( void );

private:
	bool bSuccessVisible;
    QFileInfo FileInfo;
    QCString InFilename;
    QCString OutFilename;
    const char* psInFilename;
    const char* psOutFilename;
    int FileType;
    long FileSize;
};

