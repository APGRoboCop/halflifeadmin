/*
 * ===========================================================================
 *
 * $Id: encrypt.c,v 1.1.1.1 2003/08/14 18:51:28 bugblatter Exp $
 *
 *
 * Copyright (c) 1999-2003 Alfred Reynolds, Florian Zschocke, Magua
 *
 *   This file is part of Admin Mod.
 *
 *   Admin Mod is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   Admin Mod is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Admin Mod; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ===========================================================================
 *
 * Comments:
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "encrypt_l.h"


void print_usage( void ) {
  printf( "\nUsage:  encrypt [-h] [-V] [-c|-m] <password>\n\n" );
  printf( " Encrypts a cleartext password.\n\n" );
  printf( "  -h    --help            Display help screen and exit.\n");
  printf( "  -V    --version         Report version and exit.\n");
  printf( "  -c    --crypt (default) Use Un*x crypt() method for encryption.\n");
  printf( "  -m    --md5             Use MD5 hash for encryption.\n" );
  printf( "\n" );
}  // print_usage()

void print_version( void ) {
  printf("Admin Mod encrypt: version 0.1, Copyright (c) 2003 Florian Zschocke\n");
}  // print_version()



int main ( int argc, char** argv ) {

	const char* pcEncrypted;
	t_encrypt_method tEncMethod = EM_CRYPT;
	
	
	int c = 0;
	int iOptionIndex = 0;
	static struct option longOptions[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'V'},
		{"crypt", no_argument, 0, 'c'},
		{"md5", no_argument, 0, 'm'},
		{0,0,0,0}
	};
	
	
	// check parameters
	if ( argc < 2 ) {
		print_usage();
		exit (1);
	} // if
	
	//-- command line parsing
	
	
	if ( argc > 1 ) {
		
		while( (c = getopt_long( argc, argv, "hVcm", longOptions, &iOptionIndex)) != -1 ) {
			
			switch( c ) {
			case 0:
				fprintf( stderr, "Don't really know what to do now.\n" );
				break;
				
			case 'h':
				print_usage();
				exit(0);
				break;
				
			case 'V':
				print_version();
				exit(0);
				break;
				
			case 'c':
				tEncMethod = EM_CRYPT;
				break;

			case 'm':
				tEncMethod = EM_MD5;
				break;
				
			default:
				exit(1);
			}  // switch
		}  // while
	}  // if

	pcEncrypted = encrypt_password( argv[optind], tEncMethod );

	printf( "%s\n", pcEncrypted );
	
	return 0;
}
