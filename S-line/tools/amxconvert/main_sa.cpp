#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "getopt.h"
#include "amxconv.h"


void print_usage( void ) {
  printf( "\nUsage:  amxconvert [-h] [-r] [-w|-d|-l|-u] <amxfile> [outputfile]\n\n" );
  printf( " Converts an AMX file from Windows format \n to Linux format or vice versa.\n\n" );
  printf( "  -h    --help            Display this help\n");
  printf( "  -r    --report          Report only on the file format, don't convert\n");
  printf( "  -w -d --windows --dos   Convert input file to Win32 format\n" );
  printf( "  -l -u --linux --unix    Convert input file to Unix format\n" );
  printf( "\n" );
  printf( " When no conversion option is specified the input\n" );
  printf( " file will be converted to the respective format.\n" );
  printf( " If no output file is given, the input file gets\n" );
  printf( " overwritten.\n" );
  printf("\n");
  printf("Version 1.2 Copyright 2001-2003 Florian Zschocke\n");
}  // print_usage()

int main ( int argc, char** argv ) {

  // check parameters
  if ( argc < 2 ) {
	print_usage();
	exit (1);
  } // if

  //-- command line parsing

  const int OUT_LINUX = 1;
  const int OUT_WIN32 = 2;
  int outputFormat = 0;

  bool bReportOnly = false;

  if ( argc > 1 ) {
	int c = 0;
	int iOptionIndex = 0;
	static struct option longOptions[] = {
	  {"help", no_argument, 0, 'h'},
	  {"windows", no_argument, 0, 'w'},
	  {"linux", no_argument, 0, 'l'},
	  {"dos", no_argument, 0, 'd'},
	  {"unix", no_argument, 0, 'u'},
	  {"report", no_argument, 0, 'r'},
	  {0,0,0,0}
	};

	while( (c = getopt_long( argc, argv, "hwldur", longOptions, &iOptionIndex)) != -1 ) {

	  switch( c ) {
	  case 0:
		fprintf( stderr, "Don't really know what to do now.\n" );
		break;
		
	  case 'h':
		print_usage();
		exit(0);
		break;

	  case 'w':
	  case 'd':
		  if ( outputFormat == OUT_LINUX ) {
			  fprintf( stderr, "You have to specify either Win32 or Linux as the output format.\n" );
			  exit (1);
		  }
		  outputFormat = OUT_WIN32;
		  break;
	  case 'l':
	  case 'u':
		  if ( outputFormat == OUT_WIN32 ) {
			  fprintf( stderr, "You have to specify either Win32 or Linux as the output format.\n" );
			  exit (1);
		  }
		  outputFormat = OUT_LINUX;
		  break;

	  case 'r':
		  bReportOnly = true;
		  break;

	  default:
		exit(1);
	  }  // switch
	}  // while
  }  // if


  const char* psFilename = 0;
  const char* psOutfile = 0;

  if ( optind < argc ) {
	  psFilename = argv[optind];
	  if ( (optind+1) < argc ) {
		  psOutfile = argv[optind+1];
	  } else {
		  psOutfile = 0;
	  }  // if-else
  } else {
	  print_usage();
	  exit(1);
  }  // if-else


  long lFileSize = 0;

  // check file type
  int iFileType = check_file_type( psFilename, lFileSize );



  if ( bReportOnly ) {
	  // report file type
	  if ( iFileType == WIN32_AMX ) {
		  fprintf( stdout, "The file %s is a Windows amx file.\n", psFilename );
	  } else if ( iFileType == LINUX_AMX ) {
		  fprintf( stdout, "The file %s is a Linux amx file.\n", psFilename );
	  } else if ( iFileType == AMD64_AMX ) {
		  fprintf( stdout, "The file %s is a Linux AMD64 amx file.\n", psFilename );
	  } else {
		  fprintf( stdout, "The file %s is not a valid amx file.\n", psFilename );
	  }  // if-else

	  return 0;
  } else {
	  // report file action
	  if ( iFileType == WIN32_AMX ) {
		  if ( outputFormat == OUT_WIN32 ) {
			  fprintf( stdout, "File is already in Windows format. No conversion.\n" );
			  return 0;
		  }  // if
		  fprintf( stdout, "Converting from Windows to Linux format...\n" );
	  } else if ( iFileType == LINUX_AMX ) {
		  if ( outputFormat == OUT_LINUX ) {
			  fprintf( stdout, "File is already in Linux format. No conversion.\n" );
			  return 0;
		  }  // if
		  fprintf( stdout, "Converting from Linux to Windows format...\n" );
	  } else if ( iFileType == AMD64_AMX ) {
		  fprintf( stdout, "The file is a 64-bit file. It cannot be converted to one of the other formats.\n" );
		  return 0;
	  } else {
		  fprintf( stdout, "The file %s is not a valid amx file.\n", psFilename );
	  }  // if-else
  }  // if-else

  // convert file
  int result = CONV_ERR_FAIL;
  if ( lFileSize > 0 ) {
  result = convert_file( psFilename, psOutfile, iFileType, lFileSize );
  }  // if

  if ( result == CONV_SUCC ) {
	  fprintf( stdout, "success.\n" );
  } else {
	  fprintf( stdout, "failed.\n" );
  }  // if-else

  return 0;

}  // main()
