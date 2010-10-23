/*
WASTE - util.hpp (General utility code)
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
#ifndef _UTIL_H_
#define _UTIL_H_

#include "intdefs.hpp"

#include "rsa/global.hpp"
#include "rsa/rsaref.hpp"
#include "itemlist.hpp"
#if (defined(_WIN32)&&(!defined(_DEFINE_SRV))) || defined(_DEFINE_WXUI)
	#include "listview.hpp"
#endif
#include "sha.hpp"

//global strings

extern const char szDotWastestate[];
extern const char szWastestate[];

//Seed generator(washin seed gen)
void GenerateRandomSeed(void* target,uint32_t len,R_RANDOM_STRUCT *random);

//GUID
struct T_GUID
{
	unsigned char idc[16];
};

#define dpi _S
#define sK0 szG##PL0
#define sK1 szG##PL1

void CreateID128(T_GUID *id);
void MakeID128Str(T_GUID *id, char *str);
int MakeID128FromStr(const char *str, T_GUID *id);//return 0 on success

inline void Bin2Hex_Single(char* buf,char inp);
char* Bin2Hex(char* output, unsigned char* input, int len);
char* Bin2Hex_Lf(char* output, unsigned char* input, int len, int &perline, int maxperline, bool wantCrLf=false);

//RNG
void MYSRANDUPDATE(unsigned char *buf=NULL, int bufl=0);
void MYSRAND();
extern R_RANDOM_STRUCT g_random;

//string utilities
const char *extension(const char *fn);

#if defined(_WIN32)&&(!defined(_DEFINE_SRV))
//windows shit
extern int toolwnd_state;
extern int systray_state;
BOOL systray_del(HWND hwnd);
BOOL systray_add(HWND hwnd, HICON icon);
void DoFlashWindow(HWND hwndParent, int timeoutval);
HWND CreateTooltip(HWND hWnd, LPSTR strTT);
void toolWindowSet(int twstate);
void SetWndTitle(HWND hwnd, char *titlebase);
#endif

void removeInvalidFNChars(char *filename); //careful, removes colons too, so
//when using on a full path, use +2. =)

#if defined(_WIN32)&&(!defined(_DEFINE_SRV))

int doLoadKey(HWND hwndParent, const char *pstr, const char *keyfn, R_RSA_PRIVATE_KEY *key);
int kg_writePrivateKey(char *fn, R_RSA_PRIVATE_KEY *key, R_RANDOM_STRUCT *rnd, char *passhash);
void reloadKey(const char *passstr, HWND hwndParent);

#else

int doLoadKey(const char *pstr, const char *keyfn, R_RSA_PRIVATE_KEY *key);
void reloadKey(const char *passstr);

#endif

//access control shit
struct ACitem
{
	uint32_t ip;
	char maskbits;
	char allow;
};

extern ACitem *g_aclist;
extern int g_aclist_size;
#if defined(_WIN32)&&(!defined(_DEFINE_SRV))
	void updateACList(W_ListView *lv);
#else
	void updateACList(void *lv);
#endif

bool IPv4TestIpInMask(uint32_t addr,uint32_t subnet,uint32_t mask);
uint32_t IPv4Addr(unsigned char i1,unsigned char i2,unsigned char i3,unsigned char i4);
uint32_t IPv4NetMask(uint32_t networkbits);
bool IPv4IsLoopback(uint32_t addr);
bool IPv4IsPrivateNet(uint32_t addr);
bool GetInterfaceInfoOnAddr(uint32_t addr,uint32_t &localaddr,uint32_t &localmask);
int ACStringToStruct(const char *t, ACitem *i);
bool allowIP(uint32_t a);
bool is_accessable_addr(uint32_t addr);
bool is_accessable(char *host);

//conspeed
const extern char *conspeed_strs[5];
extern int conspeed_speeds[5];
int get_speedstr(int kbps, char *str);

enum dSeverity{
	ds_BAD=0,
	ds_Console,
	ds_Critical,
	ds_Error,
	ds_Warning,
	ds_Informational,
	ds_Debug
};

class CLogfile
{
	public:
		#if defined(_DEBUG) || defined(_DEFINE_WXUI)
			CLogfile(const char* filename, const int lineno):m_szFilename(filename),m_line(lineno) {};
		#else
			CLogfile(){};
		#endif
		void operator ()(dSeverity sev,const char *text,...) const;
	private:
		void operator =(CLogfile&) const;
		#if defined(_DEBUG) || defined(_DEFINE_WXUI)
			const char* const m_szFilename;
			const int m_line;
		#endif
};

//MS Intellisense override
#undef __INTELLI
#ifdef __INTELLI
	void dbg_printf(dSeverity sev,char *text,...){};
	void log_printf(dSeverity sev,char *text,...){};
#endif

//debug
#if defined(_WASTEDEBUG)
#warning WASTE LOCAL DEBUG ON
	#define dbg_printf(a,b...) do { fprintf(stderr, "dbg:" b); fprintf(stderr, "\n"); } while (0)
	#define log_printf(a,b...) do { fprintf(stderr, "log:" b); fprintf(stderr, "\n"); } while (0)
#else
#if defined(_DEBUG) || defined(_DEFINE_WXUI)
	#define dbg_printf CLogfile(__FILE__,__LINE__)
	#define log_printf CLogfile(__FILE__,__LINE__)
#else
	#define log_printf CLogfile()
	void inline dummy_printf(dSeverity sev,char *text,...) {};
	#define dbg_printf dummy_printf
#endif
#endif // _WASTEDEBUG

bool log_UpdatePath(const char *logpath, bool bIsFilename=false);

void update_forceip_dns_resolution();

extern R_RSA_PRIVATE_KEY g_key;
extern unsigned char g_pubkeyhash[SHA_OUTSIZE];

struct PKitem
{
	unsigned char hash[SHA_OUTSIZE];
	char name[16];
	R_RSA_PUBLIC_KEY pk;
};

extern C_ItemList<PKitem> g_pklist, g_pklist_pending;

#if defined(_WIN32)&&(!defined(_DEFINE_SRV))
void copyMyPubKey(HWND hwndOwner);
#endif

void KillPkList();
void savePKList();
int loadPKList(char *fn=NULL); //returns num of keys loaded
const char *findPublicKey(unsigned char *hash, R_RSA_PUBLIC_KEY *out); //NULL on err
int findPublicKeyFromKey(R_RSA_PUBLIC_KEY *key); // 1 on found

//crc
uint32_t crc32(uint32_t crc, unsigned char *buf, uint32_t len);

#if (defined(_WIN32) && defined(_CHECK_RSA_BLINDING))
	//ADDED Md5Chap
	void CheckRsaBlinding();
#endif

//ADDED Md5Chap Moved from srchwnd coz need in server for dbg
void FormatSizeStr64(char *out, uint32_t low, uint32_t high);

//ADDED Md5Chap
bool str_return_unpack(char *dst,const char* src,uint32_t dstbuflen,const char returnChar);
bool str_return_pack(char *dst,const char* src,uint32_t dstbuflen,const char returnChar);

//ADDED Md5Chap
const char* GetNextOf(const char* buf,char c,int maxlen);
const char* CopySingleToken(char* dest,const char* source,char stopchar,int destLen,int sourceLen,int* autolen=NULL);
void RelpaceCr(char* st);

//ADDED Md5Chap
void RandomizePadding(void* buf,uint32_t bufsize,uint32_t datasize);

//ADDED Md5Chap
//Hint: hash is mandatory, strings optional, length=16+40+32=88
void MakeUserStringFromHash(unsigned char *hash,char* longstring, char*shortstring);

extern "C" void dpi(char*,unsigned);
extern "C" const uint32_t sK0[];
extern "C" unsigned char const * const sK1[];

//ADDED Md5Chap
template <class T,class U> void inline TIntData4(U data,T i)
{
	unsigned char*& data2=(unsigned char*&)data;
	data2[0]=(unsigned char)(((uint32_t)i>> 0)&0xff);
	data2[1]=(unsigned char)(((uint32_t)i>> 8)&0xff);
	data2[2]=(unsigned char)(((uint32_t)i>>16)&0xff);
	data2[3]=(unsigned char)(((uint32_t)i>>24)&0xff);
};

template <class T,class U> void inline TIntData2(U data,T i)
{
	unsigned char*& data2=(unsigned char*&)data;
	data2[0]=(unsigned char)(((uint32_t)i>> 0)&0xff);
	data2[1]=(unsigned char)(((uint32_t)i>> 8)&0xff);
};

template <class T,class U> void inline TIntData1(U data,T i)
{
	unsigned char*& data2=(unsigned char*&)data;
	data2[0]=(unsigned char)(((uint32_t)i>> 0)&0xff);
};

template <class T,class U> void inline TIntDataSwap4(U data,T i)
{
	unsigned char*& data2=(unsigned char*&)data;
	data2[0]=(unsigned char)(((uint32_t)i>>24)&0xff);
	data2[1]=(unsigned char)(((uint32_t)i>>16)&0xff);
	data2[2]=(unsigned char)(((uint32_t)i>> 8)&0xff);
	data2[3]=(unsigned char)(((uint32_t)i>> 0)&0xff);
};

template <class T,class U> void inline TIntDataSwap2(U data,T i)
{
	unsigned char*& data2=(unsigned char*&)data;
	data2[0]=(unsigned char)(((uint32_t)i>> 8)&0xff);
	data2[1]=(unsigned char)(((uint32_t)i>> 0)&0xff);
};

template <class T,class U> T inline TDataInt4(U data)
{
	uint32_t i;
	unsigned char*& data2=(unsigned char*&)data;
	i=0;
	i=i|(data2[0]<< 0);
	i=i|(data2[1]<< 8);
	i=i|(data2[2]<<16);
	i=i|(data2[3]<<24);
	return (T)i;
};

template <class T,class U> T inline TDataInt2(U data)
{
	uint32_t i;
	unsigned char*& data2=(unsigned char*&)data;
	i=0;
	i=i|(data2[0]<< 0);
	i=i|(data2[1]<< 8);
	return (T)i;
};

template <class T,class U> T inline TDataInt1(U data)
{
	uint32_t i;
	unsigned char*& data2=(unsigned char*&)data;
	i=0;
	i=i|(data2[0]<< 0);
	return (T)i;
};

template <class T,class U> T inline TDataIntSwap4(U data)
{
	uint32_t i;
	unsigned char*& data2=(unsigned char*&)data;
	i=0;
	i=i|(data2[0]<<24);
	i=i|(data2[1]<<16);
	i=i|(data2[2]<< 8);
	i=i|(data2[3]<< 0);
	return (T)i;
};

template <class T,class U> T inline TDataIntSwap2(U data)
{
	uint32_t i;
	unsigned char*& data2=(unsigned char*&)data;
	i=0;
	i=i|(data2[0]<< 8);
	i=i|(data2[1]<< 0);
	return (T)i;
};

uint32_t   inline DataUInt4	   (unsigned char* c) {return TDataInt4	   <uint32_t,  unsigned	char*>(c);};
uint32_t   inline DataUIntSwap4(unsigned char* c) {return TDataIntSwap4<uint32_t,  unsigned	char*>(c);};
uint16_t inline DataUInt2	   (unsigned char* c) {return TDataInt2	   <uint16_t,unsigned	char*>(c);};
unsigned char  inline DataUInt1	   (unsigned char* c) {return TDataInt1	   <unsigned char, unsigned	char*>(c);};

void inline	IntData4	(unsigned char*	c,int i	 ) {TIntData4	 <int,	unsigned char*>(c,i);};
void inline	IntData2	(unsigned char*	c,short	i) {TIntData2	 <short,unsigned char*>(c,i);};
void inline	IntData1	(unsigned char*	c,char i ) {TIntData1	 <char,	unsigned char*>(c,i);};
void inline	IntDataSwap4(unsigned char*	c,int i	 ) {TIntDataSwap4<int,	unsigned char*>(c,i);};

void inline	UIntData4	 (unsigned char* c,unsigned	int	  i) {TIntData4	   <uint32_t  ,unsigned	char*>(c,i);};
void inline	UIntData2	 (unsigned char* c,unsigned	short i) {TIntData2	   <uint16_t,unsigned	char*>(c,i);};
void inline	UIntData1	 (unsigned char* c,unsigned	char  i) {TIntData1	   <unsigned char, unsigned	char*>(c,i);};

#endif
