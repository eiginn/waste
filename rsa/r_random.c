/* R_RANDOM.C - random objects for RSAREF
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
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

 /* $Id: r_random.c,v 1.1 2004/01/13 08:41:10 stanleysan Exp $ */

//Added by LoraYin 2003/11/27.
#include "C5P.h"

#include "global.h"
#include "rsaref.h"
#include "r_random.h"
#include "md5.h"

#define RANDOM_BYTES_NEEDED 256

int R_RandomInit (randomStruct)
R_RANDOM_STRUCT *randomStruct;                      /* new random structure */
{
  randomStruct->bytesNeeded = RANDOM_BYTES_NEEDED;
  R_memset ((POINTER)randomStruct->state, 0, sizeof (randomStruct->state));
  randomStruct->outputAvailable = 0;
  
  return (0);
}

int R_RandomUpdate (randomStruct, block, blockLen)
R_RANDOM_STRUCT *randomStruct;                          /* random structure */
unsigned char *block;                          /* block of values to mix in */
unsigned int blockLen;                                   /* length of block */
{
  MD5_CTX context;
  unsigned char digest[16];
  unsigned int i, x;
  
  MD5Init (&context);
  MD5Update (&context, block, blockLen);
  MD5Final (digest, &context);

  /* add digest to state */
  x = 0;
  for (i = 0; i < 16; i++) {
    x += randomStruct->state[15-i] + digest[15-i];
    randomStruct->state[15-i] = (unsigned char)x;
    x >>= 8;
  }
  
  if (randomStruct->bytesNeeded < blockLen)
    randomStruct->bytesNeeded = 0;
  else
    randomStruct->bytesNeeded -= blockLen;
  
  /* Zeroize sensitive information.
   */
  R_memset ((POINTER)digest, 0, sizeof (digest));
  x = 0;
  
  return (0);
}

int R_GetRandomBytesNeeded (bytesNeeded, randomStruct)
unsigned int *bytesNeeded;                 /* number of mix-in bytes needed */
R_RANDOM_STRUCT *randomStruct;                          /* random structure */
{
  *bytesNeeded = randomStruct->bytesNeeded;
  
  return (0);
}

int R_GenerateBytes (block, blockLen, randomStruct)
unsigned char *block;                                              /* block */
unsigned int blockLen;                                   /* length of block */
R_RANDOM_STRUCT *randomStruct;                          /* random structure */
{
  MD5_CTX context;
  unsigned int available, i;

	
  /* Modified by LoraYin 2003/11/27 being. Add C5P RNG support.    */	
  
  //printf("R_GenerateBytes() check if C5P RNG available()\n");
  if (c5p_rng_available())
  {
    //128-bit boundary address aligned. A little more bytes than acturally needed.
    unsigned char *buf = (unsigned char*)c5p_malloc(blockLen + 8);
    RNG_Result result = c5p_rng_rand(blockLen, buf);
    //printf("R_GenerateBytes() use C5P RNG to generate %d random bytes()\n",blockLen);  	
    if (result == e_RNG_Successed)
    {
      memcpy(block, buf, blockLen);
      c5p_free(buf);
      return 0;
    }
    else
    {
      //printf(" C5P RNG to generate %d random bytes() failed !!!\n",blockLen);  	
    }
    c5p_free(buf);
  }
  
  /* Modified by LoraYin 2003/11/27 end.    */	
	
  
  if (randomStruct->bytesNeeded)
    return (RE_NEED_RANDOM);
  
  available = randomStruct->outputAvailable;
  
  while (blockLen > available) {
    R_memcpy
      ((POINTER)block, (POINTER)&randomStruct->output[16-available],
       available);
    block += available;
    blockLen -= available;

    /* generate new output */
    MD5Init (&context);
    MD5Update (&context, randomStruct->state, sizeof(randomStruct->state));
    MD5Final (randomStruct->output, &context);
    available = 16;

    /* increment state */
    for (i = 0; i < 32; i++)
      if (randomStruct->state[31-i]++)
        break;
  }

  R_memcpy 
    ((POINTER)block, (POINTER)&randomStruct->output[16-available], blockLen);
  randomStruct->outputAvailable = available - blockLen;

  return (0);
}

void R_RandomFinal (randomStruct)
R_RANDOM_STRUCT *randomStruct;                          /* random structure */
{
  R_memset ((POINTER)randomStruct, 0, sizeof (*randomStruct));
}
