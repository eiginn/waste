/*
WASTE - srchwnd.cpp (Search/browser dialogs and code)
Copyright (C) 2003 Nullsoft, Inc.
Copyright (C) 2004 WASTE Development Team

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

#include "stdafx.hpp"

#include "main.hpp"
#include "srchwnd.hpp"
#include "xferwnd.hpp"

#if defined(_WIN32)&&(!defined(_DEFINE_SRV)) || defined(_DEFINE_WXUI)
	#include "childwnd.hpp"
	#include "d_chat.hpp"
	#include "resource.hpp"
#endif

#include "filedb.hpp"
#include "m_chat.hpp"

#define MAX_SRCHTEXTS 15
#define SEARCHCACHE_EXPIRE_TIME (5*60*1000) // 5 minutes

#if defined(_WIN32)&&(!defined(_DEFINE_SRV))
	HWND g_search_wnd;
#endif
#if defined(_WIN32)&&(!defined(_DEFINE_SRV)) || defined(_DEFINE_WXUI)
	static W_ListView g_lvsearchres;
#endif

#if defined(_DEFINE_WXUI)
#include <wx/filedlg.h>
#include <wx/clipbrd.h>
#include "wxbrowser.h"
#include "wxappresources.h"

WxBrowser *g_search_wnd;

void srchwnd_find_wxwidgets(WxBrowser *dlg)
{
	g_search_wnd = dlg;

	g_lvsearchres.setwnd((wxListCtrl*) g_search_wnd->m_searchres);

	g_lvsearchres.AddCol("Item",g_config->ReadInt(CONFIG_search_col1,CONFIG_search_col1_DEFAULT));
	g_lvsearchres.AddCol("Size",g_config->ReadInt(CONFIG_search_col2,CONFIG_search_col2_DEFAULT));
	g_lvsearchres.AddCol("Type",g_config->ReadInt(CONFIG_search_col3,CONFIG_search_col3_DEFAULT));
	g_lvsearchres.AddCol("Date/Time",g_config->ReadInt(CONFIG_search_col6,CONFIG_search_col6_DEFAULT));
	g_lvsearchres.AddCol("Speed",g_config->ReadInt(CONFIG_search_col4,CONFIG_search_col4_DEFAULT));
	g_lvsearchres.AddCol("Location",g_config->ReadInt(CONFIG_search_col5,CONFIG_search_col5_DEFAULT));
	{
		int x;
		for (x = 0 ; x < MAX_SRCHTEXTS; x ++) {
			char buf[123];
			sprintf(buf,"srchcb_%d",x);
			const char *o=g_config->ReadString(buf,"");
			if (*o) {
				char buf[512];
				safe_strncpy(buf,o,sizeof(buf));
				wxString unibuf = cstr_to_wxstr(buf);
				g_search_wnd->m_searchtext->Append(unibuf);
			};
		};
		wxEnableWindow(g_search_wnd->m_search,0);
		wxEnableWindow(g_search_wnd->m_goparent,0);
	}
}
#endif


SearchCacheItem::SearchCacheItem()
{
	lastsearchtext[0]=0;
	search_id_time=0;
	memset(&search_id,0,sizeof(search_id));
	numcons=0;
	lastvisitem=(unsigned int)-1;
}

SearchCacheItem::~SearchCacheItem()
{
	int x;
	for (x=0;x<searchreplies.GetSize();x++)
	{
		C_MessageSearchReply *mr=searchreplies.Get(x);
		delete mr;
	};
}

SearchCacheItem *g_searchcache[SEARCHCACHE_NUMITEMS];

#define MAX_SEARCH_HISTORY_LEN 200
C_ItemList<char> g_searchhistory;
int g_searchhistory_position;

//ADDED Md5Chap moved to util!
//void FormatSizeStr64(char *out, unsigned int low, unsigned int high)

#define COL_ITEM 0
#define COL_SIZE 1
#define COL_TYPE 2
#define COL_TIME 3
#define COL_SPEED 4
#define COL_LOC 5

#if defined(_WIN32)&&(!defined(_DEFINE_SRV)) || defined(_DEFINE_WXUI)
	static unsigned int *m_srch_sort;
	static int m_srch_sort_len,m_srch_sort_alloc;
	static int m_fullsize;

	static int m_dir,m_col;

#ifdef _DEFINE_WXUI
void
SendMessageSearch (void *ignored)
{
int srch_WM_COMMAND_IDC_SEARCH();
(void) ignored;
srch_WM_COMMAND_IDC_SEARCH();
}
#else
void
SendMessageSearch (HWND wnd)
{
	SendMessage(wnd,WM_COMMAND,IDC_SEARCH,0);
}
#endif

	static int sortFunc(const void *elem1, const void *elem2)
	{
		unsigned int a=*(unsigned int *)elem1;
		unsigned int b=*(unsigned int *)elem2;
		C_MessageSearchReply *ra=g_searchcache[0]->searchreplies.Get(a>>16),
			*rb=g_searchcache[0]->searchreplies.Get(b>>16);

		char namea[SEARCHREPLY_MAX_FILESIZE],*nameptra=namea;
		char nameb[SEARCHREPLY_MAX_FILESIZE],*nameptrb=nameb;
		char metaa[SEARCHREPLY_MAX_METASIZE],metab[SEARCHREPLY_MAX_METASIZE];
		char *metaptra=metaa;
		char *metaptrb=metab;
		int ida,idb;
		int timea,timeb;
		unsigned int la_h,lb_h,la_l,lb_l;
		ra->get_item(a&0xffff,&ida,namea,metaa,(int *)&la_l,(int *)&la_h,&timea);
		rb->get_item(b&0xffff,&idb,nameb,metab,(int *)&lb_l,(int *)&lb_h,&timeb);
		int usecol=m_col,usedir=m_dir;

		if (g_searchcache[0]->lastsearchtext[0] == '/') {
			if (!strcmp(namea,PARENT_DIRSTRING)) return -1;
			if (!strcmp(nameb,PARENT_DIRSTRING)) return 1;

			int isdir1=!strcmp(metaa,DIRECTORY_STRING);
			int isdir2=!strcmp(metab,DIRECTORY_STRING);
			if (isdir1+isdir2 == 1) {
				if (isdir1) return -1;
				return 1;
			};
			if (isdir1 && isdir2) {
				if (usecol == COL_TYPE) {
					usecol=COL_SIZE;
					usedir=0;
				};
			};
		};

		if (usecol == COL_TIME) {
			int a=timeb-timea;
			if (a) return usedir?a:-a;
			usedir=0;
			usecol=COL_ITEM;
		};

		if (usecol == COL_LOC) {
			int x=strcmp(ra->_guidstr,rb->_guidstr);
			if (x) return usedir?-x:x;
			return ida-idb;
		};
		if (usecol == COL_SPEED) {
			int x=ra->_latency - rb->_latency;
			if (x) return usedir?-x:x;
			x=rb->get_conspeed()-ra->get_conspeed();
			return usedir?-x:x;
		};
		if (usecol == COL_TYPE) {
			if (!metaa[0]) {
				char *p=::extension(namea);
				if (!*p) return usedir?1:-1;
				else safe_strncpy(metaa,p,sizeof(metaa));
			};
			if (!metab[0]) {
				char *p=::extension(nameb);
				if (!*p) return usedir?-1:1;
				else safe_strncpy(metab,p,sizeof(metab));
			}
			while (*metaptra && *metaptra != ' ') metaptra++;
			while (*metaptrb && *metaptrb != ' ') metaptrb++;

			int x=strnicmp(metaa,metab,waste_min(metaptra-metaa,metaptrb-metab));

			if (!x) {
				if (*metaptra) metaptra++;
				if (*metaptrb) metaptrb++;
				x=atoi(metaptra)-atoi(metaptrb);
			};
			if (!x) x=stricmp(metaa,metab);
			if (x < 0) return (usedir?1:-1);
			if (x > 0) return (usedir?-1:1);

			usecol=COL_ITEM;

		};
		if (usecol == COL_SIZE) {
			int a=lb_h-la_h;
			if (!a) a=lb_l-la_l;
			if (a) return usedir?a:-a;
			usecol=COL_ITEM;
		};
		if (usecol == COL_ITEM) {
			if (g_search_showfull != 1) {
				while (*nameptra) nameptra++;
				while (nameptra >= namea && *nameptra != '/' && *nameptra != '\\') nameptra--;
				if (g_search_showfull == 2 && nameptra >= namea) {
					nameptra--;
					while (nameptra >= namea && *nameptra != '/' && *nameptra != '\\') nameptra--;
				};
				nameptra++;
				while (*nameptra == '/' || *nameptra == '\\') nameptra++;
				while (*nameptrb) nameptrb++;
				while (nameptrb >= nameb && *nameptrb != '/' && *nameptrb != '\\') nameptrb--;
				if (g_search_showfull == 2 && nameptrb >= nameb) {
					nameptrb--;
					while (nameptrb >= nameb && *nameptrb != '/' && *nameptrb != '\\') nameptrb--;
				};
				nameptrb++;
				while (*nameptrb == '/' || *nameptrb == '\\') nameptrb++;
			};
			int x=stricmp(nameptra,nameptrb);
			if (x < 0) return (usedir?1:-1);
			if (x > 0) return (usedir?-1:1);
		};
		return 0;
	}

	static void Search_FilterResults(char *str)
	{
		char *ssout=(char*)malloc(strlen(str)+128);
		if (*str) C_FileDB::parselist(ssout,str);
		else *ssout=0;

		int x;
		__int64 m_size_bytes=0;
		m_fullsize=0;
		//HACK init but unref
		//int r=g_searchcache[0]->searchreplies.GetSize();
		m_srch_sort_len=0;
		for (x = 0; x < g_searchcache[0]->searchreplies.GetSize(); x ++) {
			C_MessageSearchReply *t=g_searchcache[0]->searchreplies.Get(x);
			int y,n=t->get_numitems();
			for (y = 0; y < n; y ++) {
				m_fullsize++;
				int val=0;
				if (!*ssout) {
					val++;
				}
				else {
					char name[SEARCHREPLY_MAX_FILESIZE];
					t->get_item(y,NULL,name,NULL,NULL,NULL,NULL);

					if (C_FileDB::substr_search("",name,ssout)) {
						val++;
					};
				};
				if (val) {
					char m[SEARCHREPLY_MAX_METASIZE];
					unsigned int ll=0,lh=0;
					t->get_item(y,NULL,NULL,m,(int *)&ll,(int *)&lh,NULL);
					if (g_searchcache[0]->lastsearchtext[0]=='/' &&
						(!stricmp(m,DIRECTORY_STRING) || !stricmp(m,USER_STRING)))
					{
						m_size_bytes+=((__int64) lh) << 20;
					}
					else {
						m_size_bytes+=((__int64) lh) << 32;
						m_size_bytes+=((__int64) ll);
					};

					m_srch_sort[m_srch_sort_len]=(x<<16)|y;
					m_srch_sort_len++;
				};

			};
		};

		free(ssout);

#if defined(WIN32) && !defined(_DEFINE_WXUI)
		ListView_SetItemCount(g_lvsearchres.getwnd(),m_srch_sort_len);
#elif defined(_DEFINE_WXUI)
		g_lvsearchres.getwnd()->SetItemCount(m_srch_sort_len);

#endif

		char buf[2048];
		if (m_fullsize != m_srch_sort_len) {
			if (g_searchcache[0]->lastsearchtext[0] == '/')
				sprintf(buf,"Browsing %s: %d/%d item(s)",g_searchcache[0]->lastsearchtext,m_srch_sort_len,m_fullsize);
			else
				sprintf(buf,"Searching for \'%s\'... %d/%d item(s) found",g_searchcache[0]->lastsearchtext,m_srch_sort_len,m_fullsize);
		}
		else {
			if (g_searchcache[0]->lastsearchtext[0] == '/')
				sprintf(buf,"Browsing %s: %d item(s)",g_searchcache[0]->lastsearchtext,m_srch_sort_len);
			else
				sprintf(buf,"Searching for \'%s\'... %d item(s) found",g_searchcache[0]->lastsearchtext,m_srch_sort_len);
		};
		if (m_size_bytes) {
			unsigned int l = (unsigned int)(m_size_bytes&0xFFFFFFFF);
			unsigned int h = (unsigned int)(m_size_bytes>>32);
			strcat(buf," (");
			FormatSizeStr64(buf+strlen(buf),l,h);
			strcat(buf,")");
		};

#ifdef _DEFINE_WXUI
		wxSetDlgItemText(g_search_wnd->m_searchstats,buf);
#else
		SetDlgItemText(g_search_wnd,IDC_SEARCHSTATS,buf);
#endif
		PostMessage(g_search_wnd,WM_USER_TITLEUPDATE,0,0);

	}

	void Search_Resort()
	{
		if (!m_srch_sort||!g_search_wnd) return;

		char buf[256];
#ifdef _DEFINE_WXUI
		wxGetDlgItemText(g_search_wnd->m_edit_filter,buf,sizeof(buf));
#else
		GetDlgItemText(g_search_wnd,IDC_EDIT_FILTER,buf,sizeof(buf));
#endif
		Search_FilterResults(buf);

		m_col=g_config->ReadInt(CONFIG_search_sortcol,CONFIG_search_sortcol_DEFAULT);
		m_dir=g_config->ReadInt(CONFIG_search_sortdir,CONFIG_search_sortdir_DEFAULT);

		if (m_col >= 0 && m_col < 5 && m_srch_sort_len)
			qsort(m_srch_sort,m_srch_sort_len,sizeof(unsigned int),sortFunc);

#if defined(WIN32) && !defined(_DEFINE_WXUI)
		ListView_RedrawItems(g_lvsearchres.getwnd(),0,m_srch_sort_len-1);
		InvalidateRect(g_lvsearchres.getwnd(),NULL,FALSE);
#endif

	}

	void Search_Search(char *str)
	{
#ifdef _DEFINE_WXUI
		wxEnableWindow(g_search_wnd->m_search, !!*str);
#else
		EnableWindow(GetDlgItem(g_search_wnd,IDC_SEARCH),!!*str);
#endif
		//safe_strncpy(g_searchcache[0]->lastsearchtext,str,sizeof(g_searchcache[0]->lastsearchtext));
		wxSetDlgItemText(g_search_wnd->m_searchtext,str);
		SendMessageSearch(g_search_wnd);
	}

	class DirGetRec
	{
	public:
		DirGetRec(char *_prefix)
		{
			prefix=strdup(_prefix);
			expireTime=time(NULL)+SEACH_EXPIRE_DIRECTORY;
		};
		~DirGetRec()
		{
			free(prefix);
		};

		char *prefix;
		T_GUID guid;
		int expireTime;
	};

	static C_ItemList<DirGetRec> g_dirgetlist;

	void KillDirgetlist()
	{
		int x;
		for (x=0;x<g_dirgetlist.GetSize();x++)
		{
			DirGetRec *dg=g_dirgetlist.Get(x);
			delete dg;
		};
	}

	void KillSearchhistory()
	{
		int x;
		for (x=0;x<g_searchhistory.GetSize();x++)
		{
			char *sh=g_searchhistory.Get(x);
			free(sh);
		};
	}

	void Search_AddReply(T_Message *message)
	{
		int x;
		for (x = 0; x < SEARCHCACHE_NUMITEMS; x ++) {
			if (g_searchcache[x]->search_id_time && !memcmp(&g_searchcache[x]->search_id,&message->message_guid,sizeof(g_searchcache[0]->search_id))) {
				C_MessageSearchReply *t=new C_MessageSearchReply(message->data);
				t->_latency=GetTickCount()-g_searchcache[x]->search_id_time;
				MakeID128Str(t->get_guid(),t->_guidstr);

				int n=t->get_numitems();
				if (n < 1 || n > 65535 || g_searchcache[x]->searchreplies.GetSize()>4096) {
					delete t;
					return;
				};

				if (g_searchcache[x]->lastsearchtext[0] == '/') { //fucko might want to make this optional
					//let's merge any directories or users with other directories or users of the same name
					int entry;
					const char *cmpstr=strstr(g_searchcache[x]->lastsearchtext+1,"/")?DIRECTORY_STRING:USER_STRING;
					for (entry = 0; entry < n; entry ++) {
						char name[SEARCHREPLY_MAX_FILESIZE];
						char meta[SEARCHREPLY_MAX_METASIZE];
						int lb_l,lb_h,timev;

						t->get_item(entry,NULL,name,meta,&lb_l,&lb_h,&timev);
						if (strcmp(meta,cmpstr)) continue;

						int item_entry=-1;
						int item_reply=-1;

						item_entry=t->find_item(name,meta,entry);

						int totalitems=entry;
						int nr=g_searchcache[x]->searchreplies.GetSize();

						if (item_entry < 0) {
							for (item_reply = 0; item_reply < nr; item_reply ++) {
								C_MessageSearchReply *sr=g_searchcache[x]->searchreplies.Get(item_reply);
								item_entry=sr->find_item(name,meta,-1);
								totalitems+=sr->get_numitems();
								if (totalitems > 300 || item_entry >= 0) break;
							};
						};

						if (item_entry >= 0) {
							if (item_reply >= 0) {
								C_MessageSearchReply *sr=g_searchcache[x]->searchreplies.Get(item_reply);
								sr->addsize(item_entry,lb_l,lb_h,timev);
							}
							else {
								t->addsize(item_entry,lb_l,lb_h,timev);
							};
							t->delete_item(entry);
							n--;
							entry--;
						};
					};

					if (!n) { delete t; t=NULL; }
				};

				if (t) g_searchcache[x]->searchreplies.Add(t);

				if (x == 0) { //if we are displaying this one
					m_srch_sort_len=0;
					int x;
					for (x = 0; x < g_searchcache[0]->searchreplies.GetSize(); x ++) {
						C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(x);
						m_srch_sort_len+=sr->get_numitems();
					};

					if (m_srch_sort_len >= m_srch_sort_alloc) {
						m_srch_sort_alloc=(m_srch_sort_len*3)/2;
						m_srch_sort=(unsigned int *)realloc(m_srch_sort,m_srch_sort_alloc*sizeof(unsigned int));
						if (!m_srch_sort) {
							m_srch_sort_len=0;
							m_srch_sort_alloc=0;
							return;
						};
					};

					Search_Resort();
				};
				return;
			};
		};
		//scan list
		for (x = 0; x < g_dirgetlist.GetSize(); x ++) {
			DirGetRec *t=g_dirgetlist.Get(x);
			if (!memcmp(&t->guid,&message->message_guid,16)) {
				C_MessageSearchReply repl(message->data);
				int i;
				int doupdates=0;
				for (i = 0; i < repl.get_numitems(); i ++) {
					int id,lb_l,lb_h,timev;
					char name[SEARCHREPLY_MAX_FILESIZE*2];
					sprintf(name,"%s/",t->prefix);
					char *namepart=name+strlen(name);
					repl.get_item(i,&id,namepart,NULL,&lb_l,&lb_h,&timev);
					char *nameptr=name;

					int ppopt=g_config->ReadInt(CONFIG_dlppath,CONFIG_dlppath_DEFAULT);

					if (ppopt & 1) { //full paths
						if (ppopt & 32) {
							nameptr=namepart-2;
							while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
							nameptr++;
						}
						else {
							while (*nameptr == '/') nameptr++;
							while (*nameptr == '.') nameptr++;
						};
					}
					else { //otherwise just get file portion
						while (*nameptr) nameptr++;
						while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
						nameptr++;
					};
					char host[64];
					char sizestr[64];

					MakeID128Str(repl.get_guid(),host);
					sprintf(host+strlen(host),":%d",id);
					FormatSizeStr64(sizestr,lb_l,lb_h);

					if (strncmp(host,g_client_id_str,32)) {
						int p=g_lvrecvq.InsertItem(g_lvrecvq.GetCount(),nameptr,0);
						g_lvrecvq.SetItemText(p,1,sizestr);
						g_lvrecvq.SetItemText(p,2,host);
						g_lvrecvq.SetItemText(p,3,"(unresolved)");
						g_files_in_download_queue++;
						doupdates=1;
					};
				};
				t->expireTime = time(NULL)+SEACH_EXPIRE_DIRECTORY;
				if (doupdates) RecvQ_UpdateStatusText();
				return;
			};
			if (time(NULL) > t->expireTime) {
				delete t;
				g_dirgetlist.Del(x--);
			};
		};
	}

	//if browseIfDir is set, and a dir is encountered it is browsed.
	//otherwise, all files (and directories) are downloaded.

	static void SrchWnd_OnItemRun(HWND hwndDlg, int browseIfDir)
	{
		int willq=-1;
		int x;
		int l=g_lvsearchres.GetCount();

		if (browseIfDir && g_searchcache[0]->lastsearchtext[0]=='/') {
			for (x = 0; x < l; x ++) {
				if (!g_lvsearchres.GetSelected(x)) continue;

				char name[SEARCHREPLY_MAX_FILESIZE*2];
				char meta[SEARCHREPLY_MAX_METASIZE];
				int a=m_srch_sort[x];

				safe_strncpy(name,g_searchcache[0]->lastsearchtext,SEARCHREPLY_MAX_FILESIZE-1);
				if (strlen(name) > 1) {
					if (!strcmp(name+strlen(name)-2,"*s") ||
						!strcmp(name+strlen(name)-1,"*"))
					{
						char *p=name+strlen(name);
						while (p >= name && *p != '/') p--;
						*++p=0;
					};
				};
				if (name[0] && name[strlen(name)-1] != '/') strcat(name,"/");

				char *namepart=name+strlen(name);
				//HACK init but unref
				//char *nameptr=name;

				C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(a>>16);
				sr->get_item(a&0xffff,NULL,namepart,meta,NULL,NULL,NULL);

				if (!stricmp(meta,DIRECTORY_STRING) || !stricmp(meta,USER_STRING)) {
					char buf[2048];
					safe_strncpy(buf,g_searchcache[0]->lastsearchtext,sizeof(buf));
					if (strnicmp(namepart,"..",2)) {

						char *p=buf;
						while (*p) p++;
						if (p[-1] == '*') {
							while (p >= buf && *p != '/') p--;
							if (p >= buf) *p=0;
							else buf[0]=0;

							strcat(buf,"/");
							strcat(buf,namepart);
							strcat(buf,"/");
						}
						else if (p[-1] == '/') {
							strcpy(p,namepart);
							strcat(buf,"/");
						}
						else {
							strcpy(p,"/");
							strcat(buf,namepart);
							strcat(buf,"/");
						};
					}
					else {
						char *p=buf;
						while (*p) p++;
						while (p > buf+1 && p[-1] == '/') p--;
						*p=0;
						while (p >= buf && *p != '/') p--;
						if (p >= buf) p[1]=0;
					};

					Search_Search(buf);
					return;
				};
			};
		};
		//end of dir browsing

		//time to download files and/or directories
		for (x = 0; x < l; x ++) {
			if (!g_lvsearchres.GetSelected(x)) continue;

			char host[32+32];
			char name[SEARCHREPLY_MAX_FILESIZE*2];
			char sizestr[32];
			char meta[SEARCHREPLY_MAX_METASIZE];
			int idx,lenb_low, lenb_high;
			int isbrowse=g_searchcache[0]->lastsearchtext[0]=='/';
			int a=m_srch_sort[x];

			if (isbrowse) {
				safe_strncpy(name,g_searchcache[0]->lastsearchtext,SEARCHREPLY_MAX_FILESIZE-1);
				if (strlen(name) > 1) {
					if (!strcmp(g_searchcache[0]->lastsearchtext+strlen(g_searchcache[0]->lastsearchtext)-2,"*s") ||
						!strcmp(g_searchcache[0]->lastsearchtext+strlen(g_searchcache[0]->lastsearchtext)-1,"*"))
					{
						char *p=name+strlen(name);
						while (p >= name && *p != '/') p--;
						*++p=0;
					};
				};
				if (name[0] && name[strlen(name)-1] != '/') strcat(name,"/");
			}
			else {
				name[0]=0;
			};
			char *namepart=name+strlen(name);
			char *nameptr=name;
			C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(a>>16);
			sr->get_item(a&0xffff,&idx,namepart,meta,&lenb_low,&lenb_high,NULL);
			if (!strncmp(namepart,"..",2)) continue;

			safe_strncpy(host,sr->_guidstr,sizeof(host));
			sprintf(host+strlen(host),":%d",idx);
			FormatSizeStr64(sizestr,lenb_low,lenb_high);

			if (!stricmp(meta,DIRECTORY_STRING)) {
				willq=1;
				//get ready to send the dir request
				if (strncmp(host,g_client_id_str,32)) {
					DirGetRec *t=new DirGetRec(name);
					T_Message msg={0,};
					C_MessageSearchRequest req;
					req.set_min_conspeed(0);
					strcat(name,"/*s");
					req.set_searchstring(name);
					msg.data=req.Make();
					if (msg.data) {
						msg.message_type=MESSAGE_SEARCH;
						msg.message_length=msg.data->GetLength();
						g_mql->send(&msg);
						t->guid = msg.message_guid;
						g_dirgetlist.Add(t);
					}
					else delete t;
				}
				else willq=-1;
			}
			else {
				//dlppath:
				//  1: absolute on folder
				//  2: absolute on search
				//  4:   - only use one rel
				//  8: absolute on browse
				//  16:  - only use one rel
				//  32: relative on folder

				int ppopt=g_config->ReadInt(CONFIG_dlppath,CONFIG_dlppath_DEFAULT);

				//if we're going to use some pathname or other
				if (ppopt & (isbrowse ? 8 : 2)) {
					//if it must be a relative (one dir) path
					if (ppopt & (isbrowse ? 16 : 4)) {
						while (*nameptr) nameptr++;
						while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
						if (nameptr >= name) {
							nameptr--;
							while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
							nameptr++;
						};
					}
					else { //skip leading crap
						while (*nameptr == '/') nameptr++;
						while (*nameptr == '.') nameptr++;
					};
				}
				else { //otherwise just get file portion
					while (*nameptr) nameptr++;
					while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
					nameptr++;
				};

				if (strncmp(nameptr,"..",2) &&
					!strstr(nameptr,":") && !strstr(nameptr,"../") && !strstr(nameptr,"..\\") && nameptr[0] != '/')
				{
					if (willq!=0) willq=Xfer_WillQ(nameptr,host);

					if (strncmp(host,g_client_id_str,32)) {
						int p=g_lvrecvq.InsertItem(g_lvrecvq.GetCount(),nameptr,0);
						g_lvrecvq.SetItemText(p,1,sizestr);
						g_lvrecvq.SetItemText(p,2,host);
						g_lvrecvq.SetItemText(p,3,"(unresolved)");
						g_files_in_download_queue++;
						RecvQ_UpdateStatusText();
					}
					else willq=-1;
				};
			} //end of file dl
		};
		if (willq >= 0) {
			if (g_config->ReadInt(CONFIG_aotransfer,CONFIG_aotransfer_DEFAULT)) {
				ShowTransfersWindow();

#ifdef _DEFINE_WXUI
				extern void xfers_to_front();
				if (g_config->ReadInt(CONFIG_aotransfer_btf,CONFIG_aotransfer_btf_DEFAULT)) xfers_to_front();
				else g_search_wnd->Raise();
#else

				if (g_config->ReadInt(CONFIG_aotransfer_btf,CONFIG_aotransfer_btf_DEFAULT)) SetForegroundWindow(g_xferwnd);
				else SetForegroundWindow(hwndDlg);
#endif

				XferDlg_SetSel(willq);
			};
		};
	}

#if defined(_DEFINE_WXUI)
	void SrchWnd_Item_Run(int browseIfDir) {
		SrchWnd_OnItemRun(0, browseIfDir);
	}
#endif

#if defined(_WIN32)&&(!defined(_DEFINE_SRV)) && (!defined(_DEFINE_WXUI))

	int SrchWnd_GETDISPINFO(long lParam) {
		NMLVDISPINFO *lpdi = (NMLVDISPINFO*) lParam;
		int item=lpdi->item.iItem;
		if (!m_srch_sort || item < 0 || item >= m_srch_sort_len) return 0;

		int repidx=m_srch_sort[item]&0xffff;
		C_MessageSearchReply *tr=g_searchcache[0]->searchreplies.Get(m_srch_sort[item]>>16);

		if (lpdi->item.mask & LVIF_TEXT|LVIF_IMAGE) {
			char name[SEARCHREPLY_MAX_FILESIZE];
			char metadata[SEARCHREPLY_MAX_METASIZE];
			char *nameptr=name;
			int id;
			unsigned int length_low,length_high;
			int timev;
			tr->get_item(repidx,&id,name,metadata,(int*)&length_low,(int*)&length_high,&timev);
			//MessageBox(hwndDlg,name, APP_NAME " Info stuff",MB_OK|MB_ICONEXCLAMATION);
			//"name" hold the path of the download path including
			//the client name
			int isnode=!stricmp(metadata,USER_STRING);
			int isdir=!stricmp(metadata,DIRECTORY_STRING);

			if (lpdi->item.mask & LVIF_TEXT)
	{
				char str[SEARCHREPLY_MAX_FILESIZE+128];
				switch (lpdi->item.iSubItem)
				{
				case COL_ITEM:
					{
						if (g_searchcache[0]->lastsearchtext[0] != '/' && g_search_showfull!=1) {
							while (*nameptr) nameptr++;
							while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
							if (g_search_showfull == 2 && nameptr >= name) {
								nameptr--;
								while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
		};
							nameptr++;
							while (*nameptr == '/' || *nameptr == '\\') nameptr++;
						};
						break;
					};
				case COL_SIZE:
					{
						if (g_searchcache[0]->lastsearchtext[0] == '/' && !stricmp(metadata,DIRECTORY_STRING)) {
							if (strncmp(name,"..",2)) {
								__int64 a=length_high;
								a<<=20;

								int lb_l=(int)(a&0xffffffff);
								int lb_h=(int)(a>>32);

								FormatSizeStr64(str,lb_l,lb_h);
								sprintf(str+strlen(str)," in %d items",length_low);
							}
							else
								strcpy(str,"");
						}
						else if (g_searchcache[0]->lastsearchtext[0] == '/' && !stricmp(metadata,USER_STRING)) {
							if (!g_searchcache[0]->lastsearchtext[1] && !strstr(name,"/") && strlen(name)<24 &&
								name[0] != '.')
	{
								main_onGotNick(name,0);
							};
							if (length_low || length_high && strncmp(name,"..",2)) {
								__int64 a=length_high;
								a<<=20;

								int lb_l=(int)(a&0xffffffff);
								int lb_h=(int)(a>>32);

								FormatSizeStr64(str,lb_l,lb_h);
								sprintf(str+strlen(str)," in %d items",length_low);
							}
							else
								strcpy(str,"");
						}
						else {
							if ((g_search_showfullbytes || length_low < 1024) && !length_high) sprintf(str,"%u",length_low);
							else {
								FormatSizeStr64(str,length_low,length_high);
			};
		};
						nameptr=str;
						break;
					};
				case COL_TYPE:
	{
						if (!metadata[0]) {
							char *p=::extension(name);
							if (!*p) strcpy(metadata,"(no info)");
							else {
								int x;
								for (x=0; *p && x < sizeof(metadata)-1; x++, p++) metadata[x]=(char)(toupper(*p)&0xff);
								metadata[x]=0;
		};
			};
						nameptr=metadata;
						break;
					};
				case COL_TIME:
					{
						if (timev) {
							struct tm *a=localtime((const time_t *)&timev);
							if (!a) {
								time_t b=0;
								a=gmtime(&b);
		};

							if (a) strftime(str,sizeof(str),"%c",a);
							else wsprintf(str,"?:%d",timev);
							nameptr=str;
	}
						else nameptr="";
						break;
					};
				case COL_SPEED:
	{
						sprintf(str,"%dms (",tr->_latency);
						get_speedstr(tr->get_conspeed(),str+strlen(str));
						strcat(str,")");
						nameptr=str;
						break;
		};
				case COL_LOC:
		{
						sprintf(str,"%s:%d",tr->_guidstr,id);
						nameptr=str;
						break;
					};
				default:
			{
						*nameptr=0;
						break;
				};
			};
				strcpy(lpdi->item.pszText,nameptr);
			};
			if(lpdi->item.mask & LVIF_IMAGE) {
				lpdi->item.iImage = isdir?1:isnode?2:0;
			};
		};
		return 0;
	}

#elif defined(_DEFINE_WXUI)

	size_t my_strftime(char *s, size_t max, const char *fmt, const struct tm *tm) {
		return strftime(s, max, fmt, tm);
	}

	wxString SrchWnd_GetItemText (long item, long column)
	{
		if (!m_srch_sort || item < 0 || item >= m_srch_sort_len)
			return wxString(_T(""));

		int repidx=m_srch_sort[item]&0xffff;
		C_MessageSearchReply *tr=g_searchcache[0]->searchreplies.Get(m_srch_sort[item]>>16);

		char name[SEARCHREPLY_MAX_FILESIZE];
		char metadata[SEARCHREPLY_MAX_METASIZE];
		char *nameptr=name;
		int id;
		unsigned int length_low,length_high;
		int timev;
		tr->get_item(repidx,&id,name,metadata,(int*)&length_low,(int*)&length_high,&timev);
		//MessageBox(hwndDlg,name, APP_NAME " Info stuff",MB_OK|MB_ICONEXCLAMATION);
		//"name" hold the path of the download path including
		//the client name
		char str[SEARCHREPLY_MAX_FILESIZE+128];
		switch (column)
				{
		case COL_ITEM:
			{
				if (g_searchcache[0]->lastsearchtext[0] != '/' && g_search_showfull!=1) {
					while (*nameptr) nameptr++;
					while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
					if (g_search_showfull == 2 && nameptr >= name) {
						nameptr--;
						while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
					};
					nameptr++;
					while (*nameptr == '/' || *nameptr == '\\') nameptr++;
				};
				break;
			};
		case COL_SIZE:
			{
				if (g_searchcache[0]->lastsearchtext[0] == '/' && !stricmp(metadata,DIRECTORY_STRING)) {
					if (strncmp(name,"..",2)) {
						__int64 a=length_high;
						a<<=20;

						int lb_l=(int)(a&0xffffffff);
						int lb_h=(int)(a>>32);

						FormatSizeStr64(str,lb_l,lb_h);
						sprintf(str+strlen(str)," in %d items",length_low);
					}
					else
						strcpy(str,"");
				}
				else if (g_searchcache[0]->lastsearchtext[0] == '/' && !stricmp(metadata,USER_STRING)) {
					if (!g_searchcache[0]->lastsearchtext[1] && !strstr(name,"/") && strlen(name)<24 &&
						name[0] != '.')
					{
						main_onGotNick(name,0);
				};
					if (length_low || length_high && strncmp(name,"..",2)) {
						__int64 a=length_high;
						a<<=20;

						int lb_l=(int)(a&0xffffffff);
						int lb_h=(int)(a>>32);

						FormatSizeStr64(str,lb_l,lb_h);
						sprintf(str+strlen(str)," in %d items",length_low);
					}
					else
						strcpy(str,"");
				}
				else {
					if ((g_search_showfullbytes || length_low < 1024) && !length_high) sprintf(str,"%u",length_low);
					else {
						FormatSizeStr64(str,length_low,length_high);
			};
				};
				nameptr=str;
				break;
			};
		case COL_TYPE:
			{
				if (!metadata[0]) {
					char *p=::extension(name);
					if (!*p) strcpy(metadata,"(no info)");
					else {
						int x;
						for (x=0; *p && x < ((int) sizeof(metadata))-1; x++, p++) metadata[x]=(char)(toupper(*p)&0xff);
						metadata[x]=0;
					};
				};
				nameptr=metadata;
				break;
			};
		case COL_TIME:
			{
				if (timev) {
					struct tm *a=localtime((const time_t *)&timev);
					if (!a) {
						time_t b=0;
						a=gmtime(&b);
				};

					if (a) my_strftime(str,sizeof(str),"%c",a);
					else snprintf(str,31,"?:%d",timev);
					nameptr=str;
				}
				else nameptr="";
				break;
			};
		case COL_SPEED:
			{
				sprintf(str,"%dms (",tr->_latency);
				get_speedstr(tr->get_conspeed(),str+strlen(str));
				strcat(str,")");
				nameptr=str;
				break;
			};
		case COL_LOC:
			{
				sprintf(str,"%s:%d",tr->_guidstr,id);
				nameptr=str;
				break;
			};
		default:
			{
				*nameptr=0;
				break;
			};
		};

		return cstr_to_wxstr(nameptr);
	}

	int SrchWnd_GetItemImage (long item)
	{
		if (!m_srch_sort || item < 0 || item >= m_srch_sort_len) return 0;
		int repidx=m_srch_sort[item]&0xffff;
		C_MessageSearchReply *tr=g_searchcache[0]->searchreplies.Get(m_srch_sort[item]>>16);

							char name[SEARCHREPLY_MAX_FILESIZE];
		char metadata[SEARCHREPLY_MAX_METASIZE];
		int id;
		unsigned int length_low,length_high;
		int timev;
		tr->get_item(repidx,&id,name,metadata,(int*)&length_low,(int*)&length_high,&timev);
		//MessageBox(hwndDlg,name, APP_NAME " Info stuff",MB_OK|MB_ICONEXCLAMATION);
		//"name" hold the path of the download path including
		//the client name
		int isnode=!stricmp(metadata,USER_STRING);
		int isdir=!stricmp(metadata,DIRECTORY_STRING);

		return isdir?1:isnode?2:0;
					}

#if 0
	int SrchWnd_GETDISPINFO(int want, long item, long column) {
						NMLVDISPINFO *lpdi = (NMLVDISPINFO*) lParam;
						int item=lpdi->item.iItem;
						if (!m_srch_sort || item < 0 || item >= m_srch_sort_len) return 0;

						int repidx=m_srch_sort[item]&0xffff;
						C_MessageSearchReply *tr=g_searchcache[0]->searchreplies.Get(m_srch_sort[item]>>16);

						if (lpdi->item.mask & LVIF_TEXT|LVIF_IMAGE) {
							char name[SEARCHREPLY_MAX_FILESIZE];
							char metadata[SEARCHREPLY_MAX_METASIZE];
							char *nameptr=name;
							int id;
							unsigned int length_low,length_high;
							int timev;
							tr->get_item(repidx,&id,name,metadata,(int*)&length_low,(int*)&length_high,&timev);
							//MessageBox(hwndDlg,name, APP_NAME " Info stuff",MB_OK|MB_ICONEXCLAMATION);
							//"name" hold the path of the download path including
							//the client name
							int isnode=!stricmp(metadata,USER_STRING);
							int isdir=!stricmp(metadata,DIRECTORY_STRING);

							if (lpdi->item.mask & LVIF_TEXT)
							{
								char str[SEARCHREPLY_MAX_FILESIZE+128];
								switch (lpdi->item.iSubItem)
								{
								case COL_ITEM:
									{
										if (g_searchcache[0]->lastsearchtext[0] != '/' && g_search_showfull!=1) {
											while (*nameptr) nameptr++;
											while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
											if (g_search_showfull == 2 && nameptr >= name) {
												nameptr--;
												while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
											};
											nameptr++;
											while (*nameptr == '/' || *nameptr == '\\') nameptr++;
										};
										break;
									};
								case COL_SIZE:
									{
										if (g_searchcache[0]->lastsearchtext[0] == '/' && !stricmp(metadata,DIRECTORY_STRING)) {
											if (strncmp(name,"..",2)) {
												__int64 a=length_high;
												a<<=20;

												int lb_l=(int)(a&0xffffffff);
												int lb_h=(int)(a>>32);

												FormatSizeStr64(str,lb_l,lb_h);
												sprintf(str+strlen(str)," in %d items",length_low);
											}
											else
												strcpy(str,"");
										}
										else if (g_searchcache[0]->lastsearchtext[0] == '/' && !stricmp(metadata,USER_STRING)) {
											if (!g_searchcache[0]->lastsearchtext[1] && !strstr(name,"/") && strlen(name)<24 &&
												name[0] != '.')
											{
												main_onGotNick(name,0);
											};
											if (length_low || length_high && strncmp(name,"..",2)) {
												__int64 a=length_high;
												a<<=20;

												int lb_l=(int)(a&0xffffffff);
												int lb_h=(int)(a>>32);

												FormatSizeStr64(str,lb_l,lb_h);
												sprintf(str+strlen(str)," in %d items",length_low);
											}
											else
												strcpy(str,"");
										}
										else {
											if ((g_search_showfullbytes || length_low < 1024) && !length_high) sprintf(str,"%u",length_low);
											else {
												FormatSizeStr64(str,length_low,length_high);
											};
										};
										nameptr=str;
										break;
									};
								case COL_TYPE:
									{
										if (!metadata[0]) {
											char *p=::extension(name);
											if (!*p) strcpy(metadata,"(no info)");
											else {
												int x;
												for (x=0; *p && x < sizeof(metadata)-1; x++, p++) metadata[x]=(char)(toupper(*p)&0xff);
												metadata[x]=0;
											};
										};
										nameptr=metadata;
										break;
									};
								case COL_TIME:
									{
										if (timev) {
											struct tm *a=localtime((const time_t *)&timev);
											if (!a) {
												time_t b=0;
												a=gmtime(&b);
											};

											if (a) strftime(str,sizeof(str),"%c",a);
											else wsprintf(str,"?:%d",timev);
											nameptr=str;
										}
										else nameptr="";
										break;
									};
								case COL_SPEED:
									{
										sprintf(str,"%dms (",tr->_latency);
										get_speedstr(tr->get_conspeed(),str+strlen(str));
										strcat(str,")");
										nameptr=str;
										break;
									};
								case COL_LOC:
									{
										sprintf(str,"%s:%d",tr->_guidstr,id);
										nameptr=str;
										break;
									};
								default:
									{
										*nameptr=0;
										break;
									};
								};
								strcpy(lpdi->item.pszText,nameptr);
							};
							if(lpdi->item.mask & LVIF_IMAGE) {
								lpdi->item.iImage = isdir?1:isnode?2:0;
							};
						};
						return 0;
					}
#endif

#endif

#if !defined(_DEFINE_WXUI) && !defined(_DEFINE_SRV)

	static WNDPROC srch_oldWndProc;

	static BOOL CALLBACK srch_newWndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,LPARAM lParam)
	{
		if ((uMsg == WM_KEYDOWN || uMsg == WM_KEYUP)&&(wParam == VK_RETURN)) {
			if (uMsg == WM_KEYUP) SendMessageSearch(GetParent(GetParent(hwndDlg)));
			return 0;
		};
		return CallWindowProc(srch_oldWndProc,hwndDlg,uMsg,wParam,lParam);
					}

	static WNDPROC srchres_oldWndProc;

	static BOOL CALLBACK srchres_newWndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,LPARAM lParam)
	{
		if ((uMsg == WM_KEYDOWN || uMsg == WM_KEYUP)&&(wParam == VK_RETURN || wParam == VK_BACK)) {
			if (uMsg == WM_KEYUP && wParam == VK_RETURN) SrchWnd_OnItemRun(GetParent(hwndDlg),1);

			if (uMsg == WM_KEYUP && wParam == VK_BACK) {
				PostMessage(GetParent(hwndDlg),WM_COMMAND,IDC_BACK,0);
							};
			return 0;
						};
		return CallWindowProc(srchres_oldWndProc,hwndDlg,uMsg,wParam,lParam);
	}

	static UINT CALLBACK fileHookProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM /*lParam*/)
	{
		if (uMsg == WM_INITDIALOG) {
			SetDlgItemText(hwndDlg,IDC_UPATH,g_filedlg_ulpath);
		};
		if (uMsg == WM_COMMAND) {
			if (LOWORD(wParam) == IDC_UPATH && HIWORD(wParam) == EN_CHANGE) {
				GetDlgItemText(hwndDlg,IDC_UPATH,g_filedlg_ulpath,sizeof(g_filedlg_ulpath));
				return 1;
			};
		};

		return 0;
						}
#endif /* _DEFINE_WXUI || _DEFINE_SRV */
#endif

#if !defined(_DEFINE_WXUI) && !defined(_DEFINE_SRV)
int
srch_WM_CLOSE ()
{
	ShowWindow(hwndDlg,SW_HIDE);
	CheckMenuItem(GetMenu(g_mainwnd),IDC_SEARCH,MF_UNCHECKED|MF_BYCOMMAND);
	g_config->WriteInt(CONFIG_search_vis,0);
	return 0;
}
#endif

#ifndef _DEFINE_SRV

int srch_WM_COMMAND_IDC_SEARCH()
{
	//store nav position in last cached item
	{
		int n=g_lvsearchres.GetCount();
		int x;
		for (x = 0; x < n && !g_lvsearchres.GetSelected(x); x ++);
		if (x >= 0 && x < m_srch_sort_len)
			g_searchcache[0]->lastvisitem=m_srch_sort[x]; //save last pos
	};

	int ci;
	int cached=0;
	int replpos=0; //position to replace
	char text[512];
	wxGetDlgItemText(g_search_wnd->m_searchtext,text,511);
	if (!strnicmp(text,"waste:",6)) {
		char *in=text+6, *out=text;
		while (*in) {
			if (!strncmp(in,"%20",3)) {
				*out++=' ';
				in+=3;
							}
			else *out++=*in++;
							};
		*out=0;
		wxSetDlgItemText(g_search_wnd->m_searchtext,text);
	};

#ifdef _DEFINE_WXUI
	wxEnableWindow(g_search_wnd->m_goparent, text[0]=='/'&&text[1]);
#else
	EnableWindow(GetDlgItem(hwndDlg,IDC_GOPARENT),text[0]=='/'&&text[1]);
#endif

	if (strlen(text)<=0) return 0;

	{
		char *t=g_searchhistory.Get(g_searchhistory_position);
		if (!t || strcmp(t,text)) {
			if (g_searchhistory.GetSize()) g_searchhistory_position++;
			while (g_searchhistory.GetSize() > g_searchhistory_position) {
				free(g_searchhistory.Get(g_searchhistory_position));
				g_searchhistory.Del(g_searchhistory_position);
			};
			g_searchhistory.Add(strdup(text));
			if (g_searchhistory.GetSize() > MAX_SEARCH_HISTORY_LEN) {
				free(g_searchhistory.Get(0));
				g_searchhistory.Del(0);
				g_searchhistory_position--;
			};
		};
	};

#ifdef _DEFINE_WXUI
	wxEnableWindow(g_search_wnd->m_back,g_searchhistory_position>0);
	wxEnableWindow(g_search_wnd->m_back2,g_searchhistory_position>0);
	wxEnableWindow(g_search_wnd->m_forward,g_searchhistory_position<g_searchhistory.GetSize()-1);
	wxEnableWindow(g_search_wnd->m_forward2,g_searchhistory_position<g_searchhistory.GetSize()-1);
#else

	EnableWindow(GetDlgItem(hwndDlg,IDC_BACK),g_searchhistory_position>0);
	EnableWindow(GetDlgItem(hwndDlg,IDC_BACK2),g_searchhistory_position>0);
	EnableWindow(GetDlgItem(hwndDlg,IDC_FORWARD),g_searchhistory_position<g_searchhistory.GetSize()-1);
	EnableWindow(GetDlgItem(hwndDlg,IDC_FORWARD2),g_searchhistory_position<g_searchhistory.GetSize()-1);
#endif

	//see if we can't find 'text' in cache, that hasn't expired.
	//if we do find it, and it is in entry 0, then we clear entry 0
	//and go for it.
	for (ci = 0; ci < SEARCHCACHE_NUMITEMS; ci ++) {
		if (!strcmp(text,g_searchcache[ci]->lastsearchtext)) {
			replpos=ci;
			if (ci > 0 && g_searchcache[ci]->searchreplies.GetSize() &&
				g_searchcache[ci]->search_id_time+SEARCHCACHE_EXPIRE_TIME >= GetTickCount() &&
				g_searchcache[ci]->numcons*2 >= g_mql->GetNumQueues()
				) cached=1;
			break;
		};
		if (g_searchcache[ci]->search_id_time < g_searchcache[replpos]->search_id_time) replpos=ci;
	};

	if (replpos > 0) {
		int x;
		SearchCacheItem *tmp=g_searchcache[replpos];
		for (x = replpos; x > 0; x --) g_searchcache[x]=g_searchcache[x-1];
		g_searchcache[0]=tmp;
	};

#ifdef _DEFINE_WXUI

	wxString wxtext = cstr_to_wxstr(text);
	{
		int findpos = g_search_wnd->m_searchtext->FindString(wxtext);
		if (findpos != wxNOT_FOUND) { g_search_wnd->m_searchtext->Delete(findpos); }
						}

#if (wxMAJOR_VERSION >= 2 && wxMINOR_VERSION >= 5)
	g_search_wnd->m_searchtext->Insert(wxtext, 0);
	g_search_wnd->m_searchtext->SetSelection(0);
#else
	// wxWindows 2.4.x doesn't allow insertion at start of list! duh
	g_search_wnd->m_searchtext->Append(wxtext);
	int newpos = g_search_wnd->m_searchtext->GetCount() - 1;
	g_search_wnd->m_searchtext->SetSelection(newpos);
#endif

#else

	{
		int findpos=SendDlgItemMessage(hwndDlg,IDC_SEARCHTEXT,CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)text);
		if (findpos != CB_ERR) SendDlgItemMessage(hwndDlg,IDC_SEARCHTEXT,CB_DELETESTRING,findpos,0);
						};

	SendDlgItemMessage(hwndDlg,IDC_SEARCHTEXT,CB_INSERTSTRING,0,(LPARAM)text);
	SendDlgItemMessage(hwndDlg,IDC_SEARCHTEXT,CB_SETCURSEL,0,0);

#endif

	g_lvsearchres.Clear();
	free(m_srch_sort);
	m_srch_sort=0;
	m_srch_sort_len=0;
	m_srch_sort_alloc=0;
	m_fullsize=0;

	if (cached) {
		m_srch_sort_len=0;
		int x;
		for (x = 0; x < g_searchcache[0]->searchreplies.GetSize(); x ++) {
			C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(x);
			m_srch_sort_len+=sr->get_numitems();
		};

		if (m_srch_sort_len >= m_srch_sort_alloc) {
			m_srch_sort_alloc=(m_srch_sort_len*3)/2;
			m_srch_sort=(unsigned int *)realloc(m_srch_sort,m_srch_sort_alloc*sizeof(unsigned int));
			if (!m_srch_sort) {
				m_srch_sort_len=0;
				m_srch_sort_alloc=0;
			};
		};
		Search_Resort();
		for (x = 0; x < m_srch_sort_len; x ++) {
			if (m_srch_sort[x] == g_searchcache[0]->lastvisitem) {
#ifdef _DEFINE_WXUI
				g_lvsearchres.getwnd()->EnsureVisible(x);
				g_lvsearchres.getwnd()->SetItemState(x, wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED, wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED);
#else
				ListView_EnsureVisible(g_lvsearchres.getwnd(),x,FALSE);
				ListView_SetItemState(g_lvsearchres.getwnd(),x,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
#endif

				break;
			};
		};
	}
	else { //not cached
		while (g_searchcache[0]->searchreplies.GetSize()) {
			int last=g_searchcache[0]->searchreplies.GetSize()-1;
			C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(last);
			delete sr;
			g_searchcache[0]->searchreplies.Del(last);
		};
		T_Message msg={0,};
		C_MessageSearchRequest req;
		req.set_min_conspeed(0); //g_config->ReadInt(CONFIG_search_minspeed,CONFIG_search_minspeed_DEFAULT));
		req.set_searchstring(text);
		msg.data=req.Make();
		if (msg.data) {
			dbg_printf(ds_Debug,"sending search for %s!",text);
			msg.message_type=MESSAGE_SEARCH;
			msg.message_length=msg.data->GetLength();
			g_mql->send(&msg);
			g_searchcache[0]->search_id=msg.message_guid;
			g_searchcache[0]->search_id_time=GetTickCount();
			g_searchcache[0]->numcons=g_mql->GetNumQueues();
		};
		safe_strncpy(g_searchcache[0]->lastsearchtext,text,sizeof(g_searchcache[0]->lastsearchtext));
		char text2[1024];
		if (text[0] == '/')
			sprintf(text2,"Browsing %s:",text);
		else
			sprintf(text2,"Searching for \'%s\'...",text);
#ifdef _DEFINE_WXUI
		wxSetDlgItemText(g_search_wnd->m_searchstats,text2);
#else
		SetDlgItemText(hwndDlg,IDC_SEARCHSTATS,text2);
#endif
		if (g_searchcache[0]->lastsearchtext[0] == '/') {
			C_MessageSearchReply repl;
			repl.set_conspeed(g_conspeed);
			repl.clear_items();
			repl.set_guid(&g_client_id);

			if (g_searchcache[0]->lastsearchtext[1] && g_searchcache[0]->lastsearchtext[1] != '*' &&
				strcmp(g_searchcache[0]->lastsearchtext+strlen(g_searchcache[0]->lastsearchtext)-2,"*s"))
						{
				//HACK init but unref
				//T_Message msg={0,};
				//HACK init but unref
				//char *p=strstr(g_searchcache[0]->lastsearchtext+1,"/");
				repl.add_item(-1,PARENT_DIRSTRING,DIRECTORY_STRING,0,0,0);
				msg.message_guid=g_searchcache[0]->search_id;
				msg.data=repl.Make();
				if (msg.data) {
					msg.message_type=MESSAGE_SEARCH_REPLY;
					msg.message_length=msg.data->GetLength();
					main_MsgCallback(&msg,NULL,NULL);
					msg.data->Unlock();
				};
			};

			repl.clear_items();
			if (g_database->GetNumFiles() && (g_accept_downloads&5)==5)
				g_database->Search(g_searchcache[0]->lastsearchtext,&repl,NULL,0,main_MsgCallback);
		};
		PostMessage(g_search_wnd,WM_USER_TITLEUPDATE,0,0);
	} //!cached
	return 0;
}

int srch_WM_COMMAND_IDC_GOPARENT()
{
	if (g_searchcache[0]->lastsearchtext[0] == '/') {
		char buf[1024];
		safe_strncpy(buf,g_searchcache[0]->lastsearchtext,sizeof(buf));

		char *p=buf;
		while (*p) p++;
		while (p > buf+1 && p[-1] == '/') p--;
		*p=0;
		while (p >= buf && *p != '/') p--;
		if (p >= buf) p[1]=0;
		Search_Search(buf);
	};
	return 0;
}


#ifdef _DEFINE_WXUI
/* create a menu-event-handling class for wxWidgets */

class menuHandler : public wxEvtHandler {
public:
	menuHandler(int *resultstore) {
	m_resultstore = resultstore;
	}
	void OnMenuEvent (wxCommandEvent &evt) {
	*m_resultstore = evt.GetId();
	}
	int *m_resultstore;
	DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(menuHandler, wxEvtHandler)
EVT_MENU(wxID_ANY, menuHandler::OnMenuEvent)
END_EVENT_TABLE()

#endif


int srch_WM_COMMAND_IDC_FORWARD(int is_forward2)
{
	if (g_searchhistory_position < g_searchhistory.GetSize()-1) {
		if (is_forward2) {
#ifdef _DEFINE_WXUI

			wxMenu *hMenu = new wxMenu();
			int x;
			for (x = 0; x < 10; x ++) {
				if (g_searchhistory_position + (x + 1) < 0) break;
				char *str=g_searchhistory.Get(g_searchhistory_position + (x + 1));
				if (!str) break;

				hMenu->Append(1025+x, cstr_to_wxstr(str));
			};

			g_search_wnd->PushEventHandler(new menuHandler(&x));

			wxRect r = g_search_wnd->m_forward2->GetRect();
			(void) g_search_wnd->PopupMenu(hMenu, r.GetLeft(), r.GetBottom());
			delete hMenu;

			if (x > 1024) {
				x-=1024;
				Search_Search(g_searchhistory.Get(g_searchhistory_position += x));
			};
			g_search_wnd->m_forward->SetFocus();

#else

			HMENU hMenu = CreatePopupMenu();
			int x;
			for (x = 0; x < 10; x ++) {
				char *str=g_searchhistory.Get(g_searchhistory_position + x + 1);
				if (!str) break;
				MENUITEMINFO mi={sizeof(mi),MIIM_ID|MIIM_TYPE,MFT_STRING,0,1025+x};
				mi.dwTypeData=str;

				InsertMenuItem(hMenu,x,TRUE,&mi);
			};
			RECT r;
			GetWindowRect(GetDlgItem(hwndDlg,IDC_FORWARD2),&r);

			x=TrackPopupMenu(hMenu,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,r.right,r.top,0,hwndDlg,NULL);
			DestroyMenu(hMenu);
			if (x > 1024) {
				x-=1024;
				Search_Search(g_searchhistory.Get(g_searchhistory_position += x));
			};
			SetFocus(GetDlgItem(hwndDlg,IDC_FORWARD));

#endif
						}
		else
			Search_Search(g_searchhistory.Get(++g_searchhistory_position));
	};
	return 0;
}

int srch_WM_COMMAND_IDC_BACK(int is_back2)
{
	if (g_searchhistory_position > 0) {
		if (is_back2) {
#ifdef _DEFINE_WXUI

			wxMenu *hMenu = new wxMenu();
			int x;
			for (x = 0; x < 10; x ++) {
				if (g_searchhistory_position - (x + 1) < 0) break;
				char *str=g_searchhistory.Get(g_searchhistory_position - (x + 1));
				if (!str) break;

				hMenu->Append(1025+x, cstr_to_wxstr(str));
			};

			g_search_wnd->PushEventHandler(new menuHandler(&x));

			wxRect r = g_search_wnd->m_back2->GetRect();
			(void) g_search_wnd->PopupMenu(hMenu, r.GetLeft(), r.GetBottom());
			delete hMenu;

			if (x > 1024) {
				x-=1024;
				Search_Search(g_searchhistory.Get(g_searchhistory_position -= x));
			};
			g_search_wnd->m_back->SetFocus();

#else

			HMENU hMenu = CreatePopupMenu();
			int x;
			for (x = 0; x < 10; x ++) {
				if (g_searchhistory_position - (x + 1) < 0) break;
				char *str=g_searchhistory.Get(g_searchhistory_position - (x + 1));
				if (!str) break;
				MENUITEMINFO mi={sizeof(mi),MIIM_ID|MIIM_TYPE,MFT_STRING,0,1025+x};
				mi.dwTypeData=str;

				InsertMenuItem(hMenu,x,TRUE,&mi);
			};
			RECT r;
			GetWindowRect(GetDlgItem(hwndDlg,IDC_BACK2),&r);

			x=TrackPopupMenu(hMenu,TPM_RIGHTALIGN|TPM_TOPALIGN|TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,r.left,r.top,0,hwndDlg,NULL);
			DestroyMenu(hMenu);
			if (x > 1024) {
				x-=1024;
				Search_Search(g_searchhistory.Get(g_searchhistory_position -= x));
			};
			SetFocus(GetDlgItem(hwndDlg,IDC_BACK));

#endif
		}
		else
			Search_Search(g_searchhistory.Get(--g_searchhistory_position));
	};
	return 0;
}

int srchwnd_WM_DESTROY()
{
	g_config->WriteInt(CONFIG_search_col1,g_lvsearchres.GetColumnWidth(0));
	g_config->WriteInt(CONFIG_search_col2,g_lvsearchres.GetColumnWidth(1));
	g_config->WriteInt(CONFIG_search_col3,g_lvsearchres.GetColumnWidth(2));
	g_config->WriteInt(CONFIG_search_col6,g_lvsearchres.GetColumnWidth(3));
	g_config->WriteInt(CONFIG_search_col4,g_lvsearchres.GetColumnWidth(4));
	g_config->WriteInt(CONFIG_search_col5,g_lvsearchres.GetColumnWidth(5));
	int a=!!g_config->ReadInt(CONFIG_srchcb_use,CONFIG_srchcb_use_DEFAULT);
	int x;
	for (x = 0; x < MAX_SRCHTEXTS; x ++) {
		char buf[123];
		char obuf[1024];
		obuf[0]=0;
		if (a) {
#ifdef _DEFINE_WXUI
			wxString wxstr = g_search_wnd->m_searchtext->GetString(x);
			wxstr_to_cstr (obuf, wxstr, 1023);
#else
			SendDlgItemMessage(hwndDlg,IDC_SEARCHTEXT,CB_GETLBTEXT,(LPARAM)x,(WPARAM)obuf);
#endif
		};
		sprintf(buf,"srchcb_%d",x);
		g_config->WriteString(buf,obuf);
	};
	return 0;
}

int set_srch_title (char *buf, int len) 
{
	strcpy(buf,"Browser");
	int numresults=m_srch_sort_len;
	int numfresults=m_fullsize;
	int a=0;

	if (numfresults) {
		if (numfresults != numresults)
			sprintf(buf+strlen(buf),"%s%d of %d result%s",a++?", ":" (",numresults,numfresults,numfresults==1?"":"s");
		else
			sprintf(buf+strlen(buf),"%s%d result%s",a++?", ":" (",numresults,numresults==1?"":"s");
	};
	if (a) strcat(buf,")");

	strcat(buf," - " APP_NAME);
	return 1;
}

int srch_SEARCHTEXT_SELCHANGE() {
	char buf[32];
#ifdef _DEFINE_WXUI
	wxGetDlgItemText(g_search_wnd->m_searchtext,buf,31);
	wxEnableWindow(g_search_wnd->m_search,!!buf[0]);
#else
	GetDlgItemText(hwndDlg,IDC_SEARCHTEXT,buf,31);
	EnableWindow(GetDlgItem(hwndDlg,IDC_SEARCH),!!buf[0]);
#endif
	return 0;
}

void srchwnd_ID_SENDFILENODE() {
#ifdef _DEFINE_WXUI

	wxFileDialog dlg(g_search_wnd,
			_T("Open file(s) to send"),
			_T(""),
			_T(""),
			_T("All files|*.*"),
			wxMULTIPLE|wxOPEN);

	if (dlg.ShowModal() == wxID_OK) {
		wxArrayString strs;
		wxString str;

		dlg.GetPaths(strs);
		int i;
		for (i = 0; i < (int) strs.GetCount(); i++) {
			char fullfn[4096];
			str = strs.Item(i);
			wxstr_to_cstr (fullfn, str, 4095);

			int l=g_lvsearchres.GetCount();
			int x;
			for (x = 0; x < l; x ++) {
				if (g_lvsearchres.GetSelected(x)) {
					char host[33];
					C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(m_srch_sort[x]>>16);
					safe_strncpy(host,sr->_guidstr,sizeof(host));
					if (strlen(host)==32) {
						Xfer_UploadFileToUser(0,fullfn,host,g_filedlg_ulpath);
					}
				}
			}
		}
	}

#else

							char *fnroot=(char*)malloc(65536*4);
	fnroot[0]=0;

							OPENFILENAME l={sizeof(l),};
							l.hwndOwner = hwndDlg;
							l.lpstrFilter = "All files (*.*)\0*.*\0";
							l.lpstrFile = fnroot;
							l.nMaxFile = 65535*4;
							l.lpstrTitle = "Open file(s) to send";
							l.lpstrDefExt = "";
							l.hInstance=g_hInst;
							l.lpfnHook=fileHookProc;
							l.lpTemplateName=MAKEINTRESOURCE(IDD_FILESUBDLG);

							l.Flags = OFN_HIDEREADONLY|OFN_EXPLORER|OFN_ALLOWMULTISELECT|OFN_ENABLETEMPLATE|OFN_ENABLEHOOK|OFN_FILEMUSTEXIST;
							if (GetOpenFileName(&l)) {
								char *fn=fnroot;
								char *pathstr="";
								if (fn[strlen(fn)+1]) { //multiple files
									pathstr=fn;
									fn+=strlen(fn)+1;
								};
								while (*fn) {
									char fullfn[4096];
									fullfn[0]=0;
									if (*pathstr) {
										safe_strncpy(fullfn,pathstr,sizeof(fullfn));
										if (fullfn[strlen(fullfn)-1]!='\\') strcat(fullfn,"\\");
									};
									strcat(fullfn,fn);

									int l=g_lvsearchres.GetCount();
									for (x = 0; x < l; x ++) {
										if (g_lvsearchres.GetSelected(x)) {
											char host[33];
											C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(m_srch_sort[x]>>16);
											safe_strncpy(host,sr->_guidstr,sizeof(host));
											if (strlen(host)==32) {
												Xfer_UploadFileToUser(hwndDlg,fullfn,host,g_filedlg_ulpath);
											};
										};
									};
									fn+=strlen(fn)+1;
								};
							};

							free(fnroot);
#endif
}

void srchwnd_ID_DLPARENT() {
							int l=g_lvsearchres.GetCount();
	int x;
							for (x = 0; x < l; x ++) {
								if (g_lvsearchres.GetSelected(x)) {
									char buf[SEARCHREPLY_MAX_FILESIZE+36];
									char host[33];
									int a=m_srch_sort[x];
									C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(a>>16);
									sr->get_item(a&0xffff,NULL,buf,NULL,NULL,NULL,NULL);
									safe_strncpy(host,sr->_guidstr,sizeof(host));

									if (buf[0] == '/') {
										if (buf[1] == '?') {
											char buf2[SEARCHREPLY_MAX_FILESIZE+36];
											sprintf(buf2,"/%s%s",host,buf+2);
											safe_strncpy(buf,buf2,sizeof(buf));
										};
										char *p=buf+strlen(buf);
										while (p > buf && *p != '/') p--;
										*p=0;
										if (buf[0]) {
											DirGetRec *t=new DirGetRec(buf);
											T_Message msg={0,};
											C_MessageSearchRequest req;
											req.set_min_conspeed(0);
											strcat(buf,"/*s");
											req.set_searchstring(buf);
											msg.data=req.Make();
											if (msg.data) {
												msg.message_type=MESSAGE_SEARCH;
												msg.message_length=msg.data->GetLength();
												g_mql->send(&msg);
												t->guid = msg.message_guid;
												g_dirgetlist.Add(t);
											}
											else delete t;

										};
									};
									break;
								};
							};
}

void srchwnd_ID_BROWSECOPYLOC(int is_copyloc) {
							int l=g_lvsearchres.GetCount();
							int idx;
							for (idx = 0; idx < l; idx ++) {
								if (g_lvsearchres.GetSelected(idx)) {
									char buf[SEARCHREPLY_MAX_FILESIZE*3+36];
									int a=m_srch_sort[idx];

									buf[0]=0;

			if (is_copyloc && g_searchcache[0]->lastsearchtext[0] == '/') { //we were browsing
										safe_strncpy(buf,g_searchcache[0]->lastsearchtext,SEARCHREPLY_MAX_FILESIZE);
										while (buf[0] && buf[strlen(buf)-1]=='/') buf[strlen(buf)-1]=0;
										strcat(buf,"/");
										char *p=buf+strlen(buf);
										C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(a>>16);
										sr->get_item(a&0xffff,NULL,p,NULL,NULL,NULL,NULL);
										if (!strcmp(p,PARENT_DIRSTRING)) {
											*--p=0;
											while (p > buf && *p != '/') p--;
											if (*p == '/' && p == buf) p++;
											*p=0;
										};
									}
									else {
										char host[33];
										C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(a>>16);
										sr->get_item(a&0xffff,NULL,buf,NULL,NULL,NULL,NULL);
										if (buf[0] == '/') {
											safe_strncpy(host,sr->_guidstr,sizeof(host));
											if (buf[1] == '?') {
												char buf2[SEARCHREPLY_MAX_FILESIZE+36];
												sprintf(buf2,"/%s%s",host,buf+2);
												safe_strncpy(buf,buf2,sizeof(buf));
											};
										}
										else buf[0]=0;
									};
									if (buf[0]) {
				if (!is_copyloc) {
											char *p=buf+strlen(buf);
											while (p > buf && *p != '/') p--;
											*p=0;
										};
										if (buf[0]) {
					if (!is_copyloc) {
#ifdef _DEFINE_WXUI
						Search_Search(buf);
#else
												SetDlgItemText(hwndDlg,IDC_SEARCHTEXT,buf);
						SendMessageSearch(hwndDlg);
#endif
											}
											else {
												char buf2[SEARCHREPLY_MAX_FILESIZE*3+36*3+7];
												strcpy(buf2,"waste:");
												char *out=buf2+6;
												char *in=buf;
												while (*in) {
													if (*in == ' ') {
														in++;
														strcpy(out,"%20");
														out+=3;
													}
													else *out++=*in++;
												};
												*out=0;
#ifdef _DEFINE_WXUI

						if (wxTheClipboard->Open()) {
							// clipboard will call delete()
							wxTheClipboard->SetData(new
								wxTextDataObject(cstr_to_wxstr(buf2)));
							wxTheClipboard->Close();
						}

#else

												HANDLE h=GlobalAlloc(GMEM_MOVEABLE,strlen(buf2)+1);
												void *t=GlobalLock(h);
												memcpy(t,buf2,strlen(buf2)+1);
												GlobalUnlock(h);
												OpenClipboard(hwndDlg);
												EmptyClipboard();
												SetClipboardData(CF_TEXT,h);
												CloseClipboard();
#endif
											};
										};
									};
									break;
								};
							};
}

void srchwnd_ID_PRIVMSGNODE() {
							int l=g_lvsearchres.GetCount();
	int x;
							for (x = 0; x < l; x ++) {
								if (g_lvsearchres.GetSelected(x)) {
									char buf[SEARCHREPLY_MAX_FILESIZE];
									int a=m_srch_sort[x];
									C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(a>>16);
									sr->get_item(a&0xffff,NULL,buf,NULL,NULL,NULL,NULL);
									if (strlen(buf)<sizeof(g_regnick)) chat_ShowRoom(buf,1);
								};
							};
}

void srchwnd_ID_WHOISUSER() {
							int l=g_lvsearchres.GetCount();
	int x;
							for (x = 0; x < l; x ++) {
								if (g_lvsearchres.GetSelected(x)) {
									char buf[SEARCHREPLY_MAX_FILESIZE];
									int a=m_srch_sort[x];
									C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(a>>16);
									sr->get_item(a&0xffff,NULL,buf,NULL,NULL,NULL,NULL);
									if (strlen(buf)<sizeof(g_regnick)) {
										T_Message msg;
										//send a message to text that is /whois
										C_MessageChat req;
										req.set_chatstring("/whois");
										req.set_dest(buf);
										req.set_src(g_regnick);
										msg.data=req.Make();
										msg.message_type=MESSAGE_CHAT;
										if (msg.data) {
											msg.message_length=msg.data->GetLength();
											g_mql->send(&msg);
										};
									};
								};
							};
}

void srchwnd_RCLICK(int mouse_x, int mouse_y) {
	int isRoot = !(g_searchcache[0]->lastsearchtext[0] != '/' ||
		strstr(g_searchcache[0]->lastsearchtext+1,"/") ||
		(g_searchcache[0]->lastsearchtext[1] &&
		g_searchcache[0]->lastsearchtext[strlen(g_searchcache[0]->lastsearchtext)-1]!='*'));

	int isDirSelected=0;
	int sel=0;
	int isParent=0;

	int l=g_lvsearchres.GetCount();
	int x;//,a=0; //HACK "a" never used
	for (x = 0; x < l; x ++) {
		if (g_lvsearchres.GetSelected(x)) {
			char name[SEARCHREPLY_MAX_FILESIZE];
			char type[SEARCHREPLY_MAX_METASIZE];
			sel++;
			if (g_searchcache[0]->lastsearchtext[0] != '/') break;
			int a=m_srch_sort[x];
			C_MessageSearchReply *sr=g_searchcache[0]->searchreplies.Get(a>>16);
			sr->get_item(a&0xffff,NULL,name,type,NULL,NULL,NULL);
			if (!stricmp(type,DIRECTORY_STRING)) isDirSelected=1;
			if (!strncmp(name,"..",2)) isParent=1;
						};
					};

#ifdef _DEFINE_WXUI
	wxMenu *hMenu;
#define EnableMenuItem(menu,id,on)  menu->Enable(id,on)
#define MF_ENABLED true
#define MF_GRAYED false
#else
	HMENU hMenu=GetSubMenu(g_context_menus,
		isRoot?6:g_searchcache[0]->lastsearchtext[0] == '/'?7:1);
#endif

	if (isRoot) {
#ifdef _DEFINE_WXUI
		hMenu = AppResources::CreateMenuSearchresUserPopup();
#endif
		EnableMenuItem(hMenu,ID_COPYLOC,sel==1?MF_ENABLED:MF_GRAYED);
		EnableMenuItem(hMenu,ID_BROWSENODE,sel==1?MF_ENABLED:MF_GRAYED);
		EnableMenuItem(hMenu,ID_PRIVMSGNODE,sel?MF_ENABLED:MF_GRAYED);
		EnableMenuItem(hMenu,ID_SENDFILENODE,sel?MF_ENABLED:MF_GRAYED);
	}
	else if (g_searchcache[0]->lastsearchtext[0] == '/') {
#ifdef _DEFINE_WXUI
		hMenu = AppResources::CreateMenuSearchresFilePopup();
#endif
		EnableMenuItem(hMenu,ID_COPYLOC,sel==1?MF_ENABLED:MF_GRAYED);
		if (isDirSelected) {
#ifndef _DEFINE_WXUI
			ModifyMenu(hMenu,0,MF_BYPOSITION,ID_BROWSEFOLDER,"Browse directory");
			ModifyMenu(hMenu,1,MF_BYPOSITION,ID_DOWNLOADFOLDER,"Download");
#endif
			EnableMenuItem(hMenu,ID_BROWSEFOLDER,sel==1?MF_ENABLED:MF_GRAYED);
		}
		else {
#ifndef _DEFINE_WXUI
			ModifyMenu(hMenu,0,MF_BYPOSITION,ID_DOWNLOADFOLDER,"Download");
			ModifyMenu(hMenu,1,MF_BYPOSITION,ID_BROWSEFOLDER,"Browse directory");
#endif
			EnableMenuItem(hMenu,ID_BROWSEFOLDER,MF_GRAYED);
				};
		EnableMenuItem(hMenu,ID_DOWNLOADFOLDER,(sel&&(!isParent||sel>1))?MF_ENABLED:MF_GRAYED);
	}
	else { //normal search menu
#ifdef _DEFINE_WXUI
		hMenu = AppResources::CreateMenuSearchResPopup();
#endif
		EnableMenuItem(hMenu,ID_COPYLOC,sel==1?MF_ENABLED:MF_GRAYED);
		EnableMenuItem(hMenu,ID_DLPARENT,sel==1?MF_ENABLED:MF_GRAYED);
		EnableMenuItem(hMenu,ID_BROWSELOC,sel==1?MF_ENABLED:MF_GRAYED);
		EnableMenuItem(hMenu,ID_DOWNLOAD,sel?MF_ENABLED:MF_GRAYED);
			};

#ifndef _DEFINE_WXUI
	void popup_menu_wait_and_dispatch(HMENU *);
	popup_menu_wait_and_dispatch(hMenu);
#else

#if (wxMAJOR_VERSION >= 2 && wxMINOR_VERSION >= 5)
	g_search_wnd->PopupMenu(hMenu); // no loc = current mouse pos
#else
	g_search_wnd->PopupMenu(hMenu, mouse_x, mouse_y);
#endif

#endif
}

void SrchWnd_LVN_COLUMNCLICK(int col) {
	int l_sc=g_config->ReadInt(CONFIG_search_sortcol,CONFIG_search_sortcol_DEFAULT);
	int l_sd=g_config->ReadInt(CONFIG_search_sortdir,CONFIG_search_sortdir_DEFAULT);
	if (col == l_sc) l_sd=!l_sd;
	else l_sc=col;

	g_config->WriteInt(CONFIG_search_sortcol,l_sc);
	g_config->WriteInt(CONFIG_search_sortdir,l_sd);

	Search_Resort();
}

#ifndef _DEFINE_WXUI
void popup_menu_wait_and_dispatch(HMENU *hMenu) {
	POINT p;
	GetCursorPos(&p);

	x=TrackPopupMenu(hMenu,TPM_RETURNCMD|TPM_RIGHTBUTTON|TPM_LEFTBUTTON|TPM_NONOTIFY,p.x,p.y,0,hwndDlg,NULL);

	if (x == ID_DOWNLOAD || x == ID_BROWSEFOLDER || x == ID_DOWNLOADFOLDER ||
		x == ID_BROWSENODE)
				{
		SrchWnd_OnItemRun(hwndDlg,x != ID_DOWNLOADFOLDER);
	}
	else if (x == ID_SENDFILENODE) {
		srchwnd_ID_SENDFILENODE();
	}
	else if (x == ID_DLPARENT) {
		srchwnd_ID_DLPARENT();
	}
	else if (x == ID_BROWSELOC || x == ID_COPYLOC) {
		srchwnd_ID_BROWSECOPYLOC(x == ID_COPYLOC);
	}
	else if (x == ID_PRIVMSGNODE) {
		srchwnd_ID_PRIVMSGNODE();
	}
	else if (x == ID_WHOISUSER) {
		srchwnd_ID_WHOISUSER();
	};
}
#endif
#endif /* _DEFINE_SRV */

#if !defined(_DEFINE_WXUI) && !defined(_DEFINE_SRV)
	BOOL WINAPI Search_DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
					{
		static dlgSizeInfo sizeinf={"search",50,50,567,430};
		static ChildWndResizeItem rlist[]={
			{IDC_SEARCHTEXT,0x0010},
			{IDC_SEARCH,0x1010},
			{IDC_SEARCHSTATS,0x0111},
			{IDC_SEARCHRES,0x0011},
			{IDC_SRESULTLBL,0x0101},
			{IDC_EDIT_FILTER,0x0101},
					};
		switch (uMsg)
					{
		case WM_GETMINMAXINFO:
			{
				LPMINMAXINFO m=(LPMINMAXINFO)lParam;
				if (m) {
					m->ptMinTrackSize.x=164;
					m->ptMinTrackSize.y=134;
					};
				return 0;
			};
		case WM_CLOSE:
			return srch_WM_CLOSE();
		case WM_INITDIALOG:
					{
				srch_oldWndProc=(WNDPROC) SetWindowLong(FindWindowEx(GetDlgItem(hwndDlg,IDC_SEARCHTEXT),NULL,"Edit",NULL),GWL_WNDPROC,(LONG)srch_newWndProc);
				srchres_oldWndProc=(WNDPROC) SetWindowLong(GetDlgItem(hwndDlg,IDC_SEARCHRES),GWL_WNDPROC,(LONG)srchres_newWndProc);
				SetTimer(hwndDlg,1,1000,NULL);
				CreateTooltip(GetDlgItem(hwndDlg,IDC_GOROOT),"Go to user list");
				CreateTooltip(GetDlgItem(hwndDlg,IDC_GOPARENT),"Go to parent directory");
				CreateTooltip(GetDlgItem(hwndDlg,IDC_BACK),"Go back");
				CreateTooltip(GetDlgItem(hwndDlg,IDC_BACK2),"Go back (list)");
				CreateTooltip(GetDlgItem(hwndDlg,IDC_FORWARD),"Go forward");
				CreateTooltip(GetDlgItem(hwndDlg,IDC_FORWARD2),"Go forward (list)");
				CreateTooltip(GetDlgItem(hwndDlg,IDC_SEARCH),"Go to location");

				SendDlgItemMessage(hwndDlg,IDC_SEARCH,BM_SETIMAGE,IMAGE_ICON,
					(LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_SEARCH),IMAGE_ICON,16,16,0));

				SendDlgItemMessage(hwndDlg,IDC_GOROOT,BM_SETIMAGE,IMAGE_ICON,
					(LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_ROOT),IMAGE_ICON,16,16,0));
				SendDlgItemMessage(hwndDlg,IDC_GOPARENT,BM_SETIMAGE,IMAGE_ICON,
					(LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_PARENT),IMAGE_ICON,16,16,0));

				SendDlgItemMessage(hwndDlg,IDC_BACK,BM_SETIMAGE,IMAGE_ICON,
					(LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_BACKWARD),IMAGE_ICON,16,16,0));
				SendDlgItemMessage(hwndDlg,IDC_FORWARD,BM_SETIMAGE,IMAGE_ICON,
					(LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_FORWARD),IMAGE_ICON,16,16,0));

				SendDlgItemMessage(hwndDlg,IDC_BACK2,BM_SETIMAGE,IMAGE_ICON,
					(LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_MORE),IMAGE_ICON,7,4,0));
				SendDlgItemMessage(hwndDlg,IDC_FORWARD2,BM_SETIMAGE,IMAGE_ICON,
					(LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_MORE),IMAGE_ICON,7,4,0));

				g_lvsearchres.setwnd(GetDlgItem(hwndDlg,IDC_SEARCHRES));
				{
					HIMAGELIST hImageList;
					hImageList = ImageList_Create(16,16, ILC_COLOR32|ILC_MASK, 3, 3);
					ImageList_AddIcon(hImageList,(HICON)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_FILE),IMAGE_ICON,16,16,0));
					ImageList_AddIcon(hImageList,(HICON)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_DIR),IMAGE_ICON,16,16,0));
					ImageList_AddIcon(hImageList,(HICON)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_USER),IMAGE_ICON,16,16,0));
					ListView_SetImageList(g_lvsearchres.getwnd(),hImageList,LVSIL_SMALL);
					};
				g_lvsearchres.AddCol("Item",g_config->ReadInt(CONFIG_search_col1,CONFIG_search_col1_DEFAULT));
				g_lvsearchres.AddCol("Size",g_config->ReadInt(CONFIG_search_col2,CONFIG_search_col2_DEFAULT));
				g_lvsearchres.AddCol("Type",g_config->ReadInt(CONFIG_search_col3,CONFIG_search_col3_DEFAULT));
				g_lvsearchres.AddCol("Date/Time",g_config->ReadInt(CONFIG_search_col6,CONFIG_search_col6_DEFAULT));
				g_lvsearchres.AddCol("Speed",g_config->ReadInt(CONFIG_search_col4,CONFIG_search_col4_DEFAULT));
				g_lvsearchres.AddCol("Location",g_config->ReadInt(CONFIG_search_col5,CONFIG_search_col5_DEFAULT));
				childresize_init(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
					{
								int x;
					for (x = 0 ; x < MAX_SRCHTEXTS; x ++) {
						char buf[123];
						sprintf(buf,"srchcb_%d",x);
						const char *o=g_config->ReadString(buf,"");
						if (*o) {
							char buf[512];
							safe_strncpy(buf,o,sizeof(buf));
							SendDlgItemMessage(hwndDlg,IDC_SEARCHTEXT,CB_INSERTSTRING,x,(LPARAM)buf);
								};
								};
					EnableWindow(GetDlgItem(hwndDlg,IDC_SEARCH),0);
					EnableWindow(GetDlgItem(hwndDlg,IDC_GOPARENT),0);
						};
					};
		case WM_USER_TITLEUPDATE:
					{
							char buf[1024];
				set_srch_title(buf, 1023);
				SetWndTitle(hwndDlg,buf);
				return 0;
					};
		case WM_SHOWWINDOW:
						{
				static int m_hack;
				if (!m_hack) {
					m_hack=1;
					int m=g_config->ReadInt(CONFIG_search_maximized,CONFIG_search_maximized_DEFAULT);
					handleDialogSizeMsgs(hwndDlg,WM_INITDIALOG,0,0,&sizeinf);
					if (m) ShowWindow(hwndDlg,SW_SHOWMAXIMIZED);
							};
				return 0;
						};
		case WM_TIMER:
						{
				if (wParam == 1) {
					if (IsWindowVisible(hwndDlg) && g_mql && g_mql->GetNumQueues()) {
						KillTimer(hwndDlg,wParam);
						if (g_searchcache[0]->lastsearchtext[0] == '/') {
							SendMessageSearch(hwndDlg);
								};
							};
						};
				return 0;
							};
		case WM_DESTROY:
			return srchwnd_WM_DESTROY();
		case WM_SIZE:
			{
				if (wParam != SIZE_MINIMIZED) {
					childresize_resize(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
					handleDialogSizeMsgs(hwndDlg,uMsg,wParam,lParam,&sizeinf);
						};
				return 0;
						};
		case WM_MOVE:
						{
				handleDialogSizeMsgs(hwndDlg,uMsg,wParam,lParam,&sizeinf);
				return 0;
						};
		case WM_NOTIFY:
			{
				LPNMHDR l=(LPNMHDR)lParam;
				if (l->idFrom == IDC_SEARCHRES) {
					if (l->code == LVN_ODFINDITEM) {
						NMLVFINDITEM *t = (NMLVFINDITEM *)lParam;

						int i=t->iStart;
						if (i >= m_srch_sort_len) i=0;

						int cnt=m_srch_sort_len-i;
						if (t->lvfi.flags & LVFI_WRAP) cnt+=i;

						while (cnt-->0) {
							int repidx=m_srch_sort[i]&0xffff;
							C_MessageSearchReply *tr=g_searchcache[0]->searchreplies.Get(m_srch_sort[i]>>16);
							char name[SEARCHREPLY_MAX_FILESIZE];
							tr->get_item(repidx,NULL,name,NULL,NULL,NULL,NULL);
							if (t->lvfi.flags & (4|LVFI_PARTIAL)) {
								if (!strnicmp(name,t->lvfi.psz,strlen(t->lvfi.psz))) {
									SetWindowLong(hwndDlg,DWL_MSGRESULT,i);
									return 1;
								};
							}
							else if (t->lvfi.flags & LVFI_STRING) {
								if (!stricmp(name,t->lvfi.psz)) {
									SetWindowLong(hwndDlg,DWL_MSGRESULT,i);
									return 1;
							};
							}
							else {
								SetWindowLong(hwndDlg,DWL_MSGRESULT,-1);
								return 1;
								};
							if (++i == m_srch_sort_len) i=0;
							};
						SetWindowLong(hwndDlg,DWL_MSGRESULT,-1);
						return 1;
						}
					else if (l->code == LVN_GETDISPINFO) {
						return SrchWnd_GETDISPINFO(lParam);
					}
					else if (l->code == NM_DBLCLK) {
						SrchWnd_OnItemRun(hwndDlg,1);
					}
					else if (l->code == LVN_COLUMNCLICK) {
						NMLISTVIEW *p=(NMLISTVIEW*)lParam;
						SrchWnd_LVN_COLUMNCLICK(p->iSubItem);
					}
					else if (l->code == NM_RCLICK) {
						srchwnd_RCLICK();
							};
									};
				return 0;
								};
		case WM_COMMAND:
			{
				switch (LOWORD(wParam))
				{
				case IDC_EDIT_FILTER:
					{
						if (HIWORD(wParam) == EN_CHANGE)
							Search_Resort();
						break;
							};
				case IDC_GOROOT:
					{
						Search_Search("/");
						break;
					};
				case IDC_BACK:
				case IDC_BACK2:
					return srch_WM_COMMAND_IDC_BACK(LOWORD(wParam) == IDC_BACK2);
				case IDC_FORWARD:
				case IDC_FORWARD2:
					return srch_WM_COMMAND_IDC_FORWARD(LOWORD(wParam) == IDC_FORWARD2);
				case IDC_GOPARENT:
					return srch_WM_COMMAND_IDC_GOPARENT();
				case IDC_SEARCH:
					return srch_WM_COMMAND_IDC_SEARCH();
				case IDC_SEARCHTEXT:
					{
						if (HIWORD(wParam) == CBN_SELCHANGE) {
							EnableWindow(GetDlgItem(hwndDlg,IDC_SEARCH),1);
							PostMessage(hwndDlg,WM_COMMAND,IDC_SEARCH,0);
						}
						else {
							srch_SEARCHTEXT_SELCHANGE();
						};
						return 0;
					};
				};
				return 0;
			};
		};
		return 0;
	}
#endif

//vim:ts=4:sw=4:
