 /*
    WASTE - xferwnd.cpp (File transfer dialogs)
    Copyright (C) 2003 Nullsoft, Inc.

    WASTE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    WASTE is distributed in the hope that it will be useful,
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

#include "main.h"
#include "xfers.h"
#include "xferwnd.h"
#include "srchwnd.h"
#if defined(_WIN32) && !defined(QTUI)
#include "resource.h"
#include "childwnd.h"
#endif
#ifdef QTUI
	#include <qtabwidget.h>
	#include "vaste_idd_xfers.h"
        #include <sys/types.h>
        #include <sys/stat.h>
#ifdef _LINUX
        #include <unistd.h>
#endif

#endif
	#include "tag.h"

#include "m_upload.h"

#define PENDING_UPLOAD_STRING "Pending Upload"

int g_files_in_download_queue;
#if defined(_WIN32) && !defined(QTUI)
HWND g_xferwnd, g_xfer_subwnd_active;
W_ListView g_lvrecv, g_lvsend, g_lvrecvq;
/*Add by frank 2003-12-9*/
#elif defined(QTUI)
QWidget *g_xferwnd, *g_xfer_subwnd_active;
W_ListView g_lvrecv, g_lvsend, g_lvrecvq;
#endif
/*Edn*/
C_ItemList<XferSend> g_sends;
C_ItemList<XferRecv> g_recvs;
C_ItemList<char> g_uploads;
C_ItemList<C_SHBuf> g_uploadqueue;
unsigned int g_uploadqueue_lastsend;


#if defined(_WIN32) && !defined(QTUI)
static HWND g_xfer_subwnds[3];
static void doExecuteOfDownload(HWND hwndDlg, int x, int copy)
{
	TAG tag("doExecuteOfDownload()");
  XferRecv *tr;
  char *t=g_config->ReadString("downloadpath","");
  if (!t || !*t) return;

  if ((tr=(XferRecv*)g_lvrecv.GetParam(x)))
  {
    char *fn=NULL;
    if (copy) fn=tr->getOutputFileCopy();
    if (!fn) fn=tr->getActualOutputFile();

    if (fn && *fn)
    {
      ShellExecute(hwndDlg,"open",fn,NULL,t?t:"",SW_SHOW);
      return;
    }
  }

  char n[512];
  n[0]=0;
  g_lvrecv.GetText(x,0,n,sizeof(n));
  if (*n)
  {
    char *buf=(char*)malloc(strlen(t)+2+strlen(n));
    if (buf)
    {
      sprintf(buf,"%s\\%s",t,n);
      ShellExecute(hwndDlg,"open",buf,NULL,t,SW_SHOW);
      free(buf);
    }
  }
}

static int CleanupDLFolder(char *path, HWND hwndDlg, int *cancelflag) // returns 1 if files removed from a directory
{
  int hasremoved=0;
  WIN32_FIND_DATA fd;
  HANDLE hf;
  char buf[2048];
  safe_strncpy(buf,path,1024);
  if (buf[strlen(buf)-1] != '\\') strcat(buf,"\\");
  char *endofpath=buf+strlen(buf);

  strcpy(endofpath,"*.PADLOCKSLSTATE");
  hf=FindFirstFile(buf,&fd);
  if (hf != INVALID_HANDLE_VALUE) 
  {
    do
    {
      if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
        strcpy(endofpath,fd.cFileName);
        HANDLE hStatFile=CreateFile(buf,GENERIC_WRITE|GENERIC_READ,0,NULL,OPEN_ALWAYS,0,NULL);

        if (hStatFile != INVALID_HANDLE_VALUE)
        {
          int sig1, sig2;
          DWORD d;
          DWORD fullsize_l=0,fullsize_h=0;

          if (ReadFile(hStatFile,&sig1,4,&d,NULL) && d == 4 && sig1 == PADLOCKSLSTATE_SIG1 &&
              ReadFile(hStatFile,&sig2,4,&d,NULL) && d == 4 && sig2 == PADLOCKSLSTATE_SIG2 &&
              ReadFile(hStatFile,&fullsize_l,4,&d,NULL) && d == 4 &&
              ReadFile(hStatFile,&fullsize_h,4,&d,NULL) && d == 4)
          {
          }
          else fullsize_l=fullsize_h=0;

          int dodelete=0;
          char *p=endofpath+strlen(endofpath);
          while (p > endofpath && strnicmp(p,".PADLOCKSLSTATE",11)) p--;
          if (p == endofpath) dodelete=2;
          else
          {
            *p=0;
            HANDLE hActFile=CreateFile(buf,GENERIC_WRITE|GENERIC_READ,0,NULL,OPEN_ALWAYS,0,NULL);
            if (hActFile != INVALID_HANDLE_VALUE)
            {
              DWORD d_h=0;
              DWORD d_l=GetFileSize(hActFile,&d_h);
              CloseHandle(hActFile);
              if (!d) dodelete=1;
              else if (!*cancelflag)
              {
                char tmp[2048];
                char s[32],s2[32];
                FormatSizeStr64(s,d_l,d_h);
                FormatSizeStr64(s2,fullsize_l,fullsize_h);
                if (fullsize_l || fullsize_h) sprintf(tmp,"Partial download '%s' is %s of %s. Delete?",endofpath,s,s2);
                else sprintf(tmp,"Partial download '%s' is %s. Delete?",endofpath,s);
                int res=MessageBox(hwndDlg,tmp,APP_NAME " download directory cleanup",MB_YESNOCANCEL|MB_ICONQUESTION);
                dodelete = res == IDYES;
                *cancelflag = res == IDCANCEL;
              }
            }
          }
          CloseHandle(hStatFile);

          if (dodelete)
          {
            DeleteFile(buf);
            if (dodelete < 2)
            {
              strcat(buf,".PADLOCKSLSTATE");
              DeleteFile(buf);
            }
            hasremoved=1;
          }
        }
      }
    } while (FindNextFile(hf,&fd));
    FindClose(hf);
  }
  strcpy(endofpath,"*");
  hf=FindFirstFile(buf,&fd);
  if (hf != INVALID_HANDLE_VALUE) 
  {
    do
    {
      if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strcmp(fd.cFileName,"..") && strcmp(fd.cFileName,"."))
      {
        strcpy(endofpath,fd.cFileName);
        if (CleanupDLFolder(buf,hwndDlg,cancelflag))
        {
          if (RemoveDirectory(buf)) hasremoved=1;
        }
      }
    } while (FindNextFile(hf,&fd));
    FindClose(hf);
  }
  return hasremoved;
}
#endif

static void RunSends(void)
{
  int x,needrefresh=0;

  if (g_uploadqueue.GetSize() && 
        (GetTickCount() > g_uploadqueue_lastsend+1000 ||
         GetTickCount() < g_uploadqueue_lastsend))
  {
    g_uploadqueue_lastsend=GetTickCount();
    T_Message m={0,};
    m.data=g_uploadqueue.Get(0);
    m.message_length=m.data->GetLength();
    m.message_type=MESSAGE_UPLOAD;

    g_mql->send(&m);

    g_uploadqueue.Del(0);
  }


  if (!g_sends.GetSize()) return;

  for (x = 0; x < g_sends.GetSize(); x ++)
  {
    int s=g_sends.Get(x)->run_hdr(g_mql);
    if (s)
    {
#if defined(_WIN32) || defined(QTUI) //added by Raymond 2003.12.25
      int idx=g_lvsend.FindItemByParam((int)g_sends.Get(x));
      if (idx!=-1)
      {
        if (!g_config->ReadInt("send_autoclear",0))
        {
          char *a=g_sends.Get(x)->GetError();
          g_lvsend.SetItemParam(idx,0);
          if (a&&*a) g_lvsend.SetItemText(idx,3,a);
        }
        else
          g_lvsend.DeleteItem(idx);
      }
#endif
      delete g_sends.Get(x);
      g_sends.Del(x--);
      needrefresh++;
    }
  }
  static int runoffs;
  int s=g_sends.GetSize();
  for (x = 0; x < s; x ++)
  {
    g_sends.Get((x+runoffs)%s)->run(g_mql);
  }
  runoffs++; // round robin
#if defined(_WIN32) && !defined(QTUI)
  if (needrefresh) PostMessage(g_xferwnd,WM_USER_TITLEUPDATE,0,0);
#elif defined(QTUI)
  if (needrefresh) ((Vaste_Idd_Xfers *)g_xferwnd)->UpdateTitle();
#endif
}

static void RunRecvs(void)
{
	//debug_printf("RunRecvs\n");
	TAG tag("RunRecvs()");
  int needrefresh=0;
  static unsigned int next_runitem;

  unsigned int a=GetTickCount();
#if defined(_WIN32) || defined(QTUI) //Added by Raymond 2003.12.26
  if (a >= next_runitem || a < next_runitem-30000) // the second one is to detect wrap
  {
    //debug_printf("g_mql->GetNumQueues() = %d, %d\n", g_mql->GetNumQueues(), g_recvs.GetSize());
    if (g_mql->GetNumQueues()) if (g_recvs.GetSize() < g_max_simul_dl)
    {
      int qitem;
      char lhost[512];
      lhost[0]=0;
      for (qitem = 0; qitem < g_files_in_download_queue; qitem ++)
      {
        char host[512];
        g_lvrecvq.GetText(qitem,2,host,sizeof(host));
        if (!strncmp(lhost,host,32)) continue;

        lhost[0]=0;

        char name[260];
        char sizestr[64];
        g_lvrecvq.GetText(qitem,0,name,sizeof(name));
        g_lvrecvq.GetText(qitem,1,sizestr,sizeof(sizestr));

        // see if item is already beind downloaded, and if not, how many items from that host are
        int n=g_lvrecv.GetCount();
        unsigned int nhostitems=0;
        int x;
        if (!(g_max_simul_dl_host&0x80000000) && g_max_simul_dl_host) 
        {
          for (x = 0; x < n; x ++)
          {
            if (!g_lvrecv.GetParam(x)) continue;
            char host2[512];
            g_lvrecv.GetText(x,3,host2,sizeof(host2));   
            if (!strncmp(host2,host,32)) 
            {
              if (++nhostitems >= g_max_simul_dl_host) 
              {
                strcpy(lhost,host);
                break;
              }
            }
          }
        }

        if (!g_max_simul_dl_host || nhostitems < g_max_simul_dl_host)
        {
          for (x = 0; x < n; x ++)
          {
            char name2[260];
            char host2[512];
            g_lvrecv.GetText(x,0,name2,sizeof(name2));
            if (stricmp(name2,name)) continue;
            g_lvrecv.GetText(x,3,host2,sizeof(host2));   
            if (stricmp(host2,host)) continue;

            if (g_lvrecv.GetParam(x)) break;
            g_lvrecv.DeleteItem(x--);
            needrefresh++;
            n--;
          }

          needrefresh++;
          g_lvrecvq.DeleteItem(qitem);
          g_files_in_download_queue--;
#if defined(_WIN32) && !defined(QTUI)
	  RecvQ_UpdateStatusText();
#elif defined(QTUI)
          ((Vaste_Idd_Xfers *)g_xferwnd)->recvQ_UpdateStatusText();
#endif
	  //debug_printf("before add xferRecv\n");
          if (x >= n) g_recvs.Add(new XferRecv(g_mql,host,sizestr,name,g_config->ReadString("downloadpath","C:\\")));

          break;
        }
      }
    }
    next_runitem=GetTickCount()+2000;
  }
#endif//_WIN32

  int x;
  for (x = 0; x < g_recvs.GetSize(); x ++)
  {
    int s=g_recvs.Get(x)->run(g_mql);
    if (s)
    {
#if defined(_WIN32) || defined(QTUI) //Added by Raymond 2003.12.26
      int idx=g_lvrecv.FindItemByParam((int)g_recvs.Get(x)); 
      if (idx!=-1)
      {
        char text[128];
        char *a=g_recvs.Get(x)->GetError();
        if (a) g_lvrecv.SetItemText(idx,2,a);

        g_lvrecv.GetText(idx,3,text,sizeof(text));

	//debug_printf("autoclear = %d\n", g_config->ReadInt("recv_autoclear", 0));
        if (g_config->ReadInt("recv_autoclear",0) && !text[0])
        {
          g_lvrecv.DeleteItem(idx);
        }
        else
        {
          if (text[0] && a && !strnicmp(a,"Timed out",9) && g_config->ReadInt("recv_autores",1))
          {
            char sizestr[64];
            char name[260];
            g_lvrecv.GetText(idx,0,name,sizeof(name));
            g_lvrecv.GetText(idx,1,sizestr,sizeof(sizestr));
            g_lvrecv.DeleteItem(idx);

            int p=g_lvrecvq.InsertItem(g_lvrecvq.GetCount(),name,0);
            g_lvrecvq.SetItemText(p,1,sizestr);
            g_lvrecvq.SetItemText(p,2,text);
            g_files_in_download_queue++;
#if defined(_WIN32) && !defined(QTUI)
	  RecvQ_UpdateStatusText();
#elif defined(QTUI)
          ((Vaste_Idd_Xfers *)g_xferwnd)->recvQ_UpdateStatusText();
#endif
          }               
          else g_lvrecv.SetItemParam(idx,0);
        }
      }
#endif //_WIN32
      delete g_recvs.Get(x);
      g_recvs.Del(x);
      x--;
      needrefresh++;
    }
  }
#if defined(_WIN32) && !defined(QTUI)
  if (needrefresh) PostMessage(g_xferwnd,WM_USER_TITLEUPDATE,0,0);
#elif defined(QTUI)
  if (needrefresh) ((Vaste_Idd_Xfers *)g_xferwnd)->UpdateTitle();
  ((Vaste_Idd_Xfers *)g_xferwnd)->lstViewRecvSelectChanged();
#endif
}


int Xfer_WillQ(char *file, char *guidstr)
{
  unsigned int nhostitems=0;

#if defined(_WIN32) || defined(QTUI)
  // see if item is already beind downloaded, and if not, how many items from that host are
  int n=g_lvrecv.GetCount();
  int x;
  for (x = 0; x < n; x ++)
  {
    char host2[128];
    char name2[512];
    g_lvrecv.GetText(x,0,name2,sizeof(name2));
    g_lvrecv.GetText(x,3,host2,sizeof(host2));   
    if (!strncmp(host2,guidstr,32) && g_lvrecv.GetParam(x)) nhostitems++;
    if (!stricmp(name2,file) && !stricmp(host2,guidstr))
    {
      if (g_lvrecv.GetParam(x)) return 0; // go to download directly
    }
  }

  if (g_max_simul_dl_host && nhostitems >= g_max_simul_dl_host)
    return 1;
#endif

  return (g_recvs.GetSize() >= g_max_simul_dl);
}

//#ifdef _WIN32
void Xfer_UploadFileToUser(HWND hwndDlg, char *file, char *user, char *leadingpath)
{
  TAG tag("Xfer_UploadFileToUser");
  //debug_printf(" file = %s, user = %s, leadingpath = %s\n", file, user, leadingpath);
  if (!*file || !*user) return;

  int idx;
  for (idx = 0; idx < g_uploads.GetSize() && g_uploads.Get(idx); idx++);

  char *fnptr=strdup(file);

  if (idx==g_uploads.GetSize()) g_uploads.Add(fnptr);
  else g_uploads.Set(idx,fnptr);

  C_UploadRequest r;
  r.set_dest(user);
  char *f=file;
  while (*f) f++;
  while (f >= file && *f != '/' && *f != '\\' && *f != ':') f--;
  f++;
  char buf[2048];
  while (*leadingpath == '/' || *leadingpath == '\\') leadingpath++;
  if (*leadingpath)
  {
    strcpy(buf,leadingpath);
    while (buf[0] && (buf[strlen(buf)-1]=='/' || buf[strlen(buf)-1]=='\\')) buf[strlen(buf)-1]=0;
    if (buf[0])
    {
      strcat(buf,"/");
      strcat(buf,f);
    }
    else strcpy(buf,f);
  }
  else strcpy(buf,f);
  char *p=buf;
  while (*p)
  {
    if (*p == '\\') *p='/';
    p++;
  }
  //debug_printf("Xfer_Uploadfile::file = %s\n", file);
#if defined(_WIN32)
  HANDLE h=CreateFile(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
  if (h != INVALID_HANDLE_VALUE)
  {
    int fs_h=0;
    int fs_l=(int)GetFileSize(h,(LPDWORD)&fs_h);
    CloseHandle(h);
    r.set_fsize(fs_l,fs_h);
  }
#elif defined(_LINUX)
  FILE *h = fopen(file, "r"); 
//  HANDLE h=CreateFile(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
  if (NULL != h)//INVALID_HANDLE_VALUE)
  {
    int fs_h=0;
    /* Modified by LoraYin 2004/1/6 begin. */
    struct stat fs;
    stat(file, &fs);
    //fs_h = fs.st_size;
    debug_printf("the file size = %d\n", fs.st_size);
    //while(!feof(h)) {fgetc(h); fs_h++;}
    /* Modified by LoraYin 2004/1/6  end.*/
    fclose(h);
    int fs_l= fs.st_size; //fs_h;//(int)GetFileSize(h,(LPDWORD)&fs_h);
    //CloseHandle(h);
    r.set_fsize(fs_l,fs_h);
  }
#endif//_WIN32
  r.set_fn(buf);
  r.set_guid(&g_client_id);
  r.set_idx(UPLOAD_BASE_IDX+idx);
  if (g_config->ReadInt("nickonxfers",1))
    r.set_nick(g_regnick);
    
  g_uploadqueue.Add(r.Make());

  safe_strncpy(buf,file,sizeof(buf));
  p=buf;
  while (*p) 
  {
    if (*p == '\\') *p='/';
    p++;
  }
  while (p >= buf && *p != '/') p--;
  
  if (g_config->ReadInt("ulfullpaths",0))
  {
    if (p >= buf) p--;
    while (p >= buf && *p != '/') p--;
    if (p >= buf) p--;
    while (p >= buf && *p != '/') p--;
  }
  p++;

  
  g_lvsend.InsertItem(0,p,(int)fnptr);
  g_lvsend.SetItemText(0,1,user);
  g_lvsend.SetItemText(0,2,"");
  g_lvsend.SetItemText(0,3,PENDING_UPLOAD_STRING);
#if defined(QTUI)
  ((Vaste_Idd_Xfers *)g_xferwnd)->lstViewSendSelectChanged();
#endif //QTUI
  if (g_config->ReadInt("aoupload",1))
  {
#if defined(_WIN32) && !defined(QTUI)
    SendMessage(g_mainwnd,WM_COMMAND,ID_VIEW_TRANSFERS,0);

    if (g_config->ReadInt("aoupload_btf",1)) SetForegroundWindow(g_xferwnd);
    else SetForegroundWindow(hwndDlg);

    XferDlg_SetSel(2);
#elif defined(QTUI)
    XferDlg_SetSel(2);
#endif
  }
}

//#endif //WIN32
void Xfer_Run(void)
{
  TAG tag("Xfer_Run()");
  RunSends();
  RunRecvs();
}
#ifdef QTUI
void XferDlg_SetSel(int sel)
{
          ((Vaste_Idd_Xfers *)g_xferwnd)->TabWidget_IDC_TAB->setCurrentPage(sel);
}
#endif //QTUI
#if defined(_WIN32) && !defined(QTUI)
void XferDlg_SetSel(int sel)
{
  HWND tabwnd=GetDlgItem(g_xferwnd,IDC_TAB1);

  if (sel >= 0)
    TabCtrl_SetCurSel(tabwnd,sel);
  else 
    sel=TabCtrl_GetCurSel(tabwnd);

  if (sel >= 0)
  {
    if (g_xfer_subwnd_active != g_xfer_subwnds[sel])
    {
      if (g_xfer_subwnd_active) ShowWindow(g_xfer_subwnd_active,SW_HIDE);
      g_config->WriteInt("xfer_sel",sel);
      g_xfer_subwnd_active=g_xfer_subwnds[sel];
    }
    if (g_xfer_subwnd_active)
    {
			RECT r;
			GetClientRect(tabwnd,&r);

      TabCtrl_AdjustRect(tabwnd,FALSE,&r);

			SetWindowPos(g_xfer_subwnd_active,HWND_TOP,r.left,r.top,r.right-r.left,r.bottom-r.top,SWP_NOACTIVATE);
			ShowWindow(g_xfer_subwnd_active,SW_SHOWNA);
    }   
  }
}

static BOOL WINAPI Send_DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static ChildWndResizeItem rlist[]={
    {IDC_SENDLIST,0x0011},
    {IDC_CLEARCOMP2,0x0101},
    {IDC_CLEARCOMP,0x0101},
    {IDC_AUTOCLEAR,0x0101},
    {IDC_ABORTSEL,0x0101},
  };
  switch (uMsg)
  {
    case WM_SIZE:
      childresize_resize(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
    return 0;
    case WM_INITDIALOG:
      if (g_config->ReadInt("send_autoclear",0)) CheckDlgButton(hwndDlg,IDC_AUTOCLEAR,BST_CHECKED);
      g_lvsend.setwnd(GetDlgItem(hwndDlg,IDC_SENDLIST));
      g_lvsend.AddCol("File",g_config->ReadInt("send_col1",260));
      g_lvsend.AddCol("User",g_config->ReadInt("send_col2",50));
      g_lvsend.AddCol("Size",g_config->ReadInt("send_col3",72));
      g_lvsend.AddCol("Status",g_config->ReadInt("send_col4",88));

      CreateTooltip(GetDlgItem(hwndDlg,IDC_CLEARCOMP),"Clear inactive upload(s)");
      SendDlgItemMessage(hwndDlg,IDC_CLEARCOMP,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_CLEARIA),IMAGE_ICON,16,16,0));      
      
      CreateTooltip(GetDlgItem(hwndDlg,IDC_CLEARCOMP2),"Clear all upload(s)");
      SendDlgItemMessage(hwndDlg,IDC_CLEARCOMP2,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_CLEARALL),IMAGE_ICON,16,16,0));      

      CreateTooltip(GetDlgItem(hwndDlg,IDC_ABORTSEL),"Abort selected upload(s)");
      SendDlgItemMessage(hwndDlg,IDC_ABORTSEL,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_STOPDL),IMAGE_ICON,16,16,0));     

      childresize_init(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
    return 0;
    case WM_NOTIFY:
      {
        LPNMHDR l=(LPNMHDR)lParam;
        if (l->idFrom==IDC_SENDLIST)
        {
          if (l->code == NM_RCLICK)
          {
            int x;
            HMENU hMenu=GetSubMenu(g_context_menus,4);

            int sel=0;
            int l=g_lvsend.GetCount();
            for (x = 0; x < l && !sel; x ++) sel=g_lvsend.GetSelected(x);

            EnableMenuItem(hMenu,ID_ULWND_ABORTUPLOADS,sel?MF_ENABLED:MF_GRAYED);

            POINT p;
            GetCursorPos(&p);
            TrackPopupMenu(hMenu,TPM_RIGHTBUTTON|TPM_LEFTBUTTON,p.x,p.y,0,hwndDlg,NULL);
          }          
          int s2=!!ListView_GetItemCount(l->hwndFrom);
          int s=!!ListView_GetSelectedCount(l->hwndFrom);
          EnableWindow(GetDlgItem(hwndDlg,IDC_ABORTSEL),s);
          EnableWindow(GetDlgItem(hwndDlg,IDC_CLEARCOMP),s2);
          EnableWindow(GetDlgItem(hwndDlg,IDC_CLEARCOMP2),s2);

        }
      }
    return 0;
    case WM_DESTROY:
      g_config->WriteInt("send_col1",g_lvsend.GetColumnWidth(0));
      g_config->WriteInt("send_col2",g_lvsend.GetColumnWidth(1));
      g_config->WriteInt("send_col3",g_lvsend.GetColumnWidth(2));
      g_config->WriteInt("send_col4",g_lvsend.GetColumnWidth(3));
    return 0;
    case WM_COMMAND:
      switch (LOWORD(wParam))
      {
        case IDC_AUTOCLEAR:
          g_config->WriteInt("send_autoclear",
            IsDlgButtonChecked(hwndDlg,IDC_AUTOCLEAR)?1:0);
        return 0;
        case IDC_CLEARCOMP:
          {
            int x;
            int l=g_lvsend.GetCount();
            for (x = 0; x < l; x ++)
            {
              if (!g_lvsend.GetParam(x))
              {
                g_lvsend.DeleteItem(x--);
                l--;
              }
            }
          }        
        return 0;
        case IDC_CLEARCOMP2:
          if (MessageBox(hwndDlg,"Abort and clear all uploads?",APP_NAME " Question",MB_YESNO|MB_ICONQUESTION) == IDNO) break;
        case IDC_ABORTSEL:
          {
            int x;
            int l=g_lvsend.GetCount();
            for (x = 0; x < l; x ++)
            {
              if (LOWORD(wParam) == IDC_CLEARCOMP2 || g_lvsend.GetSelected(x))
              {
                void *tt=(void *)g_lvsend.GetParam(x);
                
                if (tt)
                {
                  int y,n=g_uploads.GetSize();
                  for (y = 0; y < n && (void*)g_uploads.Get(y) != tt; y ++);
                  if (y < n)
                  {
                    g_uploads.Set(y,NULL);
                    free(tt);
                    g_lvsend.SetItemParam(x,0);
                    g_lvsend.SetItemText(x,3,"Aborted");
                  }
                  else
                  {
                    XferSend *t=(XferSend *)tt;
                    t->Abort(g_mql);
                    if (t->get_idx() >= UPLOAD_BASE_IDX)
                    {
                      int wu=t->get_idx()-UPLOAD_BASE_IDX;
                      if (wu >= 0 && wu < n)
                      {
                        free(g_uploads.Get(wu));
                        g_uploads.Set(wu,NULL);
                      }
                    }
                  }
                }

                if (LOWORD(wParam) == IDC_CLEARCOMP2)
                {
                  g_lvsend.DeleteItem(x);
                  x--;
                  l--;
                }
              }
            }
          }
        return 0;
      }
    return 0;
  }
  return 0;
}
#endif//WIN32

#if defined(_WIN32) && !defined(QTUI)
void RecvQ_UpdateStatusText()
{
  char buf[256];
  
  sprintf(buf,"%d items in queue",g_files_in_download_queue);

  SetDlgItemText(GetParent(g_lvrecvq.getwnd()),IDC_QUEUEITEMS,buf);
  PostMessage(g_xferwnd,WM_USER_TITLEUPDATE,0,0);
}
#elif defined(QTUI)
void RecvQ_UpdateStatusText()
{
          ((Vaste_Idd_Xfers *)g_xferwnd)->recvQ_UpdateStatusText();
}
#endif

#if defined(_WIN32) && !defined(QTUI)
static BOOL WINAPI RecvQ_DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static ChildWndResizeItem rlist[]={
    {IDC_RECVLISTQ,0x0011},
    {IDC_REMOVEQUEUE,0x0101},
    {IDC_MOVETOTOP,0x0101},
    {IDC_MOVETOBOTTOM,0x0101},
    {IDC_DLNOW,0x0101},
    {IDC_CLEARQUEUE,0x0101},
    {IDC_QUEUEITEMS,0x0111},
  };
  switch (uMsg)
  {
    case WM_SIZE:
      childresize_resize(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
    return 0;
    case WM_INITDIALOG:
      CreateTooltip(GetDlgItem(hwndDlg,IDC_REMOVEQUEUE),"Remove item(s) from queue");
      SendDlgItemMessage(hwndDlg,IDC_REMOVEQUEUE,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_STOPDL),IMAGE_ICON,16,16,0));      

      CreateTooltip(GetDlgItem(hwndDlg,IDC_MOVETOTOP),"Move item(s) to top of queue");
      SendDlgItemMessage(hwndDlg,IDC_MOVETOTOP,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_MOVETOTOP),IMAGE_ICON,16,16,0));      
      CreateTooltip(GetDlgItem(hwndDlg,IDC_MOVETOBOTTOM),"Move item(s) to bottom of queue");
      SendDlgItemMessage(hwndDlg,IDC_MOVETOBOTTOM,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_MOVETOEND),IMAGE_ICON,16,16,0));      
      CreateTooltip(GetDlgItem(hwndDlg,IDC_DLNOW),"Download item(s) immediately");
      SendDlgItemMessage(hwndDlg,IDC_DLNOW,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_DLNOW),IMAGE_ICON,16,16,0));      

      CreateTooltip(GetDlgItem(hwndDlg,IDC_CLEARQUEUE),"Clear queue");
      SendDlgItemMessage(hwndDlg,IDC_CLEARQUEUE,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_CLEARALL),IMAGE_ICON,16,16,0));      
      
      g_lvrecvq.setwnd(GetDlgItem(hwndDlg,IDC_RECVLISTQ));
      g_lvrecvq.AddCol("File",g_config->ReadInt("recv_qcol1",339));
      g_lvrecvq.AddCol("Size",g_config->ReadInt("recv_qcol2",112));
      g_lvrecvq.AddCol("Location",g_config->ReadInt("recv_qcol3",0));
      childresize_init(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
      RecvQ_UpdateStatusText();
    return 0;
    case WM_DESTROY:
      g_config->WriteInt("recv_qcol1",g_lvrecvq.GetColumnWidth(0));
      g_config->WriteInt("recv_qcol2",g_lvrecvq.GetColumnWidth(1));
      g_config->WriteInt("recv_qcol3",g_lvrecvq.GetColumnWidth(2));
    return 0;
    case WM_NOTIFY:
      {
        LPNMHDR l=(LPNMHDR)lParam;
        if (l->idFrom==IDC_RECVLISTQ)
        {
          if (l->code == NM_DBLCLK)
          {
            SendMessage(hwndDlg,WM_COMMAND,IDC_DLNOW,0);
          }
          if (l->code == NM_RCLICK)
          {
            int x;
            HMENU hMenu=GetSubMenu(g_context_menus,3);

            int sel=0;
            int l=g_lvrecvq.GetCount();
            for (x = 0; x < l && !sel; x ++) sel=g_lvrecvq.GetSelected(x);

            EnableMenuItem(hMenu,IDC_DLNOW,sel?MF_ENABLED:MF_GRAYED);
            EnableMenuItem(hMenu,IDC_MOVETOTOP,sel?MF_ENABLED:MF_GRAYED);
            EnableMenuItem(hMenu,IDC_MOVETOBOTTOM,sel?MF_ENABLED:MF_GRAYED);
            EnableMenuItem(hMenu,IDC_REMOVEQUEUE,sel?MF_ENABLED:MF_GRAYED);

            POINT p;
            GetCursorPos(&p);
            TrackPopupMenu(hMenu,TPM_RIGHTBUTTON|TPM_LEFTBUTTON,p.x,p.y,0,hwndDlg,NULL);
          }          
          int s=!!ListView_GetSelectedCount(l->hwndFrom);
          int s2=!!ListView_GetItemCount(l->hwndFrom);
          EnableWindow(GetDlgItem(hwndDlg,IDC_DLNOW),s);
          EnableWindow(GetDlgItem(hwndDlg,IDC_MOVETOTOP),s);
          EnableWindow(GetDlgItem(hwndDlg,IDC_MOVETOBOTTOM),s);
          EnableWindow(GetDlgItem(hwndDlg,IDC_REMOVEQUEUE),s);
          EnableWindow(GetDlgItem(hwndDlg,IDC_CLEARQUEUE),s2);          
        }
      }
    return 0;
    case WM_COMMAND:
      switch (LOWORD(wParam))
      {
        case IDC_DLNOW:
          {
            int x;
            int l=g_lvrecvq.GetCount();
            int b=0;
            for (x = 0; x < l; x ++)
            {
              if (g_lvrecvq.GetSelected(x))
              {              
                char name[260];
                char sizestr[64];
                char host[512];
                g_lvrecvq.GetText(x,0,name,sizeof(name));
                g_lvrecvq.GetText(x,1,sizestr,sizeof(sizestr));
                g_lvrecvq.GetText(x,2,host,sizeof(host));
                g_lvrecvq.DeleteItem(x);
                g_files_in_download_queue--;

                int a,n=g_lvrecv.GetCount();
                for (a = 0; a < n; a ++)
                {
                  char name2[260];
                  char host2[512];
                  g_lvrecv.GetText(a,0,name2,sizeof(name2));
                  g_lvrecv.GetText(a,3,host2,sizeof(host2));   
                  if (!stricmp(name2,name) && !stricmp(host2,host))
                  {
                    if (g_lvrecv.GetParam(a)) break;
                    g_lvrecv.DeleteItem(a--);
                    n--;
                  }
                }
                if (a == n)
                {
                  g_recvs.Add(new XferRecv(g_mql,host,sizestr,name,g_config->ReadString("downloadpath","C:\\")));
                }                
                
                b=1;
                x--;
                l--;
              }
            }
            if (b)
            {
              XferDlg_SetSel(0);
            }
            RecvQ_UpdateStatusText();
          }
        return 0;
        case IDC_MOVETOBOTTOM:
          {
            int x;
            int l=g_lvrecvq.GetCount();
            int a=l-1;
            for (x = l-1; x >= 0; x --) 
            {
              if (g_lvrecvq.GetSelected(x))
              {
                char text[1024];
                char size[64];
                char loc[1024];
                int param=g_lvrecvq.GetParam(x);
                g_lvrecvq.GetText(x,0,text,sizeof(text));
                g_lvrecvq.GetText(x,1,size,sizeof(size));
                g_lvrecvq.GetText(x,2,loc,sizeof(loc));
                g_lvrecvq.DeleteItem(x);
                g_lvrecvq.InsertItem(a,text,param);
                g_lvrecvq.SetItemText(a,1,size);
                g_lvrecvq.SetItemText(a,2,loc);
                g_lvrecvq.SetSelected(a);
                a--;
              }
            }
          }
        return 0;
        case IDC_MOVETOTOP:
          {
            int x;
            int l=g_lvrecvq.GetCount();
            int a=0;
            for (x = 0; x < l; x ++)
            {
              if (g_lvrecvq.GetSelected(x))
              {
                char text[1024];
                char size[64];
                char loc[1024];
                int param=g_lvrecvq.GetParam(x);
                g_lvrecvq.GetText(x,0,text,sizeof(text));
                g_lvrecvq.GetText(x,1,size,sizeof(size));
                g_lvrecvq.GetText(x,2,loc,sizeof(loc));
                g_lvrecvq.DeleteItem(x);
                g_lvrecvq.InsertItem(a,text,param);
                g_lvrecvq.SetItemText(a,1,size);
                g_lvrecvq.SetItemText(a,2,loc);
                g_lvrecvq.SetSelected(a);
                a++;
              }
            }
          }
        return 0;
        case IDC_CLEARQUEUE:
          if (MessageBox(hwndDlg,"Clear entire download queue?",APP_NAME " Question",MB_YESNO|MB_ICONQUESTION) == IDNO) break;
        case IDC_REMOVEQUEUE:
          {
            int x;
            int l=g_lvrecvq.GetCount();
            for (x = 0; x < l; x ++)
            {
              if (LOWORD(wParam) == IDC_CLEARQUEUE || g_lvrecvq.GetSelected(x))
              {
                g_lvrecvq.DeleteItem(x--);
                l--;
                g_files_in_download_queue--;
              }
            }
          }
          RecvQ_UpdateStatusText();
        return 0;
      }
    return 0;
  }
  return 0;
}

static BOOL WINAPI Recv_DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static ChildWndResizeItem rlist[]={
    {IDC_RECVLIST,0x0011},
    {IDC_ABORTSEL,0x0101},
    {IDC_RESUME,0x0101},
    {IDC_CLEARCOMP3,0x0101},
    {IDC_CLEARCOMP,0x0101},
    {IDC_MSD,0x0101},
    {IDC_MAXSIMULDL,0x0101},
    {IDC_AUTOCLEAR,0x0101},
    {IDC_CLEARALL,0x0101},
    {IDC_AUTORESUME,0x0101},    
  };
  switch (uMsg)
  {
    case WM_SIZE:
      childresize_resize(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
    return 0;
    case WM_INITDIALOG:
      CreateTooltip(GetDlgItem(hwndDlg,IDC_ABORTSEL),"Abort selected download(s)");
      SendDlgItemMessage(hwndDlg,IDC_ABORTSEL,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_STOPDL),IMAGE_ICON,16,16,0));      

      CreateTooltip(GetDlgItem(hwndDlg,IDC_RESUME),"Resume selected download(s)");
      SendDlgItemMessage(hwndDlg,IDC_RESUME,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_DLNOW),IMAGE_ICON,16,16,0));      

      CreateTooltip(GetDlgItem(hwndDlg,IDC_CLEARCOMP),"Clear completed download(s)");
      SendDlgItemMessage(hwndDlg,IDC_CLEARCOMP,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_CLEARCOMP),IMAGE_ICON,16,16,0));      
      
      CreateTooltip(GetDlgItem(hwndDlg,IDC_CLEARCOMP3),"Clear inactive download(s)");
      SendDlgItemMessage(hwndDlg,IDC_CLEARCOMP3,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_CLEARIA),IMAGE_ICON,16,16,0));      

      CreateTooltip(GetDlgItem(hwndDlg,IDC_CLEARALL),"Clear all download(s)");
      SendDlgItemMessage(hwndDlg,IDC_CLEARALL,BM_SETIMAGE,IMAGE_ICON,
        (LPARAM)LoadImage(g_hInst,MAKEINTRESOURCE(IDI_CLEARALL),IMAGE_ICON,16,16,0));      

      if (g_config->ReadInt("recv_autoclear",0)) CheckDlgButton(hwndDlg,IDC_AUTOCLEAR,BST_CHECKED);
      if (g_config->ReadInt("recv_autores",1)) CheckDlgButton(hwndDlg,IDC_AUTORESUME,BST_CHECKED);
      SetDlgItemInt(hwndDlg,IDC_MAXSIMULDL,g_max_simul_dl,FALSE);
      g_lvrecv.setwnd(GetDlgItem(hwndDlg,IDC_RECVLIST));
      g_lvrecv.AddCol("File",g_config->ReadInt("recv_col1",282));
      g_lvrecv.AddCol("Size",g_config->ReadInt("recv_col2",80));
      g_lvrecv.AddCol("Status",g_config->ReadInt("recv_col3",101));
      g_lvrecv.AddCol("Location",g_config->ReadInt("recv_col4",0));
      childresize_init(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
    return 0;
    case WM_DESTROY:
      g_config->WriteInt("recv_col1",g_lvrecv.GetColumnWidth(0));
      g_config->WriteInt("recv_col2",g_lvrecv.GetColumnWidth(1));
      g_config->WriteInt("recv_col3",g_lvrecv.GetColumnWidth(2));
      g_config->WriteInt("recv_col4",g_lvrecv.GetColumnWidth(3));
    return 0;
    case WM_NOTIFY:
      {
        NMHDR *p=(NMHDR*)lParam;

        if (p->idFrom == IDC_RECVLIST)
        {
          if (p->code == NM_DBLCLK)
          {
            int x;
            int l=g_lvrecv.GetCount();
            for (x = 0; x < l && !g_lvrecv.GetSelected(x); x ++);
            if (x < l)
            {
              if (!g_lvrecv.GetParam(x))
              {
                char text[32];
                g_lvrecv.GetText(x,3,text,sizeof(text));
                if (text[0])
                {
                  SendMessage(hwndDlg,WM_COMMAND,IDC_RESUME,0);
                }
                else
                {
                  doExecuteOfDownload(hwndDlg,x,0);
                }
              }
            }
          }
          if (p->code == NM_RCLICK)
          {
            int x;
            HMENU hMenu=GetSubMenu(g_context_menus,0);

            int sel=0;
            int l=g_lvrecv.GetCount();
            for (x = 0; x < l && !sel; x ++) sel=g_lvrecv.GetSelected(x);

            EnableMenuItem(hMenu,ID_LAUNCHSEL,sel?MF_ENABLED:MF_GRAYED);
            EnableMenuItem(hMenu,ID_LAUNCHSEL2,sel?MF_ENABLED:MF_GRAYED);            
            EnableMenuItem(hMenu,ID_ABORTSEL,sel?MF_ENABLED:MF_GRAYED);
            EnableMenuItem(hMenu,ID_RESUME,sel?MF_ENABLED:MF_GRAYED);

            POINT p;
            GetCursorPos(&p);
            x=TrackPopupMenu(hMenu,TPM_RETURNCMD|TPM_RIGHTBUTTON|TPM_LEFTBUTTON|TPM_NONOTIFY,p.x,p.y,0,hwndDlg,NULL);

            if (x == ID_LAUNCHSEL || x == ID_LAUNCHSEL2)
            {
              int copy=x==ID_LAUNCHSEL2;
              int l=g_lvrecv.GetCount();
              for (x = 0; x < l; x ++)
              {
                if (g_lvrecv.GetSelected(x))
                {
                  doExecuteOfDownload(hwndDlg,x,copy);
                }
              }
            }
            else if (x == ID_DLFOLDER)
            {
              char *t=g_config->ReadString("downloadpath","");
              if (t&&*t) ShellExecute(hwndDlg,"open",t,NULL,t,SW_SHOW);
            }
            else if (x == ID_CLEANUPDLDIR)
            {
              char *t=g_config->ReadString("downloadpath","");
              if (t&&*t) 
              {
                int f=0;
                CleanupDLFolder(t,hwndDlg,&f);
              }
            }
            else if (x == ID_ABORTSEL)
            {
              SendMessage(hwndDlg,WM_COMMAND,IDC_ABORTSEL,0);
            }
            else if (x == ID_RESUME)
            {
              SendMessage(hwndDlg,WM_COMMAND,IDC_RESUME,0);
            }
          }          
          int s=!!ListView_GetSelectedCount(p->hwndFrom);
          int s2=!!ListView_GetItemCount(p->hwndFrom);
          EnableWindow(GetDlgItem(hwndDlg,IDC_RESUME),s);
          EnableWindow(GetDlgItem(hwndDlg,IDC_ABORTSEL),s);
          EnableWindow(GetDlgItem(hwndDlg,IDC_CLEARCOMP),s2);
          EnableWindow(GetDlgItem(hwndDlg,IDC_CLEARCOMP3),s2);
          EnableWindow(GetDlgItem(hwndDlg,IDC_CLEARALL),s2);          
        }
      }
    return 0;
    case WM_COMMAND:
      switch (LOWORD(wParam))
      {
        case IDC_MAXSIMULDL:
          if (HIWORD(wParam) == EN_CHANGE) 
          {
            int r;
            BOOL b;
            r=GetDlgItemInt(hwndDlg,IDC_MAXSIMULDL,&b,FALSE);
            if (b)
            {
              g_max_simul_dl=r;
              g_config->WriteInt("recv_maxdl",r);
            }
          }
        return 0;
        case IDC_AUTOCLEAR:
          g_config->WriteInt("recv_autoclear",
            IsDlgButtonChecked(hwndDlg,IDC_AUTOCLEAR)?1:0);
        return 0;
        case IDC_AUTORESUME:
          g_config->WriteInt("recv_autores",
            IsDlgButtonChecked(hwndDlg,IDC_AUTORESUME)?1:0);
        return 0;
        case IDC_CLEARCOMP:
        case IDC_CLEARCOMP3:
          {
            int x;
            int l=g_lvrecv.GetCount();
            for (x = 0; x < l; x ++)
            {
              char buf[64];
              g_lvrecv.GetText(x,3,buf,sizeof(buf));
              if (!g_lvrecv.GetParam(x) &&
                (LOWORD(wParam) == IDC_CLEARCOMP3 ||
                 !buf[0]))
              {
                g_lvrecv.DeleteItem(x--);
                l--;
              }
            }
          }
        return 0;
        case IDC_CLEARALL:
          {
            if (MessageBox(hwndDlg,"Abort all downloads?",APP_NAME " Question",MB_YESNO|MB_ICONQUESTION) == IDNO) break;
            int x;
            int l=g_lvrecv.GetCount();
            for (x = 0; x < l; x ++)
            {
              XferRecv *a=(XferRecv *)g_lvrecv.GetParam(x);
              if (a) a->Abort(g_mql);
              g_lvrecv.DeleteItem(x--);
              l--;
            }
          }
        return 0;
        case IDC_ABORTSEL:
          {
            int x;
            int l=g_lvrecv.GetCount();
            for (x = 0; x < l; x ++)
            {
              if (g_lvrecv.GetSelected(x))
              {
                XferRecv *a=(XferRecv *)g_lvrecv.GetParam(x);
                if (a) a->Abort(g_mql);
              }
            }
          }
        return 0;
        case IDC_RESUME:
          {
            int x;
            int l=g_lvrecv.GetCount();
            for (x = 0; x < l; x ++)
            {
              if (g_lvrecv.GetSelected(x))
              {
                int param=g_lvrecv.GetParam(x);
                if (!param)
                {
                  char sizestr[64];
                  char host[512];
                  char name[260];
                  g_lvrecv.GetText(x,3,host,sizeof(host));
                  if (host[0])
                  {
                    g_lvrecv.GetText(x,0,name,sizeof(name));
                    g_lvrecv.GetText(x,1,sizestr,sizeof(sizestr));
                    g_lvrecv.DeleteItem(x);
                    int p=g_lvrecvq.InsertItem(g_lvrecvq.GetCount(),name,0);
                    g_lvrecvq.SetItemText(p,1,sizestr);
                    g_lvrecvq.SetItemText(p,2,host);
                    g_files_in_download_queue++;
                    l--; x--;
                  }
                }
              }
            }
          }
          RecvQ_UpdateStatusText();
        return 0;
      }
    return 0;
  }
  return 0;
}



BOOL WINAPI Xfers_DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static dlgSizeInfo sizeinf={"xfers",104,189,491,249};
  static ChildWndResizeItem rlist[]={
    {IDC_TAB1,0x0011},
  };
  switch (uMsg)
  {
    case WM_GETMINMAXINFO:
      {
        LPMINMAXINFO m=(LPMINMAXINFO)lParam;
        if (m)
        {
          m->ptMinTrackSize.x=298;
          m->ptMinTrackSize.y=156;
        }
      }
    return 0;
    case WM_NOTIFY:
      {
        LPNMHDR p=(LPNMHDR) lParam;
        if (p->idFrom == IDC_TAB1 && p->code == TCN_SELCHANGE) XferDlg_SetSel();
      }
    return 0;
    case WM_CLOSE:
      CheckMenuItem(GetMenu(g_mainwnd),
        ID_VIEW_TRANSFERS,MF_UNCHECKED|MF_BYCOMMAND);
      ShowWindow(hwndDlg,SW_HIDE);
      g_config->WriteInt("xfers_vis",0);
    return 0;
    case WM_SHOWWINDOW:
      {
        static int m_hack;
        if (!m_hack)
        {
          m_hack=1;
          int m=g_config->ReadInt("xfers_maximized",0);
          handleDialogSizeMsgs(hwndDlg,WM_INITDIALOG,0,0,&sizeinf);
          if (m) ShowWindow(hwndDlg,SW_SHOWMAXIMIZED);
        }
      }
    return 0;
    case WM_MOVE:
      handleDialogSizeMsgs(hwndDlg,uMsg,wParam,lParam,&sizeinf);
    return 0;
    case WM_SIZE:
      if (wParam != SIZE_MINIMIZED)
      {
        childresize_resize(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
        handleDialogSizeMsgs(hwndDlg,uMsg,wParam,lParam,&sizeinf);
        XferDlg_SetSel();
      }
    return 0;
    case WM_INITDIALOG:
      childresize_init(hwndDlg,rlist,sizeof(rlist)/sizeof(rlist[0]));
      {
        TCITEM item;
        HWND tabwnd=GetDlgItem(hwndDlg,IDC_TAB1);
        item.mask=TCIF_TEXT;
        item.pszText="Downloads";
        TabCtrl_InsertItem(tabwnd,0,&item);
        item.pszText="Download Queue";
        TabCtrl_InsertItem(tabwnd,1,&item);
        item.pszText="Uploads";
        TabCtrl_InsertItem(tabwnd,2,&item);

        g_xfer_subwnds[0]=CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_RECV),hwndDlg,Recv_DlgProc);
        g_xfer_subwnds[1]=CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_RECVQ),hwndDlg,RecvQ_DlgProc);
        g_xfer_subwnds[2]=CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_SEND),hwndDlg,Send_DlgProc);

        TabCtrl_SetCurSel(tabwnd,g_config->ReadInt("xfer_sel",0));
      }
    case WM_USER_TITLEUPDATE:
      {
        char buf[1024];
        strcpy(buf,"Transfers");
        int numdls=g_recvs.GetSize();
        int numuls=g_sends.GetSize();
        int numq=g_files_in_download_queue;
        int a=0;

        if (numdls) sprintf(buf+strlen(buf),"%s%d DL%s",a++?", ":" (",numdls,numdls==1?"":"s");
        if (numq) sprintf(buf+strlen(buf),"%s%d DL%s Queued",a++?", ":" (",numq,numq==1?"":"s");
        if (numuls) sprintf(buf+strlen(buf),"%s%d UL%s",a++?", ":" (",numuls,numuls==1?"":"s");
        if (a) strcat(buf,")");
       
        strcat(buf," - " APP_NAME);
        SetWndTitle(hwndDlg,buf);
      }
    return 0;
    case WM_DESTROY:
    return 0;
  }
  return 0;
}

#endif//WIN32
