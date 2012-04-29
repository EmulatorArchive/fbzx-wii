/* 
 * Copyright (C) 2012 Fabio Olimpieri
 * Copyright 2003-2009 (C) Raster Software Vigo (Sergio Costas)
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
#include "computer.h"
#include "emulator.h"
#include "menus.h"
#include "characters.h"
#include "sound.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include "tape.h"
#include "microdrive.h"
#include "Virtualkeyboard.h"
#include "gui_sdl.h"

#ifdef DEBUG
extern FILE *fdebug;
#define printf(...) fprintf(fdebug,__VA_ARGS__)
#else
 #ifdef GEKKO
 #define printf(...)
 #endif
#endif

/* Returns the bus value when reading a port without a periferial */

inline byte bus_empty () {

	if (ordenador.mode128k != 3)
		return (ordenador.bus_value);
	else
		return (255);	// +2A and +3 returns always 255
}

/* calls all the routines that emulates the computer, runing them for 'tstados'
   tstates */

inline void emulate (int tstados) {

	if((procesador.I>=0x40)&&(procesador.I<=0x7F)) {
		ordenador.screen_snow=1;
	} else
		ordenador.screen_snow=0;
	show_screen (tstados);
	play_ay (tstados);
	play_sound (tstados);
	tape_read (ordenador.tap_file, tstados);
	microdrive_emulate(tstados);
	if (!ordenador.pause) {
		if (ordenador.tape_readed)
			ordenador.sound_bit = 1;
		else
			ordenador.sound_bit = 0;	// if not paused, asign SOUND_BIT the value of tape
	}
}

void computer_init () {

	int bucle;

	ordenador.bus_counter = 0;
	ordenador.port254 = 0;
	ordenador.issue = 3;
	ordenador.mode128k = 0;
	ordenador.joystick[0] = 1; //Kemposton
	ordenador.joystick[1] = 0; // Cursor
	ordenador.rumble[0] = 0;
	ordenador.rumble[1] = 0;

	ordenador.tape_readed = 0;
	ordenador.pause = 1;	// tape stop
	ordenador.tape_fast_load = 1;	// fast load by default
	ordenador.tape_current_mode = TAP_TRASH;
	ordenador.tap_file = NULL;

	ordenador.osd_text[0] = 0;
	ordenador.osd_time = 0;

	ordenador.other_ret = 0;

	ordenador.s8 = ordenador.s9 = ordenador.s10 = ordenador.s11 =
	ordenador.s12 = ordenador.s13 = ordenador.s14 =
	ordenador.s15 = 0xFF;
	ordenador.tab_extended=0;
	ordenador.esc_again=0;

	ordenador.js = 0x00;

	for (bucle = 0; bucle < 16; bucle++)
		ordenador.ay_registers[bucle] = 0;
	ordenador.ay_emul = 0;
	ordenador.aych_a = 0;
	ordenador.aych_b = 0;
	ordenador.aych_c = 0;
	ordenador.aych_n = 0;
	ordenador.aych_envel = 0;
	ordenador.vol_a = 0;
	ordenador.vol_b = 0;
	ordenador.vol_c = 0;
	ordenador.tst_ay = 0;
	ordenador.tst_ay2 = 0;

	ordenador.ayval_a = 0;
	ordenador.ayval_b = 0;
	ordenador.ayval_c = 0;
	ordenador.ayval_n = 0;
	ordenador.ay_envel_value = 0;
	ordenador.ay_envel_way = 0;
	
	ordenador.tape_loop_counter = 0;
	ordenador.kbd_buffer_pointer = 0;
	ordenador.key = SDL_GetKeyState(NULL);
	ordenador.joybuttonkey[0][0]=SDLK_LALT; //Fire button to wiimote1 button A
	ordenador.joybuttonkey[1][0]=SDLK_LALT; //Fire button to wiimote1 button A

}

void computer_set_palete() {

	SDL_Color colores[16];

	if (ordenador.bw==0) {
		// Color mode

		colores[0].r = 0;
		colores[0].g = 0;
		colores[0].b = 0;
		colores[1].r = 0;
		colores[1].g = 0;
		colores[1].b = 192;
		colores[2].r = 192;
		colores[2].g = 0;
		colores[2].b = 0;
		colores[3].r = 192;
		colores[3].g = 0;
		colores[3].b = 192;
		colores[4].r = 0;
		colores[4].g = 192;
		colores[4].b = 0;
		colores[5].r = 0;
		colores[5].g = 192;
		colores[5].b = 192;
		colores[6].r = 192;
		colores[6].g = 192;
		colores[6].b = 0;
		colores[7].r = 192;
		colores[7].g = 192;
		colores[7].b = 192;
		colores[8].r = 0;
		colores[8].g = 0;
		colores[8].b = 0;
		colores[9].r = 0;
		colores[9].g = 0;
		colores[9].b = 255;
		colores[10].r = 255;
		colores[10].g = 0;
		colores[10].b = 0;
		colores[11].r = 255;
		colores[11].g = 0;
		colores[11].b = 255;
		colores[12].r = 0;
		colores[12].g = 255;
		colores[12].b = 0;
		colores[13].r = 0;
		colores[13].g = 255;
		colores[13].b = 255;
		colores[14].r = 255;
		colores[14].g = 255;
		colores[14].b = 0;
		colores[15].r = 255;
		colores[15].g = 255;
		colores[15].b = 255;

		SDL_SetColors (ordenador.screen, colores, 16, 16);	// set 16 colors from the 16th

		if (ordenador.bpp!=1) {
			colors[0]=SDL_MapRGB(screen->format,0,0,0);
			colors[1]=SDL_MapRGB(screen->format,0,0,192);
			colors[2]=SDL_MapRGB(screen->format,192,0,0);
			colors[3]=SDL_MapRGB(screen->format,192,0,192);
			colors[4]=SDL_MapRGB(screen->format,0,192,0);
			colors[5]=SDL_MapRGB(screen->format,0,192,192);
			colors[6]=SDL_MapRGB(screen->format,192,192,0);
			colors[7]=SDL_MapRGB(screen->format,192,192,192);
			colors[8]=SDL_MapRGB(screen->format,0,0,0);
			colors[9]=SDL_MapRGB(screen->format,0,0,255);
			colors[10]=SDL_MapRGB(screen->format,255,0,0);
			colors[11]=SDL_MapRGB(screen->format,255,0,255);
			colors[12]=SDL_MapRGB(screen->format,0,255,0);
			colors[13]=SDL_MapRGB(screen->format,0,255,255);
			colors[14]=SDL_MapRGB(screen->format,255,255,0);
			colors[15]=SDL_MapRGB(screen->format,255,255,255);
		}
	} else {

		// B&W mode

		colores[0].r = 0;
		colores[0].g = 0;
		colores[0].b = 0;

		colores[1].r = 22;
		colores[1].g = 22;
		colores[1].b = 22;

		colores[2].r = 57;
		colores[2].g = 57;
		colores[2].b = 57;

		colores[3].r = 79;
		colores[3].g = 79;
		colores[3].b = 79;

		colores[4].r = 113;
		colores[4].g = 113;
		colores[4].b = 113;

		colores[5].r = 135;
		colores[5].g = 135;
		colores[5].b = 135;

		colores[6].r = 160;
		colores[6].g = 160;
		colores[6].b = 160;

		colores[7].r = 192;
		colores[7].g = 192;
		colores[7].b = 192;

		colores[8].r = 0;
		colores[8].g = 0;
		colores[8].b = 0;

		colores[9].r = 29;
		colores[9].g = 29;
		colores[9].b = 29;

		colores[10].r = 76;
		colores[10].g = 76;
		colores[10].b = 76;

		colores[11].r = 105;
		colores[11].g = 105;
		colores[11].b = 105;

		colores[12].r = 150;
		colores[12].g = 150;
		colores[12].b = 150;

		colores[13].r = 179;
		colores[13].g = 179;
		colores[13].b = 179;

		colores[14].r = 226;
		colores[14].g = 226;
		colores[14].b = 226;

		colores[15].r = 255;
		colores[15].g = 255;
		colores[15].b = 255;

		SDL_SetColors (ordenador.screen, colores, 16, 16);	// set 16 colors from the 16th

		if (ordenador.bpp!=1) {
			colors[0]=SDL_MapRGB(screen->format,0,0,0);
			colors[1]=SDL_MapRGB(screen->format,22,22,22);
			colors[2]=SDL_MapRGB(screen->format,57,57,57);
			colors[3]=SDL_MapRGB(screen->format,79,79,79);
			colors[4]=SDL_MapRGB(screen->format,113,113,113);
			colors[5]=SDL_MapRGB(screen->format,135,135,135);
			colors[6]=SDL_MapRGB(screen->format,160,160,160);
			colors[7]=SDL_MapRGB(screen->format,192,192,192);
			colors[8]=SDL_MapRGB(screen->format,0,0,0);
			colors[9]=SDL_MapRGB(screen->format,29,29,29);
			colors[10]=SDL_MapRGB(screen->format,76,76,76);
			colors[11]=SDL_MapRGB(screen->format,105,105,105);
			colors[12]=SDL_MapRGB(screen->format,150,150,150);
			colors[13]=SDL_MapRGB(screen->format,179,179,179);
			colors[14]=SDL_MapRGB(screen->format,226,226,226);
			colors[15]=SDL_MapRGB(screen->format,255,255,255);
		}
	}

	unsigned int c;

	for(c=0x10;c<60;c++) {
		colors[c]=0x00000000;
	}

	if (ordenador.bpp==1) {
		unsigned int v;
		for (c=0x10;c<0x60;c++) {
			v=c+((c<<8)&0x0000FF00)+((c<<16)&0x00FF0000)+((c<<24)&0xFF000000);
			colors[c-0x10]=v;
		}
	}
	for(c=0;c<64;c++) {
		set_palete_entry((unsigned char)c,ordenador.ulaplus_palete[c]);
	}
}

/* Registers the screen surface where the Spectrum will put the picture,
prepares the palette and creates two arrays (translate and translate2)
that gives the memory address for each scan */

void register_screen (SDL_Surface * pantalla) {

	//int resx,resy;
	int bucle, bucle2, bucle3, bucle4, bucle5;

	// we prepare the scanline transform arrays

	bucle5 = 0;
	for (bucle = 0; bucle < 3; bucle++)
		for (bucle2 = 0; bucle2 < 8; bucle2++)
			for (bucle3 = 0; bucle3 < 8; bucle3++)
				for (bucle4 = 0; bucle4 < 32; bucle4++) {
					ordenador.translate[bucle5] =
						147456 + bucle * 2048 +
						bucle2 * 32 + bucle3 * 256 +
						bucle4;
					ordenador.translate2[bucle5] =
						153600 + bucle * 256 +
						bucle2 * 32 + bucle4;
					bucle5++;
				}
	ordenador.tstados_counter = 0;

	ordenador.screen = pantalla;

	ordenador.border = 0;
	ordenador.currline = 0;
	ordenador.currpix = 0;
	ordenador.flash = 0;

	//resx = ordenador.screen->w;
	//resy = ordenador.screen->h;

	switch (ordenador.zaurus_mini) {
	case 0:
		ordenador.init_line = 0;
		ordenador.next_line = 640;
		ordenador.next_scanline = 640;
		ordenador.first_line = 40;
		ordenador.last_line = 280;
		ordenador.first_pixel = 16;
		ordenador.last_pixel = 336;
		ordenador.next_pixel = 1;
		ordenador.jump_pixel = 16;
		break;
	case 1:
		ordenador.init_line = 65;
		ordenador.next_line = 160;
		ordenador.next_scanline = 160;
		ordenador.first_line = 40;
		ordenador.last_line = 280;
		ordenador.first_pixel = 0;
		ordenador.last_pixel = 351;
		ordenador.next_pixel = 1;
		ordenador.jump_pixel = 8;
		break;
	case 2:
		ordenador.init_line = 479;
		ordenador.next_line = -(307202);
		ordenador.next_scanline = -1;
		ordenador.first_line = 40;
		ordenador.last_line = 280;
		ordenador.first_pixel = 16;
		ordenador.last_pixel = 336;
		ordenador.next_pixel = 480;
		ordenador.jump_pixel = 7680;
		break;
	case 3:
		ordenador.init_line = 0;
		ordenador.next_line = 0;
		ordenador.next_scanline = 0;
		ordenador.first_line = 40;
		ordenador.last_line = 280;
		ordenador.first_pixel = 0;
		ordenador.last_pixel = 319;
		ordenador.next_pixel = 1;
		ordenador.jump_pixel = 4;
		break;
	}
	
	ordenador.next_line*=ordenador.bpp;
	ordenador.next_scanline*=ordenador.bpp;
	ordenador.init_line*=ordenador.bpp;
	ordenador.next_pixel*=ordenador.bpp;
	ordenador.jump_pixel*=ordenador.bpp;
	
	computer_set_palete();

	ordenador.pixel = ((unsigned char *) (ordenador.screen->pixels)) +	ordenador.init_line;
	ordenador.interr = 0;

	ordenador.p_translt = ordenador.translate;
	ordenador.p_translt2 = ordenador.translate2;

	ordenador.contador_flash = 0;
	ordenador.readed = 0;

	ordenador.contended_zone=0;
	ordenador.cicles_counter=0;
	
	ordenador.tstados_counter_sound = 0;
	ordenador.current_buffer = sound[0];
	ordenador.num_buff = 0;	// first buffer
	ordenador.sound_cuantity = 0;
	ordenador.sound_current_value = 0;
	ordenador.pixancho = 447;
	ordenador.pixalto = 311;	// values for 48K mode
}

void set_memory_pointers () {

	static unsigned int rom, ram;

	// assign the offset for video page

	if (ordenador.mport1 & 0x08)
		ordenador.video_offset = 32768;	// page 7
	else
		ordenador.video_offset = 0;	// page 5

	// assign ROMs and, if in special mode, RAM for the whole blocks

	if ((ordenador.mode128k == 3)) {
		if (ordenador.mport2 & 0x01) {		// +2A/+3 special mode
			ram = (unsigned int) (ordenador.mport1 & 0x06);	// bits 1&2
			switch (ram) {
			case 0:
				ordenador.block0 = ordenador.memoria + 65536;
				ordenador.block1 = ordenador.memoria + 65536;
				ordenador.block2 = ordenador.memoria + 65536;
				ordenador.block3 = ordenador.memoria + 65536;
				break;
			case 2:
				ordenador.block0 = ordenador.memoria + 131072;
				ordenador.block1 = ordenador.memoria + 131072;
				ordenador.block2 = ordenador.memoria + 131072;
				ordenador.block3 = ordenador.memoria + 131072;
				break;
			case 4:
				ordenador.block0 = ordenador.memoria + 131072;
				ordenador.block1 = ordenador.memoria + 131072;
				ordenador.block2 = ordenador.memoria + 131072;
				ordenador.block3 = ordenador.memoria + 65536;
				break;
			case 6:
				ordenador.block0 = ordenador.memoria + 131072;
				ordenador.block1 = ordenador.memoria + 163840;
				ordenador.block2 = ordenador.memoria + 131072;
				ordenador.block3 = ordenador.memoria + 65536;
				break;
			}
			return;
		} else {		// ROMs for +2A/+3 normal mode
			rom = 0;
			if (ordenador.mport1 & 0x10)
				rom++;
			if (ordenador.mport2 & 0x04)
				rom += 2;
			// assign the first block pointer to the right block bank
			ordenador.block0 = ordenador.memoria + (16384 * rom);
		}
	} else {			// ROMs for 128K/+2 mode
		if (ordenador.mport1 & 0x10)
			ordenador.block0 = ordenador.memoria + 16384;
		else
			ordenador.block0 = ordenador.memoria;
	}

	// RAMs for 128K/+2 mode, and +2A/+3 in normal mode

	ordenador.block1 = ordenador.memoria + 131072;	// page 5 minus 16384
	ordenador.block2 = ordenador.memoria + 65536;	// page 2 minus 32768

	ram = 1 + ((unsigned int) (ordenador.mport1 & 0x07));	// RAM page for block3 plus 1
	ordenador.block3 = ordenador.memoria + (16384 * ram);	// page n minus 49152
}

/* Paints the spectrum screen during the TSTADOS tstates that the Z80 used
to execute last instruction */

inline void show_screen (int tstados) {

	static unsigned char temporal, ink, paper, fflash, tmp2;

	ordenador.tstados_counter += tstados;
	ordenador.cicles_counter += tstados;

	if (curr_frames!=jump_frames) {
		if (ordenador.tstados_counter>=69888) {
			ordenador.tstados_counter-=69888;
			ordenador.interr = 1;
			curr_frames++;
		}
		return;
	}
	
	fflash = 0; // flash flag
	while (ordenador.tstados_counter > 3) {
		ordenador.tstados_counter -= 4;

		// test if current pixel is for border or for user area

		if ((ordenador.currline < 64) || (ordenador.currline > 255)
			|| (ordenador.currpix < 48) || (ordenador.currpix > 303)) {
			
			// is border
				
			ordenador.contended_zone=0; // no contention here
			if (ordenador.ulaplus) {
				paint_pixels (255, ordenador.border+24, 0);	// paint 8 pixels with BORDER color
			} else {
				paint_pixels (255, ordenador.border, 0);	// paint 8 pixels with BORDER color
			}

			ordenador.bus_value = 255;

		} else {

			// is user area. We search for ink and paper colours

			ordenador.contended_zone=1; // can have contention
			
			temporal = ordenador.memoria[(*ordenador.p_translt2) + ordenador.video_offset];	// attributes
			ordenador.bus_value = temporal;
			ink = temporal & 0x07;	// ink colour
			paper = (temporal >> 3) & 0x07;	// paper colour
			if (ordenador.ulaplus) {
				tmp2=0x10+((temporal>>2)&0x30);
				ink+=tmp2;
				paper+=8+tmp2;
			} else {
				if (temporal & 0x40) {	// bright flag?
					ink += 8;
					paper += 8;
				}
				fflash = temporal & 0x80;	// flash flag
			}

			// Snow Effect

			if(ordenador.screen_snow) {
				temporal = ordenador.memoria[(((*ordenador.p_translt) + (ordenador.video_offset))&0xFFFFFF00)+(procesador.R)];	// data with snow
				ordenador.screen_snow=0; // no more snow for now
			} else
				temporal = ordenador.memoria[(*ordenador.p_translt) + ordenador.video_offset];	// data
			
			ordenador.p_translt++;
			ordenador.p_translt2++;
			if ((fflash) && (ordenador.flash))
				paint_pixels (temporal, paper, ink);	// if FLASH, invert PAPER and INK
			else
				paint_pixels (temporal, ink, paper);
		}
		ordenador.currpix += 8;
		if (ordenador.currpix > ordenador.pixancho) {
			ordenador.currpix = 0;
			ordenador.currline++;
			if (ordenador.currline > ordenador.first_line) {	// ordenador.first_line)
				ordenador.pixel += ordenador.next_line;	// ordenador.next_line;
			}
		}
		
		if ((ordenador.currline > ordenador.pixalto)&&(ordenador.currpix>=64)) {
			ordenador.currpix=64;
			if (ordenador.osd_time) {
				ordenador.osd_time--;
				if (ordenador.osd_time==0) {
					ordenador.tab_extended=0;
					ordenador.esc_again=0;
				}
					
				if (ordenador.osd_time)
					print_string (ordenador.screenbuffer,ordenador.osd_text, -1,450, 12, 0,ordenador.screen_width);
				else {
					if (ordenador.zaurus_mini==0)
						print_string (ordenador.screenbuffer,"                                      ",-1, 450, 12, 0,ordenador.screen_width);
					else
						print_string (ordenador.screenbuffer,"                            ",-1, 450, 12, 0,ordenador.screen_width);
				}
			}
				
			if (ordenador.mustlock) {
				SDL_UnlockSurface (ordenador.screen);
				SDL_Flip (ordenador.screen);
				SDL_LockSurface (ordenador.screen);
			} else {
				SDL_Flip (ordenador.screen);
			}
			
			curr_frames=0;
			ordenador.currline = 0;
			ordenador.interr = 1;
			ordenador.cicles_counter=0;
			ordenador.pixel = ((unsigned char *) (ordenador.screen->pixels))+ordenador.init_line;	// +ordenador.init_line;
			ordenador.p_translt = ordenador.translate;
			ordenador.p_translt2 = ordenador.translate2;
			ordenador.contador_flash++;
			if (ordenador.contador_flash == 16) {
				ordenador.flash = 1 - ordenador.flash;
				ordenador.contador_flash = 0;
			}
		}
	}
}


/* PAINT_PIXELS paints one byte with INK color for 1 bits and PAPER color
for 0 bits, and increment acordingly the pointer PIXEL */

inline void paint_pixels (unsigned char octet,unsigned char ink, unsigned char paper) {

	static int bucle,valor,*p;
	static unsigned char mask;

	if ((ordenador.currpix < 16) || (ordenador.currpix >= 336)
	    || (ordenador.currline < 40) || (ordenador.currline >= 280))
		return;

	mask = 0x80;
	for (bucle = 0; bucle < 8; bucle++) {
		valor = (octet & mask) ? (int) ink : (int) paper;
		p=(colors+valor);
		
		paint_one_pixel((unsigned char *)p,ordenador.pixel);
		if ((ordenador.zaurus_mini!=1)&&(ordenador.zaurus_mini!=3)&&(ordenador.dblscan)) {
			paint_one_pixel((unsigned char *)p,ordenador.pixel+ordenador.next_scanline);
		}
		ordenador.pixel+=ordenador.next_pixel;
		if ((ordenador.zaurus_mini!=1)&&(ordenador.zaurus_mini!=3)) {
			paint_one_pixel((unsigned char *)p,ordenador.pixel);
			if (ordenador.dblscan) {
				paint_one_pixel((unsigned char *)p,ordenador.pixel+ordenador.next_scanline);
			}
			ordenador.pixel+=ordenador.next_pixel;
		}
		mask = ((mask >> 1) & 0x7F);
	}
}

inline void paint_one_pixel(unsigned char *colour,unsigned char *address) {

	#if BYTE_ORDER == LITTLE_ENDIAN
	switch(ordenador.bpp) {
	case 1:
		*address=*colour;
	break;
	case 3:
		*(address++)=*(colour++);
	case 2:
		*(address++)=*(colour++);
		*(address++)=*(colour++);
	break;
	case 4:
		*((unsigned int *)address)=*((unsigned int *)colour);
	break;
	}
	#else //BIG ENDIAN
	switch(ordenador.bpp) {
	case 1:
		*address=*(colour+3);
	break;
	case 3:
		*(address++)=*(colour+1);
	case 2:
		*(address++)=*(colour+2);
		*(address++)=*(colour+3);
	break;
	case 4:
		*((unsigned int *)address)=*((unsigned int *)colour);
	break;
	}
	#endif
	
}

// Read the keyboard and stores the flags

inline void read_keyboard () {

	unsigned int temporal_io;
	SDL_Event evento,*pevento;
	enum joystate_x {JOY_CENTER_X, JOY_LEFT, JOY_RIGHT};
	enum joystate_y {JOY_CENTER_Y, JOY_UP, JOY_DOWN};
	int joy_axis_x[2],joy_axis_y[2], joy_n, joybutton_n; 
	static unsigned char joybutton_matrix[2][322];
	
	ordenador.k8 = ordenador.k9 = ordenador.k10 = ordenador.k11 =
		ordenador.k12 = ordenador.k13 = ordenador.k14 =
		ordenador.k15 = 0;
		ordenador.jk = 0;
	
	pevento=&evento;
	SDL_PollEvent (&evento);

	if (pevento->type==SDL_QUIT) {
		salir = 0;
		return;
	}
	
	
	SDL_JoystickUpdate();
	for(joy_n=0;joy_n<ordenador.joystick_number;joy_n++) 
	{
	joy_axis_x[joy_n] = SDL_JoystickGetAxis(ordenador.joystick_sdl[joy_n], 0);
	joy_axis_y[joy_n] = SDL_JoystickGetAxis(ordenador.joystick_sdl[joy_n], 1);
	//ordenador.joy_fire[joy_n] = SDL_JoystickGetButton(ordenador.joystick_sdl[joy_n], 0); //Wii button "A"
	
	if (SDL_JoystickGetButton(ordenador.joystick_sdl[joy_n], 6) ||
	SDL_JoystickGetButton(ordenador.joystick_sdl[joy_n], 19)) main_menu(); //Wii button "Home"
	
	if (SDL_JoystickGetButton(ordenador.joystick_sdl[joy_n], 5) ||
	SDL_JoystickGetButton(ordenador.joystick_sdl[joy_n], 18)) virtual_keyboard(); //Wii  button "+"
	
	if (joy_axis_x[joy_n] > 16384) ordenador.joy_axis_x_state[joy_n] = JOY_RIGHT; 
	else if (joy_axis_x[joy_n] < -16384) ordenador.joy_axis_x_state[joy_n] = JOY_LEFT;
		else ordenador.joy_axis_x_state[joy_n] = JOY_CENTER_X;
	
	if (joy_axis_y[joy_n] > 16384) ordenador.joy_axis_y_state[joy_n] = JOY_DOWN; 
	else if (joy_axis_y[joy_n] < -16384) ordenador.joy_axis_y_state[joy_n] = JOY_UP;
		else ordenador.joy_axis_y_state[joy_n] = JOY_CENTER_Y;
		
	for(joybutton_n=0;joybutton_n<5;joybutton_n++)
		{
		joybutton_matrix[joy_n][(ordenador.joybuttonkey[joy_n][joybutton_n])] = 
		SDL_JoystickGetButton(ordenador.joystick_sdl[joy_n], joybutton_n);
		}
		
	for(joybutton_n=7;joybutton_n<18;joybutton_n++)
		{
		joybutton_matrix[joy_n][(ordenador.joybuttonkey[joy_n][joybutton_n])] = 
		SDL_JoystickGetButton(ordenador.joystick_sdl[joy_n], joybutton_n);
		}	
	}
	
	//Keyboard buffer
	
	if (countdown)
		{ 
		if (countdown <5) 
			{
			ordenador.key[(ordenador.keyboard_buffer[0][ordenador.kbd_buffer_pointer+1])]=0;
			ordenador.key[(ordenador.keyboard_buffer[1][ordenador.kbd_buffer_pointer+1])]=0;
			ordenador.keyboard_buffer[0][ordenador.kbd_buffer_pointer+1] = 0;
			ordenador.keyboard_buffer[1][ordenador.kbd_buffer_pointer+1] = 0;
			}
		countdown--;
		}
		else if (ordenador.kbd_buffer_pointer)
			{
			ordenador.key[(ordenador.keyboard_buffer[0][ordenador.kbd_buffer_pointer])]=1;
			ordenador.key[(ordenador.keyboard_buffer[1][ordenador.kbd_buffer_pointer])]=1;
		
			ordenador.kbd_buffer_pointer--; 
			countdown=7;		
			}	

	
	temporal_io = (unsigned int) pevento->key.keysym.sym;

	if (pevento->type == SDL_KEYDOWN)
		switch (temporal_io) {
		case SDLK_ESCAPE:	// to exit from the emulator
			if (ordenador.esc_again==0) {
				ordenador.esc_again=1;
				strcpy(ordenador.osd_text,"ESC again to exit");
				ordenador.osd_time=100;
			} else
				salir = 0;
			return;
			break;
		case SDLK_F1:
			help_menu ();	// shows the help menu
			break;

		case SDLK_F2:
		case SDLK_F3:
		case SDLK_F4:
		case SDLK_F7:
		case SDLK_F8:
			launch_menu(temporal_io);
			break;

		case SDLK_F5:   // STOP tape
			if ((ordenador.tape_fast_load == 0) || (ordenador.tape_file_type==TAP_TZX))
				ordenador.pause = 1;
			break;

		case SDLK_F6:	// PLAY tape
			if ((ordenador.tape_fast_load == 0) || (ordenador.tape_file_type==TAP_TZX))
				ordenador.pause = 0;
			break;		

		case SDLK_F9:
			//Emulate load ""
			ordenador.keyboard_buffer[0][6]= SDLK_1;		//Edit
			ordenador.keyboard_buffer[1][6]= SDLK_LSHIFT;
			ordenador.keyboard_buffer[0][5]= SDLK_j;		//Load
			ordenador.keyboard_buffer[1][5]= 0;
			ordenador.keyboard_buffer[0][4]= SDLK_p;		//"
			ordenador.keyboard_buffer[1][4]= SDLK_LCTRL;
			ordenador.keyboard_buffer[0][3]= SDLK_p;		//"
			ordenador.keyboard_buffer[1][3]= SDLK_LCTRL;
			ordenador.keyboard_buffer[0][2]= SDLK_RETURN;	// Return
			ordenador.keyboard_buffer[1][2]= 0;
			ordenador.keyboard_buffer[0][1]= SDLK_F6;		//F6
			ordenador.keyboard_buffer[1][1]= 0;
			ordenador.kbd_buffer_pointer=6;
			countdown=7;
			break;

		case SDLK_F10:	// Reset emulator
			ResetComputer ();
			ordenador.pause = 1;
			if (ordenador.tap_file != NULL) {
				ordenador.tape_current_mode = TAP_TRASH;
				rewind_tape (ordenador.tap_file,1);				
			}
		break;

		case SDLK_F11:	// lower volume
			if (ordenador.volume > 3)
				set_volume (ordenador.volume - 4);
			sprintf (ordenador.osd_text, " Volume: %d ",ordenador.volume / 4);
			ordenador.osd_time = 50;
		break;
			
		case SDLK_F12:	// upper volume
			set_volume (ordenador.volume + 4);
			sprintf (ordenador.osd_text, " Volume: %d ",ordenador.volume / 4);
			ordenador.osd_time = 50;
			break;
		}

	// reorder joystick if screen is rotated
		
	if(ordenador.zaurus_mini==2) {
		switch(temporal_io) {
		case SDLK_UP:
			temporal_io=SDLK_LEFT;
		break;
		case SDLK_LEFT:
			temporal_io=SDLK_DOWN;
		break;
		case SDLK_DOWN:
			temporal_io=SDLK_RIGHT;
		break;
		case SDLK_RIGHT:
			temporal_io=SDLK_UP;
		break;
		}
	}
	for(joy_n=0;joy_n<ordenador.joystick_number;joy_n++)
	{
	 switch (ordenador.joystick[joy_n]) {
		case 0:	// cursor
			if (ordenador.joy_axis_y_state[joy_n] == JOY_UP) ordenador.k12|= 8;
			if (ordenador.joy_axis_y_state[joy_n] == JOY_DOWN) ordenador.k12|= 16;
			if (ordenador.joy_axis_x_state[joy_n] == JOY_RIGHT)ordenador.k12|= 4;
			if (ordenador.joy_axis_x_state[joy_n] == JOY_LEFT) ordenador.k11|= 16;
			if (joybutton_matrix[joy_n][SDLK_LALT]) ordenador.k12|= 1; //fire button
		break; 
			
		case 1: //Kempston
			if (ordenador.joy_axis_y_state[joy_n] == JOY_UP) ordenador.jk|= 8;
			if (ordenador.joy_axis_y_state[joy_n] == JOY_DOWN) ordenador.jk|= 4;
			if (ordenador.joy_axis_x_state[joy_n] == JOY_RIGHT) ordenador.jk|= 1;
			if (ordenador.joy_axis_x_state[joy_n] == JOY_LEFT) ordenador.jk|= 2;
			if (joybutton_matrix[joy_n][SDLK_LALT]) ordenador.jk |= 16; //fire button
		break;
		
		case 2:	// sinclair 1
			if (ordenador.joy_axis_y_state[joy_n] == JOY_UP) ordenador.k11|= 8;
			if (ordenador.joy_axis_y_state[joy_n] == JOY_DOWN)ordenador.k11|= 4;
			if (ordenador.joy_axis_x_state[joy_n] == JOY_RIGHT)ordenador.k11|= 2;
			if (ordenador.joy_axis_x_state[joy_n] == JOY_LEFT) ordenador.k11|= 1;
			if (joybutton_matrix[joy_n][SDLK_LALT]) ordenador.k11|= 16;	//fire button
		break;
		
		case 3:	// sinclair 2
			if (ordenador.joy_axis_y_state[joy_n] == JOY_UP) ordenador.k12|= 2;
			if (ordenador.joy_axis_y_state[joy_n] == JOY_DOWN)ordenador.k12|= 4;
			if (ordenador.joy_axis_x_state[joy_n] == JOY_RIGHT)ordenador.k12|= 8;
			if (ordenador.joy_axis_x_state[joy_n] == JOY_LEFT) ordenador.k12|= 16;
			if (joybutton_matrix[joy_n][SDLK_LALT]) ordenador.k12|= 1; //fire button
		break;
		}
	}	
			
	if (ordenador.key[SDLK_SPACE]) ordenador.k15|=1;
	if (ordenador.key[SDLK_RCTRL]||ordenador.key[SDLK_LCTRL]) ordenador.k15|=2; //Symbol shift
	if ((ordenador.key[SDLK_m] || joybutton_matrix[0][SDLK_m] || joybutton_matrix[1][SDLK_m])) ordenador.k15|=4;
	if ((ordenador.key[SDLK_n] || joybutton_matrix[0][SDLK_n] || joybutton_matrix[1][SDLK_n])) ordenador.k15|=8;
	if ((ordenador.key[SDLK_b] || joybutton_matrix[0][SDLK_b] || joybutton_matrix[1][SDLK_b])) ordenador.k15|=16;
	if (ordenador.key[SDLK_PERIOD]) ordenador.k15|=6;
	if (ordenador.key[SDLK_COMMA]) ordenador.k15|=10;					
					
	if ((ordenador.key[SDLK_RETURN] || joybutton_matrix[0][SDLK_RETURN] || joybutton_matrix[1][SDLK_RETURN])) ordenador.k14|=1;
	if ((ordenador.key[SDLK_l] || joybutton_matrix[0][SDLK_l] || joybutton_matrix[1][SDLK_l])) ordenador.k14|=2;
	if ((ordenador.key[SDLK_k] || joybutton_matrix[0][SDLK_k] || joybutton_matrix[1][SDLK_k])) ordenador.k14|=4;
	if ((ordenador.key[SDLK_j] || joybutton_matrix[0][SDLK_j] || joybutton_matrix[1][SDLK_j])) ordenador.k14|=8;
	if ((ordenador.key[SDLK_h] || joybutton_matrix[0][SDLK_h] || joybutton_matrix[1][SDLK_h])) ordenador.k14|=16;					
					
	if ((ordenador.key[SDLK_p] || joybutton_matrix[0][SDLK_p] || joybutton_matrix[1][SDLK_p])) ordenador.k13|=1;
	if ((ordenador.key[SDLK_o] || joybutton_matrix[0][SDLK_o] || joybutton_matrix[1][SDLK_o])) ordenador.k13|=2;
	if ((ordenador.key[SDLK_i] || joybutton_matrix[0][SDLK_i] || joybutton_matrix[1][SDLK_i])) ordenador.k13|=4;
	if ((ordenador.key[SDLK_u] || joybutton_matrix[0][SDLK_u] || joybutton_matrix[1][SDLK_u])) ordenador.k13|=8;
	if ((ordenador.key[SDLK_y] || joybutton_matrix[0][SDLK_y] || joybutton_matrix[1][SDLK_y])) ordenador.k13|=16;					

	if ((ordenador.key[SDLK_0] || joybutton_matrix[0][SDLK_0] || joybutton_matrix[1][SDLK_0])) ordenador.k12|=1;
	if ((ordenador.key[SDLK_9] || joybutton_matrix[0][SDLK_9] || joybutton_matrix[1][SDLK_9])) ordenador.k12|=2;
	if ((ordenador.key[SDLK_8] || joybutton_matrix[0][SDLK_8] || joybutton_matrix[1][SDLK_8])) ordenador.k12|=4;
	if ((ordenador.key[SDLK_7] || joybutton_matrix[0][SDLK_7] || joybutton_matrix[1][SDLK_7])) ordenador.k12|=8;
	if ((ordenador.key[SDLK_6] || joybutton_matrix[0][SDLK_6] || joybutton_matrix[1][SDLK_6])) ordenador.k12|=16;
	if ((ordenador.key[SDLK_BACKSPACE] || joybutton_matrix[0][SDLK_BACKSPACE] || joybutton_matrix[1][SDLK_BACKSPACE])) {ordenador.k12|=1; ordenador.k8 |=1;}	
					
	if ((ordenador.key[SDLK_1] || joybutton_matrix[0][SDLK_1] || joybutton_matrix[1][SDLK_1])) ordenador.k11|=1;
	if ((ordenador.key[SDLK_2] || joybutton_matrix[0][SDLK_2] || joybutton_matrix[1][SDLK_2])) ordenador.k11|=2;
	if ((ordenador.key[SDLK_3] || joybutton_matrix[0][SDLK_3] || joybutton_matrix[1][SDLK_3])) ordenador.k11|=4;
	if ((ordenador.key[SDLK_4] || joybutton_matrix[0][SDLK_4] || joybutton_matrix[1][SDLK_4])) ordenador.k11|=8;
	if ((ordenador.key[SDLK_5] || joybutton_matrix[0][SDLK_5] || joybutton_matrix[1][SDLK_5])) ordenador.k11|=16;					

	if ((ordenador.key[SDLK_q] || joybutton_matrix[0][SDLK_q] || joybutton_matrix[1][SDLK_q])) ordenador.k10|=1;
	if ((ordenador.key[SDLK_w] || joybutton_matrix[0][SDLK_w] || joybutton_matrix[1][SDLK_w])) ordenador.k10|=2;
	if ((ordenador.key[SDLK_e] || joybutton_matrix[0][SDLK_e] || joybutton_matrix[1][SDLK_e])) ordenador.k10|=4;
	if ((ordenador.key[SDLK_r] || joybutton_matrix[0][SDLK_r] || joybutton_matrix[1][SDLK_r])) ordenador.k10|=8;
	if ((ordenador.key[SDLK_t] || joybutton_matrix[0][SDLK_t] || joybutton_matrix[1][SDLK_t])) ordenador.k10|=16;
					
	if ((ordenador.key[SDLK_a] || joybutton_matrix[0][SDLK_a] || joybutton_matrix[1][SDLK_a])) ordenador.k9 |=1;
	if ((ordenador.key[SDLK_s] || joybutton_matrix[0][SDLK_s] || joybutton_matrix[1][SDLK_s])) ordenador.k9 |=2;
	if ((ordenador.key[SDLK_d] || joybutton_matrix[0][SDLK_d] || joybutton_matrix[1][SDLK_d])) ordenador.k9 |=4;
	if ((ordenador.key[SDLK_f] || joybutton_matrix[0][SDLK_f] || joybutton_matrix[1][SDLK_f])) ordenador.k9 |=8;
	if ((ordenador.key[SDLK_g] || joybutton_matrix[0][SDLK_g] || joybutton_matrix[1][SDLK_g])) ordenador.k9 |=16;					
					
	if ((ordenador.key[SDLK_RSHIFT]||ordenador.key[SDLK_LSHIFT])) ordenador.k8 |=1; //Caps shift
	if ((ordenador.key[SDLK_z] || joybutton_matrix[0][SDLK_z] || joybutton_matrix[1][SDLK_z])) ordenador.k8 |=2;
	if ((ordenador.key[SDLK_x] || joybutton_matrix[0][SDLK_x] || joybutton_matrix[1][SDLK_x])) ordenador.k8 |=4;
	if ((ordenador.key[SDLK_c] || joybutton_matrix[0][SDLK_c] || joybutton_matrix[1][SDLK_c])) ordenador.k8 |=8;
	if ((ordenador.key[SDLK_v] || joybutton_matrix[0][SDLK_v] || joybutton_matrix[1][SDLK_v])) ordenador.k8 |=16;

	if (ordenador.key[SDLK_UP]) {ordenador.k12 |=8;ordenador.k8|=1;}
	if (ordenador.key[SDLK_DOWN]) {ordenador.k12 |=16;ordenador.k8|=1;}
	if (ordenador.key[SDLK_LEFT]) {ordenador.k11 |=16;ordenador.k8|=1;}
	if (ordenador.key[SDLK_RIGHT]) {ordenador.k12 |=4;ordenador.k8|=1;}
	
	if (ordenador.key[SDLK_TAB]) {ordenador.k15|=2;ordenador.k8|=1;} //Extended mode

		ordenador.s8 = (ordenador.s8 & 0xE0) | (ordenador.k8 ^ 0x1F);
		ordenador.s9 = (ordenador.s9 & 0xE0) | (ordenador.k9 ^ 0x1F);
		ordenador.s10 = (ordenador.s10 & 0xE0)| (ordenador.k10 ^ 0x1F);
		ordenador.s11 = (ordenador.s11 & 0xE0)| (ordenador.k11 ^ 0x1F);
		ordenador.s12 = (ordenador.s12 & 0xE0)| (ordenador.k12 ^ 0x1F);
		ordenador.s13 = (ordenador.s13 & 0xE0)| (ordenador.k13 ^ 0x1F);
		ordenador.s14 = (ordenador.s14 & 0xE0)| (ordenador.k14 ^ 0x1F);
		ordenador.s15 = (ordenador.s15 & 0xE0)| (ordenador.k15 ^ 0x1F);
		ordenador.js = ordenador.jk;
	
	return;
}

// resets the computer and loads the right ROMs

void ResetComputer () {

	static int bucle;

	Z80free_reset (&procesador);
	load_rom (ordenador.mode128k);

	// reset the AY-3-8912

	for (bucle = 0; bucle < 16; bucle++)
		ordenador.ay_registers[bucle] = 0;

	ordenador.aych_a = 0;
	ordenador.aych_b = 0;
	ordenador.aych_c = 0;
	ordenador.aych_n = 0;
	ordenador.aych_envel = 0;
	ordenador.vol_a = 0;
	ordenador.vol_b = 0;
	ordenador.vol_c = 0;

	ordenador.s8|=0x1F;
	ordenador.s9|=0x1F;
	ordenador.s10|=0x1F;
	ordenador.s11|=0x1F;
	ordenador.s12|=0x1F;
	ordenador.s13|=0x1F;
	ordenador.s14|=0x1F;
	ordenador.s15|=0x1F;
	ordenador.js=0;

	ordenador.updown=0;
	ordenador.leftright=0;

	ordenador.ulaplus=0;

	ordenador.mport1 = 0;
	ordenador.mport2 = 0;
	ordenador.video_offset = 0;	// video in page 9 (page 5 in 128K)
	switch (ordenador.mode128k) {
	case 0:		// 48K
		ordenador.pixancho = 447;
		ordenador.pixalto = 311;

		ordenador.block0 = ordenador.memoria;
		ordenador.block1 = ordenador.memoria + 131072;	// video mem. in page 9 (page 5 in 128K)
		ordenador.block2 = ordenador.memoria + 65536;	// 2nd block in page 6 (page 2 in 128K)
		ordenador.block3 = ordenador.memoria + 65536;	// 3rd block in page 7 (page 3 in 128K)
		ordenador.mport1 = 32;	// access to port 7FFD disabled
	break;
	
	case 3:		// +2A/+3
		Z80free_Out (0x1FFD, 0);
	case 1:		// 128K
	case 2:		// +2
	case 4:		// spanish 128K
		Z80free_Out (0x7FFD, 0);
		ordenador.pixancho = 455;
		ordenador.pixalto = 310;
	break;
	}
	
	microdrive_reset();
}

// check if there's contention and waits the right number of tstates

void do_contention() {
	
	if (!ordenador.contended_zone)
		return;
	
	if (ordenador.cicles_counter<14335) {
		return;
	}

	int ccicles=(ordenador.cicles_counter-14335)%8;

	if (ccicles>5) {
		return;
	}
	
	emulate(6-ccicles);

}

void Z80free_Wr (register word Addr, register byte Value) {

	switch (Addr & 0xC000) {
	case 0x0000:
	// only writes in the first 16K if we are in +3 mode and bit0 of mport2 is 1

		if ((ordenador.mode128k == 3) && (1 == (ordenador.mport2 & 0x01)))
			*(ordenador.block0 + Addr) = (unsigned char) Value;
	break;

	case 0x4000:
		do_contention();
		*(ordenador.block1 + Addr) = (unsigned char) Value;
	break;
	
	case 0x8000:
		*(ordenador.block2 + Addr) = (unsigned char) Value;
	break;
	
	case 0xC000:
		*(ordenador.block3 + Addr) = (unsigned char) Value;
	break;
	}
}


byte Z80free_Rd (register word Addr) {

	if((ordenador.mdr_active)&&(ordenador.mdr_paged)&&(Addr<8192)) // Interface I
		return((byte)ordenador.shadowrom[Addr]);
	
	switch (ordenador.other_ret) {
	case 1:
		ordenador.other_ret = 0;
		return (201);	// RET instruction
	break;

	default:
		switch (Addr & 0xC000) {
		case 0x0000:
			return ((byte) (*(ordenador.block0 + Addr)));
		break;

		case 0x4000:
			do_contention();
			return ((byte) (*(ordenador.block1 + Addr)));
		break;

		case 0x8000:
			return ((byte) (*(ordenador.block2 + Addr)));
		break;

		case 0xC000:
			return ((byte) (*(ordenador.block3 + Addr)));
		break;

		default:
			printf ("Memory error\n");
			exit (1);
			return 0;
		}
		break;
	}
}

void set_palete_entry(unsigned char entry, byte Value) {


	SDL_Color color;

	color.r = ((Value<<3)&0xE0)+((Value)&0x1C)+((Value>>3)&0x03);
	color.g = (Value&0xE0)+((Value>>3)&0x1C)+((Value>>6)&0x03);
	color.b = ((Value<<6)&0xC0)+((Value<<4)&0x30)+((Value<<2)&0x0C)+((Value)&0x03);

	if (ordenador.bw!=0) {
		int final;
		final=(((int)color.r)*3+((int)color.g)*6+((int)color.b))/10;
		color.r=color.g=color.b=(unsigned char)final;
	}
	// Color mode

	SDL_SetColors (ordenador.screen, &color, 32+entry, 1); // set 16 colors from the 16th

	if (ordenador.bpp!=1) {
		colors[entry+16]=SDL_MapRGB(screen->format,color.r,color.g,color.b);
	}
}

void Z80free_Out (register word Port, register byte Value) {

	// Microdrive access
	
	register word maskport;
	
	if (((Port&0x0001)==0)||((Port>=0x4000)&&(Port<0x8000))) {
		do_contention();
	}

	// ULAPlus
	if (Port == 0xBF3B) {
		ordenador.ulaplus_reg = Value;
		return;
	}
	if (Port == 0xFF3B) {
		if (ordenador.ulaplus_reg==0x40) { // mode
			ordenador.ulaplus=Value&0x01;
			return;
		}
		if (ordenador.ulaplus_reg<0x40) { // register set mode
			ordenador.ulaplus_palete[ordenador.ulaplus_reg]=Value;
			set_palete_entry(ordenador.ulaplus_reg,Value);
		}
	}

	if(((Port &0x0018)!=0x0018)&&(ordenador.mdr_active))
		microdrive_out(Port,Value);
	
	// ULA port (A0 low)

	if (!(Port & 0x0001)) {
		ordenador.port254 = (unsigned char) Value;
		ordenador.border = (((unsigned char) Value) & 0x07);

		if (ordenador.pause) {
			if (Value & 0x10)
				ordenador.sound_bit = 1;
			else
				ordenador.sound_bit = 0;	// assign to SOUND_BIT the value
		}
	}

	// Memory page (7FFD & 1FFD)

	if (ordenador.mode128k==3) {
		maskport=0x0FFD;
	} else {
		maskport=0x3FFD;
	}
	
	if (((Port|maskport) == 0x7FFD) && (0 == (ordenador.mport1 & 0x20))) {
		ordenador.mport1 = (unsigned char) Value;
		set_memory_pointers ();	// set the pointers
	}

	if (((Port|maskport) == 0x1FFD) && (0 == (ordenador.mport1 & 0x20))) {
		ordenador.mport2 = (unsigned char) Value;
		set_memory_pointers ();	// set the pointers
	}

	// Sound chip (AY-3-8912)

	if (((Port|maskport) == 0xFFFD)&&(ordenador.ay_emul))
		ordenador.ay_latch = ((unsigned int) (Value & 0x0F));

	if (((Port|maskport) == 0xBFFD)&&(ordenador.ay_emul)) {
		ordenador.ay_registers[ordenador.ay_latch] = (unsigned char) Value;
		if (ordenador.ay_latch == 13)
			ordenador.ay_envel_way = 2;	// start cycle
	}
}


byte Z80free_In (register word Port) {

	static unsigned int temporal_io;
	byte pines;

	if (((Port&0x0001)==0)||((Port>=0x4000)&&(Port<0x8000))) {
		do_contention();
	}

	temporal_io = (unsigned int) Port;

	if (Port == 0xFF3B) {
		if (ordenador.ulaplus_reg==0x40) { // mode
			return(ordenador.ulaplus&0x01);
		}
		if (ordenador.ulaplus_reg<0x40) { // register set mode
			return(ordenador.ulaplus_palete[ordenador.ulaplus_reg]);
		}
	}

	if (!(temporal_io & 0x0001)) {
		pines = 0xBF;	// by default, sound bit is 0
		if (!(temporal_io & 0x0100))
			pines &= ordenador.s8;
		if (!(temporal_io & 0x0200))
			pines &= ordenador.s9;
		if (!(temporal_io & 0x0400))
			pines &= ordenador.s10;
		if (!(temporal_io & 0x0800))
			pines &= ordenador.s11;
		if (!(temporal_io & 0x1000))
			pines &= ordenador.s12;
		if (!(temporal_io & 0x2000))
			pines &= ordenador.s13;
		if (!(temporal_io & 0x4000))
			pines &= ordenador.s14;
		if (!(temporal_io & 0x8000))
			pines &= ordenador.s15;

		if (ordenador.pause) {
			if (ordenador.issue == 2)	{
				if (ordenador.port254 & 0x18)
					pines |= 0x40;
			} else {
				if (ordenador.port254 & 0x10)
					pines |= 0x40;
			}
		} else {
			if (ordenador.tape_readed)
				pines |= 0x40;	// sound input
			else
				pines &= 0xBF;	// sound input
		}
		return (pines);
	}

	// Joystick
	if (!(temporal_io & 0x0020)) {
		if ((ordenador.joystick[0] == 1)||(ordenador.joystick[1] == 1)) {
			return (ordenador.js);
		} else {
			return 0; // if Kempston is not selected, emulate it, but always 0
		}
	}

	if ((temporal_io == 0xFFFD)&&(ordenador.ay_emul))
		return (ordenador.ay_registers[ordenador.ay_latch]);

	// Microdrive access
	
	if(((Port &0x0018)!=0x0018)&&(ordenador.mdr_active))
		return(microdrive_in(Port));
	
	pines=bus_empty();

	return (pines);
}

void set_volume (unsigned char volume) {

	unsigned char vol2;
	int bucle;

	if (volume > 64)
		vol2 = 64;
	else
		vol2 = volume;

	ordenador.volume = vol2;

	for (bucle = 0; bucle < 4; bucle++)	{
		ordenador.sample0[bucle] = 0;
		ordenador.sample1[bucle] = 0;
		ordenador.sample1b[bucle] = 0;
	}

	switch (ordenador.format) {
	case 0: // 8 bits/sample
		ordenador.sample1[0] = 1 * vol2;
		ordenador.sample1[1] = 1 * vol2;
		ordenador.sample1b[0] = 1;
		ordenador.sample1b[1] = 1;
		break;
	case 1: // 16 bits/sample, Little Endian
		ordenador.sample1[0] = 1 * vol2;
		ordenador.sample1[1] = 1 * vol2;
		ordenador.sample1[2] = 1 * vol2;
		ordenador.sample1[3] = 1 * vol2;
		ordenador.sample1b[0] = 1;
		ordenador.sample1b[1] = 1;
		ordenador.sample1b[2] = 1;
		ordenador.sample1b[3] = 1;
		break;
	case 2: // 16 bits/sample, Big Endian
		ordenador.sample1[0] = 1 * vol2;
		ordenador.sample1[1] = 1 * vol2;
		ordenador.sample1[2] = 1 * vol2;
		ordenador.sample1[3] = 1 * vol2;
		ordenador.sample1b[0] = 1;
		ordenador.sample1b[1] = 1;
		ordenador.sample1b[2] = 1;
		ordenador.sample1b[3] = 1;
		break;
	}
}