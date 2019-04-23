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
 * This file implements the display of file browser
 * Author: Frank Yang
 * Date Created: 2003.12.1
*/
#include "main.h"
#include "vaste_idd_search.h"
#include "platform.h"
#include "config.h"
#include "itemlist.h"
#include "m_search.h"
#include "srchwnd.h"
#include "ui_listview.h"
#include "filedb.h"
#include "qcombobox.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qpopupmenu.h"
#include "qlistview.h"
#include "qheader.h"
#include "qpixmap.h"
#include "qcursor.h"
#include "qtimer.h"
#include "qapplication.h"
#include "qclipboard.h"
#include "qulf_dialog.h"
#include "qevent.h"
#include "xferwnd.h"
#include "m_chat.h"
#include "vaste_chat.h"
#include <fstream.h>
#define MAX_SRCHTEXTS 15
#define SEARCHCACHE_EXPIRE_TIME (5*60*1000) // 5 minutes
#define MAX_SEARCH_HISTORY_LEN 200
#ifdef QTUI
extern W_ListView g_lvsearchres;
extern C_Config *g_config;
extern int m_srch_sort_len,m_fullsize,m_srch_sort_alloc;
extern unsigned int *m_srch_sort;
#endif
extern SearchCacheItem *g_searchcache[SEARCHCACHE_NUMITEMS];
extern int g_searchhistory_position; 
extern C_ItemList<char> g_searchhistory;
extern C_MessageQueueList *g_mql;
extern int g_conspeed;
extern T_GUID g_client_id;
extern int g_accept_downloads;
extern C_FileDB *g_database;
extern int g_appendprofiletitles;
extern char g_profile_name[128];
extern int g_search_showfull;
extern int g_search_showfullbytes;
extern char g_regnick[32];

extern void main_MsgCallback(T_Message *message, C_MessageQueueList *_this, C_Connection *cn);
extern void main_onGotNick(char *nick, int del);
extern void SrchWnd_OnItemRun(int browseIfDir);
Vaste_Idd_Search::Vaste_Idd_Search( QWidget* parent , const char* name , WFlags fl ): IDD_SEARCH( parent, name, fl )
{
    SetWndTitle();
#include "vaste_widget_palette.xx"
    setPalette( dialog_pal );
    ListView_IDC_LIST1->setPalette( listview_pal );
    PushButton_IDC_GOROOT->setPalette( pushbutton_pal );
    PushButton_IDC_GOPARENT->setPalette( pushbutton_pal );
    PushButton_IDC_BACK1->setPalette( pushbutton_pal );
    PushButton_IDC_BACK2->setPalette( pushbutton_pal );
    PushButton_IDC_FORWARD->setPalette( pushbutton_pal );
    PushButton_IDC_FORWARD2->setPalette( pushbutton_pal );
    LineEdit_IDC_EDIT14->setPalette( texteditor_pal );
    PushButton_IDC_SEARCH1->setPalette( pushbutton_pal );
	ComboBox_IDC_SEARCHTEXT->setPalette( combobox_pal) ;
	Main_Pane->setPalette( frame_pal );

	// skin settings here.
	QPixmap button_bg("skins/vaste_toolbutton_bg.png");
	
	PushButton_IDC_GOROOT->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_GOPARENT->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_BACK1->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_FORWARD->setPaletteBackgroundPixmap(button_bg);

	QPixmap dlg_bg("skins/vaste_pane_bg.png");
	QPixmap pane_bg("skins/vaste_pane_bg1.png");
	
	setPaletteBackgroundPixmap(dlg_bg);
	Main_Pane->setPaletteBackgroundPixmap(pane_bg);

	QPixmap listview_head("skins/vaste_listview_header.png");
	ListView_IDC_LIST1->header()->setPaletteBackgroundPixmap(listview_head);
	// skin settings end.

	QPixmap turn_root_0("icons/vaste_browse_root_enable.png");
	QPixmap turn_parent_0("icons/vaste_browse_parent_enable.png");
	QPixmap turn_back_0("icons/vaste_browse_back_enable.png");
	QPixmap turn_forward_0("icons/vaste_browse_forward_enable.png");

	PushButton_IDC_GOROOT->setPixmap( turn_root_0 );
	PushButton_IDC_GOPARENT->setPixmap( turn_parent_0 );
	PushButton_IDC_BACK1->setPixmap( turn_back_0 );
	PushButton_IDC_FORWARD->setPixmap( turn_forward_0 );

	g_lvsearchres.setwnd(ListView_IDC_LIST1);
	g_lvsearchres.AddCol("Item",g_config->ReadInt("search_col1",140));
	g_lvsearchres.AddCol("Size",g_config->ReadInt("search_col2",80));
	g_lvsearchres.AddCol("Type",g_config->ReadInt("search_col3",60));
	g_lvsearchres.AddCol("Date/Time",g_config->ReadInt("search_col6",80));
	g_lvsearchres.AddCol("Speed",g_config->ReadInt("search_col4",60));
	g_lvsearchres.AddCol("Location",100);
	PushButton_IDC_GOPARENT->setEnabled(false);
	PushButton_IDC_SEARCH1->setEnabled(false);
        QHeader * header = ListView_IDC_LIST1->header();
        connect(header, SIGNAL( clicked(int) ), this, SLOT( onGoItem(int) ) );
	QTimer *timer =new QTimer(this);
        connect(timer, SIGNAL( timeout() ), this, SLOT( timerDone() ) );
	timer->start(2000,TRUE);
	
}	

void Vaste_Idd_Search::closeEvent(QCloseEvent *e)
{
	g_config->WriteInt("search_vis",0);
	e->accept();
}

void Vaste_Idd_Search::timerDone()
{
        if (g_mql && g_mql->GetNumQueues())
        {
          if (g_searchcache[0]->lastsearchtext[0] == '/') 
          	onGoSearch();
        }
	
}

Vaste_Idd_Search::~Vaste_Idd_Search()
{
      g_config->WriteInt("search_col1",g_lvsearchres.GetColumnWidth(0));
      g_config->WriteInt("search_col2",g_lvsearchres.GetColumnWidth(1));
      g_config->WriteInt("search_col3",g_lvsearchres.GetColumnWidth(2));
      g_config->WriteInt("search_col6",g_lvsearchres.GetColumnWidth(3));
      g_config->WriteInt("search_col4",g_lvsearchres.GetColumnWidth(4));
      g_config->WriteInt("search_col5",g_lvsearchres.GetColumnWidth(5));

      {
        int a=!!g_config->ReadInt("srchcb_use",0);
        int x;
        for (x = 0; x < MAX_SRCHTEXTS; x ++)
        {
          char buf[123];
          char obuf[1024];
          obuf[0]=0;
          if (a)
          {
		onGoSearchText();
          }
          sprintf(buf,"srchcb_%d",x);
          g_config->WriteString(buf,obuf);
        }
      }
}

void Vaste_Idd_Search::slotEnableSearch(const QString & string)
{
	if(strlen(string) == 0)
        	PushButton_IDC_SEARCH1->setEnabled(false);
	else
        	PushButton_IDC_SEARCH1->setEnabled(true);
		
}

void Vaste_Idd_Search::onGoEdit()
{
	Search_Resort();
	
}
void Vaste_Idd_Search::Notify(int item)
{
	  int n = g_lvsearchres.GetCount();	
         if (!m_srch_sort || (item) < 0 || (item) >= m_srch_sort_len) return;

         int repidx=m_srch_sort[item]&0xffff;
         C_MessageSearchReply *tr=g_searchcache[0]->searchreplies.Get(m_srch_sort[item]>>16);	
         
         char name[SEARCHREPLY_MAX_FILESIZE];
         char metadata[SEARCHREPLY_MAX_METASIZE];
         char *nameptr=name;
	 int id;
	 unsigned int length_low,length_high;
         int timev;
         tr->get_item(repidx,&id,name,metadata,(int*)&length_low,(int*)&length_high,&timev);
         int isnode=!stricmp(metadata,USER_STRING);
         int isdir=!stricmp(metadata,DIRECTORY_STRING);
         
          char str[SEARCHREPLY_MAX_FILESIZE+128];
	  bool insert = true;
          {
          	if (g_searchcache[0]->lastsearchtext[0] != '/' && g_search_showfull!=1)
           	{
            		while (*nameptr) nameptr++;
            		while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
            		if (g_search_showfull == 2 && nameptr >= name)
              		{
               			nameptr--;
               			while (nameptr >= name && *nameptr != '/' && *nameptr != '\\') nameptr--;
               		}
             		nameptr++;
             		while (*nameptr == '/' || *nameptr == '\\') nameptr++;
                 }
	       QPixmap *image = 0;
	       if(isdir)
	               image = new QPixmap("icons/vaste_dir.png");
	       else if(isnode)	
		       image = new QPixmap("icons/vaste_user_small.png");
	       else
	       	       image = new QPixmap("icons/vaste_file.png");
	       g_lvsearchres.InsertItem(item,nameptr,0);	     
    	       QListViewItem* ListItem =(QListViewItem *) g_lvsearchres.GetItem(item);	     
	       ListItem->setPixmap(0,*image);	
            }
           
           
           
            {
            	if (g_searchcache[0]->lastsearchtext[0] == '/' && !stricmp(metadata,DIRECTORY_STRING))
                    {
                      if (strncmp(name,"..",2))
                      {
			#if defined (_WIN32)
			__int64 a=length_high;
			#elif defined (_LINUX)
                        long long a=length_high;
			#endif
                        a<<=10;

                        int lb_l=(int)(a&0xffffffff);
                        int lb_h=(int)(a>>32);

                        FormatSizeStr64(str,lb_l,lb_h);
  			sprintf(str+strlen(str)," in %d items",length_low);
                      }
                      else 
    		      	strcpy(str,"");
                    }
                    else if (g_searchcache[0]->lastsearchtext[0] == '/' && !stricmp(metadata,USER_STRING))
                    {
                      if (!g_searchcache[0]->lastsearchtext[1] && !strstr(name,"/") && strlen(name)<24 &&
                           name[0] != '.')
                      {
                        main_onGotNick(name,0);
                      }
                      if (length_low || length_high && strncmp(name,"..",2))
                      {
			#if defined (_WIN32)
			__int64 a=length_high;
			#elif defined (_LINUX)
                        long long a=length_high;
			#endif
                        a<<=10;

                        int lb_l=(int)(a&0xffffffff);
                        int lb_h=(int)(a>>32);

                        FormatSizeStr64(str,lb_l,lb_h);
  			sprintf(str+strlen(str)," in %d items",length_low);
                      }
  		      else
                      	strcpy(str,"");
                    }
		      else 
                      {
                      	if ((g_search_showfullbytes || length_low < 1024) && !length_high) sprintf(str,"%u",length_low);
                        else 
                        {
                        	FormatSizeStr64(str,length_low,length_high);
                        }
                    }
                    nameptr=str;
	           g_lvsearchres.SetItemText(item,1,nameptr);
            	}
            	
            	{
            		if (!metadata[0])
                    	{
                      		char *p=::extension(name);
                      		if (!*p) strcpy(metadata,"(no info)");
                      		else 
                      		{
                        		int x;
                        		for (x=0; *p && x < sizeof(metadata)-1; x++, p++) metadata[x]=toupper(*p);
                        		metadata[x]=0;
                      		}
                    	}
                         nameptr=metadata;
	                 g_lvsearchres.SetItemText(item,2,nameptr);
            	}
            	
            	{
            	if (timev) {
                      struct tm *a=localtime((const time_t *)&timev);
                      if (!a)
                      {
                        time_t b=0;
                        a=gmtime(&b);
                      }

                      if (a) strftime(str,sizeof(str),"%c",a);
                     // else wsprintf(str,"?:%d",timev);
                      nameptr=str;
                    }
                    else nameptr="";	
                    nameptr=str;
	            g_lvsearchres.SetItemText(item,3,nameptr);
            	}
            	
            	{
                    sprintf(str,"%dms (",tr->_latency);
                    get_speedstr(tr->get_conspeed(),str+strlen(str));
                    strcat(str,")");
                    nameptr=str;
	            g_lvsearchres.SetItemText(item,4,nameptr);
            	}
            	
            	{
            	 sprintf(str,"%s:%d",tr->_guidstr,id);
                 nameptr=str;
	         g_lvsearchres.SetItemText(item,5,nameptr);
            	}
}
void Vaste_Idd_Search::SetWndTitle()
{
        char buf[1024];
        strcpy(buf,"Browser");
        int numresults=m_srch_sort_len;
        int numfresults=m_fullsize;
        int a=0;

        if (numfresults) 
        {
          if (numfresults != numresults)
            sprintf(buf+strlen(buf),"%s%d of %d result%s",a++?", ":" (",numresults,numfresults,numfresults==1?"":"s");
          else
            sprintf(buf+strlen(buf),"%s%d result%s",a++?", ":" (",numresults,numresults==1?"":"s");
        }
        if (a) strcat(buf,")");
       
        strcat(buf," - ");
	strcat(buf, APP_NAME);
	char titlebase[1024];      
	if (g_appendprofiletitles && g_profile_name[0] && strlen(titlebase)+strlen(g_profile_name)+3 < sizeof(titlebase))
	{	
		sprintf(titlebase,"%s/%s",buf,g_profile_name);
        	setCaption(titlebase);  
	}
	else 
	        setCaption(buf);
}

void Vaste_Idd_Search::languageChange()
{
}
void Vaste_Idd_Search::onGoRoot()
{
	Search_Search("/");
}

void Vaste_Idd_Search::onGoParent()
{
	if (g_searchcache[0]->lastsearchtext[0] == '/')
          {
            char buf[1024];
            safe_strncpy(buf,g_searchcache[0]->lastsearchtext,sizeof(buf));

            char *p=buf;
            while (*p) p++;
            while (p > buf+1 && p[-1] == '/') p--;
            *p=0;
            while (p >= buf && *p != '/') p--;
            if (p >= buf) p[1]=0; 
            Search_Search(buf);
          }
}

void Vaste_Idd_Search::onGoLeft()
{
	if (g_searchhistory_position > 0)
            {
		QPopupMenu *contextMenu= new QPopupMenu( this );
		Q_CHECK_PTR( contextMenu );
              int x;
              for (x = 0; x < 10; x ++)
              {
                if (g_searchhistory_position - (x + 1) < 0) break;
                char *str=g_searchhistory.Get(g_searchhistory_position - (x + 1));
                if (!str) break;
		contextMenu->insertItem(str);
	
              }
	      int id = contextMenu->exec(QCursor::pos());
	      if(id == -1)
	      	return ;
	      int index = contextMenu->indexOf(id) + 1;
	      char menutext[512];
	      strcpy(menutext,contextMenu->text(id));
	      contextMenu->clear();
              Search_Search(g_searchhistory.Get(g_searchhistory_position -= index));
             PushButton_IDC_BACK1->setFocus();
	}
}

void Vaste_Idd_Search::onGoForward()
{
          if (g_searchhistory_position < g_searchhistory.GetSize()-1)
              Search_Search(g_searchhistory.Get(++g_searchhistory_position));
		
}

void Vaste_Idd_Search::onGoRight()
{
          if (g_searchhistory_position < g_searchhistory.GetSize()-1)
            {
	        QPopupMenu *contextMenu= new QPopupMenu( this );
		Q_CHECK_PTR( contextMenu );
              int x;
              for (x = 0; x < 10; x ++)
              {
                char *str=g_searchhistory.Get(g_searchhistory_position + x + 1);
                if (!str) break;
		contextMenu->insertItem(str);
              }
	      int id = contextMenu->exec(QCursor::pos());
	      if(id == -1)
	      	return ;
	      int index = contextMenu->indexOf(id) + 1;
	      char menutext[512];
	      strcpy(menutext,contextMenu->text(id));
	      contextMenu->clear();
              Search_Search(g_searchhistory.Get(g_searchhistory_position += index));
      	      PushButton_IDC_FORWARD->setFocus();
            }
}

void Vaste_Idd_Search::onGoBack()
{
	if (g_searchhistory_position > 0)
        	Search_Search(g_searchhistory.Get(--g_searchhistory_position));
}

void Vaste_Idd_Search::onGoSearchText()
{
	onGoSearch();
}

void Vaste_Idd_Search::onGoSearch()
{
	    Vaste_Idd_Search * search_wnd = (Vaste_Idd_Search *)g_search_wnd;
            // store nav position in last cached item
            {
              int n=g_lvsearchres.GetCount();
              int x;
              for (x = 0; x < n && !g_lvsearchres.GetSelected(x); x ++);
              if (x >= 0 && x < m_srch_sort_len)
              	g_searchcache[0]->lastvisitem=m_srch_sort[x]; //save last pos
            }

            int ci;
            int cached=0;
            int replpos=0; // position to replace
       	    char text[512];
	    strcpy(text,search_wnd->ComboBox_IDC_SEARCHTEXT->currentText());
            if (!strnicmp(text,"padlocksl:",6))
            {
              char *in=text+6, *out=text;
              while (*in)
              {
                if (!strncmp(in,"%20",3))
                {
                  *out++=' ';
                  in+=3;
                }
                else *out++=*in++;
              }
              *out=0;
	    int count = search_wnd->ComboBox_IDC_SEARCHTEXT->count();
	    bool find = false;
	   for(int n = 0;n < count;n++)
	   {
	   	if(!strcmp(text,search_wnd->ComboBox_IDC_SEARCHTEXT->text(n)))
		{
			find = true;
			count = n;
			break;
		}
	   }
           if(!find)
	   {
	   	search_wnd->ComboBox_IDC_SEARCHTEXT->insertItem(text);	
	   }	
	 search_wnd->ComboBox_IDC_SEARCHTEXT->setCurrentItem(count);
 }


	    search_wnd->PushButton_IDC_GOPARENT->setEnabled(text[0]=='/'&&text[1]);
            if (strlen(text)<=0) return ;

            {
              char *t=g_searchhistory.Get(g_searchhistory_position);
              if (!t || strcmp(t,text))
              {
                if (g_searchhistory.GetSize()) g_searchhistory_position++;
                while (g_searchhistory.GetSize() > g_searchhistory_position)
                {
                  free(g_searchhistory.Get(g_searchhistory_position));
                  g_searchhistory.Del(g_searchhistory_position);
                }
                g_searchhistory.Add(strdup(text));
                if (g_searchhistory.GetSize() > MAX_SEARCH_HISTORY_LEN) 
                {
                  free(g_searchhistory.Get(0));
                  g_searchhistory.Del(0);
                  g_searchhistory_position--;
                }
              }
            }

	    search_wnd->PushButton_IDC_BACK1->setEnabled(g_searchhistory_position>0);
	    search_wnd->PushButton_IDC_BACK2->setEnabled(g_searchhistory_position>0);
	    search_wnd->PushButton_IDC_FORWARD->setEnabled(g_searchhistory_position<g_searchhistory.GetSize()-1);
	    search_wnd->PushButton_IDC_FORWARD2->setEnabled(g_searchhistory_position<g_searchhistory.GetSize()-1);

            // see if we can't find 'text' in cache, that hasn't expired.
            // if we do find it, and it is in entry 0, then we clear entry 0
            // and go for it.
            for (ci = 0; ci < SEARCHCACHE_NUMITEMS; ci ++)
            {
		if(g_searchcache[ci] == NULL)
              if (!strcmp(text,g_searchcache[ci]->lastsearchtext))
              {
                replpos=ci;
                if (ci > 0 && g_searchcache[ci]->searchreplies.GetSize() &&
                    g_searchcache[ci]->search_id_time+SEARCHCACHE_EXPIRE_TIME >= GetTickCount() &&
                    g_searchcache[ci]->numcons*2 >= g_mql->GetNumQueues()
                    ) cached=1;
                break;
              }
              if (g_searchcache[ci]->search_id_time < g_searchcache[replpos]->search_id_time) replpos=ci;
            }

            if (replpos > 0)
            {
              int x;
              SearchCacheItem *tmp=g_searchcache[replpos];
              for (x = replpos; x > 0; x --) g_searchcache[x]=g_searchcache[x-1];
              g_searchcache[0]=tmp;
            }




            g_lvsearchres.Clear();
            free(m_srch_sort);
            m_srch_sort=0;
            m_srch_sort_len=0;
            m_srch_sort_alloc=0;
            m_fullsize=0;

            if (cached)
            {
              m_srch_sort_len=0;
              int x;
              for (x = 0; x < g_searchcache[0]->searchreplies.GetSize(); x ++)
              {
                m_srch_sort_len+=g_searchcache[0]->searchreplies.Get(x)->get_numitems();
              }

              if (m_srch_sort_len >= m_srch_sort_alloc)
              {
                m_srch_sort_alloc=(m_srch_sort_len*3)/2;
                m_srch_sort=(unsigned int *)realloc(m_srch_sort,m_srch_sort_alloc*sizeof(unsigned int));
                if (!m_srch_sort)
                {
                  m_srch_sort_len=0;
                  m_srch_sort_alloc=0;
                }
               }
              Search_Resort();	     
              for (x = 0; x < m_srch_sort_len; x ++)
              {
                if (m_srch_sort[x] == g_searchcache[0]->lastvisitem) 
                {

                  break;
                }
              }
            }
            else // not cached
            {
              while (g_searchcache[0]->searchreplies.GetSize())
              {
                delete g_searchcache[0]->searchreplies.Get(g_searchcache[0]->searchreplies.GetSize()-1);
                g_searchcache[0]->searchreplies.Del(g_searchcache[0]->searchreplies.GetSize()-1);
              }
              T_Message msg={0,};
              C_MessageSearchRequest req;
              req.set_min_conspeed(0); //g_config->ReadInt("search_minspeed",0));
              req.set_searchstring(text);
              msg.data=req.Make();
              if (msg.data)
              {
                msg.message_type=MESSAGE_SEARCH;
                msg.message_length=msg.data->GetLength();
                g_mql->send(&msg);
                g_searchcache[0]->search_id=msg.message_guid;
                g_searchcache[0]->search_id_time=GetTickCount();
                g_searchcache[0]->numcons=g_mql->GetNumQueues();
              }         
              strcpy(g_searchcache[0]->lastsearchtext,text);
              char text2[1024];
              if (text[0] == '/')
                sprintf(text2,"Browsing %s:",text);
              else
                sprintf(text2,"Searching for \'%s\'...",text);
	      search_wnd->Label_IDC_SEARCHSTATS->setText(text2);
              if (g_searchcache[0]->lastsearchtext[0] == '/')
              {
				        C_MessageSearchReply repl;
				        repl.set_conspeed(g_conspeed);
				        repl.clear_items();
				        repl.set_guid(&g_client_id);

                if (g_searchcache[0]->lastsearchtext[1] && g_searchcache[0]->lastsearchtext[1] != '*' &&
                    strcmp(g_searchcache[0]->lastsearchtext+strlen(g_searchcache[0]->lastsearchtext)-2,"*s"))
                {
		  T_Message msg={0,};
                  char *p=strstr(g_searchcache[0]->lastsearchtext+1,"/");
                  repl.add_item(-1,PARENT_DIRSTRING,DIRECTORY_STRING,0,0,0);
                  msg.message_guid=g_searchcache[0]->search_id;
		              msg.data=repl.Make();
		              if (msg.data)
		              {
	                  msg.message_type=MESSAGE_SEARCH_REPLY;
			              msg.message_length=msg.data->GetLength();
                    main_MsgCallback(&msg,NULL,NULL);
                    msg.data->Unlock();
		              }
                }

				        repl.clear_items();
				        if (g_database->GetNumFiles() && (g_accept_downloads&5)==5)
                  g_database->Search(g_searchcache[0]->lastsearchtext,&repl,NULL,0,main_MsgCallback);
              }
            } //!cached
}

void Vaste_Idd_Search::onColumnClick()
{
            int l_sc=g_config->ReadInt("search_sortcol",0);
            int l_sd=g_config->ReadInt("search_sortdir",1);

            g_config->WriteInt("search_sortcol",l_sc);
            g_config->WriteInt("search_sortdir",l_sd);

            Search_Resort();
}
void Vaste_Idd_Search::onGoItem(int section)
{
	int l_sc=g_config->ReadInt("search_sortcol",0);
        int l_sd=g_config->ReadInt("search_sortdir",1);
        if (section == l_sc) l_sd=!l_sd;
        else l_sc=section;

        g_config->WriteInt("search_sortcol",l_sc);
        g_config->WriteInt("search_sortdir",l_sd);

        Search_Resort();
		
}
void Vaste_Idd_Search::onGoDoubleClicked()
{
	SrchWnd_OnItemRun(1);
}

void Vaste_Idd_Search::onGoRightClicked(QListViewItem* it,const QPoint& pt,int col)
{
            int isRoot = !(g_searchcache[0]->lastsearchtext[0] != '/' ||
                strstr(g_searchcache[0]->lastsearchtext+1,"/") ||
                (g_searchcache[0]->lastsearchtext[1] &&
                  g_searchcache[0]->lastsearchtext[strlen(g_searchcache[0]->lastsearchtext)-1]!='*'));

            int isDirSelected=0;
            int sel=0;
            int isParent=0;
                
            int l=g_lvsearchres.GetCount();
            int x,a=0;
            for (x = 0; x < l; x ++)
            {
              if (g_lvsearchres.GetSelected(x))
              {
                char name[SEARCHREPLY_MAX_FILESIZE];
                char type[SEARCHREPLY_MAX_METASIZE];
                sel++;
                if (g_searchcache[0]->lastsearchtext[0] != '/') break;
                int a=m_srch_sort[x];
                g_searchcache[0]->searchreplies.Get(a>>16)->get_item(a&0xffff,NULL,name,type,NULL,NULL,NULL);
                if (!stricmp(type,DIRECTORY_STRING)) isDirSelected=1;
                if (!strncmp(name,"..",2)) isParent=1;
              }
            }

if(it)
	{
	qDebug("onUserListRMBClicked item=%s",it->text(0).ascii());
	QPopupMenu *contextMenu= new QPopupMenu( this );
	Q_CHECK_PTR( contextMenu );
	
	if(!isRoot)
	{

		int downloadId = contextMenu->insertItem( "Download" );
		int browseId = contextMenu->insertItem( "Browse directory" );
		int copyId = contextMenu->insertItem( "Copy location" );
	
		int ret = contextMenu->exec( pt );
		if(ret == downloadId||ret == browseId)
		{
			qDebug("  ");
			SrchWnd_OnItemRun(ret != downloadId);
	 	}
		else if(ret == copyId)
		{
			qDebug("  ");
              		int l=g_lvsearchres.GetCount();
              		int idx;
             		for (idx = 0; idx < l; idx ++)
              		{
                	if (g_lvsearchres.GetSelected(idx))
                	{
                        	char buf[SEARCHREPLY_MAX_FILESIZE*3+36];
                 	       int a=m_srch_sort[idx];
 
                               buf[0]=0;

 	                      if (g_searchcache[0]->lastsearchtext[0] == '/') // we were browsing
             		     {
                    		safe_strncpy(buf,g_searchcache[0]->lastsearchtext,SEARCHREPLY_MAX_FILESIZE);
                    		while (buf[0] && buf[strlen(buf)-1]=='/') buf[strlen(buf)-1]=0;
                    		strcat(buf,"/");
                    		char *p=buf+strlen(buf);
                    		g_searchcache[0]->searchreplies.Get(a>>16)->get_item(a&0xffff,NULL,p,NULL,NULL,NULL,NULL);
                    		if (!strcmp(p,PARENT_DIRSTRING))
                    		{
                      			*--p=0;
                      			while (p > buf && *p != '/') p--;
                      			if (*p == '/' && p == buf) p++;
                      			*p=0;
                    		}
                  	}
                 	else
                  	{
                    	char host[33];
                    	g_searchcache[0]->searchreplies.Get(a>>16)->get_item(a&0xffff,NULL,buf,NULL,NULL,NULL,NULL);
                    	if (buf[0] == '/')
                    	{
                      		strcpy(host,g_searchcache[0]->searchreplies.Get(a>>16)->_guidstr);
                      		if (buf[1] == '?')
                      		{
                        		char buf2[SEARCHREPLY_MAX_FILESIZE+36];
                        		sprintf(buf2,"/%s%s",host,buf+2);
                        		safe_strncpy(buf,buf2,sizeof(buf));
                      		}
                    	}
                    	else buf[0]=0;
                  	}
                  	if (buf[0])
                  	{
                    if (buf[0])
                    {
                        char buf2[SEARCHREPLY_MAX_FILESIZE*3+36*3+7];
                        strcpy(buf2,"padlocksl:");
                        char *out=buf2+6;
                        char *in=buf;
                        while (*in)
                        {
                          if (*in == ' ')
                          {
                            in++;
                            strcpy(out,"%20");
                            out+=3;
                          }
                          else *out++=*in++;
                        }
                        *out=0;                 
                    }
                  }
	           QString cbtext=QString(buf);
	           QClipboard *cb=QApplication::clipboard();
		   cb->setText(cbtext);
                   break;
                  }
                	}
             		}
		}
		else
		{
			int browseuserId = contextMenu->insertItem( "Browse user" );
			int chatId = contextMenu->insertItem( "Chat user(s)" );
			int whoId = contextMenu->insertItem( "Whois user" );
			int sendfileId = contextMenu->insertItem( "Send file(s) to user(s)" );
			int copyId = contextMenu->insertItem( "Copy location" );
	
			int ret = contextMenu->exec( pt );
		
			if(ret == browseuserId)
			{
				qDebug("  ");
				SrchWnd_OnItemRun(1);
	 		}
			else if (ret == chatId)
			{
				qDebug("  ");
              			int l=g_lvsearchres.GetCount();
              			for (ret = 0; ret < l; ret ++)
              			{
                			if (g_lvsearchres.GetSelected(ret))
                			{
                  				char buf[SEARCHREPLY_MAX_FILESIZE];
                 		        	int a=m_srch_sort[ret];
                  				g_searchcache[0]->searchreplies.Get(a>>16)->get_item(a&0xffff,NULL,buf,NULL,NULL,NULL,NULL);
                  				if (strlen(buf)<32) chat_ShowRoom(buf,1);
                			}
              			}
	 		}
		else if(ret == whoId)
		{
			qDebug("  ");
              		int l=g_lvsearchres.GetCount();
              		for (x = 0; x < l; x ++)
              		{
                		if (g_lvsearchres.GetSelected(x))
                		{
                  			char buf[SEARCHREPLY_MAX_FILESIZE];
                  			int a=m_srch_sort[x];
                  			g_searchcache[0]->searchreplies.Get(a>>16)->get_item(a&0xffff,NULL,buf,NULL,NULL,NULL,NULL);
                  			if (strlen(buf)<32) 
                  			{
                    				T_Message msg={0,};
                   			 // send a message to text that is /whois
                    				C_MessageChat req;
                    				req.set_chatstring("/whois");
				            	req.set_dest(buf);
				            	req.set_src(g_regnick);
                    				msg.data=req.Make();
                    				msg.message_type=MESSAGE_CHAT;
                    				if (msg.data)
                    				{
                      					msg.message_length=msg.data->GetLength();
                      					g_mql->send(&msg);
                    				}
                  			}
                		}
              		}
	 	}
		else if (ret == sendfileId)
		{
						
		qDebug("send selected");
		Vaste_UploadFileDlg dlg;
		if(QDialog::Accepted == dlg.exec())
		{
			qDebug("TODO : xfer_UploadFileToUser, selected file=%s,upload path=%s",dlg.selectedFile().ascii(),dlg.getUpLoadPath().ascii());
			QStringList fileList = dlg.selectedFiles();
			for( QStringList::iterator f= fileList.begin();f != fileList.end();++f)
			Xfer_UploadFileToUser(NULL, (char*)((*f).ascii()), (char*)(it->text(0).ascii()), (char*)(dlg.getUpLoadPath().ascii()));

		}
	    }
	   else if(ret == copyId)
		{
			qDebug("  ");
              		int l=g_lvsearchres.GetCount();
              		int idx;
             		for (idx = 0; idx < l; idx ++)
              		{
                		if (g_lvsearchres.GetSelected(idx))
             		   	{
        	                	char buf[SEARCHREPLY_MAX_FILESIZE*3+36];
     		            	       int a=m_srch_sort[idx];
 
     		                     buf[0]=0;

 		                      if (g_searchcache[0]->lastsearchtext[0] == '/') // we were browsing
  		           		     {
    			                		safe_strncpy(buf,g_searchcache[0]->lastsearchtext,SEARCHREPLY_MAX_FILESIZE);
        			            		while (buf[0] && buf[strlen(buf)-1]=='/') buf[strlen(buf)-1]=0;
      			              		strcat(buf,"/");
           			         		char *p=buf+strlen(buf);
                			    		g_searchcache[0]->searchreplies.Get(a>>16)->get_item(a&0xffff,NULL,p,NULL,NULL,NULL,NULL);
                			    		if (!strcmp(p,PARENT_DIRSTRING))
                			    		{
                      					*--p=0;
                      					while (p > buf && *p != '/') p--;
                      					if (*p == '/' && p == buf) p++;
                      					*p=0;
                    					}
             				     	}
         	        	else
       	           	{
      		              	char host[33];
    		                	g_searchcache[0]->searchreplies.Get(a>>16)->get_item(a&0xffff,NULL,buf,NULL,NULL,NULL,NULL);
     		               	if (buf[0] == '/')
   		                 	{
   		                   		strcpy(host,g_searchcache[0]->searchreplies.Get(a>>16)->_guidstr);
   		                   		if (buf[1] == '?')
       	               		{
         		               		char buf2[SEARCHREPLY_MAX_FILESIZE+36];
        		                		sprintf(buf2,"/%s%s",host,buf+2);
        		                		safe_strncpy(buf,buf2,sizeof(buf));
     		                 		}
     		               	}
     		               	else buf[0]=0;
   	 	             	}
      		            	if (buf[0])
      		            	{
        		            if (buf[0])
      			              {
             		 	          char buf2[SEARCHREPLY_MAX_FILESIZE*3+36*3+7];
            		 	           strcpy(buf2,"padlocksl:");
       	       	          char *out=buf2+6;
          			          char *in=buf;
        	       	          while (*in)
         			          {
       			                 if (*in == ' ')
       			                   {
       				                     in++;
                				             strcpy(out,"%20");
                 				             out+=3;
                  				       }
              			            else *out++=*in++;
             			               }
     			                   *out=0;                 
         		           }
     	    	   	      }
	                QString cbtext=QString(buf);
	        	QClipboard *cb=QApplication::clipboard();

			cb->setText(cbtext /*QClipboard::Clipboard*/);
               	   break;
             		   			  }
                			}
            	 		}
			}
		}
}


