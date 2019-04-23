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
 *******************************************************************************
 *
 *	\Program
 *		PadLockSL
 *
 *	\Author
 *		MichaelZhu
 *
 *	\File
 *		prefinterface.cpp
 *
 *	\Brief
 *		Implement common interface for preference set window
 *
 *	\Code
 *		Date		Version	Author		Description
 *		--------	----	------------	------------------------
 *		03/12/31	1.0	MichaelZhu	Implement all functions
 *		04/02/05	1.0	MichaelZhu	Remove all and note debug info
 *	\End code
 *
 *	\Platform 1:
 *		\Environment
 *			RedHat Linux 9
 *		\Compiler
 *			gcc 3.2.2
 *			QT/X11 3.2.3
 *	\Platform 2:
 *		\Environment
 *			Windows 2000
 *		\Compiler
 *			Visual C++ 6.0
 *			QT/commercial 3.0.1
 *
 *******************************************************************************
*/

//include global variants
#include "main.h"

//include class definitions
#include "prefinterface.h"

//include frequently used classes and macros
#include "common.h"

//include dialogs class declarations
#include "prefdlgs.h"

//include C++ tool classes
#include <string.h>

//include QT tool classes
#include <qmessagebox.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qstringlist.h>

#ifdef _LINUX
//include Linux tool classes
#include <pthread.h>
#elif defined (_WIN32)
#include <windows.h>
#endif	//_LINUX+_WIN32

//include c5p tool
	extern "C" {
#include <C5P.h>
	};

#ifdef _LINUX
static pthread_t kgThreadID;
#elif defined(_WIN32)
static HANDLE kgThreadID;
#endif	//_LINUX+_WIN32

static char *kgPrivOut;
static char kgPassBuf[SHA_OUTSIZE];

static int kgKeySize;
static R_RANDOM_STRUCT kgRandom;

static R_RSA_PRIVATE_KEY kgPrivKey;


//eject password input dialog
//		-	return true if accept dialog, and save password to bufPassword
//		-	reutrn false if reject dialog
bool getPassword(char *bufPassword)
{
	DlgGetPassword dlgGetPassword;
	int ret=dlgGetPassword.exec();

	if(ret == QDialog::Accepted){
		if(dlgGetPassword.getPassword() != 0){
			strcpy(bufPassword, dlgGetPassword.getPassword());
		}
		else{
			strcpy(bufPassword, "");
		}

		return true;
	}

	return false;
}

//warn password
void warnPassword(const char *warningString)
{
	QMessageBox::critical(
		0, QString(APP_NAME)+QString(" Error"),
		warningString,
		"&OK"
		);
}

//show profile manager on startup
bool showProfileManager(bool launchEnabled)
{
	ProfileManager profileManager;
	profileManager.initialize();
	profileManager.setLaunchEnabled(launchEnabled);
	int ret=profileManager.exec();

	if(ret == QDialog::Accepted){
		return true;
	}

	return false;
}

//profile setup wizard
//		-	return true if successed
//		-	return false if failed
bool initializeProfile()
{
	WizardManager wizardManager;
	bool ret=wizardManager.exec();

	if(ret){
		return true;
	}

	return false;
}

static bool m_running=false;

//generate key
void generateKey(char *keyout)
{
	if(!m_running){
		m_running = true;

		memset(&kgPrivKey, 0, sizeof(kgPrivKey));
		kgThreadID = 0;
		kgKeySize = 0;
		memset(&kgRandom, 0, sizeof(kgRandom));
		kgPrivOut = keyout;

		Passphrase dlgGetPassphrase;
		int retPass = dlgGetPassphrase.exec();
		if(retPass == QDialog::Accepted){
			kgKeySize = dlgGetPassphrase.getKeySize();
			char passBuf[1024];
			if(dlgGetPassphrase.getPassphrase() != 0){
				strcpy(passBuf, dlgGetPassphrase.getPassphrase());
			}
			else{
				strcpy(passBuf, "");
			}

			SHAify c;
			c.add((unsigned char *)passBuf, strlen(passBuf));
			c.final((unsigned char *)kgPassBuf);

			//initialize random num
			unsigned char *c5pbuf=(unsigned char *)c5p_malloc(40);
			unsigned int bytesNeeded = 32;

			if(c5p_rng_available()){
			 	while (bytesNeeded){
					RNG_Result result=c5p_rng_rand(32, c5pbuf);
					if(result == e_RNG_Successed){
						R_RandomUpdate(&kgRandom, c5pbuf, 32);
						R_GetRandomBytesNeeded(&bytesNeeded, &kgRandom);
					}
					else
						break;
				}
			}
			else{
#ifdef _LINUX
				FILE *fp=fopen("/dev/urandom", "rb");
				if(fp){
					while(bytesNeeded){
						fread(c5pbuf, 32, 1, fp);
						R_RandomUpdate(&kgRandom, c5pbuf, 32);
						R_GetRandomBytesNeeded(&bytesNeeded, &kgRandom);
					}
					fclose(fp);
				}
#elif defined (_WIN32)
				unsigned __int64 pentium_tsc[1];
				int i=0;
				while (bytesNeeded){
					for(i=0; i<8; ++i){
						QueryPerformanceCounter((LARGE_INTEGER *)pentium_tsc);
						memcpy(c5pbuf+4*i, pentium_tsc, 4);
					}
					R_RandomUpdate(&kgRandom, c5pbuf, 32);
					R_GetRandomBytesNeeded(&bytesNeeded, &kgRandom);
				}
				
#endif	//_LINUX+_WIN32
			}

			c5p_free(c5pbuf);
			//end initialize random num

			KeyGenerator keyGenerator;
#ifdef _LINUX
			if(keyGenerator.kgThreadCreate(&kgThreadID, kgKeySize)){
#elif defined (_WIN32)
			if(keyGenerator.kgThreadCreate(kgThreadID, kgKeySize)){
#endif	//_LINUX+_WIN32
				int retKey=keyGenerator.exec();
				if(retKey == QDialog::Rejected){
					keyGenerator.kgThreadCancel(kgThreadID);
				}
			}
		}

		m_running = false;
	}
}

//thread to create key
#ifdef _LINUX
void *kgThread(void *)
#elif defined(_WIN32)
DWORD WINAPI kgThread(LPVOID p)
#endif	//_LINUX+_WIN32
{
	m_startTime = time((time_t *)0);

	R_RSA_PROTO_KEY protoKey;
	protoKey.bits = kgKeySize;
	protoKey.useFermat4=1;

	R_RSA_PUBLIC_KEY kg_pubKey;

	if(R_GeneratePEMKeys(&kg_pubKey, &kgPrivKey, &protoKey, &kgRandom) ||
		savePrivateKey(kgPrivOut, &kgPrivKey, &kgRandom, kgPassBuf)){
		QMessageBox::warning(0, QString(APP_NAME " Error creating keys"), "Error calling GeneratePEMKeys()");
	}

	memcpy(&g_key, &kgPrivKey, sizeof(R_RSA_PRIVATE_KEY));
	SHAify m;
	m.add((unsigned char *)g_key.modulus, MAX_RSA_MODULUS_LEN);
	m.add((unsigned char *)g_key.publicExponent, MAX_RSA_MODULUS_LEN);
	m.final(g_pubkeyhash);

	kg_end = true;

#ifdef _LINUX
	pthread_exit(0);
#elif defined (_WIN32)
	return 0;
#endif	//_LINUX+_WIN32
}

//save private key to file
int savePrivateKey(char *fn, R_RSA_PRIVATE_KEY *key, R_RANDOM_STRUCT *rnd, char *passhash)
{
	FILE *fp;
	int x;
	int lc=16;

	fp=fopen(fn, "wt");
	if(!fp){
		return 1;
	}

	fprintf(fp, "PADLOCKSL_PRIVATE_KEY 10 %d\n", key->bits);

	unsigned long tl[2];
	R_GenerateBytes((unsigned char *)&tl, 8, rnd);
	for(x=0; x<8; x++){
		fprintf(fp, "%02X", (tl[x/4]>>((x&3)*8))&0xff);
	}
   	unsigned char passhashkey[16];
  	memcpy(passhashkey,(unsigned char *)passhash,16);
  	char buf[18]="PADLOCKSLPASSWORD";
  
	writeBFdata(fp,(int *)passhashkey,16,buf,16,tl,tl+1,&lc);

        
#define WPK(x) writeBFdata(fp, (int *)passhashkey,16, key->x, sizeof(key->x), tl, tl+1, &lc)
	WPK(modulus);
	WPK(publicExponent);
	WPK(exponent);
	WPK(prime);
	WPK(primeExponent);
	WPK(coefficient);
#undef WPK

        
	if(lc % 30){
		fprintf(fp, "\n");
	}

	fprintf(fp, "PADLOCKSL_PRIVATE_KEY_END\n");

	fclose(fp);

	return 0;
}

//write buf data to file
void writeBFdata(FILE *out, int *key, int keylen,void *data, unsigned int len, unsigned long *, unsigned long *cbcr, int *lc)
{
     	int length = (len+15)&~15;
    	int* aes_buf = (int*)c5p_malloc(length);
    	memset((char *)aes_buf,0,length);
    	AES_Result result = e_AES_Failed;
      
  	if (c5p_aes_available())
    	{
      		memcpy((char *)aes_buf,(char *)data,len);
      		result = c5p_aes_encrypt(key,
                               keylen,
                               e_aes_ecb,
                               aes_buf,
                               length,
                               aes_buf,
                               NULL);
    	}
  	if (result != e_AES_Successed)
    	{
      		memcpy((char *)aes_buf,(char *)data,len);
      		sft_aes_encrypt(key,
               		       	keylen,
	                      	e_aes_ecb,
                      		aes_buf,
                      		length,
                      		aes_buf,
                      		NULL);
	  
    	}

    	int c;
    	for (c = 0; c < (int)length; c ++) 
    	{
      		fprintf(out,"%02X",((unsigned char *)aes_buf)[c]);
      		if (++*lc % 30 == 0) fprintf(out,"\n");
    	}
  	c5p_free(aes_buf);
}

//copy content of a existing file to a file
void CopyFile(const char *src, const char *dest)
{
	FILE *fp_src=fopen(src, "rb");
	if(!fp_src){
		return;
	}
	FILE *fp_dest=fopen(dest, "wb");
	if(!fp_dest){
		fclose(fp_src);
		return;
	}

	while(!feof(fp_src)){
		fputc(fgetc(fp_src), fp_dest);
	}

	fclose(fp_dest);
	fclose(fp_src);
}

//copy public key
void copyMyPubKey()
{
	int ks=(g_key.bits+7)/8;
	char buf[(MAX_RSA_MODULUS_LEN/8)*2*2+4096];
	if(!ks){
		return;
	}
	buf[0]=0;

	sprintf(buf+strlen(buf), "PADLOCKSL_PUBLIC_KEY 20 %d %s\r\n", g_key.bits, g_regnick[0]?g_regnick:"unknown");

	int x;
	int lc;
	for(x=0; x<ks; x++){
		sprintf(buf+strlen(buf), "%02X", g_key.modulus[MAX_RSA_MODULUS_LEN-ks+x]);
		if(++lc % 30 == 0){
			sprintf(buf+strlen(buf), "\r\n");
		}
	}

	int zeroes;
	for(zeroes=MAX_RSA_MODULUS_LEN-ks; zeroes<MAX_RSA_MODULUS_LEN && !g_key.publicExponent[zeroes]; zeroes++)
		;

	ks = MAX_RSA_MODULUS_LEN-zeroes;

	sprintf(buf+strlen(buf), "%02X", ks>>8);
	if(++lc % 30 == 0){
		sprintf(buf+strlen(buf), "\r\n");
	}
	sprintf(buf+strlen(buf), "%02X", ks&0xff);
	if(++lc % 30 == 0){
		sprintf(buf+strlen(buf), "\r\n");
	}

	for(x=0; x<ks; x++){
		sprintf(buf+strlen(buf), "%02X", g_key.publicExponent[MAX_RSA_MODULUS_LEN-ks+x]);
		if(++lc % 30 == 0){
			sprintf(buf+strlen(buf), "\r\n");
		}
	}
	if(lc % 30){
		sprintf(buf+strlen(buf), "\r\n");
	}
	sprintf(buf+strlen(buf), "PADLOCKSL_PUBLIC_KEY_END\r\n");

	QString text=QString(buf);
	QClipboard *cb=QApplication::clipboard();

#ifdef _LINUX
	cb->setText(text, QClipboard::Clipboard);
#elif defined (_WIN32)
	cb->setText(text);
#endif	//_LINUX+_WIN32
}

//import public keys
void importPublicKeys()
{
	DlgImportPublicKeys dlgImportPublicKeys;
	int ret=dlgImportPublicKeys.exec();
	if(ret == QDialog::Accepted){
		int num=0;
		char fn[4096];
		if((const char *)(dlgImportPublicKeys.getFileName()) != 0){
			strcpy(fn, (const char *)(dlgImportPublicKeys.getFileName()));
		}
		else{
			return;
		}

		num += loadPKList(fn);

		if(num){
			savePKList();
			char buf[1024];
			sprintf(buf, "Imported %d public keys successfully.", num);
			QMessageBox::information(0, QString(APP_NAME "Public Key Import"), buf);
		}
		else{
			QMessageBox::warning(0, QString(APP_NAME " Public Key Import Error"), "Error: No key(s) found in text");
		}
	}
}

#ifdef _WIN32
void getProfilePath(char *tmp)
{
	GetModuleFileName(NULL, tmp, 1024);
	char *p=tmp;
	while(*p){
		++p;
	}
	while(p>=tmp && *p!='\\'){
		--p;
	}
	p[1] = 0;
}
#endif	//_WIN32
