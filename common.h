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
 *		common.h
 *
 *	\Brief
 *		Define common classes and macros used by MichaelZhu
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

#ifndef COMMON_H
#define COMMON_H

//application distribute version
//		-	MDEBUG=1, debug version
//		-	MDEBUG=0, release version
#define MDEBUG 0

//define MTRACE macro for debug
//	-	Add platform macro here, for:
//			Visual C++ 6.0 is poor in supporting current C++ standard library
//			and GCC 3.2.2 will warn without current C++ head file included
#if MDEBUG
	#ifdef _LINUX
		#include <iostream>
		#define MTRACE(info) std::cout<<"Michael - "<<info<<"\n"
	#elif defined(_WIN32)
		#include <iostream.h>
		#define MTRACE(info) cout<<"Michael - "<<info<<"\n"
	#endif	//_LINUX+_WIN32
#else
	#define	MTRACE(info)
#endif //MDEBUG

//application name
#ifndef APP_NAME
extern const char *APP_NAME;
#endif	//APP_NAME

//application version
#ifndef APP_VERSION
extern const char *APP_VERSION;
#endif	//APP_VERSION

#endif	//COMMON_H
