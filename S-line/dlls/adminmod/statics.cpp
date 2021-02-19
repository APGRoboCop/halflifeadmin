/*
 * ===========================================================================
 *
 * $Id: statics.cpp,v 1.3 2003/03/26 20:44:09 darope Exp $
 *
 *
 * Copyright (c) 2002-2003 Florian Zschocke
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
 *   In addition, as a special exception, the author gives permission to
 *   link the code of this program with the Half-Life Game Engine ("HL 
 *   Engine") and Modified Game Libraries ("MODs") developed by VALVe, 
 *   L.L.C ("Valve") and Modified Game Libraries developed by Gearbox 
 *   Software ("Gearbox").  You must obey the GNU General Public License 
 *   in all respects for all of the code used other than the HL Engine and 
 *   MODs from Valve or Gearbox. If you modify this file, you may extend 
 *   this exception to your version of the file, but you are not obligated 
 *   to do so.  If you do not wish to do so, delete this exception statement
 *   from your version.
 *
 * ===========================================================================
 *
 * Comments:
 *
 */

#define DEFECLIST
#define DEFSTATLIST
#include "statics.h"


apat eclist[] = {
{0,6,"\xc2\xd8\xd2\xc2\xe6\x00",0,{0,1}},
{2,9,"\xe0\xd8\xc2\xf2\xc8\xca\xda\xde\x00",0,{2,32}},
{3,11,"\xd0\xe0\xd6\xca\xf0\xe8\xe4\xc2\xc6\xe8\x00",0,{3,23}},
{4,6,"\xc8\xca\xda\xde\xe6\x00",0,{4,21}},
{5,18,"\xc6\xd8\xbe\xc2\xd8\xd8\xde\xee\xbe\xc8\xde\xee\xdc\xd8\xde\xc2\xc8\x00",0,{5,8}},
{10,11,"\xc2\xe0\xe0\xca\xdc\xc8\xc8\xca\xda\xde\x00",0,{6,7}},
{20,12,"\xc2\xd8\xd8\xde\xee\xea\xe0\xd8\xde\xc2\xc8\x00",0,{0,6}},
{12,9,"\xc2\xea\xe8\xde\xe6\xc2\xec\xca\x00",0,{5,7}},
{6,5,"\xc4\xd2\xdc\xc8\x00",1,{8,9}},
{10,15,"\xc6\xea\xe6\xe8\xde\xda\xe4\xe6\xe4\xc6\xd8\xd2\xe6\xe8\x00",0,{10,20}},
{14,4,"\xc6\xda\xc8\x00",0,{11,19}},
{17,11,"\xc6\xd8\xd2\xca\xdc\xe8\xe0\xde\xe4\xe8\x00",0,{12,11}},
{28,19,"\xc6\xd8\xbe\xc8\xde\xee\xdc\xd8\xde\xc2\xc8\xbe\xd2\xdc\xce\xc2\xda\xca\x00",0,{13,16}},
{65,17,"\xc6\xd8\xbe\xc2\xd8\xd8\xde\xee\xc8\xde\xee\xdc\xd8\xde\xc2\xc8\x00",0,{14,15}},
{74,16,"\xc6\xd8\xbe\xc2\xd8\xd8\xde\xee\xbe\xea\xe0\xd8\xde\xc2\xc8\x00",0,{4,14}},
{66,15,"\xc6\xd8\xbe\xc2\xd8\xd8\xde\xee\xea\xe0\xd8\xde\xc2\xc8\x00",0,{13,15}},
{29,6,"\xc6\xd8\xbe\xce\xce\x00",0,{17,16}},
{89,20,"\xc6\xd8\xbe\xc8\xde\xee\xdc\xd8\xde\xc2\xc8\xd2\xdc\xe8\xca\xe4\xec\xc2\xd8\x00",0,{18,17}},
{100,16,"\xc6\xd8\xbe\xc8\xde\xee\xdc\xd8\xde\xc2\xc8\xbe\xda\xc2\xf0\x00",0,{12,18}},
{24,8,"\xc6\xda\xc8\xd8\xd2\xe6\xe8\x00",0,{10,19}},
{13,9,"\xc6\xec\xc2\xe4\xd8\xd2\xe6\xe8\x00",0,{9,20}},
{5,3,"\xce\xce\x00",0,{3,22}},
{11,7,"\xce\xd8\xbe\xd8\xde\xce\x00",0,{21,22}},
{4,9,"\xd8\xd2\xe6\xe8\xc8\xca\xda\xde\x00",0,{24,30}},
{6,3,"\xd2\xe0\x00",0,{25,28}},
{26,10,"\xd0\xe0\xd6\xe4\xca\xda\xde\xec\xca\x00",0,{26,27}},
{27,8,"\xd0\xe0\xd6\xd8\xd2\xe6\xe8\x00",0,{2,26}},
{28,7,"\xd0\xe0\xd6\xec\xc2\xd8\x00",0,{25,27}},
{16,14,"\xd2\xe0\xbe\xc6\xd8\xd2\xca\xdc\xe8\xe0\xde\xe4\xe8\x00",0,{24,29}},
{17,15,"\xd2\xe0\xf0\xbe\xc6\xd8\xd2\xca\xdc\xe8\xe0\xde\xe4\xe8\x00",0,{28,29}},
{12,5,"\xd8\xde\xc2\xc8\x00",0,{23,31}},
{32,11,"\xd8\xde\xc2\xc8\xc2\xe6\x70\xc4\xd2\xe8\x00",0,{30,31}},
{4,9,"\xe8\xd2\xda\xca\xc8\xca\xda\xde\x00",0,{33,49}},
{5,5,"\xe4\xc6\xde\xdc\x00",0,{34,36}},
{13,10,"\xe0\xde\xd2\xdc\xe8\xcc\xd2\xd8\xca\x00",0,{1,35}},
{18,5,"\xe0\xde\xe4\xe8\x00",0,{34,35}},
{6,12,"\xe6\xca\xdc\xe6\xd2\xe8\xd2\xec\xd2\xe8\xf2\x00",0,{37,43}},
{12,7,"\xe4\xca\xc6\xde\xe4\xc8\x00",0,{38,41}},
{32,13,"\xe4\xc6\xde\xdc\xbe\xc2\xc8\xc8\xe4\xca\xe6\xe6\x00",0,{33,39}},
{42,14,"\xe4\xc6\xde\xdc\xbe\xe0\xc2\xe6\xe6\xee\xde\xe4\xc8\x00",0,{38,40}},
{51,10,"\xe4\xc6\xde\xdc\xbe\xe0\xde\xe4\xe8\x00",0,{39,40}},
{18,13,"\xe4\xca\xe6\xde\xea\xe4\xc6\xca\xd8\xd2\xe6\xe8\x00",0,{42,41}},
{19,11,"\xe4\xca\xda\xde\xec\xca\xc8\xca\xda\xde\x00",0,{37,42}},
{10,11,"\xe6\xe8\xc2\xe4\xe8\xc8\xca\xda\xde\xe6\x00",0,{44,46}},
{11,10,"\xe6\xde\xea\xdc\xc8\xd8\xd2\xe6\xe8\x00",0,{45,44}},
{18,12,"\xe6\xca\xe8\xc8\xca\xda\xde\xd2\xdc\xcc\xde\x00",0,{36,45}},
{13,9,"\xe6\xee\xc2\xe0\xc8\xca\xda\xde\x00",0,{47,46}},
{19,9,"\xe6\xe8\xde\xe0\xc8\xca\xda\xde\x00",0,{48,47}},
{43,11,"\xe6\xe8\xc2\xe4\xe8\xda\xde\xec\xd2\xca\x00",0,{43,48}},
{5,7,"\xec\xde\xd8\xea\xda\xca\x00",0,{50,49}},
{6,10,"\xea\xdc\xc4\xd2\xdc\xc8\xc2\xd8\xd8\x00",0,{32,51}},
{10,7,"\xea\xe0\xd8\xde\xc2\xc8\x00",0,{50,51}},
};

unsigned char statstr_table[][2] = {
  {115,32}, {110,32}, {100,32}, {104,97}, {128,131}, {101,32}, {116,104}, {37,132}, {111,129}, {114,32}, {101,129}, {105,128}, {97,98}, {108,101}, {132,130}, {141,130},
  {136,134}, {100,133}, {46,10}, {109,10}, {135,142}, {140,143}, {111,145}, {149,144}, {101,147}, {109,150}, {151,152}, {115,101}, {108,105}, {99,156}, {116,111}, {101,110},
  {105,110}, {135,128}, {101,137}, {101,138}, {158,32}, {98,163}, {118,162}, {161,165}, {114,166}, {155,168}, {109,97}, {109,170}, {111,171}, {99,172}, {173,110}, {97,108},
  {112,111}, {105,115}
};


char* statstr[] = {
  "\224go\202\231\237\232",
  "\224go\202\231d\261\232",
  "\224no\235p \231\237\232",
  "\224no\235p \231d\261\232",
  "Th\205\251adm\240\261tra\236\211\213runn\240g \206\213\256\202fo\211you: \012",
  "Sorry, \206\205\251\213un\214l\205\244execut\205\256\202'%s' \210t\203t \235\237t\222",
  "\247te\215\260rted\222",
  "\247giv\212%i he\257\206 \260\240ts\222",
  "\247tak\212%i he\257\206 \260\240ts\222",
  "** Th\213\251\213\233t \244\257low a\201admi\201\244ru\201\256d\200\210you\211\235\237t. **\012",nullptr

};

