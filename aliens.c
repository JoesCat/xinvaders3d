/*------------------------------------------------------------------
  aliens.c:

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
   FORMATION_STEP           = 3,   /* dist_between_aliens / thurust */
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
static int fspeed[9]        = { 1000, 750, 500, 
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
   int      i, j;
   VEC   tmp_pos;
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

   for ( i=0; i<MAX_COLUMNS; i++ )
      fcolumn[i] = 0;
   
   for ( i=0; i<MAX_FORMATIONS; i++ )
   {
      Objlist_clear ( f_ptr );
      
      Vector_copy ( fstart_pos, tmp_pos );
      tmp_pos[YPOS] += (FORMATION_Y_INC * i);
      
      for ( j=0; j<ALIENS_PER_FORMATION; j++ )
      {
         Object_init    ( alien );
         Object_set_pos ( alien, tmp_pos );
         Object_set_dir ( alien, fstart_dir );
      
         alien->zone            = gv->formation_zone;
         alien->zheight     = i;
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
   for ( i=0; i<MAX_BOMBS; i++ )
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
   if ( (fleadcol == i) && (fcolumn[i] == 0) )
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
   int i, j, fdir_save, chance;
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
   if ( (gv->alien_count > 0) && player->active )
   {
      fmove += gv->msec;
      if ( fmove > fspeed[fscur] )
      {
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
         
         for ( i=0; i<MAX_FORMATIONS; i++ )
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
         /* pick formation infront of player */
         i = pm->zheight;
         if ( (i > -1) && (i<ZONE_HEIGHT_MAX) )
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

void Add_bomb ( OBJECT *alien )
{
   int    i;
   OBJECT *abomb;

   for ( i=0; i<MAX_BOMBS; i++ )
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

void Aliens_draw ( OBJECT *obj, MAT r )
{
   OBJECT *alien;
   VEC  tmp[16];
   MAT  tmp_mat;
   int     i, p[32], fmodel;

   for ( i=0; i<MAX_FORMATIONS; i++ )
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
         Matrix_vec_mult ( r, alien->pos, tmp[0] );
         Camera_project_points ( tmp, p, 1 );
         Matrix_copy ( r, tmp_mat );
         Matrix_set_trans ( tmp_mat, tmp[0] );

         if ( fanim )
            Matrix_vec_multn ( tmp_mat, &avert2[fmodel*16], tmp, 16 );
         else
            Matrix_vec_multn ( tmp_mat, &avert1[fmodel*16], tmp, 16 );

         Camera_project_points ( tmp, p, 16 );

         /* body */
         Draw_line ( p[0], p[1], p[2], p[3], GREEN );
         Draw_line ( p[2], p[3], p[4], p[5], GREEN );
         Draw_line ( p[4], p[5], p[6], p[7], GREEN );
         Draw_line ( p[6], p[7], p[0], p[1], GREEN );
      
        /* left leg */ 
         Draw_line ( p[8], p[9], p[10], p[11], GREEN );
         Draw_line ( p[10], p[11], p[12], p[13], GREEN );
         Draw_line ( p[12], p[13], p[8], p[9], GREEN );
         Draw_line ( p[10], p[11], p[14], p[15], GREEN );
         Draw_line ( p[14], p[15], p[12], p[13], GREEN );
       
        /* right leg */ 
         Draw_line ( p[8], p[9], p[16], p[17], GREEN );
         Draw_line ( p[16], p[17], p[18], p[19], GREEN );
         Draw_line ( p[18], p[19], p[8], p[9], GREEN );
         Draw_line ( p[16], p[17], p[20], p[21], GREEN );
         Draw_line ( p[20], p[21], p[18], p[19], GREEN );
        
        /* left eye */ 
         Draw_line ( p[22], p[23], p[24], p[25], RED );
         Draw_line ( p[24], p[25], p[26], p[27], RED );
         Draw_line ( p[26], p[27], p[22], p[23], RED );

         /* right eye */
         Draw_line ( p[22], p[23], p[28], p[29], RED );
         Draw_line ( p[28], p[29], p[30], p[31], RED );
         Draw_line ( p[30], p[31], p[22], p[23], RED );

         alien = alien->next;
      }
   }
}


void Alien_missile_draw ( OBJECT *obj, MAT r )
{
   MAT tmp_mat;
   VEC tmp[3];
   int p[6];

   Matrix_vec_mult ( r, obj->pos, tmp[0] );
   Matrix_copy ( r, tmp_mat );
   Matrix_set_trans ( tmp_mat, tmp[0] );

   Matrix_vec_multn ( tmp_mat, abomb_vert, tmp, 3 );
   Camera_project_points ( tmp, p, 3 );
   Draw_line ( p[0], p[1], p[2], p[3], YELLOW );
   Draw_line ( p[2], p[3], p[4], p[5], YELLOW );
   Draw_line ( p[4], p[5], p[0], p[1], YELLOW );
   
   Matrix_vec_multn ( tmp_mat, abomb_vert2, tmp, 3 );
   Camera_project_points ( tmp, p, 3 );
   Draw_line ( p[0], p[1], p[2], p[3], YELLOW );
   Draw_line ( p[2], p[3], p[4], p[5], YELLOW );
   Draw_line ( p[4], p[5], p[0], p[1], YELLOW );
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

void Ufo_draw ( OBJECT *obj, MAT r )
{
   MAT tmp_mat;
   VEC tmp[17];
   int p[34];
   unsigned int color;

   ufo->delay += gv->msec;
   if ( ufo->delay > UFO_LIGHTS_CYCLE )
   {
      ufo->delay -= UFO_LIGHTS_CYCLE;
      ucolor++;
      if ( ucolor == 4 )
         ucolor = 0;

   }
   color = RED - ( ucolor * 15 );

   Matrix_vec_mult ( r, obj->pos, tmp[0] );
   Matrix_copy ( r, tmp_mat );
   Matrix_set_trans ( tmp_mat, tmp[0] );

   Matrix_vec_multn ( tmp_mat, uvert1, tmp, 17 );
   Camera_project_points ( tmp, p, 17 );
   Draw_line ( p[0], p[1], p[2], p[3], GREEN );
   Draw_line ( p[2], p[3], p[4], p[5], GREEN );
   Draw_line ( p[4], p[5], p[6], p[7], GREEN );
   Draw_line ( p[6], p[7], p[0], p[1], GREEN );

   Draw_line ( p[8], p[9], p[10], p[11], GREEN );
   Draw_line ( p[8], p[9], p[12], p[13], GREEN );

   Draw_line ( p[14], p[15], p[16], p[17], GREEN );
   Draw_line ( p[14], p[15], p[18], p[19], GREEN );

   Draw_line ( p[20], p[21], p[22], p[23], GREEN );
   Draw_line ( p[22], p[23], p[24], p[25], GREEN );

   Draw_point ( p[26], p[27], color );
   Draw_point ( p[28], p[29], color );
   Draw_point ( p[30], p[31], color );
   Draw_point ( p[32], p[33], color );
}

/*================================================================*/
