/*
 * ===========================================================================
 *
 * $Id: encrypt_l.h,v 1.1 2003/06/09 15:40:04 darope Exp $
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


#ifndef AM_ENCRYPT_L_H
#define AM_ENCRYPT_L_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { EM_UNDEF, EM_NONE, EM_CRYPT, EM_MD5 } t_encrypt_method;

const char* encrypt_password( const char* cleartext, t_encrypt_method ); 

#ifdef __cplusplus
}
#endif

#endif  /* AM_ENCRYPT_L_H */
