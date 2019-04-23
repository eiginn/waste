/*
  Copyright (c) 1998-2003 VIA Technologies, Inc.

  This program is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTIES OR REPRESENTATIONS; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
*/
/*
 *******************************************************************************
 *
 *	\Program
 *		PadLockSL
 *
 *	\Author
 *		MichaelZhu
 *
 *	\File
 *		prefinterface.h
 *
 *	\Brief
 *		Define common interface for preference set window
 *
 *	\Code
 *		Date		Version	Author		Description
 *		--------	----	------------	------------------------
 *		03/12/31	1.0	MichaelZhu	Implement all functions
 *		04/02/05	1.0	MichaelZhu	Remove all and note debug info
 *	\End code
 *
 *	\Platform 1:
 *		\Environment
 *			RedHat Linux 9
 *		\Compiler
 *			gcc 3.2.2
 *			QT/X11 3.2.3
 *	\Platform 2:
 *		\Environment
 *			Windows 2000
 *		\Compiler
 *			Visual C++ 6.0
 *			QT/commercial 3.0.1
 *
 *******************************************************************************
*/

#ifndef PREFINTERFACE_H
#define PREFINTERFACE_H

#ifdef _WIN32
#include <windows.h>
#endif	//_WIN32

#include <stdio.h>
	extern "C" {
#include "rsa/r_random.h"
#include "rsa/rsaref.h"
#include "C5P.h"
	};

//eject password input dialog
//		-	return true if accept dialog, and save password to bufPassword
//		-	reutrn false if reject dialog
bool getPassword(char *bufPassword);

//warn password
void warnPassword(const char *warningString);

//show profile manager on startup
bool showProfileManager(bool launchEnabled=true);

//profile setup wizard
//		-	return true if successed
//		-	return false if failed
bool initializeProfile();

//generate key
void generateKey(char *keyout);

//thread to create key
#ifdef _LINUX
void *kgThread(void *);
#elif defined(_WIN32)
DWORD WINAPI kgThread(LPVOID p);
#endif	//_LINUX+_WIN32

//save private key to file
int savePrivateKey(char *fn, R_RSA_PRIVATE_KEY *key, R_RANDOM_STRUCT *rnd, char *passhash);

//write buf data to file
void writeBFdata(FILE *out, int *key, int keylen,void *data, unsigned int len, unsigned long *cbcl, unsigned long *cbcr, int *lc);
//copy content of a existing file to a file
void CopyFile(const char *src, const char *dest);

//copy public key
void copyMyPubKey();

//import public keys
void importPublicKeys();

void getProfilePath(char *tmp);

#endif	//PREFINTERFACE_H
