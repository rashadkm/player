/*
 *  Stage-1.4 driver for Player
 *  Copyright (C) 2003  Richard Vaughan (rtv) vaughan@hrl.com 
 *
 * This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 */

// STAGE-1.4 DRIVER CLASS ///////////////////////////////
//
// creates a single static Stage client. This is subclassed for each
// Player interface. Each instance shares the single Stage connection

#define PLAYER_ENABLE_TRACE 0
#define PLAYER_ENABLE_MSG 1

#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <netinet/in.h>  /* for struct sockaddr_in, htons(3) */


#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "stageclient.h"
#include "stg_time.h"

// init static vars
ConfigFile* Stage1p4::config = NULL;
stg_client_t* Stage1p4::stage_client = NULL;
stg_world_t* Stage1p4::world = NULL;

//char* Stage1p4::world_name;

// declare Player's emergency stop function (defined in main.cc)
void Interrupt( int dummy );
				   
// constructor
//
Stage1p4::Stage1p4( ConfigFile* cf, int section, 
		   size_t datasz, size_t cmdsz, int rqlen, int rplen) :
  Driver(cf, section, datasz,cmdsz,1,1)//rqlen,rplen)
{
  PLAYER_TRACE1( "Stage1p4 device created for interface %s\n", interface );
  
  this->config = cf;
    
  this->subscribe_list = NULL;

  const char *enttype = config->GetEntityType(section);

  if( strcmp( enttype, "simulation" ) == 0 )
    {
      printf( "Initializing Stage simulation device\n" );
    }
  else
    {
      printf( "Initializing Stage model with interface \"%s\"\n", enttype );

      char* model_name = 
	(char*)config->ReadString(section, "model", NULL );
      
      if( model_name == NULL )
	PLAYER_ERROR1( "device \"%s\" uses the Stage1p4 driver but has "
		       "no \"model\" value defined. You must specify a "
		       "model name that matches one of the models in "
		       "the worldfile.",
		       model_name );
      
      PLAYER_TRACE1( "attempting to resolve Stage model \"%s\"", model_name );
      
      this->model = 
	stg_world_model_name_lookup( Stage1p4::world, model_name );
      
      if( this->model  == NULL )
	PLAYER_ERROR1( "device %s can't find a Stage model with the same name",
		       model_name );
    }  
}

// destructor
Stage1p4::~Stage1p4()
{
}



void subscribe_to_property_cb( gpointer data, gpointer user )
{
  int prop = GPOINTER_TO_INT(data);
  stg_model_t* model = (stg_model_t*)user; 
    
    printf( "subscribing to model %d:%s\n",
	    model->id_server, 
	    stg_property_string( prop) );
  
  stg_model_subscribe( model, prop, 100 ); // 100ms  - fix this
}

void unsubscribe_to_property_cb( gpointer data, gpointer user )
{
  int prop = GPOINTER_TO_INT(data);
  stg_model_t* model = (stg_model_t*)user;
    
    printf( "unsubscribing to model %d:%s\n",
	    model->id_server, 
	    stg_property_string( prop) );
  
  stg_model_unsubscribe( model, prop );
}


int Stage1p4::Setup()
{ 
  printf( "SETUP" );
  if( this->model && this->subscribe_list )
    g_list_foreach( this->subscribe_list, 
		    subscribe_to_property_cb, 
		    (gpointer)this->model );
  return 0;
};

int Stage1p4::Shutdown()
{ 
  printf( "SHUTDOWN" );
  if( this->model && this->subscribe_list )
    g_list_foreach( this->subscribe_list, 
		    unsubscribe_to_property_cb, 
		    (gpointer)this->model );
  return 0;
};
