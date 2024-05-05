/*------------------------------------------------------------------
  player.c:

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

/*================================================================*/

#define PLAYER_RADIUS          20.0f
#define PLAYER_RADIUS_SQUARED  400.0f
#define MISSLE_RADIUS          20.0f
#define MISSLE_RADIUS_SQUARED  400.0f

#define MAX_X 550.0f
#define MIN_X -550.0f
#define MAX_Y 450.0f
#define MIN_Y 50.0f

#define ROT_Z 0.087266463    /* 5 degrees per frame */
#define MAX_ROT 0.785398163  /* max of 45 degrees */

enum player_enum
{
   PLAYER_BLINK        = TRUE,
   PLAYER_BLINK_TIME   = 5000  /* 5.0 sec */
};

OBJECT player_1, player_2, *player1, *player2;
OBJECT player_missile1, player_missile2, *pm1, *pm2;

static VEC start_pos      = { 0.0f, 250.0f, -25.0f, 1.0f };
static VEC start_dir      = { 0.0f, 0.0f, -1.0f, 1.0f };
static VEC thrust_up      = { 0.0f, 10.0f, 0.0f, 1.0f };
static VEC thrust_down    = { 0.0f, -10.0f, 0.0f, 1.0f };
static VEC thrust_left    = { -10.0f, 0.0f, 0.0f, 1.0f };
static VEC thrust_right   = { 10.0f, 0.0f, 0.0f, 1.0f };
static VEC missile_offset = { 0.0f, 0.0f, 0.0f, 1.0f };
static VEC missile_thrust = { 0.0f, 0.0f, -50.0f, 1.0f };

static int pcolor1, pcolor2; /* Player colors */

/*================================================================*/

void Player_init ( int pnum )
{
   if ( pnum )
   {
      player2 = &player_2;
      gv->pblink2 = TRUE;

      Object_init ( player2 );
      Object_set_actionfn ( player2, Player_update2 );
      Object_set_drawfn   ( player2, Player_blink2 );

      Object_set_pos ( player2, start_pos );
      Object_set_dir ( player2, start_dir );

      player2->active         = TRUE;
      player2->radius         = PLAYER_RADIUS;
      player2->radius_squared = PLAYER_RADIUS_SQUARED;
      player2->frame          = 0;
      player2->rot            = 0.0;
      pcolor2                 = BLUE;
      Object_update_zone ( player2 );

      pm2 = &player_missile2;
      Object_init ( pm2 );
      pm2->radius             = MISSLE_RADIUS;
      pm2->radius_squared     = MISSLE_RADIUS_SQUARED;
      Object_set_actionfn ( pm2, Player_missile_update );
      Object_set_drawfn   ( pm2, Player_missile_draw2 );
   }
   else
   {
      player1 = &player_1;
      gv->pblink1 = TRUE;

      Object_init ( player1 );
      Object_set_actionfn ( player1, Player_update1 );
      Object_set_drawfn  ( player1, Player_blink1 );

      Object_set_pos ( player1, start_pos );
      Object_set_dir ( player1, start_dir );

      player1->active         = TRUE;
      player1->radius         = PLAYER_RADIUS;
      player1->radius_squared = PLAYER_RADIUS_SQUARED;
      player1->frame          = 0;
      player1->rot            = 0.0;
      pcolor1                 = RED;
      Object_update_zone ( player1 );

      pm1 = &player_missile1;
      Object_init ( pm1 );
      pm1->radius             = MISSLE_RADIUS;
      pm1->radius_squared     = MISSLE_RADIUS_SQUARED;
      Object_set_actionfn ( pm1, Player_missile_update );
      Object_set_drawfn   ( pm1, Player_missile_draw1 );
   }
}

/*================================================================*/

void Player_update1 ( OBJECT *obj )
{
   if ( gv->key_UP1 )
   {
      obj->pos[YPOS] += thrust_up[YPOS] * gv->fadjust;
      Object_update_zone ( obj );
      if ( obj->pos[YPOS] > MAX_Y )
         obj->pos[YPOS] = MAX_Y;
   }

   if ( gv->key_DOWN1 )
   {
      obj->pos[YPOS] += thrust_down[YPOS] * gv->fadjust;
      Object_update_zone ( obj );
      if ( obj->pos[YPOS] < MIN_Y )
         obj->pos[YPOS] = MIN_Y;
   }

   if ( gv->key_LEFT1 )
   {
      obj->pos[XPOS] += thrust_left[XPOS] * gv->fadjust;
      player1->rot += ROT_Z * gv->fadjust;

      if ( obj->pos[XPOS] < MIN_X )
         obj->pos[XPOS] = MIN_X;

      if ( player1->rot > MAX_ROT )
         player1->rot = MAX_ROT;
   }

   if ( gv->key_RIGHT1 )
   {
      obj->pos[XPOS] += thrust_right[XPOS] * gv->fadjust;
      player1->rot -= ROT_Z * gv->fadjust;

      if ( obj->pos[XPOS] > MAX_X )
         obj->pos[XPOS] = MAX_X;

      if ( player1->rot < -MAX_ROT )
         player1->rot = -MAX_ROT;
   }

   if ( (gv->key_LEFT1 == FALSE) && (gv->key_RIGHT1 == FALSE) && \
         player1->rot )
   {
      if(player1->rot > 0) 
      {
         player1->rot -= ROT_Z * gv->fadjust;
	 if(player1->rot < 0)
            player1->rot = 0;
      } 
      else 
      {
         player1->rot += ROT_Z * gv->fadjust;
	 if(player1->rot > 0)
	    player1->rot = 0;
      }
   }

   if ( gv->key_FIRE1 && !pm1->active )
   {
      pm1->active      = TRUE;
      pm1->zone        = obj->zone;
      pm1->zheight     = obj->zheight;
      Vector_init ( pm1->pos );
      Vector_copy ( obj->pos, pm1->pos );
      Vector_copy ( obj->pos, pm1->old_pos );
      Vector_addd ( pm1->pos, missile_offset );
   }
}

void Player_update2 ( OBJECT *obj )
{
   if ( gv->key_UP2 )
   {
      obj->pos[YPOS] += thrust_up[YPOS] * gv->fadjust;
      Object_update_zone ( obj );
      if ( obj->pos[YPOS] > MAX_Y )
         obj->pos[YPOS] = MAX_Y;
   }

   if ( gv->key_DOWN2 )
   {
      obj->pos[YPOS] += thrust_down[YPOS] * gv->fadjust;
      Object_update_zone ( obj );
      if ( obj->pos[YPOS] < MIN_Y )
         obj->pos[YPOS] = MIN_Y;
   }

   if ( gv->key_LEFT2 )
   {
      obj->pos[XPOS] += thrust_left[XPOS] * gv->fadjust;
      player2->rot += ROT_Z * gv->fadjust;

      if ( obj->pos[XPOS] < MIN_X )
         obj->pos[XPOS] = MIN_X;

      if ( player2->rot > MAX_ROT )
         player2->rot = MAX_ROT;
   }

   if ( gv->key_RIGHT2 )
   {
      obj->pos[XPOS] += thrust_right[XPOS] * gv->fadjust;
      player2->rot -= ROT_Z * gv->fadjust;

      if ( obj->pos[XPOS] > MAX_X )
         obj->pos[XPOS] = MAX_X;

      if ( player2->rot < -MAX_ROT )
         player2->rot = -MAX_ROT;
   }

   if ( (gv->key_LEFT2 == FALSE) && (gv->key_RIGHT2 == FALSE) && \
         player2->rot )
   {
      if(player2->rot > 0) 
      {
         player2->rot -= ROT_Z * gv->fadjust;
	 if(player2->rot < 0)
            player2->rot = 0;
      } 
      else 
      {
         player2->rot += ROT_Z * gv->fadjust;
	 if(player2->rot > 0)
	    player2->rot = 0;
      }
   }

   if ( gv->key_FIRE2 && !pm2->active )
   {
      pm2->active      = TRUE;
      pm2->zone        = obj->zone;
      pm2->zheight     = obj->zheight;
      Vector_init ( pm2->pos );
      Vector_copy ( obj->pos, pm2->pos );
      Vector_copy ( obj->pos, pm2->old_pos );
      Vector_addd ( pm2->pos, missile_offset );
   }
}

void Player_blink1 ( OBJECT *obj, MAT r1, MAT r2 )
{
   static long blink = 0;
   static int color = 0;

   blink += gv->msec;
   if ( blink > 50 )
   {
      blink -= 250;
      color++;
      if ( color >= 3 )
         color = 0;
   }

   pcolor1 = RED - color * 15;

   obj->frame += gv->msec;
   if ( obj->frame > PLAYER_BLINK_TIME )
   {
      pcolor1 = RED;
      gv->pblink1 = FALSE;
      Object_set_drawfn ( obj, Player_draw1 );
   }

   Player_draw1 ( obj, r1, r2 );
}

void Player_blink2 ( OBJECT *obj, MAT r1, MAT r2 )
{
   static long blink = 0;
   static int color = 0;

   blink += gv->msec;
   if ( blink > 50 )
   {
      blink -= 250;
      color++;
      if ( color >= 3 )
         color = 0;
   }

   pcolor2 = BLUE - color * 15;

   obj->frame += gv->msec;
   if ( obj->frame > PLAYER_BLINK_TIME )
   {
      pcolor2 = BLUE;
      gv->pblink2 = FALSE;
      Object_set_drawfn ( obj, Player_draw2 );
   }

   Player_draw2 ( obj, r1, r2 );
}

static VEC player_vert [16] = { { -5.0f, 0.0f, 15.0f, 1.0f },  /*body*/
                                { -10.0f, 0.0f, -20.0f, 1.0f },
                                { 10.0f, 0.0f, -20.0f, 1.0f },
                                { 5.0f, 0.0f, 15.0f, 1.0f },
                                { -5.0f, 10.0f, -18.0f, 1.0f },
                                { 5.0f, 10.0f, -18.0f, 1.0f },
                                { -5.0f, 0.0f, 20.0f, 1.0f }, /* left e*/
                                { -10.0f, 0.0f, -15.0f, 1.0f },
                                { -12.0f, 0.0f, -12.0f, 1.0f },
                                { -10.0f, 0.0f, 25.0f, 1.0f },
                                { 5.0f, 0.0f, 20.0f, 1.0f }, /* re */
                                { 10.0f, 0.0f, -15.0f, 1.0f },
                                { 12.0f, 0.0f, -12.0f, 1.0f },
                                { 10.0f, 0.0f, 25.0f, 1.0f },
                                { -20.0f, -5.0f, 35.0f, 1.0f }, /*lw*/
                                { 20.0f, -5.0f, 35.0f, 1.0f } }; /*rw*/

static VEC player_vert1 [16] = { { -5.0f, 0.0f, 15.0f, 1.0f },  /*body*/
                                 { -10.0f, 0.0f, -20.0f, 1.0f },
                                 { 10.0f, 0.0f, -20.0f, 1.0f },
                                 { 5.0f, 0.0f, 15.0f, 1.0f },
                                 { -5.0f, 10.0f, -18.0f, 1.0f },
                                 { 5.0f, 10.0f, -18.0f, 1.0f },
                                 { -5.0f, 0.0f, 20.0f, 1.0f }, /* left e*/
                                 { -10.0f, 0.0f, -15.0f, 1.0f },
                                 { -12.0f, 0.0f, -12.0f, 1.0f },
                                 { -10.0f, 0.0f, 25.0f, 1.0f },
                                 { 5.0f, 0.0f, 20.0f, 1.0f }, /* re */
                                 { 10.0f, 0.0f, -15.0f, 1.0f },
                                 { 12.0f, 0.0f, -12.0f, 1.0f },
                                 { 10.0f, 0.0f, 25.0f, 1.0f },
                                 { -20.0f, -5.0f, 35.0f, 1.0f }, /*lw*/
                                 { 20.0f, -5.0f, 35.0f, 1.0f } }; /*rw*/

static VEC cross_hairs[4] = { { -15.0f, 0.0f, 0.0f, 1.0f },
                              { 0.0f, 15.0f, 0.0f, 1.0f },
                              { 15.0f, 0.0f, 0.0f, 1.0f },
                              { 0.0f, -15.0f, 0.0f, 1.0f } };

void Player_draw1 ( OBJECT *obj, MAT r1, MAT r2 )
{
   int pcol, p1[32], p2[32];
   int i, x1R, y1B, x2L, y2T;
   VEC tmp[16], rot_vert[16];
   MAT tmp_mat, rot_mat;
   VEC offset;

   x1R = gv->x1r;
   y1B = gv->y1b;
   x2L = gv->x2l;
   y2T = gv->y2t;

   pcol = pcolor1;

   if ( gv->pduel )
   {
      Matrix_vec_mult ( r2, obj->pos, tmp[0] );
      Matrix_copy ( r2, tmp_mat );
      Matrix_set_trans ( tmp_mat, tmp[0] );

      Matrix_z_rot ( rot_mat, player1->rot );
      Matrix_vec_multn ( rot_mat, player_vert, rot_vert, 16 );
      Matrix_vec_multn ( tmp_mat, rot_vert, tmp, 16 );
      Camera_project_points ( tmp, p2, 16 );
   }
   Matrix_vec_mult ( r1, obj->pos, tmp[0] );
   Matrix_copy ( r1, tmp_mat );
   Matrix_set_trans ( tmp_mat, tmp[0] );

   Matrix_z_rot ( rot_mat, player1->rot );
   Matrix_vec_multn ( rot_mat, player_vert, rot_vert, 16 );
   Matrix_vec_multn ( tmp_mat, rot_vert, tmp, 16 );
   Camera_project_points ( tmp, p1, 16 );

   if ( gv->pduel )
   {
      if ( gv->phorizontal )
      {
         for ( i = 1; i < 32; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + y2T;
         }
      }
      else if ( gv->pvertical )
      {
         for ( i = 0; i < 32; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + x2L;
         }
      }
      else
      {
         for ( i = 0; i < 32; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + x2L;
         }
         for ( i = 1; i < 32; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + y2T;
         }
      }

      /* body bottom */
      if ( p2[0] >= x2L && p2[2] >= x2L && \
           p2[1] >= y2T && p2[3] >= y2T )
         Draw_line ( p2[0], p2[1], p2[2], p2[3], pcol );
      if ( p2[2] >= x2L && p2[4] >= x2L && \
           p2[3] >= y2T && p2[5] >= y2T )
         Draw_line ( p2[2], p2[3], p2[4], p2[5], pcol );
      if ( p2[4] >= x2L && p2[6] >= x2L && \
           p2[5] >= y2T && p2[7] >= y2T )
         Draw_line ( p2[4], p2[5], p2[6], p2[7], pcol );
      if ( p2[6] >= x2L && p2[0] >= x2L && \
           p2[7] >= y2T && p2[1] >= y2T )
         Draw_line ( p2[6], p2[7], p2[0], p2[1], pcol );

      /* body top */
      if ( p2[0] >= x2L && p2[8] >= x2L && \
           p2[1] >= y2T && p2[9] >= y2T )
         Draw_line ( p2[0], p2[1], p2[8], p2[9], pcol );
      if ( p2[2] >= x2L && p2[2] >= x2L && \
           p2[3] >= y2T && p2[3] >= y2T )
         Draw_line ( p2[8], p2[9], p2[2], p2[3], pcol );
      if ( p2[4] >= x2L && p2[10] >= x2L && \
           p2[5] >= y2T && p2[11] >= y2T )
         Draw_line ( p2[4], p2[5], p2[10], p2[11], pcol );
      if ( p2[10] >= x2L && p2[6] >= x2L && \
           p2[11] >= y2T && p2[7] >= y2T )
         Draw_line ( p2[10], p2[11], p2[6], p2[7], pcol );
      if ( p2[8] >= x2L && p2[10] >= x2L && \
           p2[9] >= y2T && p2[11] >= y2T )
         Draw_line ( p2[8], p2[9], p2[10], p2[11], pcol );

      /* left e */
      if ( p2[12] >= x2L && p2[14] >= x2L && \
           p2[13] >= y2T && p2[15] >= y2T )
         Draw_line ( p2[12], p2[13], p2[14], p2[15], pcol );
      if ( p2[14] >= x2L && p2[16] >= x2L && \
           p2[15] >= y2T && p2[17] >= y2T )
         Draw_line ( p2[14], p2[15], p2[16], p2[17], pcol );
      if ( p2[16] >= x2L && p2[18] >= x2L && \
           p2[17] >= y2T && p2[19] >= y2T )
         Draw_line ( p2[16], p2[17], p2[18], p2[19], pcol );
      if ( p2[18] >= x2L && p2[12] >= x2L && \
           p2[19] >= y2T && p2[13] >= y2T )
         Draw_line ( p2[18], p2[19], p2[12], p2[13], pcol );

      /* right e */
      if ( p2[20] >= x2L && p2[22] >= x2L && \
           p2[21] >= y2T && p2[23] >= y2T )
         Draw_line ( p2[20], p2[21], p2[22], p2[23], pcol );
      if ( p2[22] >= x2L && p2[24] >= x2L && \
           p2[23] >= y2T && p2[25] >= y2T )
         Draw_line ( p2[22], p2[23], p2[24], p2[25], pcol );
      if ( p2[24] >= x2L && p2[26] >= x2L && \
           p2[25] >= y2T && p2[27] >= y2T )
         Draw_line ( p2[24], p2[25], p2[26], p2[27], pcol );
      if ( p2[26] >= x2L && p2[20] >= x2L && \
           p2[27] >= y2T && p2[21] >= y2T )
         Draw_line ( p2[26], p2[27], p2[20], p2[21], pcol );

      /* left wing */
      if ( p2[28] >= x2L && p2[16] >= x2L && \
           p2[29] >= y2T && p2[17] >= y2T )
         Draw_line ( p2[28], p2[29], p2[16], p2[17], pcol );
      if ( p2[28] >= x2L && p2[18] >= x2L && \
           p2[29] >= y2T && p2[19] >= y2T )
         Draw_line ( p2[28], p2[29], p2[18], p2[19], pcol );

      /* right wing */
      if ( p2[30] >= x2L && p2[24] >= x2L && \
           p2[31] >= y2T && p2[25] >= y2T )
         Draw_line ( p2[30], p2[31], p2[24], p2[25], pcol );
      if ( p2[30] >= x2L && p2[26] >= x2L && \
           p2[31] >= y2T && p2[27] >= y2T )
         Draw_line ( p2[30], p2[31], p2[26], p2[27], pcol );
   }
   /* body bottom */
   if ( p1[0] < x1R && p1[2] < x1R && \
        p1[1] < y1B && p1[3] < y1B )
      Draw_line ( p1[0], p1[1], p1[2], p1[3], pcol );
   if ( p1[2] < x1R && p1[4] < x1R && \
        p1[3] < y1B && p1[5] < y1B )
      Draw_line ( p1[2], p1[3], p1[4], p1[5], pcol );
   if ( p1[4] < x1R && p1[6] < x1R && \
        p1[5] < y1B && p1[7] < y1B )
      Draw_line ( p1[4], p1[5], p1[6], p1[7], pcol );
   if ( p1[6] < x1R && p1[0] < x1R && \
        p1[7] < y1B && p1[1] < y1B )
      Draw_line ( p1[6], p1[7], p1[0], p1[1], pcol );

   /* body top */
   if ( p1[0] < x1R && p1[8] < x1R && \
        p1[1] < y1B && p1[9] < y1B )
      Draw_line ( p1[0], p1[1], p1[8], p1[9], pcol );
   if ( p1[8] < x1R && p1[2] < x1R && \
        p1[9] < y1B && p1[3] < y1B )
      Draw_line ( p1[8], p1[9], p1[2], p1[3], pcol );
   if ( p1[4] < x1R && p1[10] < x1R && \
        p1[5] < y1B && p1[11] < y1B )
      Draw_line ( p1[4], p1[5], p1[10], p1[11], pcol );
   if ( p1[10] < x1R && p1[6] < x1R && \
        p1[11] < y1B && p1[7] < y1B )
      Draw_line ( p1[10], p1[11], p1[6], p1[7], pcol );
   if ( p1[8] < x1R && p1[10] < x1R && \
        p1[9] < y1B && p1[11] < y1B )
      Draw_line ( p1[8], p1[9], p1[10], p1[11], pcol );

   /* left e */
   if ( p1[12] < x1R && p1[14] < x1R && \
        p1[13] < y1B && p1[15] < y1B )
      Draw_line ( p1[12], p1[13], p1[14], p1[15], pcol );
   if ( p1[14] < x1R && p1[16] < x1R && \
        p1[15] < y1B && p1[17] < y1B )
      Draw_line ( p1[14], p1[15], p1[16], p1[17], pcol );
   if ( p1[16] < x1R && p1[18] < x1R && \
        p1[17] < y1B && p1[19] < y1B )
      Draw_line ( p1[16], p1[17], p1[18], p1[19], pcol );
   if ( p1[18] < x1R && p1[12] < x1R && \
        p1[19] < y1B && p1[13] < y1B )
      Draw_line ( p1[18], p1[19], p1[12], p1[13], pcol );

   /* right e */
   if ( p1[20] < x1R && p1[22] < x1R && \
        p1[21] < y1B && p1[23] < y1B )
      Draw_line ( p1[20], p1[21], p1[22], p1[23], pcol );
   if ( p1[22] < x1R && p1[24] < x1R && \
        p1[23] < y1B && p1[25] < y1B )
      Draw_line ( p1[22], p1[23], p1[24], p1[25], pcol );
   if ( p1[24] < x1R && p1[26] < x1R && \
        p1[25] < y1B && p1[27] < y1B )
      Draw_line ( p1[24], p1[25], p1[26], p1[27], pcol );
   if ( p1[26] < x1R && p1[20] < x1R && \
        p1[27] < y1B && p1[21] < y1B )
      Draw_line ( p1[26], p1[27], p1[20], p1[21], pcol );

   /* left wing */
   if ( p1[28] < x1R && p1[16] < x1R && \
        p1[29] < y1B && p1[17] < y1B )
      Draw_line ( p1[28], p1[29], p1[16], p1[17], pcol );
   if ( p1[28] < x1R && p1[18] < x1R && \
        p1[29] < y1B && p1[19] < y1B )
      Draw_line ( p1[28], p1[29], p1[18], p1[19], pcol );

   /* right wing */
   if ( p1[30] < x1R && p1[24] < x1R && \
        p1[31] < y1B && p1[25] < y1B )
      Draw_line ( p1[30], p1[31], p1[24], p1[25], pcol );
   if ( p1[30] < x1R && p1[26] < x1R && \
        p1[31] < y1B && p1[27] < y1B )
      Draw_line ( p1[30], p1[31], p1[26], p1[27], pcol );

  /* update cross-hairs */
   Vector_copy ( obj->pos, tmp[0] );
   Vector_init ( offset );
   offset[ZPOS] = -((gv->formation_zone * 100.0f) + 50.0f);

   Vector_addd ( tmp[0], offset );
   Matrix_vec_mult ( r1, tmp[0], tmp[1] );
   Matrix_copy ( r1, tmp_mat );
   Matrix_set_trans ( tmp_mat, tmp[1] );

   Matrix_vec_multn ( rot_mat, cross_hairs, rot_vert, 4 );
   Matrix_vec_multn ( tmp_mat, rot_vert, tmp, 4 );
   Camera_project_points ( tmp, p1, 4 );

   if ( gv->pduel )
   {
      if ( gv->phorizontal )
      {
         for ( i = 1; i < 8; i += 2 )
            p1[i] /= 2;
      }
      else if ( gv->pvertical )
      {
         for ( i = 0; i < 8; i += 2 )
            p1[i] /= 2;
      }
      else
      {
         for ( i = 0; i < 8; i++ )
            p1[i] /= 2;
      }
   }

   if ( p1[0] < x1R && p1[2] < x1R && \
        p1[1] < y1B && p1[3] < y1B )
      Draw_line ( p1[0], p1[1], p1[2], p1[3], WHITE );
   if ( p1[2] < x1R && p1[4] < x1R && \
        p1[3] < y1B && p1[5] < y1B )
      Draw_line ( p1[2], p1[3], p1[4], p1[5], WHITE );
   if ( p1[4] < x1R && p1[6] < x1R && \
        p1[5] < y1B && p1[7] < y1B )
      Draw_line ( p1[4], p1[5], p1[6], p1[7], WHITE );
   if ( p1[6] < x1R && p1[0] < x1R && \
        p1[7] < y1B && p1[1] < y1B )
      Draw_line ( p1[6], p1[7], p1[0], p1[1], WHITE );
}

void Player_draw2 ( OBJECT *obj, MAT r1, MAT r2 )
{
   int pcol, p1[32], p2[32];
   int i, x1R, y1B, x2L, y2T;
   VEC tmp[16], rot_vert[16];
   MAT tmp_mat, rot_mat;
   VEC offset;

   x1R = gv->x1r;
   y1B = gv->y1b;
   x2L = gv->x2l;
   y2T = gv->y2t;

   pcol = pcolor2;

   Matrix_vec_mult ( r1, obj->pos, tmp[0] );
   Matrix_copy ( r1, tmp_mat );
   Matrix_set_trans ( tmp_mat, tmp[0] );

   Matrix_z_rot ( rot_mat, player2->rot );
   Matrix_vec_multn ( rot_mat, player_vert, rot_vert, 16 );
   Matrix_vec_multn ( tmp_mat, rot_vert, tmp, 16 );
   Camera_project_points ( tmp, p1, 16 );

   Matrix_vec_mult ( r2, obj->pos, tmp[0] );
   Matrix_copy ( r2, tmp_mat );
   Matrix_set_trans ( tmp_mat, tmp[0] );

   Matrix_z_rot ( rot_mat, player2->rot );
   Matrix_vec_multn ( rot_mat, player_vert, rot_vert, 16 );
   Matrix_vec_multn ( tmp_mat, rot_vert, tmp, 16 );
   Camera_project_points ( tmp, p2, 16 );

   if ( gv->phorizontal )
   {
      for ( i = 1; i < 32; i += 2 )
      {
         p1[i] /= 2;
         p2[i] = p2[i] / 2 + y2T;
      }
   }
   else if ( gv->pvertical )
   {
      for ( i = 0; i < 32; i += 2 )
      {
         p1[i] /= 2;
         p2[i] = p2[i] / 2 + x2L;
      }
   }
   else
   {
      for ( i = 0; i < 32; i += 2 )
      {
         p1[i] /= 2;
         p2[i] = p2[i] / 2 + x2L;
      }
      for ( i = 1; i < 32; i += 2 )
      {
         p1[i] /= 2;
         p2[i] = p2[i] / 2 + y2T;
      }
   }

   /* body bottom */
   if ( p1[0] < x1R && p1[2] < x1R && \
        p1[1] < y1B && p1[3] < y1B )
      Draw_line ( p1[0], p1[1], p1[2], p1[3], pcol );
   if ( p1[2] < x1R && p1[4] < x1R && \
        p1[3] < y1B && p1[5] < y1B )
      Draw_line ( p1[2], p1[3], p1[4], p1[5], pcol );
   if ( p1[4] < x1R && p1[6] < x1R && \
        p1[5] < y1B && p1[7] < y1B )
      Draw_line ( p1[4], p1[5], p1[6], p1[7], pcol );
   if ( p1[6] < x1R && p1[0] < x1R && \
        p1[7] < y1B && p1[1] < y1B )
      Draw_line ( p1[6], p1[7], p1[0], p1[1], pcol );
   if ( p2[0] >= x2L && p2[2] >= x2L && \
        p2[1] >= y2T && p2[3] >= y2T )
      Draw_line ( p2[0], p2[1], p2[2], p2[3], pcol );
   if ( p2[2] >= x2L && p2[4] >= x2L && \
        p2[3] >= y2T && p2[5] >= y2T )
      Draw_line ( p2[2], p2[3], p2[4], p2[5], pcol );
   if ( p2[4] >= x2L && p2[6] >= x2L && \
        p2[5] >= y2T && p2[7] >= y2T )
      Draw_line ( p2[4], p2[5], p2[6], p2[7], pcol );
   if ( p2[6] >= x2L && p2[0] >= x2L && \
        p2[7] >= y2T && p2[1] >= y2T )
      Draw_line ( p2[6], p2[7], p2[0], p2[1], pcol );

   /* body top */
   if ( p1[0] < x1R && p1[8] < x1R && \
        p1[1] < y1B && p1[9] < y1B )
      Draw_line ( p1[0], p1[1], p1[8], p1[9], pcol );
   if ( p1[8] < x1R && p1[2] < x1R && \
        p1[9] < y1B && p1[3] < y1B )
      Draw_line ( p1[8], p1[9], p1[2], p1[3], pcol );
   if ( p1[4] < x1R && p1[10] < x1R && \
        p1[5] < y1B && p1[11] < y1B )
      Draw_line ( p1[4], p1[5], p1[10], p1[11], pcol );
   if ( p1[10] < x1R && p1[6] < x1R && \
        p1[11] < y1B && p1[7] < y1B )
      Draw_line ( p1[10], p1[11], p1[6], p1[7], pcol );
   if ( p1[8] < x1R && p1[10] < x1R && \
        p1[9] < y1B && p1[11] < y1B )
      Draw_line ( p1[8], p1[9], p1[10], p1[11], pcol );
   if ( p2[0] >= x2L && p2[8] >= x2L && \
        p2[1] >= y2T && p2[9] >= y2T )
      Draw_line ( p2[0], p2[1], p2[8], p2[9], pcol );
   if ( p2[8] >= x2L && p2[2] >= x2L && \
        p2[9] >= y2T && p2[3] >= y2T )
      Draw_line ( p2[8], p2[9], p2[2], p2[3], pcol );
   if ( p2[4] >= x2L && p2[10] >= x2L && \
        p2[5] >= y2T && p2[11] >= y2T )
      Draw_line ( p2[4], p2[5], p2[10], p2[11], pcol );
   if ( p2[10] >= x2L && p2[6] >= x2L && \
        p2[11] >= y2T && p2[7] >= y2T )
      Draw_line ( p2[10], p2[11], p2[6], p2[7], pcol );
   if ( p2[8] >= x2L && p2[10] >= x2L && \
        p2[9] >= y2T && p2[11] >= y2T )
      Draw_line ( p2[8], p2[9], p2[10], p2[11], pcol );

   /* left e */
   if ( p1[12] < x1R && p1[14] < x1R && \
        p1[13] < y1B && p1[15] < y1B )
      Draw_line ( p1[12], p1[13], p1[14], p1[15], pcol );
   if ( p1[14] < x1R && p1[16] < x1R && \
        p1[15] < y1B && p1[17] < y1B )
      Draw_line ( p1[14], p1[15], p1[16], p1[17], pcol );
   if ( p1[16] < x1R && p1[18] < x1R && \
        p1[17] < y1B && p1[19] < y1B )
      Draw_line ( p1[16], p1[17], p1[18], p1[19], pcol );
   if ( p1[18] < x1R && p1[12] < x1R && \
        p1[19] < y1B && p1[13] < y1B )
      Draw_line ( p1[18], p1[19], p1[12], p1[13], pcol );
   if ( p2[12] >= x2L && p2[14] >= x2L && \
        p2[13] >= y2T && p2[15] >= y2T )
      Draw_line ( p2[12], p2[13], p2[14], p2[15], pcol );
   if ( p2[14] >= x2L && p2[16] >= x2L && \
        p2[15] >= y2T && p2[17] >= y2T )
      Draw_line ( p2[14], p2[15], p2[16], p2[17], pcol );
   if ( p2[16] >= x2L && p2[18] >= x2L && \
        p2[17] >= y2T && p2[19] >= y2T )
      Draw_line ( p2[16], p2[17], p2[18], p2[19], pcol );
   if ( p2[18] >= x2L && p2[12] >= x2L && \
        p2[19] >= y2T && p2[13] >= y2T )
      Draw_line ( p2[18], p2[19], p2[12], p2[13], pcol );

   /* right e */
   if ( p1[20] < x1R && p1[22] < x1R && \
        p1[21] < y1B && p1[23] < y1B )
      Draw_line ( p1[20], p1[21], p1[22], p1[23], pcol );
   if ( p1[22] < x1R && p1[24] < x1R && \
        p1[23] < y1B && p1[25] < y1B )
      Draw_line ( p1[22], p1[23], p1[24], p1[25], pcol );
   if ( p1[24] < x1R && p1[26] < x1R && \
        p1[25] < y1B && p1[27] < y1B )
      Draw_line ( p1[24], p1[25], p1[26], p1[27], pcol );
   if ( p1[26] < x1R && p1[20] < x1R && \
        p1[27] < y1B && p1[21] < y1B )
      Draw_line ( p1[26], p1[27], p1[20], p1[21], pcol );
   if ( p2[0] >= x2L && p2[2] >= x2L && \
        p2[1] >= y2T && p2[3] >= y2T )
      Draw_line ( p2[20], p2[21], p2[22], p2[23], pcol );
   if ( p2[22] >= x2L && p2[24] >= x2L && \
        p2[23] >= y2T && p2[25] >= y2T )
      Draw_line ( p2[22], p2[23], p2[24], p2[25], pcol );
   if ( p2[24] >= x2L && p2[26] >= x2L && \
        p2[25] >= y2T && p2[27] >= y2T )
      Draw_line ( p2[24], p2[25], p2[26], p2[27], pcol );
   if ( p2[26] >= x2L && p2[20] >= x2L && \
        p2[27] >= y2T && p2[21] >= y2T )
      Draw_line ( p2[26], p2[27], p2[20], p2[21], pcol );

   /* left wing */
   if ( p1[28] < x1R && p1[16] < x1R && \
        p1[29] < y1B && p1[17] < y1B )
      Draw_line ( p1[28], p1[29], p1[16], p1[17], pcol );
   if ( p1[28] < x1R && p1[18] < x1R && \
        p1[29] < y1B && p1[19] < y1B )
      Draw_line ( p1[28], p1[29], p1[18], p1[19], pcol );
   if ( p2[28] >= x2L && p2[16] >= x2L && \
        p2[29] >= y2T && p2[17] >= y2T )
      Draw_line ( p2[28], p2[29], p2[16], p2[17], pcol );
   if ( p2[28] >= x2L && p2[18] >= x2L && \
        p2[29] >= y2T && p2[19] >= y2T )
      Draw_line ( p2[28], p2[29], p2[18], p2[19], pcol );

   /* right wing */
   if ( p1[30] < x1R && p1[24] < x1R && \
        p1[31] < y1B && p1[25] < y1B )
      Draw_line ( p1[30], p1[31], p1[24], p1[25], pcol );
   if ( p1[30] < x1R && p1[26] < x1R && \
        p1[31] < y1B && p1[27] < y1B )
      Draw_line ( p1[30], p1[31], p1[26], p1[27], pcol );
   if ( p2[30] >= x2L && p2[24] >= x2L && \
        p2[31] >= y2T && p2[25] >= y2T )
      Draw_line ( p2[30], p2[31], p2[24], p2[25], pcol );
   if ( p2[30] >= x2L && p2[26] >= x2L && \
        p2[31] >= y2T && p2[27] >= y2T )
      Draw_line ( p2[30], p2[31], p2[26], p2[27], pcol );

  /* update cross-hairs */
   Vector_copy ( obj->pos, tmp[0] );
   Vector_init ( offset );
   offset[ZPOS] = -((gv->formation_zone * 100.0f) + 50.0f);

   Vector_addd ( tmp[0], offset );
   Matrix_vec_mult ( r2, tmp[0], tmp[1] );
   Matrix_copy ( r2, tmp_mat );
   Matrix_set_trans ( tmp_mat, tmp[1] );

   Matrix_vec_multn ( rot_mat, cross_hairs, rot_vert, 4 );
   Matrix_vec_multn ( tmp_mat, rot_vert, tmp, 4 );
   Camera_project_points ( tmp, p2, 4 );

   if ( gv->phorizontal )
   {
      for ( i = 1; i < 8; i += 2 )
         p2[i] = p2[i] / 2 + y2T;
   }
   else if ( gv->pvertical )
   {
      for ( i = 0; i < 8; i += 2 )
         p2[i] = p2[i] / 2 + x2L;
   }
   else
   {
      for ( i = 0; i < 8; i += 2 )
         p2[i] = p2[i] / 2 + x2L;
      for ( i = 1; i < 8; i += 2 )
         p2[i] = p2[i] / 2 + y2T;
   }

   if ( p2[0] >= x2L && p2[2] >= x2L && \
        p2[1] >= y2T && p2[3] >= y2T )
      Draw_line ( p2[0], p2[1], p2[2], p2[3], WHITE );
   if ( p2[2] >= x2L && p2[4] >= x2L && \
        p2[3] >= y2T && p2[5] >= y2T )
      Draw_line ( p2[2], p2[3], p2[4], p2[5], WHITE );
   if ( p2[4] >= x2L && p2[6] >= x2L && \
        p2[5] >= y2T && p2[7] >= y2T )
      Draw_line ( p2[4], p2[5], p2[6], p2[7], WHITE );
   if ( p2[6] >= x2L && p2[0] >= x2L && \
        p2[7] >= y2T && p2[1] >= y2T )
      Draw_line ( p2[6], p2[7], p2[0], p2[1], WHITE );
}

/*================================================================*/

void Player_missile_update ( OBJECT *obj )
{
   Vector_copy ( obj->pos, obj->old_pos );
   obj->pos[ZPOS] += missile_thrust[ZPOS] * gv->fadjust;
   Object_update_zone ( obj );

   if ( obj->zone > ZONE_9 )
      obj->active = FALSE;
}

static VEC missile_vert[4] = { {-10.0f, 0.0f, -5.0f, 1.0f},
                               {0.0f, 0.0f, -10.0f, 1.0f},
                               {10.0f, 0.0f, -5.0f, 1.0f},
                               {0.0f, 0.0f, 20.0f, 1.0f} };

void Player_missile_draw1 ( OBJECT *obj, MAT r1, MAT r2 )
{
   VEC tmp[4];
   MAT tmp_mat;
   int i, x1R, y1B, x2L, y2T;
   int p1[8], p2[8];

   x1R = gv->x1r;
   y1B = gv->y1b;
   x2L = gv->x2l;
   y2T = gv->y2t;

   Matrix_vec_mult ( r1, obj->pos, tmp[0] );
   Matrix_copy ( r1, tmp_mat );
   Matrix_set_trans ( tmp_mat, tmp[0] );

   Matrix_vec_multn ( tmp_mat, missile_vert, tmp, 4 );

   Camera_project_points ( tmp, p1, 4 );

   if ( gv->pduel )
   {
      Matrix_vec_mult ( r2, obj->pos, tmp[0] );
      Matrix_copy ( r2, tmp_mat );
      Matrix_set_trans ( tmp_mat, tmp[0] );

      Matrix_vec_multn ( tmp_mat, missile_vert, tmp, 4 );

      Camera_project_points ( tmp, p2, 4 );

      if ( gv->phorizontal )
      {
         for ( i = 1; i < 8; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + y2T;
         }
      }
      else if ( gv->pvertical )
      {
         for ( i = 0; i < 8; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + x2L;
         }
      }
      else
      {
         for ( i = 0; i < 8; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + x2L;
         }
         for ( i = 1; i < 8; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + y2T;
         }
      }
      if ( p2[0] >= x2L && p2[2] >= x2L && \
           p2[1] >= y2T && p2[3] >= y2T )
         Draw_line ( p2[0], p2[1], p2[2], p2[3], YELLOW );
      if ( p2[2] >= x2L && p2[4] >= x2L && \
           p2[3] >= y2T && p2[5] >= y2T )
         Draw_line ( p2[2], p2[3], p2[4], p2[5], YELLOW );
      if ( p2[4] >= x2L && p2[6] >= x2L && \
           p2[5] >= y2T && p2[7] >= y2T )
         Draw_line ( p2[4], p2[5], p2[6], p2[7], YELLOW );
      if ( p2[6] >= x2L && p2[0] >= x2L && \
           p2[7] >= y2T && p2[1] >= y2T )
         Draw_line ( p2[6], p2[7], p2[0], p2[1], YELLOW );
   }
   if ( p1[0] < x1R && p1[2] < x1R && \
        p1[1] < y1B && p1[3] < y1B )
      Draw_line ( p1[0], p1[1], p1[2], p1[3], YELLOW );
   if ( p1[2] < x1R && p1[4] < x1R && \
        p1[3] < y1B && p1[5] < y1B )
      Draw_line ( p1[2], p1[3], p1[4], p1[5], YELLOW );
   if ( p1[4] < x1R && p1[6] < x1R && \
        p1[5] < y1B && p1[7] < y1B )
      Draw_line ( p1[4], p1[5], p1[6], p1[7], YELLOW );
   if ( p1[6] < x1R && p1[0] < x1R && \
        p1[7] < y1B && p1[1] < y1B )
      Draw_line ( p1[6], p1[7], p1[0], p1[1], YELLOW );
}

void Player_missile_draw2 ( OBJECT *obj, MAT r1, MAT r2 )
{
   VEC tmp[4];
   MAT tmp_mat;
   int i, x1R, y1B, x2L, y2T;
   int p1[8], p2[8];

   x1R = gv->x1r;
   y1B = gv->y1b;
   x2L = gv->x2l;
   y2T = gv->y2t;

   Matrix_vec_mult ( r1, obj->pos, tmp[0] );
   Matrix_copy ( r1, tmp_mat );
   Matrix_set_trans ( tmp_mat, tmp[0] );

   Matrix_vec_multn ( tmp_mat, missile_vert, tmp, 4 );

   Camera_project_points ( tmp, p1, 4 );

   Matrix_vec_mult ( r2, obj->pos, tmp[0] );
   Matrix_copy ( r2, tmp_mat );
   Matrix_set_trans ( tmp_mat, tmp[0] );

   Matrix_vec_multn ( tmp_mat, missile_vert, tmp, 4 );

   Camera_project_points ( tmp, p2, 4 );

   if ( gv->phorizontal )
   {
      for ( i = 1; i < 8; i += 2 )
      {
         p1[i] /= 2;
         p2[i] = p2[i] / 2 + y2T;
      }
   }
   else if ( gv->pvertical )
   {
      for ( i = 0; i < 8; i += 2 )
      {
         p1[i] /= 2;
         p2[i] = p2[i] / 2 + x2L;
      }
   }
   else
   {
      for ( i = 0; i < 8; i += 2 )
      {
         p1[i] /= 2;
         p2[i] = p2[i] / 2 + x2L;
      }
      for ( i = 1; i < 8; i += 2 )
      {
         p1[i] /= 2;
         p2[i] = p2[i] / 2 + y2T;
      }
   }

   if ( p1[0] < x1R && p1[2] < x1R && \
        p1[1] < y1B && p1[3] < y1B )
      Draw_line ( p1[0], p1[1], p1[2], p1[3], WHITE );
   if ( p1[2] < x1R && p1[4] < x1R && \
        p1[3] < y1B && p1[5] < y1B )
      Draw_line ( p1[2], p1[3], p1[4], p1[5], WHITE );
   if ( p1[4] < x1R && p1[6] < x1R && \
        p1[5] < y1B && p1[7] < y1B )
      Draw_line ( p1[4], p1[5], p1[6], p1[7], WHITE );
   if ( p1[6] < x1R && p1[0] < x1R && \
        p1[7] < y1B && p1[1] < y1B )
      Draw_line ( p1[6], p1[7], p1[0], p1[1], WHITE );

   if ( p2[0] >= x2L && p2[2] >= x2L && \
        p2[1] >= y2T && p2[3] >= y2T )
      Draw_line ( p2[0], p2[1], p2[2], p2[3], WHITE );
   if ( p2[2] >= x2L && p2[4] >= x2L && \
        p2[3] >= y2T && p2[5] >= y2T )
      Draw_line ( p2[2], p2[3], p2[4], p2[5], WHITE );
   if ( p2[4] >= x2L && p2[6] >= x2L && \
        p2[5] >= y2T && p2[7] >= y2T )
      Draw_line ( p2[4], p2[5], p2[6], p2[7], WHITE );
   if ( p2[6] >= x2L && p2[0] >= x2L && \
        p2[7] >= y2T && p2[1] >= y2T )
      Draw_line ( p2[6], p2[7], p2[0], p2[1], WHITE );
}

/*================================================================*/
