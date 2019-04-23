/*
    WASTE - srchwnd.h (Search/browser dialogs and code)
    Copyright (C) 2003 Nullsoft, Inc.

    WASTE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    WASTE  is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WASTE; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
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

#ifndef _SRCHWND_H_
#define _SRCHWND_H_

#include "mqueue.h"

#define PARENT_DIRSTRING ".. (parent directory)"


// do not change these, heh, unless all clients on the network do :)
#define DIRECTORY_STRING "Directory"
#define USER_STRING "User"

#define SEARCHCACHE_NUMITEMS 16

class SearchCacheItem
{
public:

  SearchCacheItem() {
    lastsearchtext[0]=0;
    search_id_time=0;
    memset(&search_id,0,sizeof(search_id));
    numcons=0;
    lastvisitem=~0;
  }
  C_ItemList<C_MessageSearchReply> searchreplies;
  char lastsearchtext[512];
  T_GUID search_id;
  unsigned int search_id_time;
  int numcons;
  unsigned int lastvisitem;
};

extern SearchCacheItem *g_searchcache[SEARCHCACHE_NUMITEMS];
void Search_Search(char *str);


void Search_Resort();
void Search_AddReply(T_Message *message);

void FormatSizeStr64(char *out, unsigned int low, unsigned int high);

#if defined(_WIN32) && !defined(QTUI)
BOOL WINAPI Search_DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern HWND g_search_wnd;
#endif

#ifdef QTUI
extern QWidget* g_search_wnd;
#endif

#endif//_SRCHWND_H_
