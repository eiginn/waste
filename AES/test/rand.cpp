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
 * rand.cpp
 * This file implements test for rand number generated by C5P CPU
 * Author: Raymond Cai
 * Date Created: 2003.12.1
*/
#include <stdio.h>
#include "C5P.h"

int main()
{
	unsigned int len_buf, len_rnd;
	unsigned char *buf;

	for (len_buf = 4; len_buf <= 100; len_buf++ )
	{
		printf("-----------start-----------\n Len_buf = %d\n", len_buf);
			for(len_rnd=0; len_rnd<=len_buf-4; len_rnd++)
			{
				printf("\nlen_rnd = %d\n", len_rnd);
				printf("\nbefore malloc\n");
				buf = (unsigned char *)c5p_malloc(len_buf);
				printf("\nafter malloc\n");
				c5p_rng_rand(len_rnd, buf);
				printf("\nRand buf = %d\n", len_rnd);
				for (unsigned int i=0; i<len_rnd; i++)
					printf("%x ", (int )buf[i]);
				printf("\nbefore free\n");
				c5p_free(buf);
				printf("\nafter free\n");
			}
			
		printf("\n-----------end-------------------\n\n\n");
	}//for
	return 0;
}

