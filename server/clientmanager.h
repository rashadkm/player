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
 *   class for encapsulating all the data pertaining to the clients
 */

#ifndef _CLIENTMANAGER_H
#define _CLIENTMANAGER_H

#include "clientdata.h"   
#include "replace.h"  /* for poll(2) */


class ClientManager
{
  protected:
    // dynamically managed array of structs that we'll give to poll(2)
    // when trying to read from clients
    struct pollfd* ufds;
    
    // dynamically managed array of our clients
    int num_clients;
    int size_clients;
    ClientData** clients;

    // counter used to generate quasi-unique client IDs.  obviously, it can
    // roll over, but only after 64K clients have connected.
    uint16_t curr_client_id;

    // dynamically managed array of structs that we'll give to poll(2)
    // when trying to accept new connections
    struct pollfd* accept_ufds;
    int* accept_ports;
    int num_accept_ufds;

    // authorization key to be used for clients
    char client_auth_key[PLAYER_KEYLEN];

  public:
    // constructor
    ClientManager(struct pollfd* listen_ufds, int* ports,
                  int numfds, char* auth_key);

    // destructor
    virtual ~ClientManager();
    
    // add a client to our watch list
    void AddClient(ClientData* client);

    // Update the ClientManager
    int Update();

    // mark a client for deletion
    void MarkClientForDeletion(int idx);
    // remove a client
    void RemoveBlanks();

    // These 3 methods are the primary interface to the ClientManager.
    // They must be implemented by any subclass.
    virtual int Accept() = 0;
    virtual int Read() = 0;
    virtual int Write() = 0;

    // get the index corresponding to a ClientData pointer
    int GetIndex(ClientData* ptr);
};

class ClientManagerTCP : public ClientManager
{
  public:
    ClientManagerTCP(struct pollfd* listen_ufds, int* ports,
                     int numfds, char* auth_key) :
            ClientManager(listen_ufds,ports,numfds,auth_key) {}
    virtual int Accept();
    virtual int Read();
    virtual int Write();
};

class ClientManagerUDP : public ClientManager
{
  public:
    ClientManagerUDP(struct pollfd* listen_ufds, int* ports,
                     int numfds, char* auth_key) :
            ClientManager(listen_ufds,ports,numfds,auth_key) {}

    virtual int Accept();
    virtual int Read();
    virtual int Write();

};

#endif
