/*------------------------------------------------------------------
  effects.c:

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

/*------------------------------------------------------------------
 * Stars
 *
 *
------------------------------------------------------------------*/

enum stars_enum
{
   MAX_STARS = 100
};

struct STARSSTRUCT
{
   VEC          pos;
   int          thrust;
   unsigned int color;
} stars[MAX_STARS];

VEC star_start  = { 0.0f, 0.0f, -2000.0f, 1.0f };
VEC star_thrust[4] = { { 0.0f, 0.0f, 50.0f, 1.0f },
                       { 0.0f, 0.0f, 25.0f, 1.0f },
                       { 0.0f, 0.0f, 15.0f, 1.0f },
                       { 0.0f, 0.0f, 10.0f, 1.0f } };

void Stars_init ( void )
{
   int i, m;
   for ( i=0; i<MAX_STARS; i++ )
   {
      Vector_copy ( star_start, stars[i].pos );

      stars[i].pos[XPOS] = (float)((rand () % 1600) - 800);
      stars[i].pos[YPOS] = (float)((rand () % 1600) - 800);

      m = rand () % 4;
      stars[i].thrust = m;
      stars[i].color  = WHITE - (m*15);
   }
}

void Stars_draw ( MAT r1, MAT r2 )
{
   VEC tmp;
   int i, j, m, p1[2], p2[2];
   int x1L, x1R, y1T, y1B, x2L, y2T;

   x1L = gv->x1l;
   x1R = gv->x1r;
   y1T = gv->y1t;
   y1B = gv->y1b;
   x2L = gv->x2l;
   y2T = gv->y2t;

   if ( gv->pduel )
   {
      if ( gv->phorizontal )
      {
         Draw_line ( x1L, y1B, x1R, y1B, WHITE );
      }
      else if ( gv->pvertical )
      {
         Draw_line ( x1R, y1T, x1R, y1B, WHITE );
      }
   }

   for ( i=0; i<MAX_STARS; i++ )
   {
      j = stars[i].thrust;
      stars[i].pos[ZPOS] += star_thrust[j][ZPOS] * gv->fadjust;
      if ( stars[i].pos[ZPOS] > -1.0f )
      {
         m = rand() % 4;
         stars[i].pos[XPOS] = (float)((rand () % 1600) - 800);
         stars[i].pos[YPOS] = (float)((rand () % 1600) - 800);
         stars[i].thrust = m;
         stars[i].color = WHITE - (m*15);
         stars[i].pos[ZPOS] = -2000.0f;
      }
      Matrix_vec_mult ( r1, stars[i].pos, tmp );
      Camera_project_point ( tmp, p1 );

      if ( gv->pduel )
      {
         Matrix_vec_mult ( r2, stars[i].pos, tmp );
         Camera_project_point ( tmp, p2 );

         if ( gv->phorizontal )
         {
            p1[1] /= 2;
            p2[1] = p2[1] / 2 + y2T;
         }
         else if ( gv->pvertical )
         {
            p1[0] /= 2;
            p2[0] = p2[0] / 2 + x2L;
         }
         else
         {
            p1[0] /= 2;
            p1[1] /= 2;
            p2[0] = p2[0] / 2 + x2L;
            p2[1] = p2[1] / 2 + y2T;
         }
         if ( ( p2[0] >= x2L ) && ( p2[1] >= y2T ) )
            Draw_point ( p2[0], p2[1], stars[i].color );
      }
      if ( ( p1[0] < x1R ) && ( p1[1] < y1B ) )
         Draw_point ( p1[0], p1[1], stars[i].color );
   }
}

/*------------------------------------------------------------------
 * Explosions
 *
 *
------------------------------------------------------------------*/

#define EXPLOSION_ROT 0.23f

enum explosions_enum
{
   MAX_EXPLOSIONS        = 10,
   MAX_PARTICLES         = 4,
   EXPLOSIONS_LIFE       = 1500, /* 1.5 sec */
   EXPLOSION_BLEND_TIME  = 375,  /* 0.375 sec */
   EXPLOSION_COLOR_INC   = 15
};

struct EXPLOSIONSTRUCT
{
   VEC  pos[MAX_PARTICLES];
   int  thrust[MAX_PARTICLES];
   long frame;
   long blend;
   int  color;
   int  active;
} explosions[MAX_EXPLOSIONS];

static int   ecur;   /* EXPLOSION index */
static int   ecount; /* EXPLOSION count */
static int   pcur;   /* THURST index */
static float erot;   /* EXPLOSIONS rotation */

/* shard/particle thrust vectors */
VEC pthrust[8] = { {-2.0f, 0.0f, 0.0f, 1.0f},
                   {0.0f, -2.0f, 0.0f, 1.0f},
                   {2.0f, 0.0f, 0.0f, 1.0f},
                   {0.0f, 2.0f, 0.0f, 1.0f},
                   {-2.0f, 2.0f, 0.0f, 1.0f},
                   {2.0f, -2.0f, 0.0f, 1.0f},
                   {-2.0f, -2.0f, 0.0f, 1.0f},
                   {2.0f, 2.0f, 0.0f, 1.0f} };

VEC shard[3] = { {-5.0f, 0.0f, 0.0f, 1.0f},
                 { 0.0f, 5.0f, 0.0f, 1.0f},
                 { 5.0f, -5.0f, 0.0f, 1.0f} };

void Explosions_clear ( void )
{
   int i, j;

   ecur = 0;
   ecount = 0;
   pcur = 0;

   erot = 0.0f;

   for ( i=0; i<MAX_EXPLOSIONS; i++ )
   {
      explosions[i].active = FALSE;
      explosions[i].frame = 0L;
      for (j=0; j<MAX_PARTICLES; j++ )
         Vector_init ( explosions[i].pos[j] );
   }
}

int Explosions_count ( void )
{
   return ecount;
}

void Explosions_add ( OBJECT *obj )
{
   explosions[ecur].active = TRUE;
   explosions[ecur].frame = 0;
   explosions[ecur].color = GREEN;
   explosions[ecur].blend = 0;

   /* ok there are currently only 4 */
   Vector_copy ( obj->pos, explosions[ecur].pos[0] );
   Vector_copy ( obj->pos, explosions[ecur].pos[1] );
   Vector_copy ( obj->pos, explosions[ecur].pos[2] );
   Vector_copy ( obj->pos, explosions[ecur].pos[3] );

   explosions[ecur].thrust[0] = pcur;
   explosions[ecur].thrust[1] = pcur+1;
   explosions[ecur].thrust[2] = pcur+2;
   explosions[ecur].thrust[3] = pcur+3;

   ecur++;
   pcur += 4;
   ecount++;

   if ( ecur > MAX_EXPLOSIONS-1 )
      ecur = 0;

   if ( pcur == 8 )
      pcur = 0;
}

void Explosions_draw ( MAT r1, MAT r2)
{
   int i, j, k, p1[6], p2[6];
   int x1R, y1B, x2L, y2T;
   VEC tmp[3], shard_tmp[3];
   MAT tmp_mat, tmp_mat2, erot_mat;

   x1R = gv->x1r;
   y1B = gv->y1b;
   x2L = gv->x2l;
   y2T = gv->y2t;

   erot += EXPLOSION_ROT * gv->fadjust;
   Matrix_x_rot ( tmp_mat, erot );
   Matrix_z_rot ( tmp_mat2, erot );
   Matrix_mult  ( tmp_mat, tmp_mat2, erot_mat );
   Matrix_vec_multn ( erot_mat, shard, shard_tmp, 3 );

   for ( i=0; i<MAX_EXPLOSIONS; i++ )
   {
      if ( explosions[i].active )
      {
         explosions[i].frame += gv->msec;
         if ( explosions[i].frame > EXPLOSIONS_LIFE )
         {
            explosions[i].active = FALSE;
            ecount--;
         }
         explosions[i].blend += gv->msec;
         if ( explosions[i].blend > EXPLOSION_BLEND_TIME )
         {
            explosions[i].blend -= EXPLOSION_BLEND_TIME;
            explosions[i].color -= EXPLOSION_COLOR_INC;
         }

         for ( j=0; j<MAX_PARTICLES; j++ )
         {
            k = explosions[i].thrust[j];
            explosions[i].pos[j][XPOS] += pthrust[k][XPOS] * gv->fadjust;
            explosions[i].pos[j][YPOS] += pthrust[k][YPOS] * gv->fadjust;
            explosions[i].pos[j][ZPOS] += pthrust[k][ZPOS] * gv->fadjust;

            Matrix_vec_mult ( r1, explosions[i].pos[j], tmp[0] );
            Matrix_copy ( r1, tmp_mat );
            Matrix_set_trans ( tmp_mat, tmp[0] );

            Matrix_vec_multn ( tmp_mat, shard_tmp, tmp, 3 );
            Camera_project_points ( tmp, p1, 3 );

            if ( gv->pduel )
            {
               Matrix_vec_mult ( r2, explosions[i].pos[j], tmp[0] );
               Matrix_copy ( r2, tmp_mat );
               Matrix_set_trans ( tmp_mat, tmp[0] );

               Matrix_vec_multn ( tmp_mat, shard_tmp, tmp, 3 );
               Camera_project_points ( tmp, p2, 3 );

               if ( gv->phorizontal )
               {
                  p1[1] /= 2;
                  p1[3] /= 2;
                  p1[5] /= 2;
                  p2[1] = p2[1] / 2 + y2T;
                  p2[3] = p2[3] / 2 + y2T;
                  p2[5] = p2[5] / 2 + y2T;
               }
               else if ( gv->pvertical )
               {
                  p1[0] /= 2;
                  p1[2] /= 2;
                  p1[4] /= 2;
                  p2[0] = p2[0] / 2 + x2L;
                  p2[2] = p2[2] / 2 + x2L;
                  p2[4] = p2[4] / 2 + x2L;
               }
               else
               {
                  p1[0] /= 2;
                  p1[2] /= 2;
                  p1[4] /= 2;
                  p2[0] = p2[0] / 2 + x2L;
                  p2[2] = p2[2] / 2 + x2L;
                  p2[4] = p2[4] / 2 + x2L;
                  p1[1] /= 2;
                  p1[3] /= 2;
                  p1[5] /= 2;
                  p2[1] = p2[1] / 2 + y2T;
                  p2[3] = p2[3] / 2 + y2T;
                  p2[5] = p2[5] / 2 + y2T;
               }
               if ( p2[0] >= x2L && p2[2] >= x2L && \
                    p2[1] >= y2T && p2[3] >= y2T )
                  Draw_line ( p2[0], p2[1], p2[2], p2[3], explosions[i].color );
               if ( p2[2] >= x2L && p2[4] >= x2L && \
                    p2[3] >= y2T && p2[5] >= y2T )
                  Draw_line ( p2[2], p2[3], p2[4], p2[5], explosions[i].color );
               if ( p2[4] >= x2L && p2[0] >= x2L && \
                    p2[5] >= y2T && p2[1] >= y2T )
                  Draw_line ( p2[4], p2[5], p2[0], p2[1], explosions[i].color );
            }

            if ( p1[0] < x1R && p1[2] < x1R && \
                 p1[1] < y1B && p1[3] < y1B )
               Draw_line ( p1[0], p1[1], p1[2], p1[3], explosions[i].color );
            if ( p1[2] < x1R && p1[4] < x1R && \
                 p1[3] < y1B && p1[5] < y1B )
               Draw_line ( p1[2], p1[3], p1[4], p1[5], explosions[i].color );
            if ( p1[4] < x1R && p1[0] < x1R && \
                 p1[5] < y1B && p1[1] < y1B )
               Draw_line ( p1[4], p1[5], p1[0], p1[1], explosions[i].color );
         }
      }
   }
}

/*------------------------------------------------------------------
 * Jump-gate
 *
 *
------------------------------------------------------------------*/

enum jumpgate_enum
{
   MAX_JUMPGATES   = 4,
   JUMPGATE_TIME   = 10000,
   JUMPGATE_ANIM   = 250,
   JUMPGATE_FRAMES = 3
};

struct JUMPGATESTRUCT
{
   int  active;
   long time;
   long anim;
   long frame;
   int  dir;
   VEC  pos;
} jgates[MAX_JUMPGATES];

static int jgcur;   /* JUMPGATE index */
static int jcount;  /* JUMPGATE counter */

static VEC jgvert[32] =
{
   {-10.0f, 0.0f, 10.0f, 1.0f},
   {-0.0f, 10.0f, -10.0f, 1.0f},
   {10.0f, 0.0f, -10.0f, 1.0f},
   {0.0f, -10.0f, 10.0f, 1.0f},

   {-30.0f, 0.0f, 30.0f, 1.0f},
   {0.0f, 30.0f, -30.0f, 1.0f},
   {30.0f, 0.0f, -30.0f, 1.0f},
   {0.0f, -30.0f, 30.0f, 1.0f},

   {-50.0f, 0.0f, 50.0f, 1.0f},
   {0.0f, 50.0f, -50.0f, 1.0f},
   {50.0f, 0.0f, -50.0f, 1.0f},
   {0.0f, -50.0f, 50.0f, 1.0f},

   {-70.0f, 0.0f, 70.0f, 1.0f},
   {0.0f, 70.0f, -70.0f, 1.0f},
   {70.0f, 0.0f, -70.0f, 1.0f},
   {0.0f, -70.0f, 70.0f, 1.0f},

   {-10.0f, 0.0f, -10.0f, 1.0f},
   {0.0f, 10.0f, 10.0f, 1.0f},
   {10.0f, 0.0f, 10.0f, 1.0f},
   {0.0f, -10.0f, -10.0f, 1.0f},

   {-30.0f, 0.0f, -30.0f, 1.0f},
   {0.0f, 30.0f, 30.0f, 1.0f},
   {30.0f, 0.0f, 30.0f, 1.0f},
   {0.0f, -30.0f, -30.0f, 1.0f},

   {-50.0f, 0.0f, -50.0f, 1.0f},
   {0.0f, 50.0f, 50.0f, 1.0f},
   {50.0f, 0.0f, 50.0f, 1.0f},
   {0.0f, -50.0f, -50.0f, 1.0f},

   {-70.0f, 0.0f, -70.0f, 1.0f},
   {0.0f, 70.0f, 70.0f, 1.0f},
   {70.0f, 0.0f, 70.0f, 1.0f},
   {0.0f, -70.0f, -70.0f, 1.0f}
};

void Jumpgate_init ( void )
{
   int i;
   for ( i=0; i<MAX_JUMPGATES; i++ )
   {
      jgates[i].active = FALSE;
      jgates[i].time   = 0;
      jgates[i].anim   = 0;
      jgates[i].frame  = 0;
      jgates[i].dir    = 0;
      Vector_init ( jgates[i].pos );
   }
   jgcur   = 0;
   jcount  = 0;
}

void Jumpgate_open ( VEC pos, int dir )
{
   if ( jcount > MAX_JUMPGATES-1 ) return;

   jgates[jgcur].active = TRUE;
   jgates[jgcur].time   = 0;
   jgates[jgcur].anim   = 0;
   jgates[jgcur].frame  = 0;
   jgates[jgcur].dir    = dir * 16;
   Vector_copy  ( pos, jgates[jgcur].pos );

   jgcur++;
   if ( jgcur > MAX_JUMPGATES-1 )
      jgcur = 0;

   jcount++;
}

void Jumpgate_animate ( MAT r1, MAT r2 )
{
   MAT tmp_mat;
   VEC tmp[16];
   int i, j, p1[32], p2[32], f0;
   int x1R, y1B, x2L, y2T;

   x1R = gv->x1r;
   y1B = gv->y1b;
   x2L = gv->x2l;
   y2T = gv->y2t;

   for ( i=0; i<MAX_JUMPGATES; i++ )
   {
      if ( jgates[i].active )
      {
         jgates[i].time += gv->msec;
         jgates[i].anim += gv->msec;

         if ( jgates[i].anim > JUMPGATE_ANIM )
         {
            jgates[i].anim -= JUMPGATE_ANIM;
            jgates[i].frame += 1;
            if ( jgates[i].frame > JUMPGATE_FRAMES )
               jgates[i].frame = 0;
         }

         if ( jgates[i].time > JUMPGATE_TIME )
         {
            jgates[i].active = FALSE;
            jcount--;
         }

         /* draw jumpgate */
         Matrix_vec_mult ( r1, jgates[i].pos, tmp[0] );
         Matrix_copy ( r1, tmp_mat );
         Matrix_set_trans ( tmp_mat, tmp[0] );

         f0 = ( jgates[i].frame + 1 ) * 4;
         Matrix_vec_multn ( tmp_mat,
               &jgvert[jgates[i].dir], tmp, f0 );
         Camera_project_points ( tmp, p1, f0 );

         if ( gv->pduel )
         {
            Matrix_vec_mult ( r2, jgates[i].pos, tmp[0] );
            Matrix_copy ( r2, tmp_mat );
            Matrix_set_trans ( tmp_mat, tmp[0] );

            f0 = ( jgates[i].frame + 1 ) * 4;
            Matrix_vec_multn ( tmp_mat,
                  &jgvert[jgates[i].dir], tmp, f0 );
            Camera_project_points ( tmp, p2, f0 );

            for ( j=0; j<((f0*2)-4); j+=8 )
            {
               if ( gv->phorizontal )
               {
                  p1[1+j] /= 2;
                  p1[3+j] /= 2;
                  p1[5+j] /= 2;
                  p1[7+j] /= 2;
                  p2[1+j] = p2[1+j] / 2 + y2T;
                  p2[3+j] = p2[3+j] / 2 + y2T;
                  p2[5+j] = p2[5+j] / 2 + y2T;
                  p2[7+j] = p2[7+j] / 2 + y2T;
               }
               else if ( gv->pvertical )
               {
                  p1[0+j] /= 2;
                  p1[2+j] /= 2;
                  p1[4+j] /= 2;
                  p1[6+j] /= 2;
                  p2[0+j] = p2[0+j] / 2 + x2L;
                  p2[2+j] = p2[2+j] / 2 + x2L;
                  p2[4+j] = p2[4+j] / 2 + x2L;
                  p2[6+j] = p2[6+j] / 2 + x2L;
               }
               else
               {
                  p1[0+j] /= 2;
                  p1[2+j] /= 2;
                  p1[4+j] /= 2;
                  p1[6+j] /= 2;
                  p2[0+j] = p2[0+j] / 2 + x2L;
                  p2[2+j] = p2[2+j] / 2 + x2L;
                  p2[4+j] = p2[4+j] / 2 + x2L;
                  p2[6+j] = p2[6+j] / 2 + x2L;
                  p1[1+j] /= 2;
                  p1[3+j] /= 2;
                  p1[5+j] /= 2;
                  p1[7+j] /= 2;
                  p2[1+j] = p2[1+j] / 2 + y2T;
                  p2[3+j] = p2[3+j] / 2 + y2T;
                  p2[5+j] = p2[5+j] / 2 + y2T;
                  p2[7+j] = p2[7+j] / 2 + y2T;
               }
               if ( p2[0+j] >= x2L && p2[2+j] >= x2L && \
                    p2[1+j] >= y2T && p2[3+j] >= y2T )
                  Draw_line ( p2[0+j], p2[1+j], p2[2+j], p2[3+j], GREEN );
               if ( p2[2+j] >= x2L && p2[4+j] >= x2L && \
                    p2[3+j] >= y2T && p2[5+j] >= y2T )
                  Draw_line ( p2[2+j], p2[3+j], p2[4+j], p2[5+j], GREEN );
               if ( p2[4+j] >= x2L && p2[6+j] >= x2L && \
                    p2[5+j] >= y2T && p2[7+j] >= y2T )
                  Draw_line ( p2[4+j], p2[5+j], p2[6+j], p2[7+j], GREEN );
               if ( p2[6+j] >= x2L && p2[0+j] >= x2L && \
                    p2[7+j] >= y2T && p2[1+j] >= y2T )
                  Draw_line ( p2[6+j], p2[7+j], p2[0+j], p2[1+j], GREEN );
            }
         }

         for ( j=0; j<((f0*2)-4); j+=8 )
         {
            if ( p1[0+j] < x1R && p1[2+j] < x1R && \
                 p1[1+j] < y1B && p1[3+j] < y1B )
               Draw_line ( p1[0+j], p1[1+j], p1[2+j], p1[3+j], GREEN );
            if ( p1[2+j] < x1R && p1[4+j] < x1R && \
                 p1[3+j] < y1B && p1[5+j] < y1B )
               Draw_line ( p1[2+j], p1[3+j], p1[4+j], p1[5+j], GREEN );
            if ( p1[4+j] < x1R && p1[6+j] < x1R && \
                 p1[5+j] < y1B && p1[7+j] < y1B )
               Draw_line ( p1[4+j], p1[5+j], p1[6+j], p1[7+j], GREEN );
            if ( p1[6+j] < x1R && p1[0+j] < x1R && \
                 p1[7+j] < y1B && p1[1+j] < y1B )
               Draw_line ( p1[6+j], p1[7+j], p1[0+j], p1[1+j], GREEN );
         }
      }
   }
}

/*------------------------------------------------------------------
 * One-up!!!
 *
 *
------------------------------------------------------------------*/

enum oneup_enum
{
   ONEUP_LIFE       = 2000,
   ONEUP_BLEND_TIME = 200
};

struct ONEUPSTRUCT
{
   VEC  pos;
   long frame;
   long blend;
   long color;
   int  active;
}one_up;

static VEC one_up_vert[10] =
{
   {-40.0f, 20.0f, 0.0f, 1.0f} ,  /* 1 */
   {-40.0f, -20.0f, 0.0f, 1.0f },
   {-20.0f, 20.0f, 0.0f, 1.0f  }, /* U */
   {-20.0f, -20.0f, 0.0f, 1.0f },
   { 10.0f, -20.0f, 0.0f, 1.0f },
   { 10.0f, 20.0f, 0.0f, 1.0f  },
   { 20.0f, 20.0f, 0.0f, 1.0f  }, /* P */
   { 20.0f, -20.0f, 0.0f, 1.0f },
   { 60.0f, 0.0f, 0.0f, 1.0f },
   { 20.0f, 0.0f, 0.0, 1.0f }
};

static VEC oneup_thrust = { 0.0f, 0.0f, 10.0f, 1.0f };

void One_up_init ( void )
{
   one_up.active = FALSE;
}

void One_up_add ( OBJECT *obj )
{
   if ( one_up.active == FALSE )
   {
      one_up.active = TRUE;
      Vector_copy ( obj->pos, one_up.pos );
      one_up.frame = 0;
      one_up.blend = 0;
      one_up.color = WHITE;
   }
}

void One_up_draw ( MAT r1, MAT r2 )
{
   int p1[20], p2[20];
   int i, x1R, y1B, x2L, y2T;
   VEC tmp[10];
   MAT tmp_mat;

   x1R = gv->x1r;
   y1B = gv->y1b;
   x2L = gv->x2l;
   y2T = gv->y2t;

   if ( one_up.active )
   {
      one_up.pos[ZPOS] += oneup_thrust[ZPOS] * gv->fadjust;
      if ( one_up.pos[ZPOS] > -50.0f )
         one_up.active = FALSE;
      one_up.frame += gv->msec;
      if ( one_up.frame > ONEUP_LIFE )
      {
         one_up.active = FALSE;
      }
      one_up.blend += gv->msec;
      if ( one_up.blend > ONEUP_BLEND_TIME )
      {
         one_up.blend -= ONEUP_BLEND_TIME;
         one_up.color -= 3;
      }

      /* draw one_up */
      Matrix_vec_mult ( r1, one_up.pos, tmp[0] );
      Matrix_copy ( r1, tmp_mat );
      Matrix_set_trans ( tmp_mat, tmp[0] );

      Matrix_vec_multn ( tmp_mat, one_up_vert, tmp, 10 );
      Camera_project_points ( tmp, p1, 10 );

      if ( gv->pduel )
      {
         /* draw one_up */
         Matrix_vec_mult ( r2, one_up.pos, tmp[0] );
         Matrix_copy ( r2, tmp_mat );
         Matrix_set_trans ( tmp_mat, tmp[0] );

         Matrix_vec_multn ( tmp_mat, one_up_vert, tmp, 10 );
         Camera_project_points ( tmp, p2, 10 );

         if ( gv->phorizontal )
         {
            for ( i = 1; i < 20; i += 2 )
            {
               p1[i] /= 2;
               p2[i] = p2[i] / 2 + y2T;
            }
         }
         else if ( gv->pvertical )
         {
            for ( i = 0; i < 20; i += 2 )
            {
               p1[i] /= 2;
               p2[i] = p2[i] / 2 + x2L;
            }
         }
         else
         {
            for ( i = 0; i < 20; i += 2 )
            {
               p1[i] /= 2;
               p2[i] = p2[i] / 2 + x2L;
            }
            for ( i = 1; i < 20; i += 2 )
            {
               p1[i] /= 2;
               p2[i] = p2[i] / 2 + y2T;
            }
         }
         if ( p2[0] >= x2L && p2[2] >= x2L && \
              p2[1] >= y2T && p2[3] >= y2T )
            Draw_line ( p2[0], p2[1], p2[2], p2[3], one_up.color );
         if ( p2[4] >= x2L && p2[6] >= x2L && \
              p2[5] >= y2T && p2[7] >= y2T )
            Draw_line ( p2[4], p2[5], p2[6], p2[7], one_up.color );
         if ( p2[6] >= x2L && p2[8] >= x2L && \
              p2[7] >= y2T && p2[9] >= y2T )
            Draw_line ( p2[6], p2[7], p2[8], p2[9], one_up.color );
         if ( p2[8] >= x2L && p2[10] >= x2L && \
              p2[9] >= y2T && p2[11] >= y2T )
            Draw_line ( p2[8], p2[9], p2[10], p2[11], one_up.color );
         if ( p2[12] >= x2L && p2[14] >= x2L && \
              p2[13] >= y2T && p2[15] >= y2T )
            Draw_line ( p2[12], p2[13], p2[14], p2[15], one_up.color );
         if ( p2[12] >= x2L && p2[13] >= x2L && \
              p2[13] >= y2T && p2[17] >= y2T )
            Draw_line ( p2[12], p2[13], p2[16], p2[17], one_up.color );
         if ( p2[16] >= x2L && p2[18] >= x2L && \
              p2[17] >= y2T && p2[19] >= y2T )
            Draw_line ( p2[16], p2[17], p2[18], p2[19], one_up.color );
      }

      /* draw 1 */
      if ( p1[0] < x1R && p1[2] < x1R && \
           p1[1] < y1B && p1[3] < y1B )
         Draw_line ( p1[0], p1[1], p1[2], p1[3], one_up.color );

      /* draw U */
      if ( p1[4] < x1R && p1[6] < x1R && \
           p1[5] < y1B && p1[7] < y1B )
         Draw_line ( p1[4], p1[5], p1[6], p1[7], one_up.color );
      if ( p1[6] < x1R && p1[8] < x1R && \
           p1[7] < y1B && p1[9] < y1B )
         Draw_line ( p1[6], p1[7], p1[8], p1[9], one_up.color );
      if ( p1[8] < x1R && p1[10] < x1R && \
           p1[9] < y1B && p1[11] < y1B )
         Draw_line ( p1[8], p1[9], p1[10], p1[11], one_up.color );

      /* draw P */
      if ( p1[12] < x1R && p1[14] < x1R && \
           p1[13] < y1B && p1[15] < y1B )
         Draw_line ( p1[12], p1[13], p1[14], p1[15], one_up.color );
      if ( p1[12] < x1R && p1[13] < x1R && \
           p1[13] < y1B && p1[17] < y1B )
         Draw_line ( p1[12], p1[13], p1[16], p1[17], one_up.color );
      if ( p1[16] < x1R && p1[18] < x1R && \
           p1[17] < y1B && p1[19] < y1B )
         Draw_line ( p1[16], p1[17], p1[18], p1[19], one_up.color );
   }
}
