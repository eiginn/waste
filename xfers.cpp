/*
    WASTE - xfers.cpp (File transfer implementation)
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
    along with WASTE if not, write to the Free Software
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
#ifdef _WIN32
#include "xferwnd.h"
#include "srchwnd.h"
#elif defined(QTUI)
#include "xferwnd.h"
#include "srchwnd.h"
#include "vaste_idd_xfers.h"
#endif
#include "tag.h"

#include "netkern.h"
#include "rsa/md5.h"

#define IS_VALID(x) (m_validbf[(x)>>3]&(1<<(x&7)))
#define SET_VALID(x) m_validbf[(x)>>3]|=(1<<(x&7))

#ifdef QTUI
extern QWidget *g_xferwnd;
#endif

XferSend::XferSend(C_MessageQueueList *mql,T_GUID *guid, C_FileSendRequest *req, char *fn)
{ 
  m_create_date=0;
  m_mod_date=0;
  m_last_cps=0;
  m_max_chunksent=0;
  chunks_to_send_pos=0;
  chunks_to_send_len=0;
  m_starttime=GetTickCount();
  m_lastchunkcnt=0;
  m_chunks_sent_total=0;
  m_need_reply=0;
#ifdef XFER_WIN32_FILEIO
  m_hfile=INVALID_HANDLE_VALUE;
#else
  m_file=0;
#endif
  m_err=0;
  m_fn[0]=0;
  m_guid=*guid;
  m_idx=req->get_idx();

  if (fn[0])
  {
    char *p=fn;
    while (*p) p++;
    while (p >= fn && *p != '/' && *p != '\\') p--;
    
    unsigned char hashbuf[SHA_OUTSIZE],hashbuf2[SHA_OUTSIZE];
    req->get_fn_hash(hashbuf2);
    SHAify context;
    context.add((unsigned char *)p+1, min(strlen(p+1),sizeof(m_fn)-1));
    context.final(hashbuf);

    if (g_config->ReadInt("ulfullpaths",0))
    {
      if (p >= fn) p--;
      while (p >= fn && *p != '/' && *p != '\\') p--;
      if (p >= fn) p--;
      while (p >= fn && *p != '/' && *p != '\\') p--;
    }
    safe_strncpy(m_fn,p+1,sizeof(m_fn));
    char *t=m_fn;
    while (*t)
    {
      if (*t == '\\') *t='/';
      t++;
    }

    if (memcmp(hashbuf,hashbuf2,SHA_OUTSIZE))
    {
      m_err="Filename mismatch";
    }
    else
    {
#ifdef XFER_WIN32_FILEIO
      m_hfile=CreateFile(fn,
          GENERIC_READ,
          FILE_SHARE_READ|FILE_SHARE_WRITE,
          NULL,
          OPEN_EXISTING,
          FILE_ATTRIBUTE_NORMAL, NULL);
      if (m_hfile==INVALID_HANDLE_VALUE)
#else
      m_file=fopen(fn,"rb");
      if (!m_file)
#endif
      {
        m_err="File not found on disk";
      }
      else
      {
#ifdef XFER_WIN32_FILEIO
        m_filelen_bytes_l=GetFileSize(m_hfile,(LPDWORD)&m_filelen_bytes_h);

        FILETIME ct,wt;
        if (GetFileTime(m_hfile,&ct,NULL,&wt))
        {
          m_create_date=C_FileDB::FileTimeToUnixTime(&ct);
          m_mod_date=C_FileDB::FileTimeToUnixTime(&wt);
        }

#else
        fseek(m_file,0,SEEK_END);
        m_filelen_bytes_h=0;//fucko64 for bsd
        m_filelen_bytes_l=ftell(m_file);
        fseek(m_file,0,SEEK_SET);
  
        struct stat s;
        fstat(fileno(m_file),&s);
        m_create_date=m_mod_date=s.st_mtime; // fuckobsd: create = modified time
#endif

        if (g_config->ReadInt("shafiles",1)
            && !m_filelen_bytes_h && m_filelen_bytes_l < (unsigned int)g_config->ReadInt("maxsizesha",32)*1024*1024)
        {
          context.reset();
          for (;;)
          {
            unsigned char buf[8192];
#ifdef XFER_WIN32_FILEIO
            DWORD l;
            if (!ReadFile(m_hfile,buf,sizeof(buf),&l,NULL) || !l) break;
#else
            int l=fread(buf,1,8192,m_file);
            if (!l) break;
#endif
            context.add(buf,l);
          }
          context.final(hashbuf);
#ifdef XFER_WIN32_FILEIO
          SetFilePointer(m_hfile,0,NULL,FILE_BEGIN);
#else
          fseek(m_file,0,SEEK_SET);
#endif
        }
        else
          memset(hashbuf,0,SHA_OUTSIZE);

        m_reply.set_hash(hashbuf);
        if (g_config->ReadInt("directxfers",0))
        {
          int ip,prt=0;
          req->get_dc_ipport(&ip,&prt);
          if (ip && prt)
          {
            NetKern_ConnectToHostIfOK(ip,prt);
          }
          if (g_route_traffic && g_listen && !g_listen->is_error() && mql->GetNumQueues())
          {       
            int ip=(g_forceip&&g_forceip_addr!=INADDR_NONE)?g_forceip_addr:  
                    mql->GetQueue(0)->get_con()->get_interface();
            m_reply.set_dc_ipport(ip,g_listen->port());
          }
        }
        m_lastpos_l=m_lastpos_h=0;
        m_filelen_chunks=(m_filelen_bytes_l+FILE_CHUNKSIZE-1)/FILE_CHUNKSIZE + (m_filelen_bytes_h * ((1<<30)/FILE_CHUNKSIZE) * 4);
        if (m_filelen_chunks<1) m_filelen_chunks=1;
        onGotMsg(req);
      }
    }
  }
  else
    m_err="File not found in DB";

  if (m_err) // send reply
  {
    T_Message msg={0,};
    m_reply.set_error(1);
    msg.data=m_reply.Make();
    if (msg.data)
    {
      msg.message_type=MESSAGE_FILE_REQUEST_REPLY;
      msg.message_length=msg.data->GetLength();
      msg.message_guid=m_guid;
      mql->send(&msg);
    }
  }
  m_last_talktime=time(NULL);
}

XferSend::~XferSend()
{
#ifdef XFER_WIN32_FILEIO
  if (m_hfile != INVALID_HANDLE_VALUE) CloseHandle(m_hfile);
#else
  if (m_file) fclose(m_file);
#endif
}

void XferSend::Abort(C_MessageQueueList *mql) 
{ 
  T_Message msg={0,};
  m_err="Aborted"; 
  m_reply.set_error(2);
  msg.data=m_reply.Make();
  if (msg.data)
  {
    msg.message_type=MESSAGE_FILE_REQUEST_REPLY;
    msg.message_length=msg.data->GetLength();
    msg.message_guid=m_guid;
    mql->send(&msg);
  }
}

int XferSend::run_hdr(C_MessageQueueList *mql)
{
  if (m_err) return 1;
  if (time(NULL)-m_last_talktime > 300) // 5 minutes
  {
    sprintf(m_err_buf,"Timed out @ %d%%",m_max_chunksent*100/m_filelen_chunks);
    m_err=m_err_buf;
    return 1;
  }
  if (m_need_reply)
  {
//    debug_printf("xfer_send: sent header\n");
    m_need_reply=0;
    T_Message msg={0,};
    msg.data=m_reply.Make();
    msg.message_type=MESSAGE_FILE_REQUEST_REPLY;
    msg.message_length=msg.data->GetLength();
    msg.message_guid=m_guid;
    mql->send(&msg);
    m_reply.set_dc_ipport(0,0); // only send that once
    m_last_talktime=time(NULL);
  }
  return 0;
}

void XferSend::run(C_MessageQueueList *mql)
{
  TAG tag("XferSend::run()");
  if (!m_err && chunks_to_send_pos<chunks_to_send_len)
  {
    m_last_talktime=time(NULL);
    int a=mql->find_route(&m_guid,MESSAGE_FILE_REQUEST_REPLY);
    if (a >= -1 && mql->GetQueue(a)->getlen() < mql->GetQueue(a)->getmaxlen()/4)
    {
      m_lastsendtime=GetTickCount();

      unsigned int x=chunks_to_send[chunks_to_send_pos++];
      if (x >= 0 && x < m_filelen_chunks)
      {
        unsigned int newpos_l=x*FILE_CHUNKSIZE;
#ifdef _WIN32
        unsigned int newpos_h=(unsigned int) (((__int64)x*(__int64)FILE_CHUNKSIZE)>>32);
#else
        unsigned int newpos_h=0;
#endif
        if (newpos_l != m_lastpos_l || newpos_h != m_lastpos_h)
        {
#ifdef XFER_WIN32_FILEIO
          LONG zero=newpos_h;
          SetFilePointer(m_hfile,newpos_l,&zero,FILE_BEGIN);
#else
          fseek(m_file,newpos_l,SEEK_SET);
#endif
          m_lastpos_l=newpos_l;
          m_lastpos_h=newpos_h;
        }
        unsigned char buf[FILE_CHUNKSIZE];
#ifdef XFER_WIN32_FILEIO
        DWORD l;
        if (!ReadFile(m_hfile,buf,FILE_CHUNKSIZE,&l,NULL))
          l=0;
#else
        int l=fread(buf,1,FILE_CHUNKSIZE,m_file);
#endif
        unsigned int o = m_lastpos_l;
        m_lastpos_l+=l;
        if (m_lastpos_l < o) m_lastpos_h++;
        C_FileSendReply datareply;

        datareply.set_data(buf,l);
        datareply.set_index(x);
        T_Message msg={0,};
        msg.data=datareply.Make();
        msg.message_type=MESSAGE_FILE_REQUEST_REPLY;
        msg.message_length=msg.data->GetLength();
        msg.message_guid=m_guid;
        mql->send(&msg);
        if (x > m_max_chunksent) m_max_chunksent=x;
        m_chunks_sent_total++;
      }

      if (g_extrainf)
      {
        updateStatusText();
      }
    }
  }
}

void XferSend::updateStatusText()
{
  TAG tag("XferSend::updateStatusText()");
  char s[128];
  int wcps=m_last_cps;

  sprintf(s,"%d%%@%d.%02dk/s",
    (m_max_chunksent*100)/m_filelen_chunks,
    wcps/1000,(wcps/10)%100);

  if (g_extrainf) sprintf(s+strlen(s)," %d/%d/%d (%d)",m_max_chunksent+1,m_chunks_sent_total,m_filelen_chunks,chunks_to_send_len);

#ifdef _WIN32
  int idx=g_lvsend.FindItemByParam((int)this);
  if (idx!=-1) g_lvsend.SetItemText(idx,3,s);
#elif defined(QTUI)
  int idx=g_lvsend.FindItemByParam((int)this);
  debug_printf("idx = %d, str = %s\n", idx, s);
  if (idx!=-1) g_lvsend.SetItemText(idx,3,s);
#endif
}

void XferSend::onGotMsg(C_FileSendRequest *req)
{
  TAG tag("XferSend::onGotMsg()");
  if (m_err) return;
  m_last_talktime=time(NULL);
  if (req->is_abort())
  {
    if (req->is_abort()==2)
      sprintf(m_err_buf,"Completed");
    else
    {
      sprintf(m_err_buf,"Aborted @ %d%%",
        (m_chunks_sent_total*100)/m_filelen_chunks);

      if (g_extrainf) sprintf(m_err_buf+strlen(m_err_buf)," (%d/%d)", m_chunks_sent_total,m_filelen_chunks);
    }
    m_err=m_err_buf;
    return;
  }
  if (m_idx != req->get_idx())
  {
    m_err="Got different index";
    return;
  }
  m_need_reply=1;
  m_reply.set_file_len(m_filelen_bytes_l,m_filelen_bytes_h);
  m_reply.set_file_dates(m_create_date,m_mod_date);
  m_reply.set_index(-1);
  unsigned int x;

  chunks_to_send_pos=0;
  chunks_to_send_len=req->get_chunks_needed();
  if (chunks_to_send_len)
  {
    if (chunks_to_send_len > FILE_MAX_CHUNKS_PER_REQ) chunks_to_send_len=FILE_MAX_CHUNKS_PER_REQ;
    for (x = 0; x < chunks_to_send_len; x ++) 
    {
      unsigned int nc=req->get_need_chunk(x);
      if (nc < m_filelen_chunks) chunks_to_send[x]=nc;
      else 
      {
        chunks_to_send_len--; // sending less chunks
        x--; // skip this chunk
      }
    }
  }

  if (!chunks_to_send_len) // default poopie
  {
    chunks_to_send[0]=0;
    chunks_to_send_len=1;
  }
  m_reply.set_chunkcount(chunks_to_send_len);
  unsigned int tm=GetTickCount()-m_starttime;

#ifdef _WIN32
  if (tm) 
    m_last_cps=MulDiv(m_chunks_sent_total-m_lastchunkcnt,1000*FILE_CHUNKSIZE,tm);
  else m_last_cps=0;
#else
  if (tm) m_last_cps=((m_chunks_sent_total-m_lastchunkcnt)*1000*FILE_CHUNKSIZE) / (tm);
  else m_last_cps=0;
#endif

  m_starttime=GetTickCount();
  m_lastchunkcnt=m_chunks_sent_total;

  updateStatusText();
}






void recursive_create_dir(char *directory)
{
  char *tp;
	char *p;
  p=directory;
  while (*p == ' ') p=CharNext(p);
  if (!*p) return;
  tp=CharNext(p);
#ifdef _WIN32
  if (*tp == ':' && tp[1] == '\\') p=tp+2;
  else if (p[0] == '\\' && p[1] == '\\')
  {
    int x;
    for (x = 0; x < 2; x ++)
    {
      while (*p != '\\' && *p) p=CharNext(p); // skip host then share
      if (*p) p=CharNext(p);
    }

  }
  else return;
#else
  tp=p+1;
#endif
  while (*p)
  {
    while (*p != DIRCHAR && *p) p=CharNext(p);
    if (!*p) CreateDirectory(directory,NULL);
    else
    {
      *p=0;
  	  CreateDirectory(directory,NULL);
      *p++ = DIRCHAR;
    }
  }

}



//////////// recv

XferRecv::XferRecv(C_MessageQueueList *mql, char *guididx, char *sizestr, char *filename, char *path)
{
	TAG tag("XferRecv::XferRecv()");
  debug_printf("guididx = %s, sizestr = %s, file = %s, path = %s\n", 
		  guididx, sizestr, filename, path);
#ifdef _WIN32
  g_lvrecv.InsertItem(0,filename,(int)this);
  g_lvrecv.SetItemText(0,2,"Connecting");
  g_lvrecv.SetItemText(0,1,sizestr);
  g_lvrecv.SetItemText(0,3,guididx);
#elif defined(QTUI)
  g_lvrecv.InsertItem(0,filename,(int)this);
  g_lvrecv.SetItemText(0,2,"Connecting");
  g_lvrecv.SetItemText(0,1,sizestr);
  g_lvrecv.SetItemText(0,3,guididx);
  ((Vaste_Idd_Xfers *)g_xferwnd)->lstViewRecvSelectChanged();
#endif

  m_adaptive_chunksize=FILE_MAX_CHUNKS_PER_REQ/2;

  m_path_len=0x10000000;
  m_hasgotchunks=0;
  m_validbf=0;
  m_tmpcopyfn=0;
  m_chunk_startcnt=0;
  m_total_chunks_recvd=0;
  m_done=0;
  chunks_coming=100000000;
  m_chunk_total=m_chunk_cnt=0;
  m_first_chunkilack=0;
  m_err=0;
  lasthdr=0;
#ifdef XFER_WIN32_FILEIO
  m_houtfile=INVALID_HANDLE_VALUE;
  m_hstatfile=INVALID_HANDLE_VALUE;
#else
  m_outfile=0;
  m_statfile=0;
#endif
  m_outfile_lastpos_l=m_outfile_lastpos_h=0;

  char *p=filename;
  while (*p) p++;
  while (p >= filename && 
#ifdef _WIN32
    *p != '\\' && 
#endif
    *p != '/') p--;
  p++;
  unsigned char fn_hash[SHA_OUTSIZE];
  SHAify context;
  context.add((unsigned char *)p, strlen(p));
  context.final(fn_hash);
  request.set_fn_hash(fn_hash);
  if (g_config->ReadInt("nickonxfers",1))
    request.set_nick(g_regnick);
  if (g_route_traffic && g_listen && !g_listen->is_error() && mql->GetNumQueues() && g_config->ReadInt("directxfers",0))
  {       
    int ip=(g_forceip&&g_forceip_addr!=INADDR_NONE)?g_forceip_addr:  
            mql->GetQueue(0)->get_con()->get_interface();
    request.set_dc_ipport(ip,g_listen->port());
  }

  m_statfile_fn=(char *)malloc(strlen(filename)+strlen(path)+64);
  m_outfile_fn=(char *)malloc(strlen(filename)+strlen(path)+64);

  // create dir for file if present
  if (p > filename)
  {
    strcpy(m_outfile_fn,path);
#ifdef _WIN32
    strcat(m_outfile_fn,"\\");
#else
    strcat(m_outfile_fn,"/");
#endif
    // removes trailing slash too cause safe_strncpy owns (err because of the length)
    safe_strncpy(m_outfile_fn+strlen(m_outfile_fn),filename,p-filename); 
    p=m_outfile_fn;
    while (*p)
    {
      if (*p == '/' || *p == '\\') *p=DIRCHAR;
      p++;
    }
#ifdef _WIN32
    removeInvalidFNChars(m_outfile_fn+2); //skip drive letter on win32
#else
    removeInvalidFNChars(m_outfile_fn); //skip drive letter
#endif

    recursive_create_dir(m_outfile_fn);
    m_path_len=strlen(path);
  }
  m_outfile_fn_ll=strlen(path)+1;
  
  char *ext=NULL;


  p=filename;
  while (*p) p++;
#ifdef _WIN32
  while (p >= filename  && *p != '\\' && *p != '/' && *p != '.') p--;
#else
  while (p >= filename  && *p != '/' && *p != '.') p--;
#endif
  if (p >= filename && *p == '.')
  {
    ext=strdup(p);
    *p=0;
  }
  else ext=strdup("");


  int a=0;
  do
  {
#ifdef XFER_WIN32_FILEIO
    if (m_houtfile != INVALID_HANDLE_VALUE) CloseHandle(m_houtfile);
    if (m_hstatfile != INVALID_HANDLE_VALUE) CloseHandle(m_hstatfile);
#else
    if (m_outfile) fclose(m_outfile);
    if (m_statfile) fclose(m_statfile);
#endif
    if (a) 
    {
      sprintf(m_outfile_fn,"%s%c%s.%d%s",path,DIRCHAR,filename,a+1,ext);
    }
    else sprintf(m_outfile_fn,"%s%c%s%s",path,DIRCHAR,filename,ext);

    removeInvalidFNChars(m_outfile_fn+2); // don't remove drive letter : :)

    sprintf(m_statfile_fn,"%s.PADLOCKSLSTATE",m_outfile_fn);
#ifdef XFER_WIN32_FILEIO
    m_hstatfile=CreateFile(m_statfile_fn,GENERIC_READ|GENERIC_WRITE,
      0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    m_houtfile=CreateFile(m_outfile_fn,GENERIC_READ|GENERIC_WRITE,
      FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  }
  while (m_houtfile != INVALID_HANDLE_VALUE && 
         m_hstatfile == INVALID_HANDLE_VALUE && a++ < 255);
#else
    m_statfile=fopen(m_statfile_fn,"r+b");
    m_outfile=fopen(m_outfile_fn,"r+b");
  }
  while (m_outfile && !m_statfile && a++ < 255);
#endif

  free(ext);

#ifdef XFER_WIN32_FILEIO
  if (m_hstatfile == INVALID_HANDLE_VALUE) 
    m_hstatfile=CreateFile(m_statfile_fn,GENERIC_READ|GENERIC_WRITE,
      0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
  if (m_houtfile == INVALID_HANDLE_VALUE) 
    m_houtfile=CreateFile(m_outfile_fn,GENERIC_READ|GENERIC_WRITE,
      FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
  if (m_houtfile == INVALID_HANDLE_VALUE || a >= 255)
#else
  if (!m_statfile) m_statfile=fopen(m_statfile_fn,"w+b");
  if (!m_outfile) m_outfile=fopen(m_outfile_fn,"w+b");
  if (!m_outfile || a >= 255)
#endif
  {
    m_err="Error opening output file";
    return;
  }

  unsigned int x;
  if (strlen(guididx) < 34)
  {
    m_err="Malformed address";
    return;
  }
  T_GUID gid;
  MakeID128FromStr(guididx,&gid);
  request.set_guid(&gid);
  request.set_idx(atoi(guididx+33));
  m_create_date=m_mod_date=0;
  m_bytes_total_l=0xFFFFFFFF;
  m_bytes_total_h=0xFFFFFFFF;
  memset(m_hash,0,SHA_OUTSIZE);
  m_next_stateflush_time=time(NULL)+60;

  int sig1=0, sig2=0;
#ifdef XFER_WIN32_FILEIO
  DWORD d;
  if (m_hstatfile != INVALID_HANDLE_VALUE && 
    ReadFile(m_hstatfile,&sig1,4,&d,NULL) && d == 4 && sig1 == PADLOCKSLSTATE_SIG1 &&
    ReadFile(m_hstatfile,&sig2,4,&d,NULL) && d == 4 && sig2 == PADLOCKSLSTATE_SIG2 &&
    ReadFile(m_hstatfile,&m_bytes_total_l,4,&d,NULL) && d == 4 &&
    ReadFile(m_hstatfile,&m_bytes_total_h,4,&d,NULL) && d == 4)
#else
  if (m_statfile &&    
    (fread(&sig1,1,4,m_statfile)==4) && (fread(&sig2,1,4,m_statfile)==4) &&
    (sig1 == PADLOCKSLSTATE_SIG1) && (sig2 == PADLOCKSLSTATE_SIG2) &&
    (fread(&m_bytes_total_l,1,4,m_statfile)==4) && (fread(&m_bytes_total_h,1,4,m_statfile)==4))
#endif
  {
#ifdef _WIN32
    char s[32];
    FormatSizeStr64(s,m_bytes_total_l,m_bytes_total_h);
    g_lvrecv.SetItemText(0,1,s);
#elif defined(QTUI)
    char s[32];
    FormatSizeStr64(s,m_bytes_total_l,m_bytes_total_h);
    g_lvrecv.SetItemText(0,1,s);
#endif

#ifdef XFER_WIN32_FILEIO
    if (ReadFile(m_hstatfile,m_hash,SHA_OUTSIZE,&d,NULL) && d==SHA_OUTSIZE)
#else
    if (fread(m_hash,1,SHA_OUTSIZE,m_statfile)==SHA_OUTSIZE)
#endif
    {
      m_chunk_total=(m_bytes_total_l+FILE_CHUNKSIZE-1)/FILE_CHUNKSIZE + (m_bytes_total_h * ((1<<30)/FILE_CHUNKSIZE) * 4);

      if (m_chunk_total<1) m_chunk_total=1;
      if (m_chunk_total)
      {
        free(m_validbf);
        m_validbf=(unsigned char *)malloc((m_chunk_total+7)/8);
#ifdef XFER_WIN32_FILEIO
        if (m_validbf && 
          ReadFile(m_hstatfile,m_validbf,(m_chunk_total+7)/8,&d,NULL) && d==(unsigned int)((m_chunk_total+7)/8))
#else
        if (m_validbf && fread(m_validbf,1,(m_chunk_total+7)/8,m_statfile)==(unsigned int)((m_chunk_total+7)/8))
#endif
        {
          for (x = 0; x < m_chunk_total; x ++)
          {
            if (IS_VALID(x)) 
            {
              m_total_chunks_recvd++;
              m_chunk_startcnt++;
              m_chunk_cnt++;
            }
          }
        }
      }
    }
  }
  if (m_chunk_total<1)
  {
    m_chunk_total=FILE_MAX_CHUNKS_PER_REQ; // default request as much as we can
  }
  T_Message m={0,};
  request.clear_need_chunks();
  int setfirst=0;
  for (x = m_first_chunkilack; x < m_chunk_total && request.get_chunks_needed() < m_adaptive_chunksize; x ++)
  {
    if (!m_validbf || !IS_VALID(x))  // dont have this one
    {
      request.add_need_chunk(x);
      if (!setfirst) 
      {
        m_first_chunkilack=x;
        setfirst++;
      }
    }
  }
  m.data=request.Make();
  m.message_type=MESSAGE_FILE_REQUEST;
  m.message_length=m.data->GetLength();

  mql->send(&m);


  m_guid=m.message_guid;
  request.set_dc_ipport(0,0); // never send dc ip/port again

  last_msg_time=time(NULL)+30; // give it a little more time the first time
  m_next_cpstime=GetTickCount();
  m_cps_blks_pos=0;
  m_last_cps=0;
  memset(m_cps_blks,0,sizeof(m_cps_blks));
}

XferRecv::~XferRecv()
{
  unsigned int filesize_l=0,filesize_h=0;
#ifdef XFER_WIN32_FILEIO
  if (m_houtfile!=INVALID_HANDLE_VALUE) 
  {
    filesize_l=GetFileSize(m_houtfile,(DWORD*)&filesize_h);
    if (m_done)
    {
      LONG d=m_bytes_total_h;
      SetFilePointer(m_houtfile,m_bytes_total_l,&d,FILE_BEGIN);
      SetEndOfFile(m_houtfile);

      FILETIME ct,wt;
      C_FileDB::UnixTimeToFileTime(&ct,m_create_date);
      C_FileDB::UnixTimeToFileTime(&wt,m_mod_date);
      SetFileTime(m_houtfile,m_create_date?&ct:NULL,NULL,m_mod_date?&wt:NULL);

      //BSC BEGIN : File download logging
      FILE *logfile;
      int lcv;
      char logfilename[500];
      char logline[500] = "";
      char buffer[200] = "";
      char *bufferpointer;
      char *pathlist = g_config->ReadString("databasepath", "");

      //Find the first shared directory
      if (strlen(pathlist) > 0)
      {
        for (lcv=0; lcv<strlen(pathlist); lcv++) {
          if (pathlist[lcv] == ';') break;
          logfilename[lcv] = pathlist[lcv];
        }
        logfilename[lcv] = 0;
        strcat(logfilename, "\\PADLOCKSLLog-");
        //strcat(logfilename, g_config->ReadString("nick", "BADNICK"));
        strcat(logfilename, g_regnick);
        strcat(logfilename, ".txt");
        //Open the logfile
        logfile = fopen(logfilename, "a+");

        //Move to the end of file for append
        SetFilePointer (logfile, 0, NULL, FILE_END);
        //Create the log line
        bufferpointer = strrchr(m_outfile_fn, '\\');
        strcpy(logline, &bufferpointer[1]);
        strcat(logline, "~"); //separator
        _itoa(filesize_l, buffer, 10);
        strcat(logline, buffer); //filesize
        strcat(logline, "~"); //separator
        _strdate(buffer);
        strcat(logline, buffer); //date
        strcat(logline, " ");
        _strtime(buffer);
        strcat(logline, buffer); //time
        strcat(logline, "\n"); //CRLF
        //Write out the file info
        fputs(logline, logfile);
        fclose(logfile);
      }
      //BSC END : File download logging
    }
    //BSC : Moved to before the IF so I can use it
    //filesize_l=GetFileSize(m_houtfile,(DWORD*)&filesize_h);
    CloseHandle(m_houtfile);
  }
  if (m_hstatfile != INVALID_HANDLE_VALUE) 
  {
    if (lasthdr) // flush state
    {
      DWORD d;
      SetFilePointer(m_hstatfile,0,NULL,FILE_BEGIN);
      int sig1=PADLOCKSLSTATE_SIG1, sig2=PADLOCKSLSTATE_SIG2;
      WriteFile(m_hstatfile,&sig1,4,&d,NULL);
      WriteFile(m_hstatfile,&sig2,4,&d,NULL);
      WriteFile(m_hstatfile,&m_bytes_total_l,4,&d,NULL);
      WriteFile(m_hstatfile,&m_bytes_total_h,4,&d,NULL);
      WriteFile(m_hstatfile,m_hash,SHA_OUTSIZE,&d,NULL);
      if (m_validbf) 
        WriteFile(m_hstatfile,m_validbf,(m_chunk_total+7)/8,&d,NULL);
      SetEndOfFile(m_hstatfile);
    }
    CloseHandle(m_hstatfile);
  }
#else
  if (m_outfile) 
  {
    fseek(m_outfile,0,SEEK_END);
    filesize_l=(unsigned int)ftell(m_outfile);
    filesize_h=0;
    // fucko for bsd, write date(s)
    fclose(m_outfile);
  }
  if (m_statfile) 
  {
    if (lasthdr) // flush state
    {
      fseek(m_statfile,0,SEEK_SET);
      int sig1=PADLOCKSLSTATE_SIG1, sig2=PADLOCKSLSTATE_SIG2;
      fwrite(&sig1,1,4,m_statfile);
      fwrite(&sig2,1,4,m_statfile);
      fwrite(&m_bytes_total_l,1,4,m_statfile);
      fwrite(&m_bytes_total_h,1,4,m_statfile);
      fwrite(m_hash,1,SHA_OUTSIZE,m_statfile);
      if (m_validbf) fwrite(m_validbf,1,(m_chunk_total+7)/8,m_statfile);
    }
    fclose(m_statfile);
  }
#endif
  free(m_validbf);
  if (m_done)
  {
#ifndef XFER_WIN32_FILEIO
    if (m_outfile_fn && m_outfile_fn[0])
    {
#ifdef _WIN32
      HANDLE hFile=CreateFile(m_outfile_fn,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
      if (hFile != INVALID_HANDLE_VALUE)
      {
        DWORD d=m_bytes_total_h;
        SetFilePointer(hFile,m_bytes_total_l,&d,FILE_BEGIN);
        SetEndOfFile(hFile);
        CloseHandle(hFile);
      }
#else
      //fixme: need non-win32 eof setting
#endif
    }
#endif
    if (
#ifdef XFER_WIN32_FILEIO
      m_hstatfile != INVALID_HANDLE_VALUE
#else   
      m_statfile 
#endif
      
      && m_statfile_fn && m_statfile_fn[0]) 
    {
      DeleteFile(m_statfile_fn);
    }
  }
  else if (!filesize_l && !filesize_h)
  {
    DeleteFile(m_statfile_fn);   
    DeleteFile(m_outfile_fn);

    if (m_path_len < (int)strlen(m_outfile_fn)) // cleanup any directories created
    {
      char *s=strdup(m_outfile_fn);
      char *p=s+strlen(s);
      for (;;)
      {
        while (p > s+m_path_len && *p != '/' && *p != '\\') p--;
        if (p <= s+m_path_len) break;
        *p=0;
        if (!RemoveDirectory(s)) break;
      }
      free(s);
    }
  }

  free(m_statfile_fn);   
  free(m_outfile_fn);   

  delete lasthdr;
#ifdef _WIN32
  if (m_tmpcopyfn) DeleteFile(m_tmpcopyfn);
#elif defined(QTUI)
  if (m_tmpcopyfn) remove(m_tmpcopyfn);
#endif
  free(m_tmpcopyfn);
}

char *XferRecv::getOutputFileCopy()
{
#ifdef XFER_WIN32_FILEIO
  if (m_houtfile == INVALID_HANDLE_VALUE || !m_validbf) return NULL;
  char *p=m_outfile_fn+strlen(m_outfile_fn);
  while (p >= m_outfile_fn && *p != '/' && *p != '\\') p--;
  p++;
  char tmpbuf[512];
  GetTempPath(512,tmpbuf);
  free(m_tmpcopyfn);
  m_tmpcopyfn=(char*)malloc(strlen(tmpbuf)+1+strlen(p)+1);
  wsprintf(m_tmpcopyfn,"%s\\%s",tmpbuf,p);
  HANDLE h=CreateFile(m_tmpcopyfn,
          GENERIC_WRITE,
          FILE_SHARE_READ|FILE_SHARE_WRITE,
          NULL,
          CREATE_ALWAYS,
          FILE_ATTRIBUTE_NORMAL, NULL);
  if (h == INVALID_HANDLE_VALUE) return NULL;
  int x=0;
  unsigned int old_l, old_h=0;
  old_l=SetFilePointer(m_houtfile,0,(LONG *)&old_h,FILE_CURRENT);
  SetFilePointer(m_houtfile,0,NULL,FILE_BEGIN);
  for (;;)
  {
    if (!IS_VALID(x)) break;
    DWORD d;
    char buf[4096];
    if (!ReadFile(m_houtfile,buf,sizeof(buf),&d,NULL)) break;
    WriteFile(h,buf,d,&d,NULL);
    if (d < sizeof(buf)) break;
  }
  SetFilePointer(m_houtfile,old_l,(LONG*)&old_h,FILE_BEGIN);
  CloseHandle(h);
  
  return m_tmpcopyfn;
#else
  return NULL;
#endif
}

void XferRecv::Abort(C_MessageQueueList *mql) 
{ 
  // send abort message
  request.set_prev_guid(&m_guid);
  request.set_abort(1);
  T_Message m={0,};
  m.data=request.Make();
  m.message_type=MESSAGE_FILE_REQUEST;
  m.message_length=m.data->GetLength();

  mql->send(&m);
  sprintf(m_errbuf,"Aborted @ %d%%",(m_chunk_cnt*100)/m_chunk_total);
  m_err=m_errbuf;
}


// do periodic tasks
int XferRecv::run(C_MessageQueueList *mql)
{
	TAG tag("XferRecv::run()");
  if (m_err||m_done) return 1;

#ifdef XFER_WIN32_FILEIO
  if (m_hstatfile != INVALID_HANDLE_VALUE && lasthdr && time(NULL) > m_next_stateflush_time) // flush state
  {
    m_next_stateflush_time=time(NULL)+60;
    DWORD d;
    SetFilePointer(m_hstatfile,0,NULL,FILE_BEGIN);
    int sig1=PADLOCKSLSTATE_SIG1, sig2=PADLOCKSLSTATE_SIG2;
    WriteFile(m_hstatfile,&sig1,4,&d,NULL);
    WriteFile(m_hstatfile,&sig2,4,&d,NULL);
    WriteFile(m_hstatfile,&m_bytes_total_l,4,&d,NULL);
    WriteFile(m_hstatfile,&m_bytes_total_h,4,&d,NULL);
    WriteFile(m_hstatfile,m_hash,SHA_OUTSIZE,&d,NULL);
    if (m_validbf) 
      WriteFile(m_hstatfile,m_validbf,(m_chunk_total+7)/8,&d,NULL);
    SetEndOfFile(m_hstatfile);
  }
#else
  if (m_statfile && lasthdr && time(NULL) > m_next_stateflush_time) // flush state
  {
    m_next_stateflush_time=time(NULL)+60;
    fseek(m_statfile,0,SEEK_SET);
    int sig1=PADLOCKSLSTATE_SIG1, sig2=PADLOCKSLSTATE_SIG2;
    fwrite(&sig1,1,4,m_statfile);
    fwrite(&sig2,1,4,m_statfile);
    fwrite(&m_bytes_total_l,1,4,m_statfile);
    fwrite(&m_bytes_total_h,1,4,m_statfile);
    fwrite(m_hash,1,SHA_OUTSIZE,m_statfile);
    if (m_validbf) fwrite(m_validbf,1,(m_chunk_total+7)/8,m_statfile);
    fflush(m_statfile);
  }
#endif

  if (chunks_coming<=0 || time(NULL)-last_msg_time > 30)
  {
    unsigned int x;
    // send a new request
    if (chunks_coming <= 0)
    {
      m_adaptive_chunksize += m_adaptive_chunksize/4;
      if (m_adaptive_chunksize>=FILE_MAX_CHUNKS_PER_REQ) m_adaptive_chunksize=FILE_MAX_CHUNKS_PER_REQ;
    }
    else
    {
      m_adaptive_chunksize-=chunks_coming;
      if (m_adaptive_chunksize<4) m_adaptive_chunksize=4;
    }

    chunks_coming=1000000;
    if (lasthdr && m_hasgotchunks)
    {
      request.set_prev_guid(&m_guid);
      m_chunk_cnt=0;
      for (x = 0; x < m_chunk_total; x ++)
      {
        if (IS_VALID(x)) m_chunk_cnt++;
      }
    }
    else
    {
      sprintf(m_errbuf,"Timed out @ %d%%",(m_chunk_cnt*100)/m_chunk_total);
      m_err=m_errbuf;
      return 1; // timeout
    }

    T_Message m={0,};
    request.clear_need_chunks();
    int setfirst=0;
    for (x = m_first_chunkilack; x < m_chunk_total && request.get_chunks_needed() < m_adaptive_chunksize; x ++)
    {
      if (!IS_VALID(x)) 
      {
        request.add_need_chunk(x);
        if (!setfirst) 
        {
          m_first_chunkilack=x;
          setfirst++;
        }
      }
    }

    m.data=request.Make();
    m.message_type=MESSAGE_FILE_REQUEST;
    m.message_length=m.data->GetLength();

    mql->send(&m);
    m_hasgotchunks=0;

    m_guid=m.message_guid;

    last_msg_time=time(NULL)+30; // give it a little more time the first time
  }
  return 0;
}

// process data! :)
void XferRecv::onGotMsg(C_FileSendReply *reply)
{
  TAG tag("XferRecv::onGotMsg()");
  if (m_err||m_done) { delete reply; return; }
  if (reply->get_error())
  {
    m_err=reply->get_error() == 2 ? (char*)"Aborted by remote" : (char*)"File not found";
    delete reply;
    return;
  }
  unsigned int idx=reply->get_index();
  last_msg_time=time(NULL);
  m_hasgotchunks=1;
  if (idx==0xFFFFFFFF) // woohoo header
  {
    delete lasthdr;
    lasthdr=reply;

    if (g_config->ReadInt("directxfers",0))
    {
      int ip=0,prt=0;
      lasthdr->get_dc_ipport(&ip,&prt);
      if (ip && prt)
      {
        NetKern_ConnectToHostIfOK(ip,prt);
      }
    }
    chunks_coming=lasthdr->get_chunkcount();    
    unsigned int fs_h,fs_l;
    lasthdr->get_file_len(&fs_l,&fs_h);
    lasthdr->get_file_dates(&m_create_date,&m_mod_date);
    m_chunk_total=(fs_l+FILE_CHUNKSIZE-1)/FILE_CHUNKSIZE + (fs_h * ((1<<30)/FILE_CHUNKSIZE) * 4);
    if (m_chunk_total<1) m_chunk_total=1;

    unsigned char srv_hash[SHA_OUTSIZE];
    lasthdr->get_hash(srv_hash);
    if (m_bytes_total_l != fs_l || m_bytes_total_h != fs_h || memcmp(m_hash,srv_hash,SHA_OUTSIZE) || !m_validbf)
    {
      memcpy(m_hash,srv_hash,SHA_OUTSIZE);
      m_bytes_total_l=fs_l;
      m_bytes_total_h=fs_h;


#ifdef _WIN32
      char s[32];
      FormatSizeStr64(s,fs_l,fs_h);
      int idx=g_lvrecv.FindItemByParam((int)this);
      if (idx!=-1) g_lvrecv.SetItemText(idx,1,s);
#elif defined(QTUI)
      char s[32];
      FormatSizeStr64(s,fs_l,fs_h);
      int idx=g_lvrecv.FindItemByParam((int)this);
      if (idx!=-1) g_lvrecv.SetItemText(idx,1,s);
#endif
      
      m_chunk_cnt=0;
      m_total_chunks_recvd=0;
      m_chunk_startcnt=0;
      free(m_validbf);
      m_validbf=(unsigned char *)calloc(1,(m_chunk_total+7)/8);
    }

    m_total_chunks_recvd+=chunks_coming;
  }
  else if (lasthdr)
  {
    if (idx >= m_chunk_total)
    {
      m_err="idx out of range";
      debug_printf("xfer_recv: idx out of range (%d, top is %d)\n",idx,m_chunk_total);
      delete reply;
      return;
    }

    unsigned int newpos_l=FILE_CHUNKSIZE*idx;
#ifdef _WIN32
    unsigned int newpos_h=(unsigned int) (((__int64)FILE_CHUNKSIZE*(__int64)idx)>>32);
#else
    unsigned int newpos_h=0;//fucko bsd 64 bit fn
#endif

    if (m_outfile_lastpos_l != newpos_l || m_outfile_lastpos_h != newpos_h)
    {
      m_outfile_lastpos_l=newpos_l;
      m_outfile_lastpos_h=newpos_h;
#ifdef XFER_WIN32_FILEIO
      LONG zero=m_outfile_lastpos_h;
      if (SetFilePointer(m_houtfile,m_outfile_lastpos_l,&zero,FILE_BEGIN) == 0xFFFFFFFF && 
        GetLastError() != NO_ERROR)
#else
      if (fseek(m_outfile,m_outfile_lastpos_l,SEEK_SET))
#endif
      {
        m_err="error seeking";
        delete reply;
        return;
      }
    }

    if (idx < m_chunk_total-1 && reply->get_data_len() != FILE_CHUNKSIZE)
    {
      debug_printf("xfer_recv: chunk %d, got size of %d and should have been %d (was valid=%d)\n",
        idx,reply->get_data_len(),FILE_CHUNKSIZE,IS_VALID(idx));
    }

    int n=0;
    if (reply->get_data_len()) 
#ifdef XFER_WIN32_FILEIO
    {
      DWORD d;
      if (!WriteFile(m_houtfile,reply->get_data(),reply->get_data_len(),&d,NULL))
        d=0;
      n=d;
    }
#else
      n=fwrite(reply->get_data(),1,reply->get_data_len(),m_outfile);
#endif

    if (n != reply->get_data_len())
    {
      m_err="error writing";
      delete reply;
      return;
    }
#ifndef XFER_WIN32_FILEIO
    fflush(m_outfile);
#endif
    unsigned int o=m_outfile_lastpos_l;
    m_outfile_lastpos_l+=n;
    if (m_outfile_lastpos_l < o) m_outfile_lastpos_h++;

    if (!IS_VALID(idx))
    {
      SET_VALID(idx);
      m_chunk_cnt++;
    }
    chunks_coming--;
    delete reply;

    char s[128];

    if (GetTickCount() >= m_next_cpstime)
    {
      m_next_cpstime+=CPS_WINDOWLEN;
      if (m_next_cpstime < GetTickCount()) m_next_cpstime=GetTickCount()+CPS_WINDOWLEN;
      m_cps_blks[m_cps_blks_pos]=(m_chunk_cnt-m_chunk_startcnt);
      m_cps_blks_pos++;
      m_cps_blks_pos%=CPS_WINDOWSIZE;
      unsigned int tm=CPS_WINDOWSIZE*CPS_WINDOWLEN;
#ifdef _WIN32
      m_last_cps=MulDiv(m_chunk_cnt-m_chunk_startcnt - m_cps_blks[m_cps_blks_pos],1000*FILE_CHUNKSIZE,tm);
#else
      m_last_cps=((m_chunk_cnt-m_chunk_startcnt - m_cps_blks[m_cps_blks_pos])*FILE_CHUNKSIZE)/(tm/1000);
#endif
    }
    int cps=m_last_cps;

    if (m_chunk_cnt >= m_chunk_total)
    {
      m_chunk_cnt=0;
      unsigned int x;
      for (x = 0; x < m_chunk_total; x ++) if (IS_VALID(x)) m_chunk_cnt++;
      if (m_chunk_cnt >= m_chunk_total) // done!
      {
        if (lasthdr)
        {
          unsigned char zerohash[SHA_OUTSIZE]={0,};
          unsigned char hash[SHA_OUTSIZE],hash2[SHA_OUTSIZE];
          unsigned int l_h,l_l;
          lasthdr->get_file_len(&l_l,&l_h);
          lasthdr->get_hash(hash);
          if (memcmp(zerohash,hash,SHA_OUTSIZE) && !l_h)
          {
#ifdef XFER_WIN32_FILEIO
            SetFilePointer(m_houtfile,0,NULL,FILE_BEGIN);
#else
            fseek(m_outfile,0,SEEK_SET);
#endif
            SHAify context;
            unsigned int l=l_l;

            while (l>0)
            {
              unsigned char buf[8192];
              unsigned int a=sizeof(buf);
              if (a > l) a=l;
#ifdef XFER_WIN32_FILEIO
              DWORD d;
              if (!ReadFile(m_houtfile,buf,sizeof(buf),&d,NULL)) d=0;
              a=d;
#else
              a=fread(buf,1,a,m_outfile);
#endif
              if (!a) break;
              context.add(buf,a);
              l-=a;
            }
            context.final(hash2);
            if (l || memcmp(hash,hash2,SHA_OUTSIZE))
            {
              m_err="SHA mismatch";
              m_done=1;
              request.set_prev_guid(&m_guid);
              request.set_abort(2);
              T_Message m={0,};
              m.data=request.Make();
              m.message_type=MESSAGE_FILE_REQUEST;
              m.message_length=m.data->GetLength();
              g_mql->send(&m);
              return;
            }
          }
        }
        sprintf(s,"Completed @ %d.%02dk/s",
          cps/1000,(cps/10)%100);
        if (m_total_chunks_recvd > m_chunk_cnt && m_chunk_cnt)
        {
          int p=(m_total_chunks_recvd-m_chunk_cnt)*100/m_chunk_cnt;
          if (!p) sprintf(s+strlen(s)," 0.%d%% padlocksl",((m_total_chunks_recvd-m_chunk_cnt)*1000/m_chunk_cnt)%10);
          else sprintf(s+strlen(s)," %d%% padlocksl",p);
        }
#ifdef _WIN32
        int idx=g_lvrecv.FindItemByParam((int)this);
        if (idx!=-1)
        {
          g_lvrecv.SetItemText(idx,2,s);
          g_lvrecv.SetItemText(idx,3,"");
          g_lvrecv.SetItemText(idx,0,m_outfile_fn+m_outfile_fn_ll);
        }
#elif defined(QTUI)
        int idx=g_lvrecv.FindItemByParam((int)this);
        if (idx!=-1)
        {
          g_lvrecv.SetItemText(idx,2,s);
          g_lvrecv.SetItemText(idx,3,"");
          g_lvrecv.SetItemText(idx,0,m_outfile_fn+m_outfile_fn_ll);
        }
#endif
        m_done=1;
        request.set_prev_guid(&m_guid);
        request.set_abort(2);
        T_Message m={0,};
        m.data=request.Make();
        m.message_type=MESSAGE_FILE_REQUEST;
        m.message_length=m.data->GetLength();
        g_mql->send(&m);
      }
    }
    if (!m_done)
    {
      int left;
      if (!cps) left=0;
#ifdef _WIN32
      else left=MulDiv(m_chunk_total-m_chunk_cnt,FILE_CHUNKSIZE,cps);
#else
      else left=((m_chunk_total-m_chunk_cnt)*FILE_CHUNKSIZE)/cps;
#endif
      if (left<0)left=0;

      sprintf(s,"%d%%@%d.%02dk/s [%d:%02d ETA]",
        (m_chunk_cnt*100)/m_chunk_total,
        cps/1000,(cps/10)%100,
        left/60/60,(left/60)%60
        );

      if (g_extrainf)
        sprintf(s+strlen(s)," [%d/%d/%d/%d]",
        m_chunk_cnt,m_total_chunks_recvd,m_chunk_total,m_adaptive_chunksize);

#ifdef _WIN32
      int idx=g_lvrecv.FindItemByParam((int)this);
      if (idx!=-1) g_lvrecv.SetItemText(idx,2,s);
#elif defined(QTUI)
      int idx=g_lvrecv.FindItemByParam((int)this);
      if (idx!=-1) g_lvrecv.SetItemText(idx,2,s);
#endif
    }
  }
  else
  {
    m_err="data with no header";
    debug_printf("xfer_recv: got %s\n",m_err);
    delete reply;
  }
}

