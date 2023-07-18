/*------------------------------------------------------------------
  externs.h: Global Game Variables 

    XINVADERS 3D - 3d Shoot'em up
    Copyright (C) 2000 Don Llopis

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

------------------------------------------------------------------*/

/* general game variables */
typedef struct GAMEVARSSTRUCT GAMEVARS;
struct GAMEVARSSTRUCT
{
   /* obj pointers for rendering */
   OBJECT *gobjs[MAX_OBJECTS];
   int    gobjcount;

   double fps;     /* frames per second */
   double ftime;   /* frame-time in milliseconds as double */
   double fadjust; /* frame-time adjust = rfps / ftime     */
   long   msec;    /* frame-time in milliseconds as int    */
   long   fcount;  /* frames rendered */
   double rfps;    /* reference frames per second */
   double fpsavg;  /* just an approx running fps average   */
   int    fpsfast; /* count-down, we consider fast if zero */

   long sw_t;      /* stop-watch current # of seconds */
   long sw_save;   /* stop-watch pause */

   long win_width;
   long win_height;

   int key_QUIT;
   int key_UP1;
   int key_DOWN1;
   int key_LEFT1;
   int key_RIGHT1;
   int key_FIRE1;
   int key_1PLAYER;
   int key_DUELPLAY;
   int key_DUELPLAY_H;
   int key_DUELPLAY_V;
   int key_UP2;
   int key_DOWN2;
   int key_LEFT2;
   int key_RIGHT2;
   int key_FIRE2;

   /* */
   int formation_zone;
   int ufo_zone;
   int alien_count;

   /* flags */
   int display_fps;
   int paused;
   int gameover;
   int new_level;
   int intro_done;

   long hi_score;
   long pscore1;   /* player's current score */
   long pscore2;
   long pbonus1;   /* player bonus counter */
   long pbonus2;
   int  plives1;   /* player's current # of lives */
   int  plives2;
   int  pblink1;   /* player invunerable */
   int  pblink2;

   int  pduel;     /* one players, or player duel */
   int  pfocus;    /* player focus 0 or 1 */
   int  phorizontal;
   int  pvertical;
   int  *hi_score_c[20];
   int  *pscore_c1[20];
   int  *pscore_c2[20];
   int  x1l, x1r, y1t, y1b, x2l, x2r, y2t, y2b;
};

/* game.c */
extern GAMEVARS gvars, *gv;
/* arrays for both 'about' and 'rules'. Null-terminated
   so that code to handle them only has to be written once. TBB 1.22 */
extern char *game_about_info[];
extern char *game_rules_info[];

/* timer.c : timer(s) */
extern TIMER gtimer, *gt;

/* player.c : the player(s) */
extern OBJECT  *player1, *player2;
extern OBJECT  *pm1, *pm2;

/* alien.c : the aliens, bombs, and ufo */
extern OBJECT  *ufo;
extern OBJECT  the_formation;
extern OBJLIST *af_list;
extern OBJLIST *abombs;
