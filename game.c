/*------------------------------------------------------------------
  game.c:

    XINVADERS 3D - 3d Shoot'em up
    Copyright (C) 2000 Don Llopis
    +Copyright(C) 2023 Jose Da Silva
    

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
#include "game.h"
#include "gtext.h"
#include <unistd.h>

#define SW_UPDATE  gv->sw_t += gv->msec
#define SW_PAUSE   gv->sw_save = gv->sw_t
#define SW_UNPAUSE gv->sw_t = gv->sw_save
#define SW_RESET   gv->sw_t = gv->sw_save = 0

static VEC cam_up = { 0.0f, 1.0f, 0.0f, 1.0f };
static VEC cam_from = { 0.0f, 0.0f, 100.0f, 1.0f };
static VEC cam_at = { 0.0f, 0.0f, 0.0f, 1.0f };

static void  Add_obj_2_world ( OBJECT * );
static void  Clear_obj_list ( void );
static void  Sort_obj_list ( void );
static void  Draw_obj_list ( MAT, MAT );
static int   Z_compare ( const void *obj0, const void *obj1 );
static float Dist_pt_2_line ( VEC p, VEC l0, VEC l1 );

static void  Draw_vector_font ( int *s[], int x, int y, unsigned int color );

/*------------------------------------------------------------------
 * Variables
------------------------------------------------------------------*/

GAMEVARS gvars, *gv;
TIMER    gtimer, *gt;

static long
score_table[ZONE_HEIGHT_MAX] = { 10L, 50L, 100L, 150L, 200L };

/*================================================================*/

static void Game_scores ( long new_score, int focus )
{
   int        i;
   char       buffer[256], tmp_num[2];

   if ( focus )
   {
      if ( new_score > gv->pscore2 )
      {
         gv->pscore2 = new_score;
         /* convert hi-score into a vector-font */
         sprintf ( buffer, "%ld", new_score );
         tmp_num[0] = '0';
         tmp_num[1] = '\0';
         for ( i = 0; buffer[i]; i++ )
         {
            tmp_num[0] = buffer[i];
            gv->pscore_c2[i] = NUMBER[atoi(tmp_num)];
         }
         gv->pscore_c2[i] = NULL;
      }
   }
   else
   {
      /* let's reserve hi_score for lone defender */
      if ( gv->pduel == 0 && new_score > gv->hi_score )
      {
         gv->hi_score = new_score;
         sprintf ( buffer, "%ld", new_score );
         tmp_num[0] = '0';
         tmp_num[1] = '\0';
         for ( i = 0; buffer[i]; i++ )
         {
            tmp_num[0] = buffer[i];
            gv->hi_score_c[i] = NUMBER[atoi(tmp_num)];
         }
         gv->hi_score_c[i] = NULL;
      }

      if ( new_score > gv->pscore1 ) {
         gv->pscore1 = new_score;
         /* convert hi-score into a vector-font */
         sprintf ( buffer, "%ld", new_score );
         tmp_num[0] = '0';
         tmp_num[1] = '\0';
         for ( i = 0; buffer[i]; i++ )
         {
            tmp_num[0] = buffer[i];
            gv->pscore_c1[i] = NUMBER[atoi(tmp_num)];
         }
         gv->pscore_c1[i] = NULL;
      }
   }
}

static void Game_scores_draw ( void )
{
   if ( gv->pduel )
   {
      if ( gv->phorizontal )
      {
         Draw_vector_font ( SCORE, 30, 8, RED );
         Draw_vector_font ( gv->pscore_c1, 210, 8, RED );
         Draw_vector_font ( SCORE, 30, 248, GREEN );
         Draw_vector_font ( gv->pscore_c2, 210, 248, GREEN );
      }
      else if ( gv->pvertical )
      {
         Draw_vector_font ( SCORE, 30, 8, RED );
         Draw_vector_font ( gv->pscore_c1, 30, 48, RED );
         Draw_vector_font ( SCORE, 350, 8, GREEN );
         Draw_vector_font ( gv->pscore_c2, 350, 48, GREEN );
      }
      else
      {
         Draw_vector_font ( SCORE, 30, 8, RED );
         Draw_vector_font ( gv->pscore_c1, 30, 48, RED );
         Draw_vector_font ( SCORE, 350, 248, GREEN );
         Draw_vector_font ( gv->pscore_c2, 350, 288, GREEN );
      }
   }
   else
   {
      Draw_vector_font ( SCORE, 150, 14, GREEN );
      Draw_vector_font ( gv->pscore_c1, 330, 14, GREEN );
   }
}

/*================================================================*/

void Game_main ( void )
{
   while ( Handle_events() )
   {
      /* get start-time of current frame */
      gv->msec    = Timer_msec ( gt );
      gv->ftime   = (double)gv->msec/1000L;
      gv->fps     = 1.0 / gv->ftime;
      gv->fadjust = gv->rfps / gv->fps;

      (*Game_actionfn)();

      Update_display ();

      /* let game sleep for about 10msec */
      if ( gv->fpsfast )
      {
         gv->fpsavg = ( gv->fpsavg + gv->fps ) / 2;
         if ( gv->fpsavg > 100 )
            gv->fpsfast--;
      }
      else
         usleep(10000);
   }
}

/*================================================================*/

void (*Game_actionfn) ();

/*================================================================*/

int Game_init ( float win_width, float win_height )
{
   /* initialize variables and timers */
   gv = &gvars;
   gv->win_width  = (long)win_width;
   gv->win_height = (long)win_height;
   gv->pduel = gv->phorizontal = gv->pvertical = 0;
   gv->x1l = gv->y1t = 0;
   gv->x1r        = (int)gv->win_width;
   gv->y1b        = (int)gv->win_height;
   Game_init_vars ( INITGAME );
   Game_init_keys ();

   Stars_init ();
   Jumpgate_init ();
   Camera_init ( win_width, win_height, 0.785398163 );

   gt = &gtimer;
   Timer_init( gt );

   /* setup stopwatch timer and run menu */
   SW_RESET;
   Game_actionfn = Game_menu;

   return TRUE;
}

/*================================================================*/

void Game_init_vars ( int init_type )
{
   switch ( init_type )
   {
      case INITGAME:
         gv->rfps         = REFERENCE_FRAMERATE;
         gv->display_fps  = FALSE;
         gv->fpsavg       = -1000000;
         gv->fpsfast      = 1000;
         gv->pscore1      = 0;
         gv->hi_score     = -1;
         Game_scores ( HISCORE, 0 );

      case NEWGAME:
         gv->paused       = FALSE;
         gv->gameover     = FALSE;
         gv->pscore2      = -1;
         Game_scores ( 0, 1 );
         gv->pscore1      = -1;
         Game_scores ( 0, 0 );
         gv->pbonus1 = gv->pbonus2 = 0;
         gv->plives1 = gv->plives2 = PLAYER_LIVES_START;

      case NEWLEVEL:
         gv->fcount       = 0;
         gv->msec         = 0;
         gv->ftime        = 0.0;
         gv->fps          = 0.0;
         gv->fadjust      = 0.0;
         gv->new_level    = FALSE;
         gv->pfocus       = 0;

      default:
      break;
   }
}

/*================================================================*/

void Game_init_keys ( void )
{
   gv->key_QUIT       = FALSE;
   gv->key_FIRE1      = FALSE;
   gv->key_UP1        = FALSE;
   gv->key_DOWN1      = FALSE;
   gv->key_LEFT1      = FALSE;
   gv->key_RIGHT1     = FALSE;

   gv->key_1PLAYER    = FALSE;
   gv->key_DUELPLAY_H = FALSE;
   gv->key_DUELPLAY_V = FALSE;
   gv->key_DUELPLAY   = FALSE;

   gv->key_FIRE2      = FALSE;
   gv->key_UP2        = FALSE;
   gv->key_DOWN2      = FALSE;
   gv->key_LEFT2      = FALSE;
   gv->key_RIGHT2     = FALSE;
}

/*================================================================*/

void Game_newlevel ( void )
{
   srand ( time( NULL ) );
   Game_init_vars ( NEWLEVEL );
   Game_init_keys ();

   Stars_init ();
   Explosions_clear ();
   Jumpgate_init ();
   One_up_init ();

   Player_init ( 0 );
   Player_init ( 1 );
   Aliens_init ();

   SW_RESET;
   Game_actionfn = Game_ready;
}

/*================================================================*/

void Game_menu ( void )
{
   static int blink = 0;
   static int color = 0;
   int        i;
   MAT        cam_mat;

   Camera_transform ( cam_mat, cam_up, cam_from, cam_at, 0 );
   Stars_draw ( cam_mat, cam_mat );

   if ( gv->pduel )
       Game_scores_draw ();
   else
   {
      Draw_vector_font ( HI_SCORE, 120, 15, GREEN );
      Draw_vector_font ( gv->hi_score_c, 360, 15, GREEN );
   }

   blink += gv->msec;
   if ( blink > 250 )
   {
      color++;
      blink -= 250;
      if ( color > 3 ) color = 0;
   }

   i = RED - ( color * 15 );

   Draw_vector_font ( XINVADERS3D_LOGO, 160, 150, GREEN );
   Draw_vector_font ( PRESS, 40, 400, i );
   Draw_vector_font ( FIRE, 220, 400, i );
   Draw_vector_font ( TO, 370, 400, i );
   Draw_vector_font ( START, 460, 400, i );

   if ( gv->key_FIRE1 || ( gv->pduel && gv->key_FIRE2 ) )
   {
      Game_actionfn = Game_newlevel;
   }

   if ( gv->key_1PLAYER )
   {
      gv->pduel = gv->phorizontal = gv->pvertical = 0;
      gv->x1l = -(int)(gv->win_width);     /* let x11 chop this */
      gv->y1t = -(int)(gv->win_height);    /* let x11 chop this */
      gv->x1r = (int)(gv->win_width)*2;    /* let x11 chop this */
      gv->y1b = (int)(gv->win_height)*2;   /* let x11 chop this */
   }
   else if ( gv->key_DUELPLAY_H )
   {
      gv->pduel = gv->phorizontal = 1;
      gv->pvertical = 0;
      gv->x1l = -(int)(gv->win_width);     /* let x11 chop this */
      gv->y1t = -(int)(gv->win_height);    /* let x11 chop this */
      gv->x1r = (int)(gv->win_width)*2;    /* let x11 chop this */
      gv->y1b = (int)(gv->win_height)/2;
      gv->x2l = -(int)(gv->win_width);     /* let x11 chop this */
      gv->y2t = (int)(gv->win_height)/2 + 1;
      gv->x2r = (int)(gv->win_width)*2;    /* let x11 chop this */
      gv->y2b = (int)(gv->win_height)*2;   /* let x11 chop this */
   }
   else if ( gv->key_DUELPLAY_V )
   {
      gv->pduel = gv->pvertical = 1;
      gv->phorizontal = 0;
      gv->x1l = -(int)(gv->win_width);     /* let x11 chop this */
      gv->y1t = -(int)(gv->win_height);    /* let x11 chop this */
      gv->x1r = (int)(gv->win_width)/2;
      gv->y1b = (int)(gv->win_height)*2;   /* let x11 chop this */
      gv->x2l = (int)(gv->win_width)/2 + 1;
      gv->y2t = -(int)(gv->win_height);   /* let x11 chop this */
      gv->x2r = (int)(gv->win_width)*2;   /* let x11 chop this */
      gv->y2b = (int)(gv->win_height)*2;  /* let x11 chop this */
   }
   else if ( gv->key_DUELPLAY )
   {
      gv->pduel = 1;
      gv->phorizontal = gv->pvertical = 0;
      gv->x1l = -(int)(gv->win_width);     /* let x11 chop this */
      gv->y1t = -(int)(gv->win_height);    /* let x11 chop this */
      gv->x1r = (int)(gv->win_width)/2;
      gv->y1b = (int)(gv->win_height)/2;
      gv->x2l = (int)(gv->win_width)/2 + 1;
      gv->y2t = (int)(gv->win_height)/2 + 1;
      gv->x2r = (int)(gv->win_width)*2;   /* let x11 chop this */
      gv->y2b = (int)(gv->win_height)*2;  /* let x11 chop this */
   }
}

/*================================================================*/

void Game_ready ( void )
{
   static int blink = 0;
   static int color = 0;
   int i;

   MAT cam_mat;

   Camera_transform ( cam_mat, cam_up, cam_from, cam_at, 0 );
   Stars_draw ( cam_mat, cam_mat );

   blink += gv->msec;
   if ( blink > 250 )
   {
      blink -= 250;
      color++;
      if ( color == 3 )
         color = 0;
   }

   i = RED - ( color * 15 );
   Draw_vector_font ( GET, 140, 200, i );
   Draw_vector_font ( READY, 260, 200, i );

   Game_overlay ();
   SW_UPDATE;

   if ( gv->key_FIRE1 || gv->sw_t > READY_TIME || \
        ( gv->pduel && gv->key_FIRE2 ) )
   {
      SW_RESET;
      Game_actionfn = Game_run;
   }
}

/*================================================================*/

void Game_paused_toggle ( void )
{
   static void (*saved_action_fn)( void ) = NULL;

   gv->paused ^= TRUE;

   if ( gv->paused )
   {
      SW_PAUSE;
      saved_action_fn = Game_actionfn;
      Game_actionfn = Game_paused;
   }
   else
   {
      SW_UNPAUSE;
      Game_actionfn = saved_action_fn;
   }
}

void Game_paused ( void )
{
   MAT cam_mat;

   Camera_transform ( cam_mat, cam_up, cam_from, cam_at, 0 );
   Stars_draw ( cam_mat, cam_mat );
   Draw_vector_font ( PAUSED, 230, 200, RED );

   Game_overlay ();
   SW_UPDATE;
}

/*================================================================*/

void Game_reset ( void )
{
   SW_RESET;
   Game_init_vars ( INITGAME );
   Game_init_keys ();
   Game_actionfn = Game_menu;
}

void Game_gameover ( void )
{
   MAT cam_mat1, cam_mat2;

   Camera_transform ( cam_mat1, cam_up, cam_from, cam_at, 0 );
   Camera_transform ( cam_mat2, cam_up, cam_from, cam_at, 1 );
   Stars_draw ( cam_mat1, cam_mat2 );
   Draw_vector_font ( GAME, 185, 200, RED );
   Draw_vector_font ( OVER, 335, 200, RED );
   Game_overlay ();
   SW_UPDATE;

   if (  gv->key_FIRE1 || gv->sw_t > GAMEOVER_TIME || \
         ( gv->pduel && gv->key_FIRE2 ) )
   {
      SW_RESET;
      Game_init_vars ( NEWGAME );
      Game_init_keys ();
      Game_actionfn = Game_menu;
   }
}

/*================================================================*/

void Game_overlay ( void )
{
   char buffer[256];
   int  i, x, x1, x2, y1, y2;
   int  life[6] = { 0, 25, 25, 25, 12, 0 };

   Game_scores_draw ();

   if ( gv->display_fps )
   {
      if ( gv->ftime > 0.0f )
      {
         sprintf ( buffer, "FPS:%3.0f:(%1.3f):ms(%ld)", gv->fps, \
                            gv->fadjust,gv->msec );
      }
      else
         sprintf ( buffer, "FPS: n/a" );

      Draw_text ( buffer, 0, 475, RED );
   }

   x1 = x2 = 0;
   y1 = y2 = 425;
   if ( gv->pduel )
   {
      if ( gv->phorizontal )
         y1 -= 240;
      else if ( gv->pvertical )
         x2 += 320;
      else
      {
         x2 += 320;
         y1 -= 240;
      }
      if ( gv->plives2 > 1 )
      {
         for ( i = 0, x = 15+x2; i < (gv->plives2-1); i++ )
         {
            Draw_line ( life[0]+x, life[1]+y2, life[2]+x, life[3]+y2, GREEN );
            Draw_line ( life[2]+x, life[3]+y2, life[4]+x, life[5]+y2, GREEN );
            Draw_line ( life[4]+x, life[5]+y2, life[0]+x, life[1]+y2, GREEN );
            x+= 30;
         }
#ifdef GAME_DEBUG
         sprintf ( buffer, "Lives: %d", gv->plives2-1 );
         Draw_text ( buffer, x2+230, y2+45, GREEN );
#endif
      }
   }
   if ( gv->plives1 > 1 )
   {
      for ( i = 0, x = 15; i < (gv->plives1-1); i++ )
      {
         Draw_line ( life[0]+x, life[1]+y1, life[2]+x, life[3]+y1, GREEN );
         Draw_line ( life[2]+x, life[3]+y1, life[4]+x, life[5]+y1, GREEN );
         Draw_line ( life[4]+x, life[5]+y1, life[0]+x, life[1]+y1, GREEN );
         x+= 30;
      }
#ifdef GAME_DEBUG
      sprintf ( buffer, "Lives: %d", gv->plives1-1 );
      Draw_text ( buffer, 550-x2, y1+45, GREEN );
#endif
   }

#ifdef GAME_DEBUG
   sprintf ( buffer, "Time: %10.0fs", (double)gv->sw_t/1000);
   Draw_text ( buffer, 0, 470, RED );

   if ( gv->pduel )
      sprintf ( buffer, "U:%d D:%d L:%d R:%d F:%d - U:%d D:%d L:%d R:%d F:%d", \
                gv->key_UP1, gv->key_DOWN1, gv->key_LEFT1, gv->key_RIGHT1, \
                gv->key_FIRE1, gv->key_UP2, gv->key_DOWN2, gv->key_LEFT2, \
                gv->key_RIGHT2, \ );
   else
      sprintf ( buffer, "U: %d D: %d L: %d R: %d F: %d", \
                gv->key_UP1, gv->key_DOWN1, gv->key_LEFT1, gv->key_RIGHT1, \
                gv->key_FIRE1 );
   Draw_text ( buffer, 200, 470, WHITE );
#endif
}

/*================================================================*/

void Game_run ( void )
{
   float  dist = 0.0f;
   OBJECT *alien1, *alien2, *abomb;
   VEC    up1 = { 0.0f, 1.0f, 0.0f, 1.0f };
   VEC    from1 = { 0.0f, 25.0f, 100.0f, 1.0f };
   VEC    at1 = { 0.0f, 0.0f, -100.0f, 1.0f };
   VEC    up2 = { 0.0f, 1.0f, 0.0f, 1.0f };
   VEC    from2 = { 0.0f, 25.0f, 100.0f, 1.0f };
   VEC    at2 = { 0.0f, 0.0f, -100.0f, 1.0f };
   MAT    cam_mat1, cam_mat2;
   int    hit1, hit2;

   /* prepare for new frame */
   Clear_obj_list ();

   /* update-player1 */
   if ( player1->active )
      player1->actionfn ( player1 );

   /* update camera1 */
   Vector_addd ( from1, player1->pos );
   Vector_addd ( at1, player1->pos );
   Camera_transform ( cam_mat1, up1, from1, at1, 0 );
   if ( gv->pduel )
   {
      /* update-player2 */
      if ( player2->active )
         player2->actionfn ( player2 );

      /* update camera2 */
      Vector_addd ( from2, player2->pos );
      Vector_addd ( at2, player2->pos );
      Camera_transform ( cam_mat2, up2, from2, at2, 1 );
   }

   /* Do collisions checking : test player_missile(s) vs aliens */
   hit1 = hit2 = 0;
   if ( pm1->active && pm1->zone == gv->formation_zone && \
        pm1->zheight > -1 && pm1->zheight < ZONE_HEIGHT_MAX )
   {
      hit1 = 1;
      /* if so which height-level?? */
      alien1 = (af_list+pm1->zheight)->head;
   }
   if ( gv->pduel && \
        pm2->active && pm2->zone == gv->formation_zone && \
        pm2->zheight > -1 && pm2->zheight < ZONE_HEIGHT_MAX )
   {
      hit2 = 1;
      alien2 = (af_list+pm2->zheight)->head;
   }
   if ( hit1 && hit2 && alien1 == alien2 )
   {
      /*
       * slim posibility that both players will hit the same aliens,
       * so we need to check each player's missile vs each alien.
       */
      while ( alien1 )
      {
         hit1 = hit2 = 0;
         dist = Dist_pt_2_line ( alien1->pos, pm1->old_pos, pm1->pos ) - \
                pm1->radius_squared - 1.0f;
         if ( dist < alien1->radius_squared )
         {
            hit1 = 1;
            pm1->active = FALSE;
            Game_scores ( gv->pscore1 + score_table[pm1->zheight], 0 );
            gv->pbonus1 += score_table[pm1->zheight];
            if ( gv->pbonus1 > PLAYER_LIFE_BONUS-1 )
            {
               gv->pbonus1 -= PLAYER_LIFE_BONUS;
               if ( gv->plives1 < PLAYER_LIFE_MAX )
               {
                  gv->plives1++;
                  hit1 = 2;
               }
            }
         }
         dist = Dist_pt_2_line ( alien1->pos, pm2->old_pos, pm2->pos ) - \
                pm2->radius_squared - 1.0f;
         if ( dist < alien1->radius_squared )
         {
            hit2 = 1;
            pm2->active = FALSE;
            Game_scores ( gv->pscore2 + score_table[pm1->zheight], 1 );
            gv->pbonus2 += score_table[pm1->zheight];
            if ( gv->pbonus2 > PLAYER_LIFE_BONUS-1 )
            {
               gv->pbonus2 -= PLAYER_LIFE_BONUS;
               if ( gv->plives2 < PLAYER_LIFE_MAX )
               {
                  gv->plives2++;
                  hit2 = 2;
               }
            }
         }
         if ( hit1 || hit2 )
         {
            alien1->active = FALSE;
            gv->alien_count--;
            Objlist_del ( (af_list+pm1->zheight), alien1 );
            Explosions_add ( alien1 );
            Update_fcolumn ( alien1 );

            /* update player score & alien count */
            if ( ( hit1 > 1 ) || ( hit2 > 1 ) )
               One_up_add ( alien1 );
            if ( gv->alien_count == 0 )
            {
               gv->new_level = TRUE;
               break;
            }
         }
         alien1 = alien1->next;
      }
      hit1 = hit2 = 0;
   }
   if ( hit1 )
   {
      while ( alien1 )
      {
         dist = Dist_pt_2_line ( alien1->pos, pm1->old_pos, pm1->pos ) - \
                pm1->radius_squared - 1.0f;

         if ( dist < alien1->radius_squared )
         {
            pm1->active = alien1->active = FALSE;
            gv->alien_count--;
            Objlist_del ( (af_list+pm1->zheight), alien1 );
            Explosions_add ( alien1 );
            Update_fcolumn ( alien1 );

            /* update player score & alien count */
            Game_scores ( gv->pscore1 + score_table[pm1->zheight], 0 );
            gv->pbonus1 += score_table[pm1->zheight];
            if ( gv->pbonus1 > PLAYER_LIFE_BONUS-1 )
            {
               gv->pbonus1 -= PLAYER_LIFE_BONUS;
               if ( gv->plives1 < PLAYER_LIFE_MAX )
               {
                  gv->plives1++;
                  One_up_add ( alien1 );
               }
            }
            if ( gv->alien_count == 0 )
               gv->new_level = TRUE;

            break;
         }
         alien1 = alien1->next;
      }
   }
   if ( hit2 )
   {
      while ( alien2 )
      {
         dist = Dist_pt_2_line ( alien2->pos, pm2->old_pos, pm2->pos ) - \
                pm2->radius_squared - 1.0f;

         if ( dist < alien2->radius_squared )
         {
            pm2->active = alien2->active = FALSE;
            gv->alien_count--;
            Objlist_del ( (af_list+pm2->zheight), alien2 );
            Explosions_add ( alien2 );
            Update_fcolumn ( alien2 );

            /* update player score & alien count */
            Game_scores ( gv->pscore2 + score_table[pm2->zheight], 1 );
            gv->pbonus2 += score_table[pm2->zheight];
            if ( gv->pbonus2 > PLAYER_LIFE_BONUS-1 )
            {
               gv->pbonus2 -= PLAYER_LIFE_BONUS;
               if ( gv->plives2 < PLAYER_LIFE_MAX )
               {
                  gv->plives2++;
                  One_up_add ( alien2 );
               }
            }
            if ( gv->alien_count == 0 )
               gv->new_level = TRUE;

            break;
         }
         alien2 = alien2->next;
      }
   }

   /* is player missile(s) in the UFO zone?? */
   if ( ufo->active )
   {
      /*
       * allow for very slim possibility that both players hit UFO
       * hit the UFO at the same time, therefore test for hit first,
       * and then add the score(s) afterwards.
       */
      hit1 = hit2 = 0;
      if ( pm1->active && \
           ( pm1->zone == ufo->zone ) && ( pm1->zheight == ufo->zheight ) )
      {
         dist = Dist_pt_2_line ( ufo->pos, pm1->old_pos, pm1->pos ) - \
                pm1->radius_squared - 1.0f;
         if ( dist < ufo->radius_squared )
            hit1 = 1;
      }
      if ( gv->pduel && pm2->active && \
           ( pm2->zone == ufo->zone ) && ( pm2->zheight == ufo->zheight ) )
      {
         dist = Dist_pt_2_line ( ufo->pos, pm2->old_pos, pm2->pos ) - \
                pm2->radius_squared - 1.0f;
         if ( dist < ufo->radius_squared )
            hit2 = 1;
      }

      if ( hit1 )
      {
         pm1->active = ufo->active = FALSE;
         Game_scores ( gv->pscore1 + UFO_BONUS, 0 );
         gv->pbonus1 += UFO_BONUS;
         if ( gv->pbonus1 > PLAYER_LIFE_BONUS-1 )
         {
            gv->pbonus1 -= PLAYER_LIFE_BONUS;
            if ( gv->plives1 < PLAYER_LIFE_MAX )
               gv->plives1++;
         }
      }
      if ( hit2 )
      {
         pm2->active = ufo->active = FALSE;
         Game_scores ( gv->pscore2 + UFO_BONUS, 1 );
         gv->pbonus2 += UFO_BONUS;
         if ( gv->pbonus2 > PLAYER_LIFE_BONUS-1 )
         {
            gv->pbonus2 -= PLAYER_LIFE_BONUS;
            if ( gv->plives2 < PLAYER_LIFE_MAX )
               gv->plives2++;
         }
      }
      if ( hit1 || hit2 )
      {
         Explosions_add ( ufo );
         if ( ( hit1 > 1 ) || ( hit2 > 1 ) )
            One_up_add ( ufo );
      }
   }

   /* alien bombs vs player(s) */
   if ( !gv->gameover )
   {
      hit1 = hit2 = 0;
      if ( player1->active && !gv->pblink1 )
         hit1 = 1;
      if ( gv->pduel && player2->active && !gv->pblink2 )
         hit2 = 1;
      if ( hit1 || hit2 )
      {
         for (abomb = abombs->head; abomb != NULL; abomb = abomb->next )
         {
            /* one bomb might hit one or both players */
            if ( hit1 && \
                 abomb->zone == player1->zone && \
                 abomb->zheight == player1->zheight )
            {
               dist = Dist_pt_2_line ( player1->pos, abomb->old_pos, abomb->pos ) - \
                      abomb->radius_squared - 1.0f;
               if ( dist < player1->radius_squared )
               {
                  player1->active = FALSE;
                  Explosions_add ( player1 );
                  hit1 = 2;
               }
            }
            if ( hit2 && \
                 abomb->zone == player2->zone && \
                 abomb->zheight == player2->zheight )
            {
               dist = Dist_pt_2_line ( player2->pos, abomb->old_pos, abomb->pos ) - \
                      abomb->radius_squared - 1.0f;
               if ( dist < player2->radius_squared )
               {
                  player2->active = FALSE;
                  Explosions_add ( player2 );
                  hit2 = 2;
               }
            }
            if ( hit1 > 1 || hit2 > 1 )
            {
               abomb->active = FALSE;
               Objlist_del ( abombs, abomb );
               if ( hit1 > 1 )
                  hit1 = 0;
               if ( hit2 > 1 )
                  hit2 = 0;
            }
            if ( !( hit1 || hit2 ) )
               break;
         }
      }
   }

   /*
    * player(s) vs aliens:
    * if the remaining aliens reach the player_zone
    * then the game is over! Reduce the number of
    * remaining lives to zero and blowup the player(s) and
    * then initiate a game-over!
    */

   if ( gv->formation_zone == ZONE_0 )
   {
      gv->plives1 = gv->plives2 = 0;
      if ( player1->active )
      {
         Explosions_add ( player1 );
         player1->active = FALSE;
      }
      if ( gv->pduel && player2->active )
      {
         Explosions_add ( player2 );
         player2->active = FALSE;
      }
   }

   /*
    *
    * Move Objects and Update Animations :
    * formation, alien_bombs, ufo, and player_missile
    *
    */
   Aliens_update ();

   if ( pm1->active )
      pm1->actionfn ( pm1 );
   if ( ( gv->pduel ) && ( pm2->active ) )
      pm2->actionfn ( pm2 );

   /*
    *
    *  Draw Objects :
    *
    *  special effects
    *  alien: formation, bombs, and ufo
    *  player missile
    *  player
    *
    */

   Stars_draw ( cam_mat1, cam_mat2 );
   Jumpgate_animate ( cam_mat1, cam_mat2 );
   Explosions_draw ( cam_mat1, cam_mat2 );
   One_up_draw ( cam_mat1, cam_mat2 );

   /*
    * major hack!! this is NOT the right way to draw in 3d
    * may the universe forgive my transgression!
    * ...but it's KISS and visually seems close enough :-P
    * */
   if ( ufo->active )
      Add_obj_2_world ( ufo );
   Add_obj_2_world ( &the_formation );

   for (abomb=abombs->head; abomb != NULL; abomb=abomb->next )
   {
      Add_obj_2_world ( abomb );
   }

   if ( pm1->active )
      Add_obj_2_world ( pm1 );
   if ( player1->active )
      Add_obj_2_world ( player1 );
   if ( gv->pduel )
   {
      if ( pm2->active )
         Add_obj_2_world ( pm2 );
      if ( player2->active )
         Add_obj_2_world ( player2 );
   }

   Sort_obj_list ();
   Draw_obj_list ( cam_mat1, cam_mat2 );

   SW_UPDATE;
   Game_overlay ();

   /*
    *  Before acknowledging flags make sure ALL explosions
    *  are finished:
    *  gameover flag has precedence over newlevel flag
    *  since it is possible for a player to have killed
    *  all the aliens but then get killed by an alien bomb
    *  resulting in both a newlevel and gameover condition!!
    */
   if ( Explosions_count() == 0 )
   {
      hit1 = hit2 = 0;
      if ( gv->pduel )
      {
         if ( player1->active == FALSE && \
              player2->active == FALSE ) 
            hit2 = 1;
      }
      else
      {
         if ( player1->active == FALSE )
            hit1 = 1;
      }
      if ( hit2 )
      {
         /* reduce player life and check for gameover */
         gv->plives1--;
         gv->plives2--;
         if ( gv->plives1 <= 0 || gv->plives2 <= 0 )
            gv->gameover = TRUE;

         Player_init ( 0 );
         Player_init ( 1 );
         SW_RESET;
         Game_actionfn = Game_ready;
      }
      else if ( hit1 )
      {
         /* reduce player life and check for gameover */
         gv->plives1--;
         if ( gv->plives1 <= 0 )
            gv->gameover = TRUE;

         Player_init ( 0 );
         SW_RESET;
         Game_actionfn = Game_ready;
      }

      if ( gv->gameover )
      {
         SW_RESET;
         Game_actionfn = Game_gameover;
      }
      else
      {
         if ( gv->new_level && ( Objlist_count ( abombs ) == 0 ) )
         {
            SW_RESET;
            Game_actionfn = Game_newlevel;
         }
      }
   }
}

/*================================================================*/

void Object_update_zone ( OBJECT *obj )
{
   obj->zone    = lrint ( floor ( (double)(obj->pos[ZPOS]/-ZONE_WIDTH) ) );
   obj->zheight = lrint ( floor ( (double)(obj->pos[YPOS]/ZONE_HEIGHT) ) );
}

/*================================================================*/

static void Draw_vector_font ( int *s[], int x, int y, unsigned int color )
{
   int *letter, segments, x1, y1, x2, y2;

   /* first integer of pointer array is always # of line segments */
   while ( (letter=*s++) ) {
      segments=*letter++;
      while ( --segments>=0 ) {
         x1 = (*letter++)+x; y1 = (*letter++)+y;
         x2 = (*letter++)+x; y2 = (*letter++)+y;
         Draw_line( x1, y1, x2, y2, color );
      }
      x += 30;
   }
}

/*================================================================*/

static void Add_obj_2_world ( OBJECT *obj )
{
   int i;

   i = gv->gobjcount;

   if ( i < MAX_OBJECTS-1 )
   {
      gv->gobjs[i] = obj;
      gv->gobjcount++;
   }
}

static void Clear_obj_list ( void )
{
   gv->gobjcount = 0;
}

static void Sort_obj_list ( void )
{
   qsort ( gv->gobjs, gv->gobjcount, sizeof ( OBJECT * ), Z_compare );
}

static int Z_compare ( const void *obj0, const void *obj1 )
{
   OBJECT *o0, *o1;

   o0 = ( OBJECT * ) obj0;
   o1 = ( OBJECT * ) obj1;

   if ( o0->pos[ZPOS] < o1->pos[ZPOS] )
      return -1;
   else if ( o0->pos[ZPOS] > o1->pos[ZPOS] )
      return 1;
   else
      return 0;
}

static void Draw_obj_list ( MAT r1, MAT r2 )
{
   int i;
   OBJECT *obj;

   if ( gv->gobjcount > 0 )
   {
      i = 0;
      while ( i < gv->gobjcount )
      {
         obj = gv->gobjs[i];
         obj->drawfn ( obj, r1, r2 );
         i++;
      }
   }
}
/*================================================================*/

static float Dist_pt_2_line ( VEC p, VEC l0, VEC l1 )
{
   VEC l0_p;
   VEC l0_l1;
   VEC q;
   float n, d, w, t, dist;

   /*
    * calc adjusted position of projectile along the
    * line segment formed by old_pos to new_pos
    * use this for calculating distance between moving
    * objects. This must be done since movement is
    * not in discreet amounts due to varying framerates.
    *
    * routine uses the properties of the unit vector
    * and dot product. see ``Computer Graphics:
    * principles and practice'' appendix for
    * explaination. Then checks to see if generated
    * line falls within the line segment l0l1
    *
    */

   Vector_sub ( p, l0, l0_p );
   Vector_sub ( l1, l0, l0_l1 );

   /*
    * don't try to take Vector_norm if division by zero
    * will result. TBB
    *
    * check using Vec_mag_squared instead of
    * xpos == 0 && ypos == 0 && zpos == 0
    * Don
    */

   if ( Vector_mag_squared ( l0_l1 ) < 1.0f )
   {
      q[XPOS] = l0[XPOS];
      q[YPOS] = l0[YPOS];
      q[ZPOS] = l0[ZPOS];

      dist = Vector_dist_squared ( q, p );
   }
   else
   {
      d = l1[ZPOS] - l0[ZPOS];

      /* l0 and l1 are very close */
      if ( (d > -0.025 ) && ( d < 0.025 ) )
      {
         q[XPOS] = l0[XPOS];
         q[YPOS] = l0[YPOS];
         q[ZPOS] = l0[ZPOS];

         dist = Vector_dist_squared ( q, p );
      }
      else
      {
         Vector_norm ( l0_l1 );
         w = Vector_dot ( l0_p, l0_l1 );

         q[XPOS] = l0[XPOS] + ( l0_l1[XPOS] * w );
         q[YPOS] = l0[YPOS] + ( l0_l1[YPOS] * w );
         q[ZPOS] = l0[ZPOS] + ( l0_l1[ZPOS] * w );

         n = q[ZPOS] - l0[ZPOS];

         t = n / d;
         if ( ( t > -0.025f ) && ( t < 1.5f ) )
            dist = Vector_dist_squared ( q, p );
         else
            dist = 1000000.0f;
      }
   }

   return dist;
}
