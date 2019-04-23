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
 * This file implements the kernel of PadLockSL
 * Author: Stanley San
 * Date Created: 2003.12.1
*/
#include "main.h"
#include "rsa/md5.h"

#include "m_upload.h"
#include "m_chat.h"
#include "m_search.h"
#include "m_ping.h"
#include "m_file.h"
#include "m_lcaps.h"
#include "m_keydist.h"
#include "netkern.h"
#include "xfers.h"
#include "xferwnd.h"
#include "netq.h"
#include "srchwnd.h"
#include "tag.h"
#ifdef QTUI
#include <qmessagebox.h>
#endif


extern unsigned char g_networkhash[SHA_OUTSIZE];
extern int g_use_networkhash;

extern char *g_nameverstr;
extern char *g_def_extlist;

extern C_FileDB *g_database, *g_newdatabase;
extern C_AsyncDNS *g_dns;
extern C_Listen *g_listen;
extern C_MessageQueueList *g_mql;
extern C_Config *g_config;

extern char g_config_prefix[1024];

extern char g_profile_name[128];
extern int g_extrainf;
extern int g_keepup;
extern int g_conspeed,g_route_traffic;
extern int g_do_log;
extern int g_max_simul_dl;
extern unsigned int g_max_simul_dl_host;
extern int g_forceip, g_forceip_addr;
extern int g_use_accesslist;
extern int g_appendprofiletitles;

extern time_t g_last_pingtime,g_last_bcastkeytime;

extern C_ItemList<C_UploadRequest> uploadPrompts;
extern C_ItemList<C_KeydistRequest> keydistPrompts;

void vasteKernRun()
{
	TAG tag("vasteKernRun()");
	static int upcnt;

//step.1
        if (uploadPrompts.GetSize() && !upcnt)
        {
          upcnt=1;
          C_UploadRequest *t=uploadPrompts.Get(0);
          uploadPrompts.Del(0);
          char buf[1024];
          char str[64];
          MakeID128Str(t->get_guid(),str);
          sprintf(str+strlen(str),":%d",t->get_idx());
          if (strlen(t->get_fn()) > 768)
            t->get_fn()[768]=0;
#ifdef _WIN32
          int f=MB_YESNO;
#elif defined(QTUI)
	  int f=0;
#endif
          char sizebuf[64];
          sizebuf[0]=0;
          int fs_l,fs_h;
          t->get_fsize(&fs_l,&fs_h);
          if (fs_l != -1 || fs_h != -1)
          {
            strcpy(sizebuf," (");
            FormatSizeStr64(sizebuf+2,fs_l,fs_h);
            strcat(sizebuf,")");
          }
          if (uploadPrompts.GetSize() > 1)
          {
#ifdef _WIN32
            f=MB_YESNOCANCEL;
#elif defined(QTUI)
#endif
            sprintf(buf,"Accept upload of file named '%s'%s from user '%s'?\r\n(Cancel will remove all %d pending uploads)",t->get_fn(),sizebuf,t->get_nick(),uploadPrompts.GetSize()-1);
          }
          else sprintf(buf,"Accept upload of file named '%s'%s from user '%s'?",t->get_fn(),sizebuf,t->get_nick());
#ifdef _WIN32
          f=MessageBox(NULL,buf,APP_NAME " - Accept Upload?",f|MB_TOPMOST|MB_ICONQUESTION);
          if (f==IDYES) main_handleUpload(str,t->get_fn(),t);
          else if (f == IDCANCEL)
          {
            while (uploadPrompts.GetSize()>0)
            {
              delete uploadPrompts.Get(0);
              uploadPrompts.Del(0);
            }
          }
#elif defined(QTUI)
	QMessageBox mb( APP_NAME " - Accept Upload?" ,
          	buf,
        QMessageBox::Information,
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No,
        QMessageBox::Escape );
switch( mb.exec() ) {
    case QMessageBox::Yes:
	  //qDebug("TODO main_handleUpload() three parameter\n");
          main_handleUpload(str,t->get_fn(),t);
          //main_handleUpload(str,t->get_fn());
        break;
    case QMessageBox::No:
        break;
    case QMessageBox::Cancel:
            while (uploadPrompts.GetSize()>0)
            {
              delete uploadPrompts.Get(0);
              uploadPrompts.Del(0);
            }
        break;
}	

#endif
          delete t;
          upcnt=0;
        }


//step.2
        static int kdcnt;
        if (keydistPrompts.GetSize() && !kdcnt)
        {
          kdcnt=1;
          C_KeydistRequest *t=keydistPrompts.Get(0);
          keydistPrompts.Del(0);

          if (!findPublicKeyFromKey(t->get_key()))
          {
            char buf[1024];
            char sign[SHA_OUTSIZE*2+1];
            unsigned char hash[SHA_OUTSIZE];
            SHAify m;
            m.add((unsigned char *)t->get_key()->modulus,MAX_RSA_MODULUS_LEN);
            m.add((unsigned char *)t->get_key()->exponent,MAX_RSA_MODULUS_LEN);
            m.final(hash);
            int x;
            for (x = 0; x < SHA_OUTSIZE; x ++) sprintf(sign+x*2,"%02X",hash[x]);

            sprintf(buf,"Authorize public key with signature '%s' from user '%s'?\r\n(Cancel will keep this key and any remaining prompts in the pending key list)",sign,t->get_nick());

#if defined(_WIN32) && !defined(QTUI)
            int f=MessageBox(NULL,buf,APP_NAME " - Accept Public Key?",MB_YESNOCANCEL|MB_TOPMOST|MB_ICONQUESTION);

            if (f==IDYES) main_handleKeyDist(t,0);
            else if (f == IDCANCEL)
            {
              main_handleKeyDist(t,1);
              while (keydistPrompts.GetSize()>0)
              {
                main_handleKeyDist(keydistPrompts.Get(0),1);
                delete keydistPrompts.Get(0);
                keydistPrompts.Del(0);
              }
            }
#elif defined(QTUI)
#endif
          }
          delete t;
          kdcnt=0;
        }

//step.3
        static int in_timer;
        if (!in_timer)
        {
          in_timer=1;

          int scanworking=0;
          if (g_scanloadhack)
          {
            g_scanloadhack=0;
            if (g_database)
            {
              sprintf(g_scan_status_buf,"%d files (cached)",g_database->GetNumFiles());
            }
          }
          if (g_newdatabase) 
          {
            int r=g_newdatabase->DoScan(15, 
                      (g_newdatabase != g_database && g_database->GetNumFiles()) ? g_database : NULL
                  );
            if (r != -1)
            {
              scanworking=1;
              sprintf(g_scan_status_buf,"Scanning %d",r);
            }
            else
            {
              sprintf(g_scan_status_buf,"Total files shared: %d",g_newdatabase->GetNumFiles());
              if (g_newdatabase != g_database) 
              {
                debug_printf("DB: replacing old database with temp database\r\n");
                delete g_database;
                g_database=g_newdatabase;
              }
              else
                debug_printf("DB: removing temp database reference, scanning done\r\n");
              g_newdatabase=0;
            }
          }

          int lastqueues=g_mql->GetNumQueues();

          int x;
          NetKern_Run();
          int n=1;
          if (g_conspeed>=64) n++;
          if (g_conspeed>=384) n++;
          if (g_conspeed>=1600) n+=2; // fast connections
          if (g_conspeed>=20000) n+=3; // really fast connections

          for (x = 0; x < n; x ++)
          {
            Xfer_Run();

            g_mql->run(g_route_traffic);
          }
          int newl=g_mql->GetNumQueues();
          if (lastqueues != newl) 
          {
            MYSRANDUPDATE((unsigned char *)&lastqueues,sizeof(lastqueues));

            char text[32];

            sprintf(text,":%d",newl);

#if defined(_WIN32) && !defined(QTUI)
            SetDlgItemText(g_mainwnd,IDC_NETSTATUS,text);
#elif defined(QTUI)
#endif
          }

          if (time(NULL) > g_next_refreshtime && g_do_autorefresh && !g_newdatabase)
          {
            if (!scanworking)
            {
              if (g_next_refreshtime)
              {
                g_next_refreshtime=0;
                debug_printf("DB: initiating autorescan of files\n");
                g_newdatabase=new C_FileDB();
                if (g_config->ReadInt("use_extlist",0))
                  g_newdatabase->UpdateExtList(g_config->ReadString("extlist",g_def_extlist));
                else 
                  g_newdatabase->UpdateExtList("*");
                g_newdatabase->Scan(g_config->ReadString("databasepath",""));
                if (!g_database->GetNumFiles())
                {
                  debug_printf("DB: making scanning db live\n");
                  delete g_database;
                  g_database=g_newdatabase;
                }
              }
              else g_next_refreshtime = time(NULL)+60*g_config->ReadInt("refreshint",300);
            }
          }

          if (time(NULL) > g_last_pingtime && newl) // send ping every 2m
          {
            g_last_pingtime = time(NULL)+120;
            DoPing(NULL);

            // flush config and netq
            g_config->Flush();
            // Modified by LoraYin 2003/12/13 
            SaveNetQ();
          }
          if (time(NULL) > g_last_bcastkeytime && newl)
          {
            g_last_bcastkeytime = time(NULL)+60*60; // hourly
            if (g_config->ReadInt("bcastkey",1))
              main_BroadcastPublicKey();
          }

          in_timer=0;
        }
}
