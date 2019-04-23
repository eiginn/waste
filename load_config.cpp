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
   This file implements a function to load the config file's content 
   into global variables. If need setup, invoke the profile setup wizard. 
   Author: Stanley San, Frank Yang, Lora Yin
   Date created: 2003/12/5 
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
#include "srchwnd.h"
#ifdef QTUI
#include "prefinterface.h"
#include "ui_listview.h"
#include <qfileinfo.h>
#endif
#include "util.h"
#define SEARCHCACHE_NUMITEMS 16


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
extern SearchCacheItem *g_searchcache[SEARCHCACHE_NUMITEMS];

extern char g_config_mainini[1024];

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

void load_config(char * argv)
{
  char tmp[1024+8];

#ifdef QTUI
  QFileInfo fileInfo;
#ifdef WIN32
  char path[1028];
  GetModuleFileName(NULL,path, sizeof(path));
  fileInfo.setFile(path);
#else
  fileInfo.setFile(argv);
  fileInfo.convertToAbs();
#endif    
  strcpy(g_config_prefix,fileInfo.filePath().ascii());
  strcpy(g_config_mainini, fileInfo.filePath().ascii());
#else
  strcpy(g_config_prefix,argv);
  strcpy(g_config_mainini, argv);

#endif


  char *p=g_config_prefix;
  while (*p) p++;
  while (p >= g_config_prefix && *p != '/') p--;
  *++p=0;
#ifdef _LINUX
  //printf("current dir = %s\n", g_config_prefix);
  chdir(g_config_prefix);
#endif

  MYSRAND();

  
  // remove the .exe from the config file path.
  if (strlen(g_config_mainini) > strlen(".exe"))
  {  
    p=g_config_mainini+strlen(g_config_mainini) - strlen(".exe");
    if (strcmp(p, ".exe") == 0)
    *p = 0;
  }


  strcat(g_config_mainini,".ini");

  // Get user profile. load the config based on the profile.
#ifdef QTUI
  if (GetPrivateProfileInt("config", "showprofiles", 0, g_config_mainini))
    showProfileManager(); 
#endif

  GetPrivateProfileString("config","lastprofile", "Default", g_profile_name,sizeof(g_profile_name),g_config_mainini);
  if (!g_profile_name[0]) strcpy(g_profile_name,"Default");
  //else
    //printf("get profile_name = %s\n",g_profile_name);
  WritePrivateProfileString("config","lastprofile", g_profile_name, g_config_mainini);

  strcat(g_config_prefix,g_profile_name);

  sprintf(tmp,"%s.pr0",g_config_prefix);
  //printf("loading config from %s\n",tmp);
  g_config = new C_Config(tmp);

   
  g_do_log=g_config->ReadInt("debuglog",0);
  
  
  g_aes_keysize=g_config->ReadInt("aeskeysize",128);
  g_config->WriteInt("debuglog",g_do_log);
  g_config->WriteInt("aeskeysize",g_aes_keysize);

  if (!g_config->ReadString("downloadpath","")[0])
  {

#ifdef QTUI
    strcpy(tmp, fileInfo.filePath().ascii());
#else
    strcpy(tmp,argv);
#endif
    char *p=tmp;
    while (*p) p++;
    while (p >= tmp && *p != '/') p--;
    strcpy(++p,"downloads");
    CreateDirectory(tmp,NULL);
    g_config->WriteString("downloadpath",tmp);
    //printf("%s\n",tmp);
  }

  loadPKList();

  // If need_setup, call the gui of setup.
#ifdef QTUI
  int need_setup=g_config->ReadInt("valid",0)<5;
  if (need_setup)
  {
    if (!initializeProfile())
    {
      delete g_config;
      memset(&g_key, 0, sizeof (g_key));
      // WARNING: here must be a better way!
      exit(1) ;
    }
    g_config->WriteInt("valid",5);
    g_config->Flush();
  }
#endif
  
  strncpy(g_client_id_str,g_config->ReadString("clientid128",""),32);
  g_client_id_str[32]=0;

  if (!g_key.bits) reloadKey(g_config->ReadInt("storepass",0)?
    g_config->ReadString("keypass",NULL):
      NULL);

  if (MakeID128FromStr(g_client_id_str,&g_client_id)) 
  {
    CreateID128(&g_client_id);
    MakeID128Str(&g_client_id,g_client_id_str);
    g_config->WriteString("clientid128",g_client_id_str);
  }

  if (g_config->ReadInt("db_save",1)) 
  {
    g_newdatabase=g_database=new C_FileDB();
    if (g_config->ReadInt("use_extlist",0))
      g_newdatabase->UpdateExtList(g_config->ReadString("extlist",g_def_extlist));
    else 
      g_newdatabase->UpdateExtList("*");
    char dbfile[1024+8];
    sprintf(dbfile,"%s.pr2",g_config_prefix);
    g_newdatabase->readIn(dbfile);
    g_scanloadhack=1;
  }

  if (g_config->ReadInt("scanonstartup",1)) doDatabaseRescan();

  //networkname.
  {
    char *buf=g_config->ReadString("networkname","");
    if (buf[0])
    {
      SHAify m;
      m.add((unsigned char *)buf,strlen(buf));
      m.final(g_networkhash);
      m.reset();
      g_use_networkhash=1;
    }
    else 
    {
      memset(g_networkhash,0,sizeof(g_networkhash));
      g_use_networkhash=0;
    }
  }

  g_conspeed=g_config->ReadInt("conspeed",28);
  g_route_traffic=g_config->ReadInt("route",g_conspeed>64);
  g_forceip=g_config->ReadInt("forceip",0);
  g_forceip_addr=g_config->ReadInt("forceip_addr",INADDR_NONE);
  g_appendprofiletitles=g_config->ReadInt("appendpt",0);
  g_extrainf=g_config->ReadInt("extrainf",0);
  g_keepup=g_config->ReadInt("keepupnet",4);
  g_use_accesslist=g_config->ReadInt("ac_use",0);
  g_accept_downloads=g_config->ReadInt("downloadflags",7);
  g_do_autorefresh=g_config->ReadInt("dorefresh",0);
  g_max_simul_dl=g_config->ReadInt("recv_maxdl",4);
  g_max_simul_dl_host=g_config->ReadInt("recv_maxdl_host",1);
  g_chat_timestamp=g_config->ReadInt("chat_timestamp",0);
  g_search_showfull=g_config->ReadInt("search_showfull",1);
  
  g_search_showfullbytes=g_config->ReadInt("search_showfullb",0);
  
  g_keydist_flags=g_config->ReadInt("keydistflags",4|2|1);

  g_throttle_flag=g_config->ReadInt("throttleflag",0);
  g_throttle_send=g_config->ReadInt("throttlesend",64);
  g_throttle_recv=g_config->ReadInt("throttlerecv",64);

  g_dns=new C_AsyncDNS;
  g_mql = new C_MessageQueueList(main_MsgCallback,6);
  updateACList();
  update_set_port();
  for (int x = 0; x < SEARCHCACHE_NUMITEMS; x ++) g_searchcache[x]=new SearchCacheItem;
  safe_strncpy(g_regnick,g_config->ReadString("nick",""),sizeof(g_regnick));
  if (g_regnick[0] == '#' || g_regnick[0] == '&') g_regnick[0]=0;

  /* timeout value for update user list or chatroom list: 
     less than 120s + 60s maybe cause the user list display error(
     add a username and then remove it and then add.) 
  */
  
  g_timeout = g_config->ReadInt("timeout",240);
  if (g_timeout < 180)
     g_timeout = 180;
  //printf("timeout = %d\n", g_timeout);
  g_config->WriteInt("timeout",g_timeout);
}
