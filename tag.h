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
 * This file is for debug
 * Author: Raymond Cai
 * Date Created: 2003.12.1
*/
#ifndef _VASTE_TAG_H_
#define _VASTE_TAG_H_

#include <iostream>

//#define DEBUG
#ifdef DEBUG
	#define TRACE(info) std::cout << info << "\n"
#else
	#define TRACE(info)
#endif

//#define RDEBUG
#ifdef RDEBUG
	#define RTRACE(info) std::cout<<"Raymond - " << info <<"\n"
#else
	#define RTRACE(info)
#endif
class TAG
{
public:
	TAG(char *str);
	~TAG();
private:
	char buf[240];
};
#endif /* _VASTE_TAG_H_ */
