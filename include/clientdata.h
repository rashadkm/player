/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2000  
 *     Brian Gerkey, Kasper Stoy, Richard Vaughan, & Andrew Howard
 *                      
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
 *   class for encapsulating all the data pertaining to a single
 *   client
 */

#ifndef _CLIENTDATA_H
#define _CLIENTDATA_H

#include <pthread.h>
#include <messages.h>

typedef enum
{
  CONTINUOUS,
  REQUESTREPLY,
  UPDATE //RTV - new fresh data only mode
} server_mode_t;

// keep a linked list of these
class CDeviceSubscription
{
  public:
    unsigned short code;
    unsigned short index;
    unsigned char access;

    // record the last time we got fresh data
    uint32_t last_sec, last_usec; 

    CDeviceSubscription* next;

    CDeviceSubscription() { next = NULL; access = 'e'; }
};
    

class CClientData 
{
  private:
    CDeviceSubscription* requested;
    int numsubs;
    pthread_mutex_t access;
    char auth_key[PLAYER_KEYLEN];
    unsigned char *readbuffer;
    unsigned char *writebuffer;

    void MotorStop();
    void PrintRequested(char*);

    bool CheckAuth(player_msghdr_t hdr, unsigned char* payload,
                   unsigned int payload_size);

    void RemoveBlanks();  
    void RemoveRequests();
    void UpdateRequested(player_device_req_t req);
    bool CheckPermissions(unsigned short code, unsigned short index);
    unsigned char FindPermission( unsigned short code, unsigned short index);
    void Unsubscribe( unsigned short code, unsigned short index );
    int Subscribe( unsigned short code, unsigned short index );

 public:
    double last_write;
    bool auth_pending;
    //int writeThreadId, readThreadId;
    pthread_t writeThread, readThread;

    server_mode_t mode;
    unsigned short frequency;  // Hz
    pthread_mutex_t datarequested, socketwrite;

    int socket;

    CClientData(char* key);
    ~CClientData();

    //void HandleRequests( unsigned char *buffer, int readcnt );
    int HandleRequests(player_msghdr_t hdr, unsigned char *payload,
                        unsigned int payload_size);
    int BuildMsg( unsigned char *data, size_t maxsize );

    int Read();
    int Write();
    int WriteIdentString();
};


#endif
