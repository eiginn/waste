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

#include "common.h"
#include "platform.h"

#include "main.h"
extern "C" {
#include "rsa/r_random.h"
#include "C5P.h"
};


static void writeBFdata(FILE *out, int *key, int keylen,void *data, unsigned int len, unsigned long *cbcl, unsigned long *cbcr, int *lc)
{
  int length = (len+15)&~15;
  int* aes_buf = (int*)c5p_malloc(length);
  memset((char *)aes_buf,0,length);
  AES_Result result = e_AES_Failed;
  if (c5p_aes_available())
  {
    memcpy(aes_buf,data,len);
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
    memcpy(aes_buf,data,len);
    sft_aes_encrypt(key,
                    keylen,
                    e_aes_ecb,
                    aes_buf,
                    length,
                    aes_buf,
                    NULL);
	  
  }
  int c;
  for (c = 0; c < length; c ++)
  {
    fprintf(out,"%02X",((unsigned char *)aes_buf)[c]);
    if (++*lc % 30 == 0) fprintf(out,"\n");
  }
  c5p_free(aes_buf);
}

int kg_writePrivateKey(char *fn, R_RSA_PRIVATE_KEY *key, R_RANDOM_STRUCT *rnd, char *passhash)
{
  FILE *fp;
  int ks=(key->bits+7)/8;
  int x;
  int lc=16;

  debug_printf("kg_writePrivateKey to %s\n",fn);
  fp=fopen(fn,"wt");
  if (!fp) return 1;

  fprintf(fp,"PADLOCKSL_PRIVATE_KEY 10 %d\n",key->bits);

  unsigned long tl[2];
  R_GenerateBytes((unsigned char *)&tl,8,rnd);
  for (x = 0; x < 8; x ++)
  {
    fprintf(fp,"%02lX",(tl[x/4]>>((x&3)*8))&0xff);
  }
   unsigned char passhashkey[16];
  memcpy(passhashkey,(unsigned char *)passhash,16);
  char buf[18]="PADLOCKSLPASSWORD";

  MTRACE("here__1");
  writeBFdata(fp,(int *)passhashkey,16,buf,16,tl,tl+1,&lc);

  MTRACE("here__2");
#define WPK(x) writeBFdata(fp,(int *)passhashkey,16,key->x,sizeof(key->x),tl,tl+1,&lc);
  WPK(modulus)
  WPK(publicExponent)
  WPK(exponent)
  WPK(prime)
  WPK(primeExponent)
  WPK(coefficient)
#undef WPK
  MTRACE("here__3");
  if (lc % 30) fprintf(fp,"\n");
  fprintf(fp,"PADLOCKSL_PRIVATE_KEY_END\n");
  fclose(fp);

 
  return 0;
}

