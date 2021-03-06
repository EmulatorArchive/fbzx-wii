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

#ifndef emulator_h
#define emulator_h

#define NUM_SNDBUF 2
#define MAX_PATH_LENGTH 256

extern char debug_var;

extern SDL_Surface *screen;
extern Z80FREE procesador;
extern struct computer ordenador;
extern unsigned int *sound[NUM_SNDBUF];
extern char path_snaps[MAX_PATH_LENGTH];
extern char path_taps[MAX_PATH_LENGTH];
extern char path_mdrs[MAX_PATH_LENGTH];
extern char path_scr1[MAX_PATH_LENGTH];
extern char path_scr2[MAX_PATH_LENGTH];
extern char path_confs[MAX_PATH_LENGTH];
extern char path_paste[MAX_PATH_LENGTH];
extern char path_copy[MAX_PATH_LENGTH];
extern char path_delete[MAX_PATH_LENGTH];
extern char path_tmp[MAX_PATH_LENGTH];
extern char path_rzx[MAX_PATH_LENGTH];
extern char load_path_snaps[MAX_PATH_LENGTH];
extern char load_path_taps[MAX_PATH_LENGTH];
extern char load_path_scr1[MAX_PATH_LENGTH];
extern char load_path_poke[MAX_PATH_LENGTH];
extern char load_path_rzx[MAX_PATH_LENGTH];
extern char rom_cartridge[MAX_PATH_LENGTH];
extern char pasted_file[MAX_PATH_LENGTH];

extern unsigned int colors[80];
extern unsigned int curr_frames, turbo_n;
extern  int jump_frames;
extern unsigned char sdismount, usbismount, smbismount, tmpismade, ftpismount,networkisinit;

void SDL_Fullscreen_Switch(void);
void load_rom(char);
void load_main_game(char *nombre);
int load_config(struct computer *object, char *filename);
int save_config(struct computer *object, char *filename);
int save_config_game(struct computer *object, char *filename, int overwrite);
FILE *myfopen(char *filename,char *mode);
char *myfile(char *filename);
void init_sdl();
void init_sound();
void init_screen(int resx,int resy,int depth,int fullscreen,int dblbuffer,int hwsurface);
int set_video_mode();
int remove_dir(char *dir);
unsigned char ConnectFTP ();
unsigned char ConnectShare ();
unsigned char InitNetwork();

#endif
