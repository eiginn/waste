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
 * This file implemented aes en/decryption and rand number interfaces 
 * which supported by VIA C5P CPU.
 * Author: Raymond Cai
 * Date Created: 2003.11.24
*/

#include "C5P.h"
#include "aes.h"
#include <memory.h>
#include <malloc.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef _DEBUG
#include <stdio.h>
#endif

#ifndef NULL
#define NULL 0
#endif

#define ALIGN16(x) (char *)((((unsigned long)x) + 15)&(~15UL))

/*
static const int ctrl_buf_len = 16+15;
static const int extd_key_len = 256+15;
*/
#define ctrl_buf_len 31 //16+15
#define extd_key_len 271 //256+15

#define BLOCK_LEN_BYTE 16    //length of one block of cipertxt or plaintxt
#define BLOCK_LEN_INT 4

#ifdef _WIN32
#define XCRYPT_ECB __asm _emit 0xF3 __asm _emit 0x0F __asm _emit 0xA7 __asm _emit 0xC8

#define XCRYPT_CBC	__asm _emit 0xF3 __asm _emit 0x0F __asm _emit 0xA7 __asm _emit 0xD0
					
#define XCRYPT_CTR  __asm _emit 0xF3 __asm _emit 0x0F __asm _emit 0xA7 __asm _emit 0xD8

#define XCRYPT_CFB  __asm _emit 0xF3 __asm _emit 0x0F __asm _emit 0xA7 __asm _emit 0xE0

#define XCRYPT_OFB  __asm _emit 0xF3 __asm _emit 0x0F __asm _emit 0xA7 __asm _emit 0xE8	

#define XCRYPT_F0  __asm _emit 0x0F __asm _emit 0xA7 __asm _emit 0xF0

#define XCRYPT_F8  __asm _emit 0x0F __asm _emit 0xA7 __asm _emit 0xF8

#define NEW_KEYS   __asm pushfd  __asm popfd

#define XSTORE	__asm _emit 0x0F __asm _emit 0xA7 __asm _emit 0xC0

#define REPXSTORE	__asm _emit 0xF3 __asm _emit 0x0F __asm _emit 0xA7 __asm _emit 0xC0

#define PUSHREG //__asm push eax __asm push ebx __asm push ecx __asm push edx __asm push esi __asm push edi
#define POPREG //__asm pop edi __asm pop esi __asm pop edx __asm pop ecx __asm pop ebx __asm pop eax

#endif /* _WIN32 */
#ifdef _LINUX

#define PUSHREG	//asm("pushl %eax\n pushl %ebx\n pushl %ecx\n pushl %edx\n pushl %edx\n pushl %esi\n pushl %edi\n");

#define POPREG //asm("popl %edi\n popl %esi\n popl %edx\n popl %ecx\n popl %ebx\n popl %eax\n");

#define XCRYPT_ECB      asm ("rep\n" \
		             ".byte 0x0F, 0xA7, 0xC8\n");
                                                                                
#define XCRYPT_CBC      asm ("rep\n" \
		             ".byte 0x0F, 0xA7, 0xD0\n");
                                                                                
#define XCRYPT_CTR      asm ("rep\n" \
		             ".byte 0x0F, 0xA7, 0xD8\n");
                                                                                
#define XCRYPT_CFB      asm ("rep\n" \
		             ".byte 0x0F, 0xA7, 0xE0\n");
                                                                                
#define XCRYPT_OFB      asm ("rep\n" \
		             ".byte 0x0F, 0xA7, 0xE8\n");
                                                                                
#define XCRYPT_F0       asm (".byte 0x0F, 0xA7, 0xF0\n");
                                                                                
#define XCRYPT_F8       asm (".byte 0x0F, 0xA7, 0xF8\n");
                                                                                
#define NEW_KEYS        asm ("pushfl\n" \
                             "popfl\n");

#define XSTORE		asm(".byte 0x0F, 0xA7, 0xC0\n");

#endif /* _LINUX */

#define LDKEY		0x080
#define HWKEY		0x000

#define INTERMEDIATE	0x100
		
#define KEYSIZE_128	0x000 + 0x00a
#define KEYSIZE_192	0x400 + 0x00c
#define KEYSIZE_256	0x800 + 0x00e

#define ENCRYPT		0x000
#define DECRYPT		0x200

		
static int HwkyEncrypt128[4] = {ENCRYPT + HWKEY + KEYSIZE_128, 0, 0, 0};
static int HwkyDecrypt128[4] = {DECRYPT + HWKEY + KEYSIZE_128, 0, 0, 0};

static int LdkyEncrypt192[4] = {ENCRYPT + LDKEY + KEYSIZE_192, 0, 0, 0};
static int LdkyDecrypt192[4] = {DECRYPT + LDKEY + KEYSIZE_192, 0, 0, 0};

static int LdkyEncrypt256[4] = {ENCRYPT + LDKEY + KEYSIZE_256, 0, 0, 0};
static int LdkyDecrypt256[4] = {DECRYPT + LDKEY + KEYSIZE_256, 0, 0, 0};

/*
static int HwKeyEncrypt[4] = {ENCRYPT + HWKEY, 0, 0, 0};
static int HwKeyDecrypt[4] = {DECRYPT + HWKEY, 0, 0, 0};

static int KIntermediate[4]= {ENCRYPT + HWKEY + INTERMEDIATE, 0, 0, 0};
static int KDntermediate[4]= {DECRYPT + LDKEY + INTERMEDIATE, 0, 0, 0};
*/
/* C5P AES check */
BOOL	c5p_aes_available()
{
	BOOL result = false;
#ifdef _WIN32
	__asm {
		mov eax,  0xC0000000;
		cpuid;

	    cmp eax, 0xC0000001;  
		jnz  local_label;

		cpuid;

	    and edx, 0xC0
	    jz   local_label

	    mov result, true;

local_label:
	};
#endif /* _WIN32 */
#ifdef _LINUX
	PUSHREG
	asm("movl $0xC0000000, %%eax\n \
	     cpuid\n \
		\
	     cmpl $0xC0000001, %%eax\n  \
	     jnz  local_label\n \
		\
	     cpuid\n \
		\
	     andl $0xC0, %%edx\n \
	     jz  local_label\n \
		\
	     movl %1, %0 \n \
		\
local_label:\n"
		:"=m"(result)
		:"i"(true));
	POPREG
#endif /* _LINUX */

	return result;
}

#if defined(_WIN32)
#define  ENPREPAREREG \
		{\
			__asm	mov ecx, count  \
			__asm	mov edx, ctrl  \
			__asm	mov ebx, extdkey  \
			__asm	mov esi, plaintxt  \
			__asm	mov eax, IV  \
			__asm	mov edi, cipertxt  \
		}
#endif /* _WIN32 */
#if defined(_LINUX)
#define  ENPREPAREREG \
		{\
			asm("movl %0, %%eax \n \
			     movl %1, %%ebx \n \
			     movl %2, %%ecx \n \
			     movl %3, %%edx \n \
			     movl %4, %%esi \n \
			     movl %5, %%edi \n" \
			     :  \
			     :"m"(IV), "m"(extdkey), "m"(count), "m"(ctrl), "m"(plaintxt), "m"(cipertxt) \
			     :"eax", "ebx", "ecx", "edx", "esi", "edi"); \
		}
#endif /* _LINUX */

/* Part of Encode */
AES_Result c5p_aes_encrypt128(int *key, AES_MODE mode,
				int *plaintxt, int plainlen, int *cipertxt, int *IV)
{
	int count = plainlen / 16;
	int *extdkey = key;
	char buf[ctrl_buf_len];

	char *ctrl = ALIGN16(buf);
	memcpy((void *)ctrl, (void *)HwkyEncrypt128, 4*sizeof(int));

	switch(mode){
		case e_aes_ecb:
	
			NEW_KEYS
			ENPREPAREREG
			XCRYPT_ECB;

			break;
		
		case e_aes_cbc:
			
			NEW_KEYS;
			ENPREPAREREG
			XCRYPT_CBC;

			break;

		case e_aes_cfb:

			NEW_KEYS;
			ENPREPAREREG
			XCRYPT_CFB;
			
			break;

		case e_aes_ofb:

			NEW_KEYS;
			ENPREPAREREG
			XCRYPT_OFB;
			
			break;

		default:
#ifdef _DEBUG
			printf("The encrypt mode isn't implemented now !");
#endif
			break;
	}
	
	//c5p_free(ctrl);
	return e_AES_Successed;
}

AES_Result c5p_aes_encrypt192(int *key, AES_MODE mode, 
				int *plaintxt, int plainlen, int *cipertxt, int *IV)
{
	int count = plainlen / 16;
	char ctrl_buf[ctrl_buf_len];
	char extd_key_buf[extd_key_len];
	char *ctrl = ALIGN16(ctrl_buf);
	aes_encrypt_ctx *extdkey = (aes_encrypt_ctx *)ALIGN16(extd_key_buf);

	memcpy((void *)ctrl, (void *)LdkyEncrypt192, 4*sizeof(int));

	c5p_encrypt_key(key, 192, extdkey);
	
	switch(mode){
		case e_aes_ecb:

			NEW_KEYS;
			ENPREPAREREG
			XCRYPT_ECB;
			break;
		
		case e_aes_cbc:
	
			NEW_KEYS;
			ENPREPAREREG
			XCRYPT_CBC;
			break;

		case e_aes_cfb:
			
			NEW_KEYS;
			ENPREPAREREG
			XCRYPT_CFB;
		
			break;

		case e_aes_ofb:
			
			NEW_KEYS;
			ENPREPAREREG
			XCRYPT_OFB;
		
			break;

		default:
#ifdef _DEBUG
			printf("The encrypt mode isn't implemented now !");
#endif
			break;
	}

//	c5p_free(ctrl);
//	c5p_free(extdkey);

	return e_AES_Successed;
}

AES_Result c5p_aes_encrypt256(int *key, AES_MODE mode, 
				int *plaintxt, int plainlen, int *cipertxt, int *IV)
{
	int count = plainlen / 16;
	char ctrl_buf[ctrl_buf_len];
	char extd_key_buf[extd_key_len];
	
	char *ctrl = ALIGN16(ctrl_buf);//(int *)c5p_malloc(4*sizeof(int));
	aes_encrypt_ctx *extdkey = (aes_encrypt_ctx *)ALIGN16(extd_key_buf);

	memcpy((void *)ctrl, (void *)LdkyEncrypt256, 4*sizeof(int));
	c5p_encrypt_key(key, 256, extdkey);

	switch(mode){
		case e_aes_ecb:
		
			NEW_KEYS;
			ENPREPAREREG
			XCRYPT_ECB;
	
			break;
		
		case e_aes_cbc:
			
			NEW_KEYS;
			ENPREPAREREG
			XCRYPT_CBC;
	
			break;

		case e_aes_cfb:
			
			NEW_KEYS;
			ENPREPAREREG
			XCRYPT_CFB;
	
			break;

		case e_aes_ofb:
			
			NEW_KEYS;
			ENPREPAREREG
			XCRYPT_OFB;
			
			break;

		default:
#ifdef _DEBUG
			printf("The encrypt mode isn't implemented now !");
#endif
			break;
	}

	return e_AES_Successed;
}

AES_Result c5p_aes_encrypt(int *key, int klen, AES_MODE mode,  
						   int *plaintxt, int plainlen, int *cipertxt, int *IV)
{
	if (((unsigned int)key | (unsigned int) plaintxt | 
		(unsigned int) cipertxt | (unsigned int)IV ) & 0x0000000F)
	{
#ifdef _DEBUG
		printf("The address is not aligned\n");
#endif
		return e_AES_Addr_Not_Aligned;
	}

	switch(klen) {
	case 16:
	case 128:
		c5p_aes_encrypt128(key, mode, plaintxt, plainlen, cipertxt, IV);
		break;
	case 24:
	case 192:
		c5p_aes_encrypt192(key, mode, plaintxt, plainlen, cipertxt, IV);
		break;
	case 32:
	case 256:
		c5p_aes_encrypt256(key, mode, plaintxt, plainlen, cipertxt, IV);
		break;
	default:
#ifdef _DEBUG
		printf("Key len is error key len = %d!\n", klen);
#endif
		return e_AES_Failed;
	}
	
	return e_AES_Successed;
}

#if defined(_WIN32)
#define  DEPREPAREREG \
		{\
			__asm	mov ecx, count  \
			__asm	mov edx, ctrl  \
			__asm	mov ebx, extdkey  \
			__asm	mov esi, cipertxt  \
			__asm	mov eax, IV  \
			__asm	mov edi, plaintxt  \
		}
#endif /* _WIN32 */
#if defined(_LINUX)
#define  DEPREPAREREG \
		{\
			asm("movl %0, %%eax \n \
			     movl %1, %%ebx \n \
			     movl %2, %%ecx \n \
			     movl %3, %%edx \n \
			     movl %4, %%esi \n \
			     movl %5, %%edi \n" \
			     :  \
			     :"m"(IV), "m"(extdkey), "m"(count), "m"(ctrl), "m"(cipertxt), "m"(plaintxt) \
			     :"eax", "ebx", "ecx", "edx", "esi", "edi"); \
		}
#endif /* _LINUX */

/* Part of Decrypt */
AES_Result c5p_aes_decrypt128(int *key, AES_MODE mode, 
				int *cipertxt, int ciperlen, int *plaintxt, int *IV)
{
	int count = ciperlen / 16;
	int *extdkey = key;
	char buf[ctrl_buf_len];

	char *ctrl = ALIGN16(buf);
	memcpy((void *)ctrl, (void *)HwkyDecrypt128, 4*sizeof(int));

	switch(mode){
		case e_aes_ecb:
 
			NEW_KEYS;
			DEPREPAREREG
			XCRYPT_ECB;
	
			break;
		
		case e_aes_cbc:
 
			NEW_KEYS;
			DEPREPAREREG		
			XCRYPT_CBC;
  	 
			break;

		case e_aes_cfb:
	 
			NEW_KEYS;
			DEPREPAREREG
			XCRYPT_CFB;
	 
			break;

		case e_aes_ofb:
	 
			NEW_KEYS;
			DEPREPAREREG
			XCRYPT_OFB;
	 
			break;

		default:
#ifdef _DEBUG
			printf("The encrypt mode isn't implemented now !");
#endif
			break;
	}
	
	return e_AES_Successed;
}

AES_Result c5p_aes_decrypt192(int *key, AES_MODE mode, 
				int *cipertxt, int ciperlen, int *plaintxt, int *IV)
{
	int count = ciperlen / 16;
	char ctrl_buf[ctrl_buf_len];
	char extd_key[extd_key_len];
	
	char *ctrl = ALIGN16(ctrl_buf);
	aes_decrypt_ctx *extdkey = (aes_decrypt_ctx *)ALIGN16(extd_key);
	
	memcpy((void *)ctrl, (void *)LdkyDecrypt192, 4*sizeof(int));
	
	if ((e_aes_ecb == mode) || (e_aes_cbc == mode))
		c5p_decrypt_key(key, 192, extdkey);	
	else
		c5p_encrypt_key(key, 192, (aes_encrypt_ctx *)extdkey);


	switch(mode){
		case e_aes_ecb:
	 
			NEW_KEYS;
			DEPREPAREREG
			XCRYPT_ECB;
	 
			break;
		
		case e_aes_cbc:
	 
			NEW_KEYS;
			DEPREPAREREG
			XCRYPT_CBC;
	 
			break;

		case e_aes_cfb:
			
			NEW_KEYS;
			DEPREPAREREG
			XCRYPT_CFB;
	
			break;

		case e_aes_ofb:
	
			NEW_KEYS;
			DEPREPAREREG
			XCRYPT_OFB;
	
			break;

		default:
#ifdef _DEBUG
			printf("The encrypt mode isn't implemented now !");
#endif
			break;
	}	

	return e_AES_Successed;
}

AES_Result c5p_aes_decrypt256(int *key, AES_MODE mode, 
				int *cipertxt, int ciperlen, int *plaintxt, int *IV)
{
	int count = ciperlen / 16;
	char ctrl_buf[ctrl_buf_len];
	char extd_buf[extd_key_len];
	
	char *ctrl = ALIGN16(ctrl_buf);
	aes_decrypt_ctx *extdkey = (aes_decrypt_ctx *)ALIGN16(extd_buf);

	memcpy((void *)ctrl, (void *)LdkyDecrypt256, 4*sizeof(int));

	if ((e_aes_ecb == mode) || (e_aes_cbc == mode))
		c5p_decrypt_key(key, 256, extdkey);	
	else
		c5p_encrypt_key(key, 256, (aes_encrypt_ctx *)extdkey);

	switch(mode){
		case e_aes_ecb:
	
			NEW_KEYS;
			DEPREPAREREG
			XCRYPT_ECB;
	
			break;
		
		case e_aes_cbc:
	
			NEW_KEYS;
			DEPREPAREREG
			XCRYPT_CBC;
	
			break;

		case e_aes_cfb:
			
			NEW_KEYS;
			DEPREPAREREG
			XCRYPT_CFB;
	
			break;

		case e_aes_ofb:
			
			NEW_KEYS;
			DEPREPAREREG
			XCRYPT_OFB;
		
			break;

		default:
#ifdef _DEBUG
			printf("The encrypt mode isn't implemented now !");
#endif
			break;
	}
	
	return e_AES_Successed;
}

AES_Result c5p_aes_decrypt(int *key, int klen,
					 AES_MODE mode, int *cipertxt, int ciperlen, int *plaintxt, int *IV)
{
	if (((unsigned int)key | (unsigned int) plaintxt | 
		(unsigned int) cipertxt | (unsigned int)IV ) & 0x0000000F)
	{
#ifdef _DEBUG
		printf("The address is not aligned\n");
#endif
		return e_AES_Addr_Not_Aligned;
	}

	switch(klen) {
	case 16:
	case 128:
		c5p_aes_decrypt128(key, mode, cipertxt, ciperlen, plaintxt, IV);
		break;
	case 24:
	case 192:
		c5p_aes_decrypt192(key, mode, cipertxt, ciperlen, plaintxt, IV);
		break;
	case 32:
	case 256:
		c5p_aes_decrypt256(key, mode, cipertxt, ciperlen, plaintxt, IV);
		break;
	default:
#ifdef _DEBUG
		printf("Key len is error key len = %d!\n", klen);
#endif
		return e_AES_Failed;
	}
	
	return e_AES_Successed;
}

// For RNG
BOOL c5p_rng_available()
{
	BOOL result = false;
#ifdef _WIN32
	__asm {
		mov eax,  0xC0000000;
		cpuid;

	    cmp eax, 0xC0000001;  
		jnz  local_label;

		cpuid;

	    and edx, 0x0C
	    jz   local_label

	    mov result, true;

local_label:
	};
#endif /* _WIN32 */
#ifdef _LINUX
	PUSHREG
	asm("movl $0xC0000000, %%eax\n"
	    "cpuid\n"
				
	    "cmpl $0xC0000001, %%eax\n"
	    "jnz  local_label\n" 
			
	    "cpuid\n"	
			
	    "andl $0x0C, %%edx\n" 
	    "jz  err_label \n" 
	     		
	    "movl %1, %0 \n"
	    		
"err_label:\n"
	     :"=m"(result)
	     :"i"(true));
	POPREG
#endif /* _LINUX */

	return result;
}

RNG_Result c5p_rng_rand(int bufl, unsigned char *buf)
{

	if ((0 == bufl) || (NULL == buf))
		return e_RNG_Successed;

	while(bufl){
#ifdef _WIN32
		__asm{
			mov edi, buf
			mov edx, 3
			XSTORE;
			mov buf, edi
			and eax, 0x0000000F
			sub bufl, eax
		}
#elif defined(_LINUX)
		asm("pushl %edi\n"
		    "pushl %edx\n"
		   );
		asm("movl %0, %%edi\n"
		    "movl $3, %%edx\n"
		    :
		    :"m"(buf)
		);
		XSTORE
		asm("movl %%edi, %0\n"
		    "andl $0x0000000F, %%eax\n"
		    "subl %%eax, %1\n" 
		    :"=m"(buf), "=m"(bufl)
		    :
		);
		asm("popl %edx\n"
		    "popl %edi\n"
		   );
#endif
	}
	
	return e_RNG_Successed;
}

void *c5p_malloc(size_t size)
{
	unsigned char len;
	unsigned char *p2;
	void *p1;

	//allocate the memory 
	while(NULL == (p1 = malloc(size + 16)));

	p2 = (unsigned char *)p1 + 16;
	//how many bytes is ignored before p2
	len = 16 - (unsigned char )((unsigned int)p2 & (0x0000000F));
	//p2 is aligned 128bits
	p2 = (unsigned char *)((unsigned int)p2 & (0xFFFFFFF0));
	// use one byte to record how many bytes is ignored before p2
	*(p2-1) = len;
	return p2;
}

void c5p_free(void *p)
{
	unsigned char *p1 = NULL;
	unsigned char len = 0;
	char *p2 = NULL;

	//if p is null do nothing
	if (NULL == p)
		return;
	
	//mov the pointer forward the bytes it ignored
	p1 = (unsigned char *)p;
	len = *(p1 - 1);
	p2 = (char *)p;

	p2 -= len;

	free(p2);
}

/*
 *	Software en/decrypt
 */

AES_Result sft_aes_encrypt_ecb(int *plaintxt, int len, int *cipertxt, aes_encrypt_ctx *ctx)
{
	int num_of_block = len / BLOCK_LEN_BYTE;

	while (num_of_block)
	{
		aes_encrypt(plaintxt, cipertxt, ctx);

		plaintxt += BLOCK_LEN_INT;
		cipertxt += BLOCK_LEN_INT;
		num_of_block--;
	}//while (num_of_block)

	return e_AES_Successed;
}

AES_Result sft_aes_encrypt_cbc(int *plaintxt, int len, 
							   int *cipertxt, aes_encrypt_ctx *ctx, int *IV)
{
	int num_of_block = len / BLOCK_LEN_BYTE;
	int i;

	while (num_of_block)
	{
		for (i=0; i<BLOCK_LEN_INT; i++)
			IV[i] ^= plaintxt[i];

		aes_encrypt(IV, cipertxt, ctx);

		//reinitialize IV
		memcpy((void *)IV, (void *)cipertxt, BLOCK_LEN_BYTE);

		cipertxt += BLOCK_LEN_INT;
		plaintxt += BLOCK_LEN_INT;
		num_of_block--;
	}//while (num_of_block)
	
	return e_AES_Successed;
}

AES_Result sft_aes_encrypt_cfb(int *plaintxt, int len, 
							   int *cipertxt, aes_encrypt_ctx *ctx, int *IV)
{
	int num_of_block = len / BLOCK_LEN_BYTE;
	int i;

	while (num_of_block)
	{
		aes_encrypt(IV, IV, ctx);

		for (i=0; i<BLOCK_LEN_INT; i++)
			*(IV + i) ^= *(plaintxt + i);

		//store the cipertxt
		memcpy((void *)cipertxt, (void *)IV, BLOCK_LEN_BYTE);

		plaintxt += BLOCK_LEN_INT;
		cipertxt += BLOCK_LEN_INT;
		num_of_block--;
	}//while (num_of_block)

	return e_AES_Successed;
}

AES_Result sft_aes_encrypt_ofb(int *plaintxt, int len, 
							   int *cipertxt, aes_encrypt_ctx *ctx, int *IV)
{
	int num_of_block = len / BLOCK_LEN_BYTE;
	int i;

	while (num_of_block)
	{
		aes_encrypt(IV, IV, ctx);

		for (i=0; i<BLOCK_LEN_INT; i++)
			*(cipertxt + i) = (*(IV + i)) ^ (*(plaintxt + i));

		plaintxt += BLOCK_LEN_INT;
		cipertxt += BLOCK_LEN_INT;
		num_of_block--;
	}//while (num_of_block)

	return e_AES_Successed;
}

AES_Result sft_aes_decrypt_ecb(int *cipertxt, int len, 
							   int *plaintxt, aes_decrypt_ctx *ctx)
{
	int num_of_block = len / BLOCK_LEN_BYTE;

	while (num_of_block)
	{
		aes_decrypt(cipertxt, plaintxt, ctx);

		plaintxt += BLOCK_LEN_INT;
		cipertxt += BLOCK_LEN_INT;
		num_of_block--;
	}//while (num_of_block)

	return e_AES_Successed;
}

AES_Result sft_aes_decrypt_cbc(int *cipertxt, int len, 
							   int *plaintxt, aes_decrypt_ctx *ctx, int *IV)
{
	int num_of_block = len / BLOCK_LEN_BYTE;
	int i;
	int tmp[BLOCK_LEN_INT] = {0	};

	while (num_of_block)
	{
		memcpy((void *)tmp, (void *)cipertxt, BLOCK_LEN_BYTE);
		aes_decrypt(cipertxt, plaintxt, ctx);

		for (i=0; i<BLOCK_LEN_INT; i++)
			*(plaintxt + i) ^= *(IV + i);

		//restore IV
		memcpy((void *)IV, (void *)tmp, BLOCK_LEN_BYTE);

		plaintxt += BLOCK_LEN_INT;
		cipertxt += BLOCK_LEN_INT;
		num_of_block--;
	}//while (num_of_block)
	
	return e_AES_Successed;
}

AES_Result sft_aes_decrypt_cfb(int *cipertxt, int len, 
							   int *plaintxt, aes_encrypt_ctx *ctx, int *IV)
{
	int num_of_block = len / BLOCK_LEN_BYTE;
	int i;
	int tmp[BLOCK_LEN_INT] = {0	};

	while (num_of_block)
	{
		memcpy((void *)tmp, (void *)cipertxt, BLOCK_LEN_BYTE);
		aes_encrypt(IV, IV, ctx);

		for (i=0; i<BLOCK_LEN_INT; i++)
			*(plaintxt + i) = (*(IV +i)) ^ (*(cipertxt + i));

		//restore IV
		memcpy((void *)IV, (void *)tmp, BLOCK_LEN_BYTE);

		plaintxt += BLOCK_LEN_INT;
		cipertxt += BLOCK_LEN_INT;
		num_of_block--;
	}//while (num_of_block)

	return e_AES_Successed;
}

AES_Result sft_aes_decrypt_ofb(int *cipertxt, int len, 
							   int *plaintxt, aes_encrypt_ctx *ctx, int *IV)
{
	int num_of_block = len / BLOCK_LEN_BYTE;
	int i;

	while (num_of_block)
	{
		aes_encrypt(IV, IV, ctx);

		for (i=0; i<BLOCK_LEN_INT; i++)
			*(plaintxt + i) = (*(IV + i)) ^ (*(cipertxt + i));

		plaintxt += BLOCK_LEN_INT;
		cipertxt += BLOCK_LEN_INT;
		num_of_block--;
	}//while (num_of_block)

	return e_AES_Successed;
}

AES_Result sft_aes_encrypt(int *key, int klen, AES_MODE mode, 
						   int *plaintxt, int plainlen, int *cipertxt, int *IV)
{
	aes_encrypt_ctx ctx;

	if (plainlen % 16)
		return e_AES_Not_Block;
		
	aes_encrypt_key(key, klen, &ctx);

	switch(mode) {
	case e_aes_ecb:
		return sft_aes_encrypt_ecb(plaintxt, plainlen, cipertxt, &ctx);
	
	case e_aes_cbc:
		return sft_aes_encrypt_cbc(plaintxt, plainlen, cipertxt, &ctx, IV);
	
	case e_aes_cfb:
		return sft_aes_encrypt_cfb(plaintxt, plainlen, cipertxt, &ctx, IV);

	case e_aes_ofb:
		return sft_aes_encrypt_ofb(plaintxt, plainlen, cipertxt, &ctx, IV);

	default:
#ifdef _DEBUG
		printf("The encrypt mode is err mode = %d!\n", mode);
#endif
		return e_AES_Not_Key;
	}
	
	return e_AES_Successed;
}

AES_Result sft_aes_decrypt(int *key, int klen,
					 AES_MODE mode, int *cipertxt, int ciperlen, int *plaintxt, int *IV)
{
	aes_encrypt_ctx enctx;
	aes_decrypt_ctx dectx;

	if (ciperlen % 16)
		return e_AES_Not_Block;

	switch(mode) {
	case e_aes_ecb:
		aes_decrypt_key(key, klen, &dectx);
		return sft_aes_decrypt_ecb(plaintxt, ciperlen, cipertxt, &dectx);
	
	case e_aes_cbc:
		aes_decrypt_key(key, klen, &dectx);
		return sft_aes_decrypt_cbc(plaintxt, ciperlen, cipertxt, &dectx, IV);
	
	case e_aes_cfb:
		aes_encrypt_key(key, klen, &enctx);
		return sft_aes_decrypt_cfb(plaintxt, ciperlen, cipertxt, &enctx, IV);

	case e_aes_ofb:
		aes_encrypt_key(key, klen, &enctx);
		return sft_aes_decrypt_ofb(plaintxt, ciperlen, cipertxt, &enctx, IV);

	default:
#ifdef _DEBUG
		printf("The decrypt mode is err mode = %d!\n", mode);
#endif
		return e_AES_Mode_Not_Support;
	}
	
	return e_AES_Successed;
}

#if defined(__cplusplus)
}
#endif
