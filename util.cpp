/*
    WASTE - util.cpp (General utility code)
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
/*
 * This file implement several functions with C5P RNG support, 
 * and implement functions read and write profile value with config file.
 */

#include "main.h"
#include "netkern.h"
#include "util.h"
#include "rsa/md5.h"

#include "config.h"
#ifdef QTUI
#include "prefinterface.h"
#endif

extern "C" {
#include "rsa/r_random.h"
#include "C5P.h"
};

#if defined(_WIN32) && !defined(QTUI)
#include "resource.h"
#endif

////// guid


void CreateID128(T_GUID *id)
{
  R_GenerateBytes((unsigned char *)id->idc, 16, &g_random);
}

void MakeID128Str(T_GUID *id, char *str)
{
  int x;
  str[0]=0;
  for (x = 0; x < 16; x ++)
  {
    sprintf(str+strlen(str),"%02X",id->idc[x]); 
  }
} 

int MakeID128FromStr(char *str, T_GUID *id)
{
  int x;
  char *p=str;
  for (x = 0; x < 16; x ++)
  {
    int h=*p++;
    int l=*p++;
    if (l >= '0' && l <= '9') l-='0';
    else if (l >= 'A' && l <= 'F') l -= 'A'-10;
    else return 1;
    if (h >= '0' && h <= '9') h-='0';
    else if (h >= 'A' && h <= 'F') h -= 'A'-10;
    else return 1;
    id->idc[x]=l|(h<<4);
  }
  return 0;
}





/// rng

R_RANDOM_STRUCT g_random;

#if defined(_WIN32) && !defined(QTUI)
static WNDPROC rng_oldWndProc;
static unsigned int rng_movebuf[7];
static int rng_movebuf_cnt;

static BOOL CALLBACK rng_newWndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,LPARAM lParam)
{
  /* Modified by LoraYin 2003/11/27 being. Add C5P RNG support.   */
  //Use c5p RNG if it is available.
  debug_printf("rng_newWndProc() check if C5P RNG available()\n");
  if (c5p_rng_available())
  {
    unsigned int bytesNeeded = 32;
    //128-bit boundary address aligned. A little more bytes than acturally needed.
    unsigned char *buf = (unsigned char*)c5p_malloc(40); 
    debug_printf("rng_newWndProc() use C5P RNG to generate random numbers.\n");
    while (bytesNeeded) 
    {
      RNG_Result result = c5p_rng_rand(32, buf);
      if (result == e_RNG_Successed)
      {
        R_RandomUpdate(&g_random,buf,32);
        R_GetRandomBytesNeeded(&bytesNeeded, &g_random);
      }
      else
        debug_printf("Error: rng_newWndProc() c5p_rng_rand() return result %d!!! \n",result);
    }
    c5p_free(buf);	
    if (bytesNeeded<1) EndDialog(hwndDlg,1);
  }
  else
  //Use original RNG generation.
  /* Modified by LoraYin 2003/11/27 end.    */
	
  if (uMsg == WM_MOUSEMOVE)
  {
    rng_movebuf[rng_movebuf_cnt%7]+=lParam;
    rng_movebuf[(rng_movebuf_cnt+1)%7]+=GetTickCount();
    rng_movebuf[(rng_movebuf_cnt+2)%7]+=GetMessageTime()+GetMessagePos();
    if (++rng_movebuf_cnt >= 53)
    {
      rng_movebuf_cnt=0;
      R_RandomUpdate(&g_random,(unsigned char *)rng_movebuf,sizeof(rng_movebuf));

      unsigned int bytesNeeded;
      R_GetRandomBytesNeeded(&bytesNeeded, &g_random);
      SendDlgItemMessage(hwndDlg,IDC_PROGRESS1,PBM_SETPOS,(WPARAM)(64-bytesNeeded/4),0);
      if (bytesNeeded<1) EndDialog(hwndDlg,1);
    }
  }
  return CallWindowProc(rng_oldWndProc,hwndDlg,uMsg,wParam,lParam);
}


static BOOL WINAPI RndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_INITDIALOG)
  {
    SetWindowText(hwndDlg,APP_NAME " Random Number Generator Initialization");
    ShowWindow(GetDlgItem(hwndDlg,IDC_PROGRESS1),SW_SHOWNA);
    SendDlgItemMessage(hwndDlg,IDC_PROGRESS1,PBM_SETRANGE,0,MAKELPARAM(0,64));
    rng_oldWndProc=(WNDPROC) SetWindowLong(hwndDlg,GWL_WNDPROC,(LONG)rng_newWndProc);
  }
  return 0;
}

#endif//WIN32

static int last_srtime;

void MYSRANDUPDATE(unsigned char *buf, int bufl)
{
  MD5_CTX m;
  int x;
  int t=GetTickCount()-last_srtime;
  last_srtime=GetTickCount();
  for (x = 0; x < 16; x ++) 
  {
    g_random.state[x+16]+=(unsigned char) (t+x*7+g_random.state[x]);
    if (bufl > 0)
    {
      g_random.state[x+16]^=*buf++; 
      bufl--;
    }
  }
  MD5Init(&m);
  MD5Update(&m,(unsigned char *)g_random.state,32);
  MD5Final(g_random.state+16,&m);
}

void MYSRAND()
{
  R_RandomInit(&g_random);
#if defined(_WIN32) && !defined(QTUI)
  if (!GetPrivateProfileStruct("config","rngseedn",g_random.state,16,g_config_mainini))
  {
    DialogBox(g_hInst,MAKEINTRESOURCE(IDD_RNGINIT),NULL,RndProc);
    rng_movebuf_cnt=0;
    memset(rng_movebuf,0,sizeof(rng_movebuf));
  }
  else 
  {
    g_random.outputAvailable=0;
    g_random.bytesNeeded=0;
  }
#else //WIN32

	
  /* Modified by LoraYin 2003/12/5 being. Add C5P RNG support.    */	
  
  RNG_Result result = e_RNG_Failed;
  if (c5p_rng_available())
  {
    //128-bit boundary address aligned. A little more bytes than acturally needed.
    unsigned char *buf = (unsigned char*)c5p_malloc(32 + 8);
    //printf("MYSRAND() use C5P RNG to generate 32 random bytes()\n");  	
    result = c5p_rng_rand(32, buf);
    if (result == e_RNG_Successed)
    {
      memcpy(g_random.state, buf, 32);
      g_random.outputAvailable=0;
      g_random.bytesNeeded=0;
    }
    c5p_free(buf);
  }
  if (result != e_RNG_Successed)
  {
#ifdef _LINUX
    debug_printf("MYSRAND() use /dev/urandom to generate random seeds.\n");
  

    FILE *fp=fopen("/dev/urandom","rb");
    if (fp)
    {
      fread(g_random.state,32,1,fp);
      fclose(fp);
    }
#else //WIN32
    debug_printf("MYSRAND() use QueryPerformanceCounter() to generate random seeds. \n");
    unsigned __int64 pentium_tcs[1];
    int i;
    for (i = 0; i < 8; i ++)
    {  
      QueryPerformanceCounter((LARGE_INTEGER*)pentium_tcs);
      memcpy(g_random.state + 4 * i, pentium_tcs, 4);
    }
#endif //!LINUX
    g_random.outputAvailable=0;
    g_random.bytesNeeded=0;
  }
  /* Modified by LoraYin 2003/12/15 end.    */	
  
#endif//!(WIN32 & !QTUI)

  MYSRANDUPDATE();

  MD5_CTX m;
  MD5Init(&m);
  MD5Update(&m,(unsigned char *)g_random.state,16);
  unsigned char tmp[16];
  MD5Final(tmp,&m);
#if defined(_WIN32) && !defined(QTUI)
  WritePrivateProfileStruct("config","rngseedn",tmp,16,g_config_mainini);
#endif
}

//// misc string shit


char *extension(char *fn) 
{
  char *s = fn+strlen(fn);
  while (s > fn && *s != '.' 
#ifdef _WIN32
    && *s != '\\' 
#endif
    && *s != '/') s=CharPrev(fn,s);
  if (s == fn || *s != '.') return "";
  return (s+1);
}

void removeInvalidFNChars(char *filename)
{
  char *p=filename;
  while (*p)
  {
    if (*p == '?' || *p == '*' || *p == '>' || *p == '<' || *p == '|' || *p == ':') *p='_';
    p=CharNext(p);
  }
}


#if defined(_WIN32) && !defined(QTUI)
//// windows shit

int toolwnd_state;
int systray_state;

BOOL systray_add(HWND hwnd, HICON icon)
{
  NOTIFYICONDATA tnid;
  systray_state=1;
  tnid.cbSize = sizeof(NOTIFYICONDATA);
  tnid.hWnd = hwnd;
  tnid.uID = 232;
  tnid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
  tnid.uCallbackMessage = WM_USER_SYSTRAY;
  tnid.hIcon = icon;
  GetWindowText(g_mainwnd,tnid.szTip,sizeof(tnid.szTip));
  return (Shell_NotifyIcon(NIM_ADD, &tnid));
}

BOOL systray_del(HWND hwnd) {
  NOTIFYICONDATA tnid;
  systray_state=0;
  tnid.cbSize = sizeof(NOTIFYICONDATA);
  tnid.hWnd = hwnd;
  tnid.uID = 232;
  return(Shell_NotifyIcon(NIM_DELETE, &tnid));
}

// BSC Comment for compat
typedef struct {
   UINT  cbSize;
   HWND  hwnd;
   DWORD dwFlags;
   UINT  uCount;
   DWORD dwTimeout;
 } FLASHWINFO, *PFLASHWINFO;
 
 #define FLASHW_TRAY 2
 #define FLASHW_TIMERNOFG 12
static int (__stdcall *fflashWindowEx)(FLASHWINFO *);

void DoFlashWindow(HWND hwndParent, int timeoutval)
{
  static HMODULE hUser32=NULL;
  if (!hUser32)
  {
    hUser32=LoadLibrary("USER32.DLL");
    *((void**)&fflashWindowEx)=GetProcAddress(hUser32,"FlashWindowEx");
  }

  if (fflashWindowEx && GetForegroundWindow() != hwndParent && GetParent(GetForegroundWindow()) != hwndParent)
  {
    FLASHWINFO fi;
    fi.cbSize=sizeof(fi);
    fi.hwnd=hwndParent;
    fi.dwFlags=FLASHW_TRAY|FLASHW_TIMERNOFG;
    fi.uCount=timeoutval;
    fi.dwTimeout=0;
    fflashWindowEx(&fi);
  }
} 


HWND CreateTooltip(HWND hWnd, LPSTR strTT)
{
    HWND hWndTT;                 // handle to the ToolTip control
    TOOLINFO ti;
    unsigned int uid = 0;       // for ti initialization
    LPTSTR lptstr = strTT;
    RECT rect;                  // for client area coordinates

	
    /* CREATE A TOOLTIP WINDOW */
    hWndTT = CreateWindowEx(WS_EX_TOPMOST,
        TOOLTIPS_CLASS,
        NULL,
        WS_POPUP | TTS_NOPREFIX,		
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hWnd,
        NULL,
        g_hInst,
        NULL
        );

    SetWindowPos(hWndTT,
        HWND_TOPMOST,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    /* GET COORDINATES OF THE MAIN CLIENT AREA */
    GetClientRect (hWnd, &rect);
	
    /* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hWnd;
    ti.hinst = g_hInst;
    ti.uId = uid;
    ti.lpszText = lptstr;
        // ToolTip control will cover the whole window
    ti.rect.left = rect.left;    
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;
    
    /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
    SendMessage(hWndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
    return hWndTT;
}

void toolWindowSet(int twstate)
{
  if (!toolwnd_state != !twstate)
  {
    toolwnd_state=twstate;
    int a=0;
    if (IsWindowVisible(g_mainwnd))
    {
      ShowWindow(g_mainwnd,SW_HIDE);
      a++;
    }
    if (toolwnd_state)
    {
		  SetWindowLong(g_mainwnd,GWL_EXSTYLE,(GetWindowLong(g_mainwnd,GWL_EXSTYLE)|WS_EX_TOOLWINDOW)&~WS_EX_APPWINDOW);
      //SetWindowLong(g_mainwnd,GWL_STYLE,GetWindowLong(g_mainwnd,GWL_STYLE)(WS_MINIMIZEBOX|WS_SYSMENU));
    }
	  else 
    {
		  SetWindowLong(g_mainwnd,GWL_EXSTYLE,GetWindowLong(g_mainwnd,GWL_EXSTYLE)&~WS_EX_TOOLWINDOW);
		  //SetWindowLong(g_mainwnd,GWL_STYLE,GetWindowLong(g_mainwnd,GWL_STYLE)|(WS_MINIMIZEBOX|WS_SYSMENU));
    }

    if (a)
    {
      ShowWindow(g_mainwnd,SW_SHOWNA);
      SetWindowPos(g_mainwnd,NULL,0,0,0,0,SWP_NOMOVE|SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE|SWP_DRAWFRAME);
    }
  }
}
#elif defined(QTUI)
void DoFlashWindow(QWidget* hwndParent, int timeoutval)
{
	qDebug("TODO: DoFlashWindow");
}

#endif //WIN32



/// access control shit

ACitem *g_aclist;
int g_aclist_size;

int allowIP(unsigned int a)
{
  if (!g_use_accesslist) return 1;

  ACitem *p=g_aclist;
  int x;
  if (!p) return 1;
  a=ntohl(a);
  for (x = 0; x < g_aclist_size; x ++)
  {
    unsigned int mask=~(0xFFFFFFFF >> p->maskbits);
//    debug_printf("comparing %08X to %08X, mask=%08X\n",a,p->ip,mask);
    if ((a&mask) == (p->ip&mask))
    {
  //    debug_printf("allow=%d\n",p->allow);
      return p->allow;
    }
    p++;
  }
  return 1;
}

int ACStringToStruct(char *t, ACitem *i)
{
  char buf[64];
  safe_strncpy(buf,t,64);
  t=buf;
  if (*t == 'A') i->allow=1;
  else if (*t == 'D') i->allow=0;
  else return 0;
  t++;
  char *p=strstr(t,"/");
  if (!p || !p[1]) return 0;
  i->maskbits=atoi(++p);
  if (*p < '0' || *p > '9') return 0;

  p[-1]=0;
  i->ip=inet_addr(t);
  i->ip=ntohl(i->ip);
//  debug_printf("converted %s to %d:%08X/%d\n",t-1,i->allow,i->ip,i->maskbits);
  return 1;
}

#if defined(_WIN32) && !defined(QTUI)
void updateACList(W_ListView *lv)
#else
void updateACList()
#endif
{
  free(g_aclist);
#if defined(_WIN32) && !defined(QTUI)
  if (lv)
  {
    g_aclist_size=lv->GetCount();
    g_aclist=(ACitem *)malloc(sizeof(ACitem)*g_aclist_size);
    g_config->WriteInt("ac_cnt",g_aclist_size);
    int x;
    int a=0;
    for (x = 0; x < g_aclist_size; x ++)
    {
      char b[512];
      lv->GetText(x,0,b,sizeof(b));
      lv->GetText(x,1,b+1,sizeof(b)-1);
      char nstr[32];
      sprintf(nstr,"ac_%d",x);
      g_config->WriteString(nstr,b);
      if (ACStringToStruct(b,g_aclist+a)) a++;
    }
    g_aclist_size=a;
  }
  else // read it from config
#endif//WIN32
  {
    g_aclist_size=g_config->ReadInt("ac_cnt",0);
    g_aclist=(ACitem *)malloc(sizeof(ACitem)*g_aclist_size);
    int x; 
    int a=0;
    for (x = 0; x < g_aclist_size; x ++)
    {
      char buf[64];
      sprintf(buf,"ac_%d",x);
      char *t=g_config->ReadString(buf,"");
      if (ACStringToStruct(t,g_aclist+a)) a++;
    }
    g_aclist_size=a;
  }
}





//// key shit

#if defined(_WIN32) && !defined(QTUI)

static char tmp_passbuf[256];

BOOL WINAPI PassWordProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_INITDIALOG:
      {
        SetWindowText(hwndDlg,APP_NAME " : password");
        char buf[4096];
        sprintf(buf,"Password:");

        SetDlgItemText(hwndDlg,IDC_LABEL,buf);
      }
      memset(tmp_passbuf,0,256);
      if (g_config->ReadInt("storepass",0))
      {
        if (g_config->ReadString("keypass",NULL))
          safe_strncpy(tmp_passbuf,g_config->ReadString("keypass",NULL),256);
        CheckDlgButton(hwndDlg,IDC_CHECK1,BST_CHECKED);
      }
      SetDlgItemText(hwndDlg,IDC_EDIT1,tmp_passbuf);
      memset(tmp_passbuf,0,256);
    return 0;
    case WM_CLOSE:
      wParam=IDOK;
    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK) 
      {
        GetDlgItemText(hwndDlg,IDC_EDIT1,tmp_passbuf,255);
        tmp_passbuf[255]=0;
        if (IsDlgButtonChecked(hwndDlg,IDC_CHECK1))
        {
          g_config->WriteInt("storepass",1);
          g_config->WriteString("keypass",tmp_passbuf);
        }
        else
        {
          g_config->WriteInt("storepass",0);
          g_config->WriteString("keypass","");
        }
        EndDialog(hwndDlg,0);
      }
      if (LOWORD(wParam) == IDCANCEL)
      {
        EndDialog(hwndDlg,1);
      }
    return 0;
  }
  return 0;
}

#endif

static int readEncodedChar(FILE *in) // returns -1 on error
{
  char buf[2];
  int ret;
  do buf[0]=fgetc(in);
  while (buf[0] == '\n' || 
         buf[0] == '\r' || 
         buf[0] == '\t' || 
         buf[0] == ' ' || 
         buf[0] == '-' ||
         buf[0] == '_' ||
         buf[0] == '>' ||
         buf[0] == '<' ||
         buf[0] == ':' ||
         buf[0] == '|'
         );
  buf[1]=fgetc(in);

  if (buf[0] >= '0' && buf[0] <= '9') ret=(buf[0]-'0')<<4;
  else if (buf[0] >= 'A' && buf[0] <= 'F') ret=(buf[0]-'A'+10)<<4;
  else return -1;

  if (buf[1] >= '0' && buf[1] <= '9') ret+=(buf[1]-'0');
  else if (buf[1] >= 'A' && buf[1] <= 'F') ret+=(buf[1]-'A'+10);
  else return -1;

  return ret;
}

static int readBFdata(FILE *in, int *key, int keylen,void *data, unsigned int len, unsigned long *cbc)
{
  unsigned int x;
  int length = (len+15)&~15;
  int* aes_buf = (int*)c5p_malloc(length);
  memset((char *)aes_buf,0,length);
  AES_Result result = e_AES_Failed;
  for (x = 0; x < length; x++)
  {
    int c=readEncodedChar(in);
    if (c<0) return 1;
    ((unsigned char *)aes_buf)[x]=(unsigned char)c;
  }
  
  if (c5p_aes_available())
  {
     // memcpy((unsigned char *)aes_buf,(unsigned char *)data,len);
    result = c5p_aes_decrypt(key,
                               keylen,
                               e_aes_ecb,
                               aes_buf,
                               length,
                               aes_buf,
                               NULL);
  }
  if (result != e_AES_Successed)
  {
    // memcpy((unsigned char *)aes_buf,(unsigned char *)data,len);
    sft_aes_decrypt(key,
                      keylen,
                      e_aes_ecb,
                      aes_buf,
                      length,
                      aes_buf,
                      NULL);
	  
  }
  memcpy((unsigned char *)data,(unsigned char *)aes_buf,len);
  c5p_free(aes_buf);

  return 0;

}


#if defined(_WIN32) && !defined(QTUI)
int doLoadKey(HWND hwndParent, char *pstr, char *keyfn, R_RSA_PRIVATE_KEY *key)
#else
int doLoadKey(char *pstr, char *keyfn, R_RSA_PRIVATE_KEY *key)
#endif
{
  int goagain=0;
  SHAify m;
  m.add((unsigned char *)pstr,strlen(pstr));
  unsigned char tmp[SHA_OUTSIZE];
  m.final(tmp);

  unsigned char tmpkey[16];
  memcpy(tmpkey,tmp,16);

  FILE *fp;
  if (!(fp=fopen(keyfn,"rt")))
  {
    key->bits=0;
#if defined(_WIN32) && !defined(QTUI)
    MessageBox(hwndParent,"Error loading private key",APP_NAME " Error",MB_OK|MB_ICONSTOP);
#else
#ifdef QTUI
    warnPassword("Error loading private key");
#else
    printf(APP_NAME " Error: Error loading private key: %s\n", keyfn);
#endif

#endif
    return 1;
  }

  char *err=NULL;
  char linebuf[1024];
  while (!err)
  {
    fgets(linebuf,1023,fp);
    linebuf[1023]=0;
    if (feof(fp) || !linebuf[0]) err="No private key found in file";
    if (!strncmp(linebuf,"PADLOCKSL_PRIVATE_KEY ",strlen("PADLOCKSL_PRIVATE_KEY "))) break;
    if (!strncmp(linebuf,"JSM_PRIVATE_KEY ",strlen("JSM_PRIVATE_KEY "))) break;
    if (!strncmp(linebuf,"VIASecIM_PRIVATE_KEY ",strlen("VIASecIM_PRIVATE_KEY "))) break;
  }

  unsigned char tl[8]={0,};
  if (!err)
  {
    char *p=strstr(linebuf," ");
    while (p && *p == ' ') p++;
    if (p && atoi(p) >= 10 && atoi(p) < 20)
    {
      p=strstr(p," ");
      while (p && *p == ' ') p++;
      if (p && (key->bits=atoi(p)) <= MAX_RSA_MODULUS_BITS && key->bits >= MIN_RSA_MODULUS_BITS)
      {
        int x;
        for (x = 0; x < 8 && !err; x ++)
        {
          int c=readEncodedChar(fp);
          if (c < 0) err="Private key corrupt";
          else
          {
            tl[x]=c;
//            tl[x/4]|=c<<((x&3)*8);
          }
        }
        if (!err)
        {
          char buf[16];
          if (readBFdata(fp,(int *)tmpkey,16,buf,16,(unsigned long *)tl)) err="Private key corrupt";
          else if (memcmp(buf,"PADLOCKSLPASSWORD",16)) 
          {
            goagain++;
            err="Invalid password for private key";
          }
          else
          {
            int a=0;
          #define WPK(x) if (!readBFdata(fp,(int *)tmpkey,16,g_key.x,sizeof(g_key.x),(unsigned long *)tl))
            WPK(modulus)
            WPK(publicExponent)
            WPK(exponent)
            WPK(prime)
            WPK(primeExponent)
            WPK(coefficient)
            a++;
          #undef WPK
            if (!a) err="Private key corrupt";
          }
        }
        // read key now
      }
      else
        err="Private key found but size incorrect";
    }
    else err="Private key found but version incorrect";
  }
  fclose(fp);

  if (err)
  {
    key->bits=0;
#if defined(_WIN32) && !defined(QTUI)
    MessageBox(hwndParent,err,APP_NAME " Error",MB_OK|MB_ICONSTOP);
#else
#ifdef QTUI
    warnPassword(err);
#else

    printf(APP_NAME " Error: %s\n",err);

#endif

#endif
    return goagain?2:1;
  }
  return 0;
}


#if defined(_WIN32) && !defined(QTUI)
void reloadKey(char *passstr, HWND hwndParent)
#else
void reloadKey(char *passstr)
#endif
{
#if !defined(_WIN32) || defined(QTUI)
  char tmp_passbuf[256];
#endif

  char keyfn[1024];
  sprintf(keyfn,"%s.pr4",g_config_prefix);

#if defined(_WIN32) && !defined(QTUI)
  if (!passstr)
  {
giveitanothergo:
    if (DialogBox(g_hInst,MAKEINTRESOURCE(IDD_PASSWD),hwndParent,PassWordProc))
    {
      g_key.bits=0;
      return;
    }
    passstr=tmp_passbuf;
  }
#else

#ifdef QTUI
  if (!passstr)
  {
giveitanothergo:
    //printf("\nPassword: ");
    if(!getPassword(tmp_passbuf))
    {
      g_key.bits=0;
      return;
    }
    if (tmp_passbuf[0] && tmp_passbuf[strlen(tmp_passbuf)-1] == '\n')
       tmp_passbuf[strlen(tmp_passbuf)-1]=0;
    passstr=tmp_passbuf;
  }
#else

  if (!passstr)
  {
giveitanothergo:
    printf("\nPassword: ");
    fgets(tmp_passbuf,sizeof(tmp_passbuf),stdin);
    if (tmp_passbuf[0] && tmp_passbuf[strlen(tmp_passbuf)-1] == '\n')
       tmp_passbuf[strlen(tmp_passbuf)-1]=0;

    passstr=tmp_passbuf;
  }

#endif

#endif

  SHAify m;
  m.add((unsigned char *)passstr,strlen(passstr));
  unsigned char tmp[SHA_OUTSIZE];
  m.final(tmp);



#if defined(_WIN32) && !defined(QTUI)
  int ret=doLoadKey(hwndParent,passstr,keyfn,&g_key);
#else
  int ret=doLoadKey(passstr,keyfn,&g_key);
#endif
  if (ret)
  {
    if (ret == 2) goto giveitanothergo;
    return;
  }
  
  m.reset();

  m.add((unsigned char *)g_key.modulus,MAX_RSA_MODULUS_LEN);
  m.add((unsigned char *)g_key.publicExponent,MAX_RSA_MODULUS_LEN);
  m.final(g_pubkeyhash);

#if 0
  int x;
  for (x=0; x < g_new_net.GetSize(); x ++)
  {
    g_new_net.Get(x)->close(1);
  }
  if (g_mql) for (x = 0; x < g_mql->GetNumQueues(); x ++)
  {
    g_mql->GetQueue(x)->get_con()->close(1);
  }
#endif
}

C_ItemList<PKitem> g_pklist,g_pklist_pending;

int loadPKList(char *fn)
{
  int err=0;
  int cnt=0;
  char str[1024+8];
  if (!fn) sprintf(str,"%s.pr3",g_config_prefix);

  FILE *fp=fopen(fn?fn:str,"rt");

  if (fp) while (!err)
  {
    char linebuf[1024];
    char *lineptr;
    PKitem item;
    memset(&item,0,sizeof(PKitem));

    int pending=0;
    while (!err)
    {
      lineptr=linebuf;
      fgets(lineptr,1023,fp);
      lineptr[1023]=0;
      if (feof(fp) || !lineptr[0]) err++;
      if (!strncmp(lineptr,"JSM_PENDING ",12) || !strncmp(lineptr,"PADLOCKSL_PENDING ",14))
      {
        pending=1;
        lineptr+=(*lineptr == 'W')?14:12;
      }
      else pending=0;

      if (!strncmp(lineptr,"JSM_PUBLIC_KEY ",strlen("JSM_PUBLIC_KEY "))) break;
      if (!strncmp(lineptr,"PADLOCKSL_PUBLIC_KEY ",strlen("PADLOCKSL_PUBLIC_KEY "))) break;
      if (!strncmp(lineptr,"VIASecIM_PUBLIC_KEY ",strlen("VIASecIM_PUBLIC_KEY "))) break;
    }

    if (!err)
    {
      char *p=strstr(lineptr," ");
      while (p && *p == ' ') p++;
      if (p && atoi(p) >= 10 && atoi(p) <= 20)
      {
        int newkeyfmt=0;
        if (atoi(p) >= 20) newkeyfmt++;
        p=strstr(p," ");
        while (p && *p == ' ') p++;
        if (p && (item.pk.bits=atoi(p)) <= MAX_RSA_MODULUS_BITS && item.pk.bits >= MIN_RSA_MODULUS_BITS)
        {
          while (*p && *p != ' ') p++;
          while (*p == ' ') p++;
          // p is name\n now :)
          while (*p && (p[strlen(p)-1] == '\n' || p[strlen(p)-1] == '\r' ||
                        p[strlen(p)-1] == '\t' || p[strlen(p)-1] == ' ')) p[strlen(p)-1]=0;

          safe_strncpy(item.name,p,sizeof(item.name));

          int x;
          int ks=(item.pk.bits+7)/8;
          for (x = 0; x < ks && !err; x ++)
          {
            int c=readEncodedChar(fp);
            if (c<0) err++;
            item.pk.modulus[MAX_RSA_MODULUS_LEN-ks+x]=c;
          }
          if (newkeyfmt)
          {
            int a=readEncodedChar(fp);
            if (a<0) err++;
            else
            {
              int b=readEncodedChar(fp);
              if (b < 0) err++;
              else
              {
                int expsize=(a<<8)|b;
                if (expsize < 1 || expsize > ks)
                  err++;
                else ks=expsize;
              }
            }
          }
          for (x = 0; x < ks && !err; x ++)
          {
            int c=readEncodedChar(fp);
            if (c<0) err++;
            item.pk.exponent[MAX_RSA_MODULUS_LEN-ks+x]=c;
          }
        }
        else err++;
      }
      else err++;
    }
  
    if (err) break;

    SHAify m;
    m.add((unsigned char *)item.pk.modulus,MAX_RSA_MODULUS_LEN);
    m.add((unsigned char *)item.pk.exponent,MAX_RSA_MODULUS_LEN);
    m.final(item.hash);

    PKitem *p=(PKitem *)malloc(sizeof(PKitem));
    memcpy(p,&item,sizeof(item));
    if (!pending) g_pklist.Add(p);
    else g_pklist_pending.Add(p);
    cnt++;
  }
  if (fp) fclose(fp);
  return cnt;
}


#if defined(_WIN32) && !defined(QTUI)

void copyMyPubKey(HWND hwndOwner)
{
  int ks=(g_key.bits+7)/8;
  char buf[(MAX_RSA_MODULUS_LEN/8)*2*2+4096];
  if (!ks) return;
  buf[0]=0;

  sprintf(buf+strlen(buf),"PADLOCKSL_PUBLIC_KEY 20 %d %s\r\n",g_key.bits,g_regnick[0]?g_regnick:"unknown");

  int x;
  int lc=0;
  for (x = 0; x < ks; x ++)
  {
    sprintf(buf+strlen(buf),"%02X",g_key.modulus[MAX_RSA_MODULUS_LEN-ks+x]);
    if (++lc % 30 == 0) sprintf(buf+strlen(buf),"\r\n");
  }

  int zeroes;
  for (zeroes = MAX_RSA_MODULUS_LEN-ks; zeroes < MAX_RSA_MODULUS_LEN && !g_key.publicExponent[zeroes]; zeroes ++);

  ks=MAX_RSA_MODULUS_LEN-zeroes;

  sprintf(buf+strlen(buf),"%02X",ks>>8);
  if (++lc % 30 == 0) sprintf(buf+strlen(buf),"\r\n");
  sprintf(buf+strlen(buf),"%02X",ks&0xff);
  if (++lc % 30 == 0) sprintf(buf+strlen(buf),"\r\n");

  for (x = 0; x < ks; x ++)
  {
    sprintf(buf+strlen(buf),"%02X",g_key.publicExponent[MAX_RSA_MODULUS_LEN-ks+x]);
    if (++lc % 30 == 0) sprintf(buf+strlen(buf),"\r\n");
  }
  if (lc % 30) sprintf(buf+strlen(buf),"\r\n");
  sprintf(buf+strlen(buf),"PADLOCKSL_PUBLIC_KEY_END\r\n");

  HANDLE h=GlobalAlloc(GMEM_MOVEABLE,strlen(buf)+1);
  void *t=GlobalLock(h);
  memcpy(t,buf,strlen(buf)+1);
  GlobalUnlock(h);
  OpenClipboard(hwndOwner);
  EmptyClipboard();
  SetClipboardData(CF_TEXT,h);
  CloseClipboard();
}
#endif

void savePKList()
{
  char str[1024+8];
  sprintf(str,"%s.pr3",g_config_prefix);
  FILE *fp=fopen(str,"wt");
  if (fp)
  {
    int x;
    int total=g_pklist.GetSize()+g_pklist_pending.GetSize();
    for (x = 0; x < total; x ++)
    {
      PKitem *t;
      int pending=x>=g_pklist.GetSize();
      if (pending) t=g_pklist_pending.Get(x-g_pklist.GetSize());
      else t=g_pklist.Get(x);

      int ks=(t->pk.bits+7)/8;
      if (!ks) continue;

      fprintf(fp,"%sPADLOCKSL_PUBLIC_KEY 20 %d %s\n",pending?"PADLOCKSL_PENDING ":"",t->pk.bits,t->name[0]?t->name:"unknown");

      int x;
      int lc=0;
      for (x = 0; x < ks; x ++)
      {
        fprintf(fp,"%02X",t->pk.modulus[MAX_RSA_MODULUS_LEN-ks+x]);
        if (++lc % 30 == 0) fprintf(fp,"\n");
      }

      int zeroes;
      for (zeroes = MAX_RSA_MODULUS_LEN-ks; zeroes < MAX_RSA_MODULUS_LEN && !t->pk.exponent[zeroes]; zeroes ++);

      ks=MAX_RSA_MODULUS_LEN-zeroes;

      fprintf(fp,"%02X",ks>>8);
      if (++lc % 30 == 0) fprintf(fp,"\n");
      fprintf(fp,"%02X",ks&0xff);
      if (++lc % 30 == 0) fprintf(fp,"\n");

      for (x = 0; x < ks; x ++)
      {
        fprintf(fp,"%02X",t->pk.exponent[MAX_RSA_MODULUS_LEN-ks+x]);
        if (++lc % 30 == 0) fprintf(fp,"\n");
      }
      if (lc % 30) fprintf(fp,"\n");
      fprintf(fp,"PADLOCKSL_PUBLIC_KEY_END\n\n");
    }
    fclose(fp);
  }
}

int findPublicKeyFromKey(R_RSA_PUBLIC_KEY *key) // 1 on found, searches pending too!
{
  int x;
  for (x = 0; x < g_pklist.GetSize(); x ++)
  {
    R_RSA_PUBLIC_KEY *t=&g_pklist.Get(x)->pk;
    if (key->bits == t->bits &&
        !memcmp(key->exponent,t->exponent,MAX_RSA_MODULUS_LEN) &&
        !memcmp(key->modulus,t->modulus,MAX_RSA_MODULUS_LEN))
      return 1;
  }
  for (x = 0; x < g_pklist_pending.GetSize(); x ++)
  {
    R_RSA_PUBLIC_KEY *t=&g_pklist_pending.Get(x)->pk;
    if (key->bits == t->bits &&
        !memcmp(key->exponent,t->exponent,MAX_RSA_MODULUS_LEN) &&
        !memcmp(key->modulus,t->modulus,MAX_RSA_MODULUS_LEN))
      return 1;
  }
  if (key->bits == g_key.bits &&
      !memcmp(key->exponent,g_key.publicExponent,MAX_RSA_MODULUS_LEN) &&
      !memcmp(key->modulus,g_key.modulus,MAX_RSA_MODULUS_LEN))
    return 1;

  return 0; // no key found
}

char *findPublicKey(unsigned char *hash, R_RSA_PUBLIC_KEY *out)
{
  int x;
  for (x = 0; x < g_pklist.GetSize(); x ++)
  {
    if (!memcmp(hash,g_pklist.Get(x)->hash,SHA_OUTSIZE))
    {
      if (out) memcpy(out,&g_pklist.Get(x)->pk,sizeof(R_RSA_PUBLIC_KEY));
      return g_pklist.Get(x)->name;
    }
  }
  if (!memcmp(hash,g_pubkeyhash,SHA_OUTSIZE))
  {
    if (out)
    {
      out->bits=g_key.bits;
      memcpy(out->exponent,g_key.publicExponent,MAX_RSA_MODULUS_LEN);
      memcpy(out->modulus,g_key.modulus,MAX_RSA_MODULUS_LEN);
    }
    return "(local)";
  }

  return NULL; // no key found
}


char *conspeed_strs[5]={"Modem","ISDN","Slow DSL/Cable","T1/Fast DSL/Cable","T3/LAN"};
int conspeed_speeds[5]={32,64,384,1600,20000};
int get_speedstr(int kbps, char *str)
{
  int x;
  for (x = 0; x < 5; x ++)
    if (kbps <= conspeed_speeds[x]) break;
  if (x == 5) x--;
  if (str) strcpy(str,conspeed_strs[x]);
  return x;
}



int isaccessable(char *host)
{
  if (g_use_accesslist) return 1;
  int cmpl=0;

  if (!strncmp(host,"10.",3))
  {
    cmpl=3;
  }
  else if (!strncmp(host,"192.168.",8))
  {
    cmpl=8;
  }
  else if (!strncmp(host,"127.",4))
  {
    return 0;
  }
  else return 1;

  if (!g_mql->GetNumQueues()) return 1;

  if (cmpl) // private network detected
  {
//    debug_printf("checking host '%s'\n",host);
    int wc;
    for (wc =0;wc<g_mql->GetNumQueues(); wc++)
    {
      struct in_addr in;
      in.s_addr=g_mql->GetQueue(wc)->get_con()->get_interface();
      char *ad=inet_ntoa(in);
//      debug_printf("comparing against '%s' (%d)\n",ad,cmpl);
      if (ad && !strncmp(ad,host,cmpl)) 
      {
        char tmp[64];
        safe_strncpy(tmp,host,64);
        char *p=strstr(tmp,":");
        if (p) *p=0;
 //       debug_printf("making sure host not interface '%s' '%s'\n",ad,tmp);
        return strcmp(ad,tmp);;
      }
    }
    if (g_forceip&&g_forceip_addr!=INADDR_NONE)
    {
      struct in_addr in;
      in.s_addr=g_forceip_addr;
      char *ad=inet_ntoa(in);
      if (ad && !strncmp(ad,host,cmpl)) 
      {
        char tmp[64];
        safe_strncpy(tmp,host,64);
        char *p=strstr(tmp,":");
        if (p) *p=0;
        return strcmp(ad,tmp);;
      }
    }
    return 0;
  }
  return 1;
}


#if defined(_WIN32) && !defined(QTUI)
void SetWndTitle(HWND hwnd, char *titlebase)
{
  char buf[1024];
  if (g_appendprofiletitles && g_profile_name[0] && strlen(titlebase)+strlen(g_profile_name)+3 < sizeof(buf))
  {
    sprintf(buf,"%s/%s",titlebase,g_profile_name);
    SetWindowText(hwnd,buf);
  }
  else 
    SetWindowText(hwnd,titlebase);
}
#endif

#ifdef QTUI
void SetWndTitle(QWidget* hwnd, char *titlebase)
{
  char buf[1024];
  if (g_appendprofiletitles && g_profile_name[0] && strlen(titlebase)+strlen(g_profile_name)+3 < sizeof(buf))
  {
    sprintf(buf,"%s/%s",titlebase,g_profile_name);
    hwnd->setCaption(buf);
  }
  else 
    hwnd->setCaption(titlebase);
}
#endif

void debug_printf(char *text,...)
{
  if (g_do_log)
  {
    static int logidx;
    char str[2048];
    time_t t; struct tm *tm; 
    t = time(NULL);
    tm = localtime(&t);
  
    va_list list;
    va_start(list,text);
    if (tm) sprintf(str,"<%02d/%02d/%02d %02d:%02d:%02d:%d> ",tm->tm_year%100, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, logidx++);
    else strcpy(str,"<????> ");
    vsprintf(str+strlen(str),text,list);
    va_end(list);
#ifdef _WIN32
    OutputDebugString(str);
#else
    fprintf(stderr,"%s",str);
#endif
  }
}

/* Added by LoraYin 2003/12/9 begin. */
#if defined(_LINUX) || defined(QTUI)

// for preference operation when manage profile.

// Get the keyName's int value from the file with fileName,
// return get value if find it, else return defaultValue.

int GetPrivateProfileInt(char *appName, 
                         char* keyName, 
                         int defaultValue, 
                         char* fileName)
{
  C_Config *config=new C_Config(fileName);
  int ret=config->ReadInt(keyName, defaultValue);
  delete config;
  return ret;
}

// Get the keyName's string value from the file with fileName,
//  return 1 if find it, else return 0.

int GetPrivateProfileString(char *appName, 
                         char* keyName, 
                         char* defaultValue, 
                         char* returnBuf,
			 int bufSize,
			 char* fileName)
{
  C_Config *config=new C_Config(fileName);

  char *string=config->ReadString(keyName, defaultValue);
  int find = (string != defaultValue);
  if (strlen(string) < bufSize)
    strcpy(returnBuf, string);
  else
    strncpy(returnBuf, string, bufSize );
  delete config;
  return find;  

}

// Write the keyName and its' value to the file with fileName,
// return 1 if write successful, else return 0.

int WritePrivateProfileString(char *appName, 
                              char* keyName, 
			      char *value, 
                              char* fileName)
{
  C_Config *config=new C_Config(fileName);
  int ret=0;
  if (config->WriteString(keyName, value))
    ret = 1;
  delete config;
  return ret;
}


#endif

/* Added by LoraYin 2003/12/9  end.*/
