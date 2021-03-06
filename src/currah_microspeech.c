/* 
 * Copyright (C) 2014 Fabio Olimpieri
 * This file is part of FBZX Wii
 *
 * FBZX Wii is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * FBZX Wii is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include "z80free/Z80free.h"
#include "currah_microspeech.h"
#include "computer.h"
#include "emulator.h"

#ifdef DEBUG
extern FILE *fdebug;
#define printf(...) fprintf(fdebug,__VA_ARGS__)
#else
 #ifdef GEKKO
 #define printf(...)
 #endif
#endif

int allophone_lenght[ALLOPHONES];
signed char *allophone_buffer[ALLOPHONES];

char *allophone_list[] = {"pa1","pa2","pa3","pa4","pa5","oy","ay","eh","kk3","pp","jh","nn1","ih","tt2","rr1",
"ax","mm","tt1","dh1","iy","ey","dd1","uw1","ao","aa","yy2","ae","hh1","bb1","th","uh","uw2","aw","dd2","gg3","vv",
"gg1","sh","zh","rr2","ff","kk2","kk1","zz","ng","ll","ww","xr","wh","yy1","ch","er1","er2","ow","dh2","ss","nn2","hh2",
"or","ar","yr","gg2","el","bb2"};

void currah_microspeech_init() {
	
	int i;
	FILE *fichero;
	char allophone_name[16];
	
	ordenador.currah_paged = 0;
	ordenador.currah_status = 0;
	ordenador.current_allophone = 0;
	ordenador.allophone_sound_cuantity = 0;
	ordenador.intonation_allophone = 0;
	
	for (i=0; i<ALLOPHONES; i++)
	{
		sprintf(allophone_name, "fbzx/allophones/%s.raw",allophone_list[i]);	
		
		fichero=myfopen(allophone_name,"rb");
		if(fichero==NULL) {
			printf("Can't open allophone: %s\n", allophone_list[i]);
			exit(1);
			}
	
		fseek (fichero, 0, SEEK_END);
		allophone_lenght[i]=ftell (fichero);
		fseek (fichero, 0, SEEK_SET);
	
		allophone_buffer[i]= (signed char *) malloc(allophone_lenght[i]);
	
		if (allophone_buffer[i]==NULL) {
			printf("Can't allocate allophone: %d\n",i);
			exit(1);
			}
		
		fread(allophone_buffer[i], 1, allophone_lenght[i], fichero); 	
	
		fclose(fichero);		
	}
      
for (i=0; i<5; i++)
		memset(allophone_buffer[i], 0x00, allophone_lenght[i]); 
}

void currah_microspeech_fini() {

	int i;
	
	for (i=4; i<ALLOPHONES; i++)
	{	
		free(allophone_buffer[i]); 			
	}		
}

void currah_microspeech_reset() {
	
	ordenador.currah_paged = 0;
	ordenador.currah_status = 0;
	ordenador.current_allophone = 0;
	ordenador.allophone_sound_cuantity = 0;
	ordenador.intonation_allophone = 0;
}


