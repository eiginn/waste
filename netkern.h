/*
    WASTE - netkern.h (Common network runtime code declarations)
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

#ifndef _NETKERN_H_
#define _NETKERN_H_

#include "itemlist.h"
#include "connection.h"
#include "mqueuelist.h"

extern C_ItemList<C_Connection> g_new_net;

#if	defined(_WIN32) || defined(QTUI)
extern W_ListView g_lvnetcons;
#endif


#if defined(_WIN32) && !defined(QTUI)
extern HWND g_netstatus_wnd;
#elif defined(QTUI)
#include "ui_listview.h"
#include <qwidget.h>
extern QWidget* g_netstatus_wnd;
#endif

void NetKern_Run();
void AddConnection(char *str, int port, int rating);
void DoPing(C_MessageQueue *mq);
void RebroadcastCaps(C_MessageQueueList *mql);
void NetKern_ConnectToHostIfOK(int ip, int port);

/* Added by LoraYin 2003/11/25 begin.*/
void SendSwitchToAESCaps(C_MessageQueue *mq);
/* Added by LoraYin 2003/11/25 end.*/

#if defined(_WIN32) && !defined(QTUI)
BOOL WINAPI Net_DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif


#endif//_NETKERN_H_
