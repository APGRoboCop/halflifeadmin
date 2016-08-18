#include "osdep.h"
#include "amxconv.h"
#include "amx_l.h"



int check_file_type( const char* _pcFilename, long& _iSize ) {

  AMX_LINUX_HEADER oLinHeader;
  int retval = INVAL_ERR;

  _iSize = 0;

  FILE* fpInFile = fopen( _pcFilename, "rb" );
  if ( fpInFile == 0 ) {
	  display_error( "Could not open input file." );
	  return retval;
  }  // if

  // read the header
  fread(&oLinHeader, sizeof(oLinHeader), 1, fpInFile);

  fclose( fpInFile );

  // check the type of the header
  retval = check_header_type( oLinHeader );

  if ( retval == INVAL_AMX_HDR ) {
	display_error( "The file has no valid AMX header." );
	return retval;
  }  // if

  if ( retval == INVAL_AMX_VER ) {
	display_error( "The amx file has the wrong version." );
	return retval;
  }  // if

  _iSize = oLinHeader.size;

  return retval;
}  // check_file_type()





int convert_file( const char* _psInfile, const char* _psOutfile, const int _iFiletype, long _iSize ) {

  bool bOverwriteFile = true;

  if ( _psInfile == 0 ) return CONV_ERR_INF;

  // open the input file 
  FILE* fpInFile = fopen( _psInfile, "rb" );
  if ( fpInFile == 0 ) {
	  display_error( "Could not open input file." );
	  return CONV_ERR_INF;
  }  // if

  FILE* fpOutfile = 0;

  // read file to memory
  char* pcMemFile = new char[_iSize];
  if ( pcMemFile != 0 ) {
	fread( pcMemFile, 1, _iSize, fpInFile );
  } else {
	display_error( "You are suffering form severe memory shortage.\nBuy a new computer." );
	return CONV_ERR_MEM;
  }  // if

  fclose( fpInFile );

 // if we cannot map the file to memory we need to use a tmp file
  // oh fudge it, that won't happen anyway

  AMX_LINUX_HEADER oLinHeader;
  AMX_WIN32_HEADER oWinHeader;
  AMX_LINUX_HEADER_V4 oLinHeaderV4;
  AMX_WIN32_HEADER_V4 oWinHeaderV4;
  char* pcCodeBlock;
  char cFileVersion;

  // get the file version
  cFileVersion = (reinterpret_cast<AMX_LINUX_HEADER*>(pcMemFile))->file_version;
  
  // process the file
  if ( _iFiletype == WIN32_AMX ) {  // Windows to Linux conversion

	// convert the header
	if ( cFileVersion == 4 ) {  // file version 4
	  pcCodeBlock = convert_header( pcMemFile, oLinHeaderV4 ); 
	} else {                    // file version 5
	  pcCodeBlock = convert_header( pcMemFile, oLinHeader );
	}  // if-else

  } else {  // Linux to Windows conversion

	// convert the header
	if ( cFileVersion == 4 ) {  // file version 4
	  pcCodeBlock = convert_header( pcMemFile, oWinHeaderV4 ); 
	} else {                    // file version 5
	  pcCodeBlock = convert_header( pcMemFile, oWinHeader ); 
	}  // if-else

  }  // if-else


  // check if we want to create a new file
  if ( _psOutfile != 0 && (strcmp(_psInfile, _psOutfile) != 0) ) {
	  // we want to create a new file
	  bOverwriteFile = false;
	  fpOutfile = fopen( _psOutfile, "wb" );
  } else {
	  // we want to overwrite the file
	  fpOutfile = fopen( _psInfile, "wb" );
  }  // if-else

  if ( fpOutfile == 0 ) {
	  display_error( "Could not create the output file." );
	  if ( pcMemFile != 0 ) delete[] pcMemFile;
	  pcMemFile = 0;
	  return CONV_ERR_OUT;
  }  // if


  // write ne new file the file
  if ( _iFiletype == WIN32_AMX ) {  // Windows to Linux conversion

	// write the header
	if ( cFileVersion == 4 ) {  // file version 4
	  fwrite( &oLinHeaderV4, 1, sizeof(oLinHeaderV4), fpOutfile );
	} else {                    // file version 5
	  fwrite( &oLinHeader, 1, sizeof(oLinHeader), fpOutfile );
	}  // if-else

  } else {  // Linux to Windows conversion

	// write the header
	if ( cFileVersion == 4 ) {  // file version 4
	  fwrite( &oWinHeaderV4, 1, sizeof(oWinHeaderV4), fpOutfile );
	} else {                    // file version 5
	  fwrite( &oWinHeader, 1, sizeof(oWinHeader), fpOutfile );
	}  // if-else

  }  // if-else


  // write the rest
  if ( pcMemFile != 0 ) {
	fwrite( pcCodeBlock, 1, (_iSize - (pcCodeBlock - pcMemFile)), fpOutfile );
  } else {
    fclose( fpOutfile );
	return CONV_ERR_FAIL;
  }  // if-else


  // close the outfile
  fclose( fpOutfile );

  delete[] pcMemFile;
  pcMemFile = 0;

  return 0;
}  // convert_file()


