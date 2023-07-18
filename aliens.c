/*------------------------------------------------------------------
  aliens.c:

    XINVADERS 3D - 3d Shoot'em up
    Copyright (C) 2000 Don Llopis
    +Copyright(C) 2022 Jose Da Silva

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


#define ALIEN_RADIUS         20.0f
#define ALIEN_RADIUS_SQUARED 400.0f
#define BOMB_RADIUS          20.0f
#define BOMB_RADIUS_SQUARED  400.0f

#define UFO_RADIUS           20.0f
#define UFO_RADIUS_SQUARED   400.0f
#define UFO_MIN_X_POS        -600.0f
#define UFO_MAX_X_POS        600.0f
#define UFO_Z_POS            -950.0f

#define FORMATION_X_INC      75.0f
#define FORMATION_Y_INC      100.0f
#define FORMATION_MAX_X_POS  550.0f
#define FORMATION_MIN_X_POS  -550.0f
#define FORMATION_MAX_Y_POS  450.0f
#define FORMATION_MIN_Y_POS  50.0f
#define FORMATION_MAX_Z_POS  -850.0f
#define FORMATION_MIN_Z_POS  50.0f

enum alien_enum
{
   MAX_FORMATIONS           = 5,
   MAX_ALIENS               = 40,
   ALIENS_PER_FORMATION     = 8,
   MAX_BOMBS                = 12,

   LEFT                     = 0,
   RIGHT                    = 1,
   FORWARD                  = 2,
   DIRECTION_TOGGLE         = 1,
   ANIMATION_TOGGLE         = 1,

   MAX_COLUMNS              = 8,
   START_LEAD_COLUMN        = 7,
   FORMATION_STEP           = 3,   /* dist_between_aliens / thrust */
   FORWARD_START            = 21,  /* formation x FORMATION_STEP */
   FORWARD_MAX              = 45,  /* must calc by hand */
   FORWARD_MIN              = 0,

   DROP_BOMB_TIME           = 500, /* 0.5 sec */
   DROP_BOMB_CHANCE_1       = 50,
   DROP_BOMB_CHANCE_2       = 40,

   UFO_ZONE                 = 9,
   UFO_LIGHTS_CYCLE         = 250,
   SPAWN_UFO_TIME           = 5000,
   SPAWN_UFO_CHANCE_1       = 10,
   SPAWN_UFO_CHANCE_2       = 5
};

OBJECT aliens [MAX_ALIENS];
OBJECT bombs  [MAX_BOMBS];
OBJECT alien_ufo, *ufo;
OBJECT the_formation;

OBJLIST flist[MAX_FORMATIONS], *af_list;
OBJLIST abomblist, *abombs;

/* formation speed in msec */
static int fspeed[9]     = { 1000, 750, 500,
                             400, 400, 300,
                             200, 150, 100 };

static VEC fstart_pos    =
{
   FORMATION_MIN_X_POS,
   FORMATION_MIN_Y_POS,
   FORMATION_MAX_Z_POS,
   1.0f
};
static VEC fstart_dir    = { 0.0f, 0.0f, 1.0f, 1.0f };

static VEC fthrust[3]    = { { -25.0f, 0.0f, 0.0f, 1.0f },
                             { 25.0f, 0.0f, 0.0f, 1.0f },
                             { 0.0f, 0.0f, 100.0f, 1.0f } };

static VEC abomb_thrust  = { 0.0f, 0.0f, 25.0f, 1.0f };
static VEC ufo_thrust    = { 5.0f, 0.0f, 0.0f, 1.0f };

static VEC avert1[48] = {
   { -20.0f, 20.0f, 0.0f, 1.0f },      /* A0: body*/
   { 20.0f, 20.0f, 0.0f, 1.0f },
   { 20.0f, 0.0f, 0.0f, 1.0f },
   { -20.0f, 0.0f, 0.0f, 1.0f },
   { 0.0f, 0.0f, 0.0f, 1.0f },         /* A0: legs*/
   { -20.0f, -10.0f, 0.0f, 1.0f },
   { -15.0f, -15.0f, 0.0f, 1.0f },
   { -20.0f, -25.0f, 0.0f, 1.0f },
   { 20.0f, -10.0f, 0.0f, 1.0f },
   { 15.0f, -15.0f, 0.0f, 1.0f },
   { 20.0f, -25.0f, 0.0f, 1.0f },
   { 0.0f, 10.0f, 0.0f, 1.0f },        /* A0: eyes*/
   { -15.0, 18.0f, 0.0f, 1.0f },
   { -15.0, 2.0f, 0.0f, 1.0f },
   { 15.0, 18.0f, 0.0f, 1.0f },
   { 15.0, 2.0f, 0.0f, 1.0f },

   { -15.0f, 20.0f, 0.0f, 1.0f },      /* A1: body*/
   { 15.0f, 20.0f, 0.0f, 1.0f },
   { 5.0f, 0.0f, 0.0f, 1.0f },
   { -5.0f, 0.0f, 0.0f, 1.0f },
   { 0.0f, 0.0f, 0.0f, 1.0f },         /* A1: legs*/
   { -20.0f, -10.0f, 0.0f, 1.0f },
   { -15.0f, -15.0f, 0.0f, 1.0f },
   { -20.0f, -25.0f, 0.0f, 1.0f },
   { 20.0f, -10.0f, 0.0f, 1.0f },
   { 15.0f, -15.0f, 0.0f, 1.0f },
   { 20.0f, -25.0f, 0.0f, 1.0f },
   { 0.0f, 0.0f, 0.0f, 1.0f },        /* A1: eyes*/
   { -20.0, 15.0f, 0.0f, 1.0f },
   { -15.0, 0.0f, 0.0f, 1.0f },
   { 20.0, 15.0f, 0.0f, 1.0f },
   { 15.0, 0.0f, 0.0f, 1.0f },

   { -5.0f, 20.0f, 0.0f, 1.0f },      /* A2: body*/
   { 5.0f, 20.0f, 0.0f, 1.0f },
   { 20.0f, 0.0f, 0.0f, 1.0f },
   { -20.0f, 0.0f, 0.0f, 1.0f },
   { 0.0f, 0.0f, 0.0f, 1.0f },         /* A2: legs*/
   { -20.0f, -10.0f, 0.0f, 1.0f },
   { -15.0f, -15.0f, 0.0f, 1.0f },
   { -20.0f, -25.0f, 0.0f, 1.0f },
   { 20.0f, -10.0f, 0.0f, 1.0f },
   { 15.0f, -15.0f, 0.0f, 1.0f },
   { 20.0f, -25.0f, 0.0f, 1.0f },
   { 0.0f, 20.0f, 0.0f, 1.0f },        /* A2: eyes*/
   { -20.0, 25.0f, 0.0f, 1.0f },
   { -15.0, 15.0f, 0.0f, 1.0f },
   { 20.0, 25.0f, 0.0f, 1.0f },
   { 15.0, 15.0f, 0.0f, 1.0f }
};

static VEC avert2[48] = {
   { -20.0f, 20.0f, 0.0f, 1.0f },      /*A0: body*/
   { 20.0f, 20.0f, 0.0f, 1.0f },
   { 20.0f, 0.0f, 0.0f, 1.0f },
   { -20.0f, 0.0f, 0.0f, 1.0f },
   { 0.0f, 0.0f, 0.0f, 1.0f },         /*A0: legs*/
   { -10.0f, -5.0f, 0.0f, 1.0f },
   { -5.0f, -10.0f, 0.0f, 1.0f },
   { -10.0f, -25.0f, 0.0f, 1.0f },
   { 10.0f, -5.0f, 0.0f, 1.0f },
   { 5.0f, -10.0f, 0.0f, 1.0f },
   { 10.0f, -25.0f, 0.0f, 1.0f },
   { 0.0f, 15.0f, 0.0f, 1.0f },        /* A0: eyes*/
   { -10.0, 25.0f, 0.0f, 1.0f },
   { -15.0, 7.0f, 0.0f, 1.0f },
   { 10.0, 25.0f, 0.0f, 1.0f },
   { 15.0, 7.0f, 0.0f, 1.0f },

   { -15.0f, 20.0f, 0.0f, 1.0f },      /* A1: body*/
   { 15.0f, 20.0f, 0.0f, 1.0f },
   { 5.0f, 0.0f, 0.0f, 1.0f },
   { -5.0f, 0.0f, 0.0f, 1.0f },
   { 0.0f, 0.0f, 0.0f, 1.0f },         /*A1: legs*/
   { -10.0f, -5.0f, 0.0f, 1.0f },
   { -5.0f, -10.0f, 0.0f, 1.0f },
   { -10.0f, -30.0f, 0.0f, 1.0f },
   { 10.0f, -5.0f, 0.0f, 1.0f },
   { 5.0f, -10.0f, 0.0f, 1.0f },
   { 10.0f, -30.0f, 0.0f, 1.0f },
   { 0.0f, 10.0f, 0.0f, 1.0f },        /* A1: eyes*/
   { -20.0, 15.0f, 0.0f, 1.0f },
   { -15.0, 0.0f, 0.0f, 1.0f },
   { 20.0, 15.0f, 0.0f, 1.0f },
   { 15.0, 0.0f, 0.0f, 1.0f },

   { -5.0f, 20.0f, 0.0f, 1.0f },      /* A2: body*/
   { 5.0f, 20.0f, 0.0f, 1.0f },
   { 20.0f, 0.0f, 0.0f, 1.0f },
   { -20.0f, 0.0f, 0.0f, 1.0f },
   { 0.0f, 0.0f, 0.0f, 1.0f },         /*A2: legs*/
   { -10.0f, -5.0f, 0.0f, 1.0f },
   { -5.0f, -10.0f, 0.0f, 1.0f },
   { -10.0f, -30.0f, 0.0f, 1.0f },
   { 10.0f, -5.0f, 0.0f, 1.0f },
   { 5.0f, -10.0f, 0.0f, 1.0f },
   { 10.0f, -30.0f, 0.0f, 1.0f },
   { 0.0f, 5.0f, 0.0f, 1.0f },        /* A2: eyes*/
   { -20.0, 20.0f, 0.0f, 1.0f },
   { -15.0, 0.0f, 0.0f, 1.0f },
   { 20.0, 20.0f, 0.0f, 1.0f },
   { 15.0, 0.0f, 0.0f, 1.0f }
};

static VEC abomb_vert[3] = {
   { -10.0f, 0.0f, -10.0f, 1.0f },
   { 0.0f, 0.0f, 10.0f, 1.0f },
   { 10.0f, 0.0f, -10.0f, 1.0f }
};

static VEC abomb_vert2[3] = {
   { 0.0f, 10.0f, 0.0f, 1.0f },
   { 0.0f, 0.0f, 10.0f, 1.0f },
   { 0.0f, -10.0f, 0.0f, 1.0f }
};

static VEC uvert1[17] = {
   { -20.0f, 0.0f, 0.0f, 1.0f },    /* UFO body */
   { -10.0f, 10.0f, 0.0f, 1.0f },
   { 10.0f, 10.0f, 0.0f, 1.0f },
   { 20.0f, 0.0f, 0.0f, 1.0f },
   { -20.0f, -20.0f, 0.0f, 1.0f },    /* left leg */
   { -10.0f, 0.0f, 0.0f, 1.0f },
   { -5.0f, 0.0f, 0.0f, 1.0f },
   { 20.0f, -20.0f, 0.0f, 1.0f },    /* right leg */
   { 10.0f, 0.0f, 0.0f, 1.0f },
   { 5.0f, 0.0f, 0.0f, 1.0f },
   { -10.0f, 10.0f, 0.0f, 1.0f },    /* top */
   { 0.0f, 20.0f, 0.0f, 1.0f },
   { 10.0f, 10.0f, 1.0f, 1.0f },
   { -15.0f, 5.0f, 0.0f, 1.0f },     /* windows */
   { -5.0f, 5.0f, 0.0f, 1.0f },
   {  5.0f, 5.0f, 0.0f, 1.0f },
   { 15.0f, 5.0f, 0.0f, 1.0f }
};

static int fmove;         /* FORMATION move sideways timer */
static int fforward;      /* FORMATION move forward counter */
static int fforward_max;  /* FORMATION move forward max */
static int fleadcol;      /* FORMATION lead column */
static int fanim;         /* FORMATION animation counter */
static int fdir;          /* FORMATION movement direction  */
static int fscur;         /* FORMATION speed variable */
static int drop_bomb;     /* drop bomb timer */
static
int fcolumn[MAX_COLUMNS]; /* FORMATION column count */

static int spawn_ufo;     /* spawn ufo timer */
static int ucolor;        /* ufo lights cycle timer */

static void Add_bomb ( OBJECT * );

/*================================================================*/

void Aliens_init ( void )
{
   int     i, j;
   VEC     tmp_pos;
   OBJECT  *alien;
   OBJLIST *f_ptr;

   /* initialize formations */

   /*
    * the formation is a dummy-object used for
    * object-sorting purposes
    */
   Object_init ( &the_formation );
   Object_set_drawfn ( &the_formation, Aliens_draw );
   Vector_copy ( fstart_pos, the_formation.pos );
   the_formation.zone = ZONE_8;
   gv->formation_zone = ZONE_8;
   gv->alien_count    = MAX_ALIENS;

   fmove         = 0;
   fforward      = FORWARD_START;
   fforward_max  = FORWARD_MAX;
   fleadcol      = START_LEAD_COLUMN;
   fdir          = RIGHT;
   fanim         = 0;
   fscur         = 0;
   drop_bomb     = 0;

   /* place aliens into their appropriate formations */
   /* 8 aliens per formation -- total of 5 formations */
   af_list    = &flist[0];
   f_ptr      = &flist[0];
   alien      = &aliens[0];
   Vector_init ( tmp_pos );

   for ( i = 0; i < MAX_COLUMNS; i++ )
      fcolumn[i] = 0;

   for ( i = 0; i < MAX_FORMATIONS; i++ )
   {
      Objlist_clear ( f_ptr );

      Vector_copy ( fstart_pos, tmp_pos );
      tmp_pos[YPOS] += (FORMATION_Y_INC * i);

      for ( j = 0; j < ALIENS_PER_FORMATION; j++ )
      {
         Object_init    ( alien );
         Object_set_pos ( alien, tmp_pos );
         Object_set_dir ( alien, fstart_dir );

         alien->zone            = gv->formation_zone;
         alien->zheight         = i;
         alien->vpos            = j;
         alien->radius          = ALIEN_RADIUS;
         alien->radius_squared  = ALIEN_RADIUS_SQUARED;

         Objlist_add ( f_ptr, alien );

         tmp_pos[XPOS] += FORMATION_X_INC;
         fcolumn[j] +=1;
         alien++;
      }

      f_ptr++;
   }

   /* initialize alien bombs */
   for ( i = 0; i < MAX_BOMBS; i++ )
   {
      Object_init ( &bombs[i] );
      bombs[i].radius = BOMB_RADIUS;
      bombs[i].radius_squared = BOMB_RADIUS_SQUARED;
      Object_set_drawfn ( &bombs[i], Alien_missile_draw );
   }
   abombs = &abomblist;
   Objlist_clear ( abombs );

   /* initialize the UFO */
   spawn_ufo = 0;
   gv->ufo_zone = ZONE_9;
   ufo = &alien_ufo;
   Object_init ( ufo );
   Object_set_drawfn ( ufo, Ufo_draw );
   ufo->radius         = UFO_RADIUS;
   ufo->radius_squared = UFO_RADIUS_SQUARED;
   ufo->delay = 0;
   ucolor = 0;
}

/*================================================================*/

void Update_fcolumn ( OBJECT *obj )
{
   int i, j, tmp;

   if ( gv->alien_count == 0 ) return;

   i = obj->vpos;
   fcolumn[i] -= 1;

   /* select new lead-column if current lead-column is empty */
   if ( fleadcol == i && fcolumn[i] == 0 )
   {
      j = fleadcol;
      if ( fdir == LEFT )
      {
         while ( j < MAX_COLUMNS )
         {
            if ( fcolumn[j] > 0 )
               break;
            j++;
         }
         tmp = (j-fleadcol) * 3;
      }
      else
      {
         while ( j > -1 )
         {
            if ( fcolumn[j] > 0 )
               break;
            j--;
         }
         tmp = (fleadcol-j) * 3;
      }

      fforward -= tmp;
      fleadcol = j;
   }

   /* update alien movement speed */
   if ( ( gv->alien_count % 5 ) == 0 )
      fscur++;
}

void Aliens_update ( void )
{
   int    i, j, fdir_save, chance;
   OBJECT *alien, *abomb, *tmp;

   /* move alien bombs */
   drop_bomb += gv->msec;
   abomb = abombs->head;
   while ( abomb )
   {
      Vector_copy ( abomb->pos, abomb->old_pos );
      abomb->pos[ZPOS] += abomb_thrust[ZPOS] * gv->fadjust;

      if ( abomb->pos[ZPOS] > 0.0 )
      {
         abomb->active = FALSE;
         tmp = abomb->next;
         Objlist_del ( abombs, abomb );
         abomb = tmp;
      }
      else
      {
         Object_update_zone ( abomb );
         abomb = abomb->next;
      }
   }

   /* update alien formation */
   if ( gv->alien_count > 0 && \
        ( player1->active || ( gv->pduel && player2->active ) ) )
   {
      fmove += gv->msec;
      if ( fmove > fspeed[fscur] )
      {
         while ( fmove > fspeed[fscur] )
            fmove -= fspeed[fscur];
         fforward++;
         fanim ^= ANIMATION_TOGGLE;
         fdir_save = fdir;

         /* is it time to move the formation forward?? */
         if ( fforward == fforward_max )
         {
            fforward = 0;
            gv->formation_zone--;
            the_formation.zone--;
            Vector_addd ( the_formation.pos, fthrust[FORWARD] );

            /* choose new lead column */
            if ( fdir == LEFT )
            {
               i = MAX_COLUMNS - 1;
               while ( i > -1 )
               {
                  if ( fcolumn[i] > 0 )
                     break;
                  i--;
               }
               fforward = ( ( i - fleadcol ) * FORMATION_STEP );
               fdir_save = RIGHT;
            }
            else
            {
               i = 0;
               while ( i < MAX_COLUMNS )
               {
                  if ( fcolumn[i] > 0 )
                     break;
                  i++;
               }
               fforward = ( ( fleadcol - i ) * FORMATION_STEP );
               fdir_save = LEFT;
            }
            fleadcol = i;
            fdir = FORWARD;
         }

         for ( i = 0; i < MAX_FORMATIONS; i++ )
         {
            alien = (af_list+i)->head;
            while ( alien )
            {
               Vector_addd ( alien->pos, fthrust[fdir] );
               Object_update_zone ( alien );
               alien = alien->next;
            }
         }
         fdir = fdir_save;
      }
   }

   /* move ufo */
   if ( ufo->active )
   {
      /* adjust velocity according to frame time */
      /* set new position */
      ufo->pos[XPOS] += ufo_thrust[XPOS] * gv->fadjust;
      ufo->pos[YPOS] += ufo_thrust[YPOS] * gv->fadjust;
      ufo->pos[ZPOS] += ufo_thrust[ZPOS] * gv->fadjust;
      Object_update_zone ( ufo );
      if ( ufo->pos[XPOS] > UFO_MAX_X_POS )
      {
         Jumpgate_open ( ufo->pos, RIGHT );
         ufo->active = FALSE;
      }
   }
   else
   {
      spawn_ufo += gv->msec;
      if ( spawn_ufo > SPAWN_UFO_TIME )
      {
         spawn_ufo -= SPAWN_UFO_TIME;
         Ufo_spawn ();
      }
   }

   /* drop a bomb or two */
   if ( drop_bomb > DROP_BOMB_TIME )
   {
      drop_bomb -= DROP_BOMB_TIME;
      chance = rand() % DROP_BOMB_CHANCE_1;
      if ( chance < DROP_BOMB_CHANCE_2 )
      {
         /* pick formation in front of highest score player */
         if ( gv->pduel && gv->pscore2 > gv->pscore1 )
            i = pm2->zheight;
         else
            i = pm1->zheight;
         if ( i >= 0 && i < ZONE_HEIGHT_MAX )
         {
            alien = (af_list+i)->head;
            if ( alien )
            {
               /* pick an alien who will drop a bomb */
               j = (rand() % (af_list+i)->obj_count );
               while ( j )
               {
                  alien = alien->next;
                  j--;
               }

               /* find an inactive bomb and use it */
               Add_bomb ( alien );
            }
         }

         /* pick a random formation */
         i = rand () % MAX_FORMATIONS;
         alien = (af_list+i)->head;
         if ( alien )
         {
            /* pick an alien who will drop a bomb */
            j = (rand() % (af_list+i)->obj_count );
            while ( j )
            {
               alien = alien->next;
               j--;
            }

            /* find an inactive bomb and use it */
            Add_bomb ( alien );
         }
      }
   }
}

/*================================================================*/

static void Add_bomb ( OBJECT *alien )
{
   int    i;
   OBJECT *abomb;

   for ( i = 0; i < MAX_BOMBS; i++ )
   {
      abomb = &bombs[i];
      if ( abomb->active == FALSE )
      {
         abomb->active = TRUE;
         Vector_copy ( alien->pos, abomb->pos );
         Vector_copy ( alien->pos, abomb->old_pos );
         Objlist_add ( abombs, abomb );
         break;
      }
   }
}

/*================================================================*/

void Aliens_draw ( OBJECT *obj, MAT r1, MAT r2 )
{
   OBJECT *alien;
   VEC    tmp[16];
   MAT    tmp_mat;
   int    p1[32], p2[32], fmodel;
   int    i, j, x1R, y1B, x2L, y2T;

   x1R = gv->x1r;
   y1B = gv->y1b;
   x2L = gv->x2l;
   y2T = gv->y2t;

   for ( i = 0; i < MAX_FORMATIONS; i++ )
   {
      if ( i == MAX_FORMATIONS-1 )
         fmodel = 2;
      else if ( i > 1 )
         fmodel = 1;
      else
         fmodel = 0;

      alien = (af_list+i)->head;
      while ( alien )
      {
         Matrix_vec_mult ( r1, alien->pos, tmp[0] );
         Camera_project_points ( tmp, p1, 1 );
         Matrix_copy ( r1, tmp_mat );
         Matrix_set_trans ( tmp_mat, tmp[0] );

         if ( fanim )
            Matrix_vec_multn ( tmp_mat, &avert2[fmodel*16], tmp, 16 );
         else
            Matrix_vec_multn ( tmp_mat, &avert1[fmodel*16], tmp, 16 );

         Camera_project_points ( tmp, p1, 16 );

         if ( gv->pduel )
         {
            Matrix_vec_mult ( r2, alien->pos, tmp[0] );
            Camera_project_points ( tmp, p2, 1 );
            Matrix_copy ( r2, tmp_mat );
            Matrix_set_trans ( tmp_mat, tmp[0] );

            if ( fanim )
               Matrix_vec_multn ( tmp_mat, &avert2[fmodel*16], tmp, 16 );
            else
               Matrix_vec_multn ( tmp_mat, &avert1[fmodel*16], tmp, 16 );

            Camera_project_points ( tmp, p2, 16 );

            if ( gv->phorizontal )
            {
               for ( j = 1; j < 32; j += 2 )
               {
                  p1[j] /= 2;
                  p2[j] = p2[j] / 2 + y2T;
               }
            }
            else if ( gv->pvertical )
            {
               for ( j = 0; j < 32; j += 2 )
               {
                  p1[j] /= 2;
                  p2[j] = p2[j] / 2 + x2L;
               }
            }
            else
            {
               for ( j = 0; j < 32; j += 2 )
               {
                  p1[j] /= 2;
                  p2[j] = p2[j] / 2 + x2L;
               }
               for ( j = 1; j < 32; j += 2 )
               {
                  p1[j] /= 2;
                  p2[j] = p2[j] / 2 + y2T;
               }
            }

            /* body */
            if ( p2[0] >= x2L && p2[2] >= x2L && \
                 p2[1] >= y2T && p2[3] >= y2T )
               Draw_line ( p2[0], p2[1], p2[2], p2[3], GREEN );
            if ( p2[2] >= x2L && p2[4] >= x2L && \
                 p2[3] >= y2T && p2[5] >= y2T )
               Draw_line ( p2[2], p2[3], p2[4], p2[5], GREEN );
            if ( p2[4] >= x2L && p2[6] >= x2L && \
                 p2[5] >= y2T && p2[7] >= y2T )
               Draw_line ( p2[4], p2[5], p2[6], p2[7], GREEN );
            if ( p2[6] >= x2L && p2[0] >= x2L && \
                 p2[7] >= y2T && p2[1] >= y2T )
               Draw_line ( p2[6], p2[7], p2[0], p2[1], GREEN );

            /* left leg */
            if ( p2[8] >= x2L && p2[10] >= x2L && \
                 p2[9] >= y2T && p2[11] >= y2T )
               Draw_line ( p2[8], p2[9], p2[10], p2[11], GREEN );
            if ( p2[10] >= x2L && p2[12] >= x2L && \
                 p2[11] >= y2T && p2[13] >= y2T )
               Draw_line ( p2[10], p2[11], p2[12], p2[13], GREEN );
            if ( p2[12] >= x2L && p2[8] >= x2L && \
                 p2[13] >= y2T && p2[9] >= y2T )
               Draw_line ( p2[12], p2[13], p2[8], p2[9], GREEN );
            if ( p2[10] >= x2L && p2[14] >= x2L && \
                 p2[11] >= y2T && p2[15] >= y2T )
               Draw_line ( p2[10], p2[11], p2[14], p2[15], GREEN );
            if ( p2[14] >= x2L && p2[12] >= x2L && \
                 p2[15] >= y2T && p2[13] >= y2T )
               Draw_line ( p2[14], p2[15], p2[12], p2[13], GREEN );

            /* right leg */
            if ( p2[8] >= x2L && p2[16] >= x2L && \
                 p2[9] >= y2T && p2[17] >= y2T )
               Draw_line ( p2[8], p2[9], p2[16], p2[17], GREEN );
            if ( p2[16] >= x2L && p2[18] >= x2L && \
                 p2[17] >= y2T && p2[19] >= y2T )
               Draw_line ( p2[16], p2[17], p2[18], p2[19], GREEN );
            if ( p2[18] >= x2L && p2[8] >= x2L && \
                 p2[19] >= y2T && p2[9] >= y2T )
               Draw_line ( p2[18], p2[19], p2[8], p2[9], GREEN );
            if ( p2[16] >= x2L && p2[20] >= x2L && \
                 p2[17] >= y2T && p2[21] >= y2T )
               Draw_line ( p2[16], p2[17], p2[20], p2[21], GREEN );
            if ( p2[20] >= x2L && p2[18] >= x2L && \
                 p2[21] >= y2T && p2[19] >= y2T )
               Draw_line ( p2[20], p2[21], p2[18], p2[19], GREEN );

            /* left eye */
            if ( p2[22] >= x2L && p2[24] >= x2L && \
                 p2[23] >= y2T && p2[25] >= y2T )
               Draw_line ( p2[22], p2[23], p2[24], p2[25], RED );
            if ( p2[24] >= x2L && p2[26] >= x2L && \
                 p2[25] >= y2T && p2[27] >= y2T )
               Draw_line ( p2[24], p2[25], p2[26], p2[27], RED );
            if ( p2[26] >= x2L && p2[22] >= x2L && \
                 p2[27] >= y2T && p2[23] >= y2T )
               Draw_line ( p2[26], p2[27], p2[22], p2[23], RED );

            /* right eye */
            if ( p2[22] >= x2L && p2[28] >= x2L && \
                 p2[23] >= y2T && p2[29] >= y2T )
               Draw_line ( p2[22], p2[23], p2[28], p2[29], RED );
            if ( p2[28] >= x2L && p2[30] >= x2L && \
                 p2[29] >= y2T && p2[31] >= y2T )
               Draw_line ( p2[28], p2[29], p2[30], p2[31], RED );
            if ( p2[30] >= x2L && p2[22] >= x2L && \
                 p2[31] >= y2T && p2[23] >= y2T )
               Draw_line ( p2[30], p2[31], p2[22], p2[23], RED );
         }

         /* body */
         if ( p1[0] < x1R && p1[2] < x1R && \
              p1[1] < y1B && p1[3] < y1B )
            Draw_line ( p1[0], p1[1], p1[2], p1[3], GREEN );
         if ( p1[2] < x1R && p1[4] < x1R && \
              p1[3] < y1B && p1[5] < y1B )
            Draw_line ( p1[2], p1[3], p1[4], p1[5], GREEN );
         if ( p1[4] < x1R && p1[6] < x1R && \
              p1[5] < y1B && p1[7] < y1B )
            Draw_line ( p1[4], p1[5], p1[6], p1[7], GREEN );
         if ( p1[6] < x1R && p1[0] < x1R && \
              p1[7] < y1B && p1[1] < y1B )
            Draw_line ( p1[6], p1[7], p1[0], p1[1], GREEN );

         /* left leg */
         if ( p1[8] < x1R && p1[10] < x1R && \
              p1[9] < y1B && p1[11] < y1B )
            Draw_line ( p1[8], p1[9], p1[10], p1[11], GREEN );
         if ( p1[10] < x1R && p1[12] < x1R && \
              p1[11] < y1B && p1[13] < y1B )
            Draw_line ( p1[10], p1[11], p1[12], p1[13], GREEN );
         if ( p1[12] < x1R && p1[8] < x1R && \
              p1[13] < y1B && p1[9] < y1B )
            Draw_line ( p1[12], p1[13], p1[8], p1[9], GREEN );
         if ( p1[10] < x1R && p1[14] < x1R && \
              p1[11] < y1B && p1[15] < y1B )
            Draw_line ( p1[10], p1[11], p1[14], p1[15], GREEN );
         if ( p1[14] < x1R && p1[12] < x1R && \
              p1[15] < y1B && p1[13] < y1B )
            Draw_line ( p1[14], p1[15], p1[12], p1[13], GREEN );

         /* right leg */
         if ( p1[8] < x1R && p1[16] < x1R && \
              p1[9] < y1B && p1[17] < y1B )
            Draw_line ( p1[8], p1[9], p1[16], p1[17], GREEN );
         if ( p1[16] < x1R && p1[18] < x1R && \
              p1[17] < y1B && p1[19] < y1B )
            Draw_line ( p1[16], p1[17], p1[18], p1[19], GREEN );
         if ( p1[18] < x1R && p1[8] < x1R && \
              p1[19] < y1B && p1[9] < y1B )
            Draw_line ( p1[18], p1[19], p1[8], p1[9], GREEN );
         if ( p1[16] < x1R && p1[20] < x1R && \
              p1[17] < y1B && p1[21] < y1B )
            Draw_line ( p1[16], p1[17], p1[20], p1[21], GREEN );
         if ( p1[20] < x1R && p1[18] < x1R && \
              p1[21] < y1B && p1[19] < y1B )
            Draw_line ( p1[20], p1[21], p1[18], p1[19], GREEN );

         /* left eye */
         if ( p1[22] < x1R && p1[24] < x1R && \
              p1[23] < y1B && p1[25] < y1B )
            Draw_line ( p1[22], p1[23], p1[24], p1[25], RED );
         if ( p1[24] < x1R && p1[26] < x1R && \
              p1[25] < y1B && p1[27] < y1B )
            Draw_line ( p1[24], p1[25], p1[26], p1[27], RED );
         if ( p1[26] < x1R && p1[22] < x1R && \
              p1[27] < y1B && p1[23] < y1B )
            Draw_line ( p1[26], p1[27], p1[22], p1[23], RED );

         /* right eye */
         if ( p1[22] < x1R && p1[28] < x1R && \
              p1[23] < y1B && p1[29] < y1B )
            Draw_line ( p1[22], p1[23], p1[28], p1[29], RED );
         if ( p1[28] < x1R && p1[30] < x1R && \
              p1[29] < y1B && p1[31] < y1B )
            Draw_line ( p1[28], p1[29], p1[30], p1[31], RED );
         if ( p1[30] < x1R && p1[22] < x1R && \
              p1[31] < y1B && p1[23] < y1B )
            Draw_line ( p1[30], p1[31], p1[22], p1[23], RED );

         alien = alien->next;
      }
   }
}

void Alien_missile_draw ( OBJECT *obj, MAT r1, MAT r2 )
{
   MAT tmp_mat1, tmp_mat2;
   VEC tmp[3];
   int x1R, y1B, x2L, y2T;
   int p1[6], p2[6], p3[6], p4[6];

   x1R = gv->x1r;
   y1B = gv->y1b;

   Matrix_vec_mult ( r1, obj->pos, tmp[0] );
   Matrix_copy ( r1, tmp_mat1 );
   Matrix_set_trans ( tmp_mat1, tmp[0] );

   Matrix_vec_multn ( tmp_mat1, abomb_vert, tmp, 3 );
   Camera_project_points ( tmp, p1, 3 );
   Matrix_vec_multn ( tmp_mat1, abomb_vert2, tmp, 3 );
   Camera_project_points ( tmp, p3, 3 );

   if ( gv->pduel )
   {
      x2L = gv->x2l;
      y2T = gv->y2t;

      Matrix_vec_mult ( r2, obj->pos, tmp[0] );
      Matrix_copy ( r2, tmp_mat2 );
      Matrix_set_trans ( tmp_mat2, tmp[0] );

      Matrix_vec_multn ( tmp_mat2, abomb_vert, tmp, 3 );
      Camera_project_points ( tmp, p2, 3 );
      Matrix_vec_multn ( tmp_mat2, abomb_vert2, tmp, 3 );
      Camera_project_points ( tmp, p4, 3 );

      if ( gv->phorizontal )
      {
         p1[1] /= 2;
         p1[3] /= 2;
         p1[5] /= 2;
         p3[1] /= 2;
         p3[3] /= 2;
         p3[5] /= 2;
         p2[1] = p2[1] / 2 + y2T;
         p2[3] = p2[3] / 2 + y2T;
         p2[5] = p2[5] / 2 + y2T;
         p4[1] = p4[1] / 2 + y2T;
         p4[3] = p4[3] / 2 + y2T;
         p4[5] = p4[5] / 2 + y2T;
      }
      else if ( gv->pvertical )
      {
         p1[0] /= 2;
         p1[2] /= 2;
         p1[4] /= 2;
         p3[0] /= 2;
         p3[2] /= 2;
         p3[4] /= 2;
         p2[0] = p2[0] / 2 + x2L;
         p2[2] = p2[2] / 2 + x2L;
         p2[4] = p2[4] / 2 + x2L;
         p4[0] = p4[0] / 2 + x2L;
         p4[2] = p4[2] / 2 + x2L;
         p4[4] = p4[4] / 2 + x2L;
      }
      else
      {
         p1[0] /= 2;
         p1[2] /= 2;
         p1[4] /= 2;
         p3[0] /= 2;
         p3[2] /= 2;
         p3[4] /= 2;
         p2[0] = p2[0] / 2 + x2L;
         p2[2] = p2[2] / 2 + x2L;
         p2[4] = p2[4] / 2 + x2L;
         p4[0] = p4[0] / 2 + x2L;
         p4[2] = p4[2] / 2 + x2L;
         p4[4] = p4[4] / 2 + x2L;
         p1[1] /= 2;
         p1[3] /= 2;
         p1[5] /= 2;
         p3[1] /= 2;
         p3[3] /= 2;
         p3[5] /= 2;
         p2[1] = p2[1] / 2 + y2T;
         p2[3] = p2[3] / 2 + y2T;
         p2[5] = p2[5] / 2 + y2T;
         p4[1] = p4[1] / 2 + y2T;
         p4[3] = p4[3] / 2 + y2T;
         p4[5] = p4[5] / 2 + y2T;
      }

      if ( p2[0] >= x2L && p2[2] >= x2L && \
           p2[1] >= y2T && p2[3] >= y2T )
         Draw_line ( p2[0], p2[1], p2[2], p2[3], YELLOW );
      if ( p2[2] >= x2L && p2[4] >= x2L && \
           p2[3] >= y2T && p2[5] >= y2T )
         Draw_line ( p2[2], p2[3], p2[4], p2[5], YELLOW );
      if ( p2[4] >= x2L && p2[0] >= x2L && \
           p2[5] >= y2T && p2[1] >= y2T )
         Draw_line ( p2[4], p2[5], p2[0], p2[1], YELLOW );

      if ( p4[0] >= x2L && p4[2] >= x2L && \
           p4[1] >= y2T && p4[3] >= y2T )
         Draw_line ( p4[0], p4[1], p4[2], p4[3], YELLOW );
      if ( p2[2] >= x2L && p4[4] >= x2L && \
           p2[3] >= y2T && p4[5] >= y2T )
         Draw_line ( p4[2], p4[3], p4[4], p4[5], YELLOW );
      if ( p4[4] >= x2L && p4[0] >= x2L && \
           p4[5] >= y2T && p4[1] >= y2T )
         Draw_line ( p4[4], p4[5], p4[0], p4[1], YELLOW );
   }
   if ( p1[0] < x1R && p1[2] < x1R && \
        p1[1] < y1B && p1[3] < y1B )
      Draw_line ( p1[0], p1[1], p1[2], p1[3], YELLOW );
   if ( p1[2] < x1R && p1[4] < x1R && \
        p1[3] < y1B && p1[5] < y1B )
      Draw_line ( p1[2], p1[3], p1[4], p1[5], YELLOW );
   if ( p1[4] < x1R && p1[0] < x1R && \
        p1[5] < y1B && p1[1] < y1B )
      Draw_line ( p1[4], p1[5], p1[0], p1[1], YELLOW );

   if ( p3[0] < x1R && p3[2] < x1R && \
        p3[1] < y1B && p3[3] < y1B )
      Draw_line ( p3[0], p3[1], p3[2], p3[3], YELLOW );
   if ( p3[2] < x1R && p3[4] < x1R && \
        p3[3] < y1B && p3[5] < y1B )
      Draw_line ( p3[2], p3[3], p3[4], p3[5], YELLOW );
   if ( p3[4] < x1R && p3[0] < x1R && \
        p3[5] < y1B && p3[1] < y1B )
      Draw_line ( p3[4], p3[5], p3[0], p3[1], YELLOW );
}

/*================================================================*/

void Ufo_spawn ( void )
{
   int height;
   float y, z;

   height = rand() % MAX_FORMATIONS;
   y = ( (float)(height) * 100.0f ) + 50.0f;
   z = UFO_Z_POS;
   ufo->zone = UFO_ZONE;
   ufo->zheight = height;
   Vector_set ( ufo->pos, UFO_MIN_X_POS, y, z );
   Jumpgate_open ( ufo->pos, LEFT );
   ufo->active = TRUE;
}

void Ufo_draw ( OBJECT *obj, MAT r1, MAT r2 )
{
   MAT tmp_mat;
   VEC tmp[17];
   int p1[34], p2[34];
   int i, x1R, y1B, x2L, y2T;
   unsigned int color;

   x1R = gv->x1r;
   y1B = gv->y1b;

   ufo->delay += gv->msec;
   if ( ufo->delay > UFO_LIGHTS_CYCLE )
   {
      ufo->delay -= UFO_LIGHTS_CYCLE;
      ucolor++;
      if ( ucolor >= 4 )
         ucolor = 0;
   }
   color = RED - ( ucolor * 15 );

   Matrix_vec_mult ( r1, obj->pos, tmp[0] );
   Matrix_copy ( r1, tmp_mat );
   Matrix_set_trans ( tmp_mat, tmp[0] );

   Matrix_vec_multn ( tmp_mat, uvert1, tmp, 17 );
   Camera_project_points ( tmp, p1, 17 );
   if ( gv->pduel )
   {
      x2L = gv->x2l;
      y2T = gv->y2t;

      Matrix_vec_mult ( r2, obj->pos, tmp[0] );
      Matrix_copy ( r2, tmp_mat );
      Matrix_set_trans ( tmp_mat, tmp[0] );

      Matrix_vec_multn ( tmp_mat, uvert1, tmp, 17 );
      Camera_project_points ( tmp, p2, 17 );

      if ( gv->phorizontal )
      {
         for ( i = 1; i < 34; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + y2T;
         }
      }
      else if ( gv->pvertical )
      {
         for ( i = 0; i < 34; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + x2L;
         }
      }
      else
      {
         for ( i = 0; i < 34; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + x2L;
         }
         for ( i = 1; i < 34; i += 2 )
         {
            p1[i] /= 2;
            p2[i] = p2[i] / 2 + y2T;
         }
      }

      if ( p2[0] >= x2L && p2[2] >= x2L && \
           p2[1] >= y2T && p2[3] >= y2T )
         Draw_line ( p2[0], p2[1], p2[2], p2[3], GREEN );
      if ( p2[2] >= x2L && p2[4] >= x2L && \
           p2[3] >= y2T && p2[5] >= y2T )
         Draw_line ( p2[2], p2[3], p2[4], p2[5], GREEN );
      if ( p2[4] >= x2L && p2[6] >= x2L && \
           p2[5] >= y2T && p2[7] >= y2T )
         Draw_line ( p2[4], p2[5], p2[6], p2[7], GREEN );
      if ( p2[6] >= x2L && p2[0] >= x2L && \
           p2[7] >= y2T && p2[1] >= y2T )
         Draw_line ( p2[6], p2[7], p2[0], p2[1], GREEN );

      if ( p2[8] >= x2L && p2[10] >= x2L && \
           p2[9] >= y2T && p2[11] >= y2T )
         Draw_line ( p2[8], p2[9], p2[10], p2[11], GREEN );
      if ( p2[8] >= x2L && p2[12] >= x2L && \
           p2[9] >= y2T && p2[13] >= y2T )
         Draw_line ( p2[8], p2[9], p2[12], p2[13], GREEN );

      if ( p2[14] >= x2L && p2[16] >= x2L && \
           p2[15] >= y2T && p2[17] >= y2T )
         Draw_line ( p2[14], p2[15], p2[16], p2[17], GREEN );
      if ( p2[14] >= x2L && p2[18] >= x2L && \
           p2[15] >= y2T && p2[19] >= y2T )
         Draw_line ( p2[14], p2[15], p2[18], p2[19], GREEN );

      if ( p2[20] >= x2L && p2[22] >= x2L && \
           p2[21] >= y2T && p2[23] >= y2T )
         Draw_line ( p2[20], p2[21], p2[22], p2[23], GREEN );
      if ( p2[22] >= x2L && p2[24] >= x2L && \
           p2[23] >= y2T && p2[25] >= y2T )
         Draw_line ( p2[22], p2[23], p2[24], p2[25], GREEN );

      if ( p2[26] >= x2L && p2[27] >= y2T )
         Draw_point ( p2[26], p2[27], color );
      if ( p2[28] >= x2L && p2[29] >= y2T )
         Draw_point ( p2[28], p2[29], color );
      if ( p2[30] >= x2L && p2[31] >= y2T )
         Draw_point ( p2[30], p2[31], color );
      if ( p2[32] >= x2L && p2[33] >= y2T )
         Draw_point ( p2[32], p2[33], color );
   }
   if ( p1[0] < x1R && p1[2] < x1R && \
        p1[1] < y1B && p1[3] < y1B )
      Draw_line ( p1[0], p1[1], p1[2], p1[3], GREEN );
   if ( p1[2] < x1R && p1[4] < x1R && \
        p1[3] < y1B && p1[5] < y1B )
      Draw_line ( p1[2], p1[3], p1[4], p1[5], GREEN );
   if ( p1[4] < x1R && p1[6] < x1R && \
        p1[5] < y1B && p1[7] < y1B )
      Draw_line ( p1[4], p1[5], p1[6], p1[7], GREEN );
   if ( p1[6] < x1R && p1[0] < x1R && \
        p1[7] < y1B && p1[1] < y1B )
      Draw_line ( p1[6], p1[7], p1[0], p1[1], GREEN );

   if ( p1[8] < x1R && p1[10] < x1R && \
        p1[9] < y1B && p1[11] < y1B )
      Draw_line ( p1[8], p1[9], p1[10], p1[11], GREEN );
   if ( p1[8] < x1R && p1[12] < x1R && \
        p1[9] < y1B && p1[33] < y1B )
      Draw_line ( p1[8], p1[9], p1[12], p1[13], GREEN );

   if ( p1[14] < x1R && p1[16] < x1R && \
        p1[15] < y1B && p1[17] < y1B )
      Draw_line ( p1[14], p1[15], p1[16], p1[17], GREEN );
   if ( p1[14] < x1R && p1[18] < x1R && \
        p1[15] < y1B && p1[19] < y1B )
      Draw_line ( p1[14], p1[15], p1[18], p1[19], GREEN );

   if ( p1[20] < x1R && p1[22] < x1R && \
        p1[21] < y1B && p1[23] < y1B )
      Draw_line ( p1[20], p1[21], p1[22], p1[23], GREEN );
   if ( p1[22] < x1R && p1[24] < x1R && \
        p1[23] < y1B && p1[25] < y1B )
      Draw_line ( p1[22], p1[23], p1[24], p1[25], GREEN );

   if ( p1[26] < x1R && p1[27] < y1B )
      Draw_point ( p1[26], p1[27], color );
   if ( p1[28] < x1R && p1[29] < y1B )
      Draw_point ( p1[28], p1[29], color );
   if ( p1[30] < x1R && p1[31] < y1B )
      Draw_point ( p1[30], p1[31], color );
   if ( p1[32] < x1R && p1[33] < y1B )
      Draw_point ( p1[32], p1[33], color );
}

/*================================================================*/
