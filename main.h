/*
    WASTE - main.h (a bunch of global declarations and definitions)
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

#ifndef _MAIN_H_
#define _MAIN_H_

#define APP_NAME "PadLock SL"

#include "platform.h"

#include "listen.h"
#include "mqueuelist.h"
#include "filedb.h"
#include "config.h"
extern bool g_netaccept;
extern char *g_nameverstr;
extern char *g_def_extlist;

extern C_FileDB *g_database, *g_newdatabase;
extern C_AsyncDNS *g_dns;
extern C_Listen *g_listen;
extern C_MessageQueueList *g_mql;
extern C_Config *g_config;

extern char g_config_prefix[1024];
extern char g_config_mainini[1024];
extern char g_profile_name[128];

// cached config items
/* Added by LoraYin 2003.11.24 */
extern int g_aes_keysize;

extern int g_extrainf;
extern int g_keepup;
extern int g_conspeed,g_route_traffic;
extern int g_do_log;
extern int g_max_simul_dl;
extern unsigned int g_max_simul_dl_host;
extern int g_forceip, g_forceip_addr;
extern int g_use_accesslist;
extern int g_appendprofiletitles;
extern int g_do_autorefresh;
extern int g_accept_downloads;
extern int g_port;
extern int g_chat_timestamp;
extern int g_keydist_flags; // &4=route, &3= 2=prompt,1=all,0=ignore
extern char g_regnick[32];

extern char g_filedlg_ulpath[256];

extern int g_throttle_flag, g_throttle_send, g_throttle_recv;

extern int g_search_showfull,g_search_showfullbytes;

extern int g_scanloadhack;
extern char g_scan_status_buf[128];
extern time_t g_next_refreshtime;
extern T_GUID g_last_scanid;
extern int g_last_scanid_used;
extern T_GUID g_last_pingid;
extern int g_last_pingid_used;
extern T_GUID g_client_id;
extern char g_client_id_str[33];

extern unsigned char g_networkhash[SHA_OUTSIZE];
extern int g_use_networkhash;

extern int g_timeout; // timeout in seconds.

void main_MsgCallback(T_Message *message, C_MessageQueueList *_this, C_Connection *cn);
void doDatabaseRescan();
void update_set_port();
void main_onGotNick(char *nick, int del);
void main_onGotChannel(char *cnl);
void main_BroadcastPublicKey(T_Message *src=NULL);


// win32 specific shit
#if defined(_WIN32) && !defined(QTUI)
void handleWasteURL(char *url);

void RunKeyGen(HWND hwndParent, char *keyout);

#define WM_USER_TITLEUPDATE (WM_USER+0x11)
#define WM_USER_SYSTRAY (WM_USER+0x12)
#define WM_USER_PROFILECHECK (WM_USER+0x15)

extern int g_hidewnd_state;
extern HMENU g_context_menus;
extern HWND g_mainwnd;
extern HICON g_hSmallIcon;
extern HINSTANCE g_hInst;
void UserListContextMenu(HWND htree);
void UserListOnDropFiles(HWND hwndDlg, HWND htree, HDROP hdrop, char *forcenick);
#elif defined(_WIN32)
extern HINSTANCE g_hInst;

#endif//WIN32

#ifdef QTUI
extern QWidget* g_mainwnd;
#include "m_upload.h"
extern void main_handleUpload(char *guidstr, char *fnstr, C_UploadRequest *t);
#endif

#endif//_MAIN_H_

