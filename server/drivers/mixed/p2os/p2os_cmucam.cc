/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2003 James McKenna james@cis.saic.com & SAIC
 *
 *  This program is free software; you can redistribute it and/or modify
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
 */

/*
 * $Id$
 *
 *   A CMUcam driver for P2OS.  The camera returns blob tracking data via
 *   the blobfinder interface.  The driver takes no commands.  The driver
 *   takes no configurations.
 */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <p2os.h>

class P2OSCMUcam: public P2OS
{
 public:
   P2OSCMUcam( ConfigFile* cf, int section);
   
   virtual size_t GetData(void*, unsigned char *, size_t maxsize,
                           uint32_t* timestamp_sec, uint32_t* timestamp_usec);
};

Driver* P2OSCMUcam_Init( ConfigFile* cf, int section)
{
  if(strcmp( PLAYER_BLOBFINDER_STRING))
  {
    PLAYER_ERROR1("driver \"p2os_cmucam\" does not support interface \"%s\"\n",
                  interface);
    return(NULL);
  }
  else
    return((Driver*)(new P2OSCMUcam( cf, section)));
}

// a driver registration function
void 
P2OSCMUcam_Register(DriverTable* table)
{
  table->AddDriver("p2os_cmucam", PLAYER_READ_MODE, P2OSCMUcam_Init);
}

// Constructor
P2OSCMUcam::P2OSCMUcam( ConfigFile* cf, int section):
           P2OS( cf, section)
{
    P2OS::cmucam = 1;	// Activate the CMUcam code in p2os.cc
}


size_t P2OSCMUcam::GetData(void* client,unsigned char *dest, size_t maxsize,
                             uint32_t* timestamp_sec, uint32_t* timestamp_usec)
{
  Lock();
  *((player_blobfinder_data_t*)dest) =
                  ((player_p2os_data_t*)device_data)->blobfinder;
  *timestamp_sec = data_timestamp_sec;
  *timestamp_usec = data_timestamp_usec;

/*
printf("Centroid: %d %d\n", 
             ntohs(((player_p2os_data_t*)device_data)->blobfinder.blobs[0].x),
             ntohs(((player_p2os_data_t*)device_data)->blobfinder.blobs[0].y));
*/
  Unlock();

  return( sizeof(player_blobfinder_data_t) );
}

