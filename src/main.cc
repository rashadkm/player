/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2000  Brian Gerkey   &  Kasper Stoy
 *                      gerkey@usc.edu    kaspers@robotics.usc.edu
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
 * the main code for the Player robot server.  here we instantiate
 * the device objects, do socket connection control and spawn
 * client reader/writer threads.
 */

//#define VERBOSE
//#define DEBUG

#include <stdio.h>
#include <errno.h>
#include <string.h> // for bzero()
#include <stdlib.h>  /* atoi(3) */
#include <signal.h>
#include <pthread.h>  /* for pthread stuff */
#include <netinet/in.h> /* for struct sockaddr_in, SOCK_STREAM */
#include <unistd.h>  /* for close(2) */
#include <sys/types.h>  /* for accept(2) */
#include <sys/socket.h>  /* for accept(2) */

#include <pubsub_util.h> /* for create_and_bind_socket() */

#ifdef PLAYER_SOLARIS
  #include <strings.h>
#endif

#ifdef INCLUDE_LASER
#include <laserdevice.h>
#endif
#ifdef INCLUDE_SONAR
#include <sonardevice.h>
#endif
#ifdef INCLUDE_VISION
#include <visiondevice.h>
#endif
#ifdef INCLUDE_POSITION
#include <positiondevice.h>
#endif
#ifdef INCLUDE_GRIPPER
#include <gripperdevice.h>
#endif
#ifdef INCLUDE_MISC
#include <miscdevice.h>
#endif
#ifdef INCLUDE_PTZ
#include <ptzdevice.h>
#endif
#ifdef INCLUDE_AUDIO
#include <audiodevice.h>
#endif
#ifdef INCLUDE_LASERBEACON
#include <laserbeacondevice.hh>
#endif
#ifdef INCLUDE_BROADCAST
#include <broadcastdevice.hh>
#endif
#ifdef INCLUDE_SPEECH
#include <speechdevice.h>
#endif

#include <clientdata.h>
#include <clientmanager.h>
#include <devicetable.h>
#include <nodevice.h>

#include <sys/mman.h> // for mmap
#include <fcntl.h>

#ifdef INCLUDE_STAGE
#include <stagedevice.h>
//#include <truthdevice.h>
#endif

player_stage_info_t *arenaIO; //address for memory mapped IO to Stage
size_t ioSize = 0; // size of the IO buffer

CDeviceTable* deviceTable = new CDeviceTable();

// storage for the TruthDevice's info - there is no
// space in shared memory for this device
// this is zeroed, then passed into the TruthDevice's constructor
// in CreateStageDevices()
//player_stage_info_t *truth_info = new player_stage_info_t();

// keep track of the pointers to our various clients.
// that way we can cancel them at Shutdown
ClientManager* clientmanager;

// for use in other places (cliendata.cc, for example)
char playerversion[] = PLAYER_VERSION;

bool experimental = false;
bool debug = false;

int global_playerport = PLAYER_PORTNUM; // used to gen. useful output & debug

char* sane_spec[] = { "-misc:0",
                      "-gripper:0",
                      "-position:0",
                      "-sonar:0",
                      "-laser:0",
                      "-vision:0",
                      "-ptz:0",
                      "-audio:0",
                      "-laserbeacon:0",
                      "-broadcast:0",
                      "-speech:0" };

/* Usage statement */
void
Usage()
{
  puts("");
  fprintf(stderr, "USAGE: player [-port <port>] [-stage <path>] [-sane] "
          "[DEVICE]...\n");
  fprintf(stderr, "  -port <port>  : TCP port where Player will listen. "
          "Default: %d\n", PLAYER_PORTNUM);
  fprintf(stderr, "  -stage <path> : use memory-mapped IO with Stage "
          "through this file.\n");
  fprintf(stderr, "  -sane         : use the compiled-in device defaults:\n");
  for(int i=0;i<ARRAYSIZE(sane_spec);i++)
    fprintf(stderr, "                      %s\n",sane_spec[i]);
  fprintf(stderr, "\n  Each [DEVICE] should be of the form:\n");
  fprintf(stderr, "    -<name>:<index> [options]\n");
  fprintf(stderr, "  If omitted, <index> is assumed to be 0.\n");
  fprintf(stderr, "  [options] is a string of device-specific options.\n");
  fprintf(stderr, "  For example:\n");
  fprintf(stderr, "    -laser:0 \"port /dev/ttyS0\"\n");
}

/* sighandler to shut everything down properly */
void Interrupt( int dummy ) 
{
  // delete the manager; it will delete the clients
  delete clientmanager;

  // next, delete the deviceTable; it will delete all the devices internally
  delete deviceTable;

  printf("** Player [port %d] quitting **\n", global_playerport );

  exit(0);
}

/* for debugging */
void PrintHeader(player_msghdr_t hdr)
{
  printf("stx: %u\n", hdr.stx);
  printf("type: %u\n", hdr.type);
  printf("device: %u\n", hdr.device);
  printf("index: %u\n", hdr.device_index);
  printf("time: %u:%u\n", hdr.time_sec,hdr.time_usec);
  printf("times: %u:%u\n", hdr.timestamp_sec,hdr.timestamp_usec);
  printf("reserved: %u\n", hdr.reserved);
  printf("size:%u\n", hdr.size);
}

bool CreateStageDevices( player_stage_info_t *arenaIO, int playerport )
{
  player_stage_info_t *end = (player_stage_info_t*)((char*)arenaIO + ioSize);
  
  // iterate through the mmapped buffer
  for( player_stage_info_t *info = arenaIO; 
       info < end; 
       info = (player_stage_info_t*)((char*)info + (size_t)(info->len) ))
    {      
#ifdef DEBUG
      printf( "Processing mmap at base: %p info: %p (len: %d total: %d)" 
	      "next: %p end: %p\r", 
	      arenaIO, info, info->len, ioSize, 
	      (char*)info + info->len , end );
      fflush( stdout );
#endif	  

      CStageDevice *dev = 0; // declare outside switch statement
      
      switch( info->player_id.type )
	{
	  //create a generic stage IO device for these types:
	case PLAYER_PLAYER_CODE: 
	case PLAYER_MISC_CODE:
	case PLAYER_POSITION_CODE:
	case PLAYER_SONAR_CODE:
	case PLAYER_LASER_CODE:
	case PLAYER_VISION_CODE:  
	case PLAYER_PTZ_CODE:     
	case PLAYER_LASERBEACON_CODE: 
	case PLAYER_BROADCAST_CODE:   
	case PLAYER_TRUTH_CODE:
	case PLAYER_OCCUPANCY_CODE:
	case PLAYER_GPS_CODE:
	case PLAYER_GRIPPER_CODE:
	   
	  // Create a StageDevice with this IO base address
	  dev = new CStageDevice( info );
	  
	  // only devices on the my port or the global port are
	  // available to clients
	  if( info->player_id.port == playerport || 
	      info->player_id.port == GLOBALPORT )
	    deviceTable->AddDevice( info->player_id.type, 
				    info->player_id.index, 
				    PLAYER_ALL_MODE, dev );
	  

#ifdef DEBUG
      printf( "Player [port %d] created StageDevice (%d,%d,%d)\n", 
	      playerport,
	      info->player_id.port, 
	      info->player_id.type, 
	      info->player_id.index ); 
      fflush( stdout );
#endif	  

	  break;
	      
	  // devices not implemented
	case PLAYER_AUDIO_CODE:   
#ifdef VERBOSE
	  printf( "Device type %d not yet implemented in Stage\n", 
		  info->player_id.type);
	  fflush( stdout );
#endif
	  break;
	      
	case 0:
#ifdef VERBOSE
	  printf( "Player ignoring Stage device type %d\n", 
		  info->player_id.type);
	  fflush( stdout );
#endif
	  break;	  

	  // unknown device 
	  default: printf( "Unknown device type %d for object ID (%d,%d,%d)\n", 			   info->player_id.type, 
			   info->player_id.port, 
			   info->player_id.type, 
			   info->player_id.index ); 
	  break;
	}
    }

#ifdef DEBUG  
  printf( "finished creating stage devices\n" );
  fflush( stdout );
#endif
  
  return true;
}

/*
 * parses strings that look like "-laser:2"
 *   <str1> is the device string; <str2> is the argument string for the device
 *
 * if the string can be parsed, then the appropriate device will be created, 
 *    and 0 will be returned.
 * otherwise, -1 is retured
 */
int
parse_device_string(char* str1, char* str2)
{
  char* colon;
  uint16_t index = 0;
  CDevice* tmpdevice;

  if(!str1 || (str1[0] != '-') || (strlen(str1) < 2))
    return(-1);

  /* get the index */
  if((colon = strchr(str1,':')))
  {
    if(strlen(colon) < 2)
      return(-1);
    index = atoi(colon+1);
  }

  /* parse the config string into argc/argv format */
  int argc = 0;
  char* argv[32];
  char* tmpptr;
  if(str2 && strlen(str2))
  {
    argv[argc++] = strtok(str2, " ");
    while(argc < (int)sizeof(argv))
    {
      tmpptr = strtok(NULL, " ");
      if(tmpptr)
        argv[argc++] = tmpptr;
      else
        break;
    }
  }

  /* get the device name */
  if(!strncmp(PLAYER_MISC_STRING,str1+1,strlen(PLAYER_MISC_STRING)))
  {
#ifdef INCLUDE_MISC
    tmpdevice = new CMiscDevice(argc,argv);
    deviceTable->AddDevice(PLAYER_MISC_CODE, index, 
                           PLAYER_READ_MODE, tmpdevice);
#else
    fputs("\nWarning: Support for misc device was not included at compile time.\n",stderr);
    return(0);
#endif
  }
  else if(!strncmp(PLAYER_GRIPPER_STRING,str1+1,strlen(PLAYER_GRIPPER_STRING)))
  {
#ifdef INCLUDE_GRIPPER
    tmpdevice = new CGripperDevice(argc,argv);
    deviceTable->AddDevice(PLAYER_GRIPPER_CODE, index, 
                           PLAYER_ALL_MODE, tmpdevice);
#else
    fputs("\nWarning: Support for gripper device was not included at compile time.\n",
          stderr);
    return(0);
#endif
  }
  else if(!strncmp(PLAYER_POSITION_STRING,str1+1,
                   strlen(PLAYER_POSITION_STRING)))
  {
#ifdef INCLUDE_POSITION
    tmpdevice = new CPositionDevice(argc,argv);
    deviceTable->AddDevice(PLAYER_POSITION_CODE, index, 
                           PLAYER_ALL_MODE, tmpdevice);
#else
    fputs("\nWarning: Support for position device was not included at compile time.\n",
          stderr);
    return(0);
#endif
  }
  else if(!strncmp(PLAYER_SONAR_STRING,str1+1,strlen(PLAYER_SONAR_STRING)))
  {
#ifdef INCLUDE_SONAR
    tmpdevice = new CSonarDevice(argc,argv);
    deviceTable->AddDevice(PLAYER_SONAR_CODE, index, 
                           PLAYER_READ_MODE, tmpdevice);
#else
    fputs("\nWarning: Support for sonar device was not included at compile time.\n",
          stderr);
    return(0);
#endif
  }
  else if(!strncmp(PLAYER_LASER_STRING,str1+1,strlen(PLAYER_LASER_STRING)))
  {
#ifdef INCLUDE_LASER
    tmpdevice = new CLaserDevice(argc,argv);
    deviceTable->AddDevice(PLAYER_LASER_CODE, index, 
                           PLAYER_READ_MODE, tmpdevice);
#else
    fputs("\nWarning: Support for laser device was not included at compile time.\n",
          stderr);
    return(0);
#endif
  }
  else if(!strncmp(PLAYER_VISION_STRING,str1+1,strlen(PLAYER_VISION_STRING)))
  {
#ifdef INCLUDE_VISION
    tmpdevice = new CVisionDevice(argc,argv);
    deviceTable->AddDevice(PLAYER_VISION_CODE, index, 
                           PLAYER_READ_MODE, tmpdevice);
#else
    fputs("\nWarning: Support for vision device was not included at compile time.\n",
          stderr);
    return(0);
#endif
  }
  else if(!strncmp(PLAYER_PTZ_STRING,str1+1,strlen(PLAYER_PTZ_STRING)))
  {
#ifdef INCLUDE_PTZ
    tmpdevice = new CPtzDevice(argc,argv);
    deviceTable->AddDevice(PLAYER_PTZ_CODE, index, 
                           PLAYER_ALL_MODE, tmpdevice);
#else
    fputs("\nWarning: Support for ptz device was not included at compile time.\n",
          stderr);
    return(0);
#endif
  }
  else if(!strncmp(PLAYER_AUDIO_STRING,str1+1,strlen(PLAYER_AUDIO_STRING)))
  {
#ifdef INCLUDE_AUDIO
    tmpdevice = new CAudioDevice(argc,argv);
    deviceTable->AddDevice(PLAYER_AUDIO_CODE, index, 
                           PLAYER_ALL_MODE, tmpdevice);
#else
    fputs("\nWarning: Support for audio device was not included at compile time.\n",
          stderr);
    return(0);
#endif
  }
  else if(!strncmp(PLAYER_LASERBEACON_STRING,str1+1,
                   strlen(PLAYER_LASERBEACON_STRING)))
  {
#ifdef INCLUDE_LASERBEACON
    tmpdevice = new CLaserBeaconDevice(argc,argv); 
    deviceTable->AddDevice(PLAYER_LASERBEACON_CODE, index, 
                           PLAYER_READ_MODE, tmpdevice);
#else
    fputs("\nWarning: Support for laserbeacon device was not included at compile time.\n",
          stderr);
    return(0);
#endif
  }
  else if(!strncmp(PLAYER_BROADCAST_STRING,str1+1,
                   strlen(PLAYER_BROADCAST_STRING)))
  {
#ifdef INCLUDE_BROADCAST
    tmpdevice = new CBroadcastDevice(argc,argv);
    deviceTable->AddDevice(PLAYER_BROADCAST_CODE, index, 
                           PLAYER_ALL_MODE, tmpdevice);
#else
    fputs("\nWarning: Support for broadcast device was not included at compile time.\n",
          stderr);
    return(0);
#endif
  }
  else if(!strncmp(PLAYER_SPEECH_STRING,str1+1,strlen(PLAYER_SPEECH_STRING)))
  {
#ifdef INCLUDE_SPEECH
    tmpdevice = new CSpeechDevice(argc,argv);
    deviceTable->AddDevice(PLAYER_SPEECH_CODE, index, 
                           PLAYER_WRITE_MODE, tmpdevice);
#else
    fputs("\nWarning: Support for speech device was not included at compile time.\n",
          stderr);
    return(0);
#endif
  }
  else if(!strncmp(PLAYER_GPS_STRING,str1+1,strlen(PLAYER_GPS_STRING)))
  {
    fputs("GPS device is not yet implemented in Player.\n",stderr);
    return(-1);
  }
  else
  {
    printf("Error: unknown device \"%s\"\n", str1);
    return(-1);
  }
  return(0);
}

int main( int argc, char *argv[] )
{
  bool special_config = false;
  bool already_sane = false;
  bool use_stage = false;
  struct sockaddr_in listener;
  char auth_key[PLAYER_KEYLEN] = "";
  //struct sockaddr_in sender;
#ifdef PLAYER_LINUX
  socklen_t sender_len;
#else
  int sender_len;
#endif
  CClientData *clientData;
  int player_sock = 0;

  char arenaFile[MAX_FILENAME_SIZE]; // filename for mapped memory

  // make a copy of argv, so that strtok in parse_device_string
  // doesn't screw with it 
  char *new_argv[argc];
  for(int i=0;i<argc;i++)
    new_argv[i] = strdup(argv[i]);

  printf("** Player v%s ** ", PLAYER_VERSION);
  fflush(stdout);

  // parse args
  for( int i = 1; i < argc; i++ ) 
  {
    if(!strcmp(new_argv[i],"-stage"))
    {
      if(++i<argc) 
      {
	strncpy(arenaFile, new_argv[i], sizeof(arenaFile));
	use_stage = true;
	printf("[Stage]");
      }
      else 
      {
	Usage();
	exit(-1);
      }
    }
    else if(!strcmp(new_argv[i], "-key"))
    {
      if(++i<argc) 
      { 
        strncpy(auth_key,new_argv[i],sizeof(auth_key));
        // just in case...
        auth_key[sizeof(auth_key)-1] = '\0';
      }
      else 
      {
	Usage();
	exit(-1);
      }
    }
    else if(!strcmp(new_argv[i], "-port"))
    {
      if(++i<argc) 
      { 
	global_playerport = atoi(new_argv[i]);
	
	printf("[Port %d]", global_playerport);
      }
      else 
      {
	Usage();
	exit(-1);
      }
    }
    else if(!strcmp(new_argv[i], "-sane"))
    {
      for(int i=0;i<ARRAYSIZE(sane_spec);i++)
      {
        if(parse_device_string(sane_spec[i],NULL) < 0)
        {
          fprintf(stderr, "Warning: got error while creating sane "
                  "device \"%s\"\n", sane_spec[i]);
        }
      }
      already_sane = true;
    }
    else if((i+1)<argc && new_argv[i+1][0] != '-')
    {
      if(parse_device_string(new_argv[i],new_argv[i+1]) < 0)
      {
        Usage();
        exit(-1);
      }
      i++;
      special_config = true;
    }
    else
    {
      if(parse_device_string(new_argv[i],NULL) < 0)
      {
        Usage();
        exit(-1);
      }
      special_config = true;
    }
  }

  // default behavior is to use the sane spec
  if(!already_sane && !special_config && !use_stage)
  {
    for(int i=0;i<ARRAYSIZE(sane_spec);i++)
    {
      if(parse_device_string(sane_spec[i],NULL) < 0)
      {
        fprintf(stderr, "Warning: got error while creating sane "
                "device \"%s\"\n", sane_spec[i]);
      }
    }
  }

  // This argv shit apparently segfaults on Linux 2.2. Fuck it.
  //
  /*
  // get rid of our argv copy
  for(int i = 0;i<argc;i++)
    free(new_argv[i]);

  // pretty up our entry in the process table (and also hide the auth_key)
  char buffer[32];
  if(!use_stage)
    sprintf(buffer,"Player [Port %d]",global_playerport);
  else
    sprintf(buffer,"Player [Port %d] [Stage]",global_playerport);
  int len = 0;
  for(int i=0;i<argc;i++)
    len += strlen(argv[i])+1;
  len--;
  bzero(argv[0],len);
  if(len<(int)sizeof(buffer))
    argv[0] = (char*)realloc(argv[0],sizeof(buffer));
  strcpy(argv[0],buffer);
  */

  puts( "" ); // newline, flush
  
#ifdef INCLUDE_STAGE
  if( use_stage )
  {
      // create the shared memory connection to Stage
      
#ifdef VERBOSE
      printf( "Mapping shared memory through %s\n", arenaFile );
#endif
      
      int tfd = 0;
    if( (tfd = open( arenaFile, O_RDWR )) < 0 )
      {
	perror( "Failed to open file" );
	exit( -1 );
      }
    
    
    // find out how big the file is - we need to mmap that many bytes
    ioSize = lseek( tfd, 0, SEEK_END );
    
#ifdef VERBOSE
    printf( "Mapping %d bytes.\n", ioSize );
    fflush( stdout );
#endif
    
    if( (arenaIO = 
	 (player_stage_info_t*)mmap( NULL, ioSize, PROT_READ | PROT_WRITE, 
				     MAP_SHARED, tfd, (off_t)0 ))  
	== MAP_FAILED )
      {
	perror( "Failed to map memory" );
	exit( -1 );
      }
      
    close( tfd ); // can close fd once mapped

    // make all the stage devices, scan
    CreateStageDevices( arenaIO, global_playerport );
  }  

#endif // INCLUDE_STAGE  

  /* set up to handle SIGPIPE (happens when the client dies) */
  if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
  {
    perror("signal(2) failed while setting up for SIGPIPE");
    exit(1);
  }

  if(signal(SIGINT, &Interrupt ) == SIG_ERR)
  {
    perror("signal(2) failed while setting up for SIGINT");
    exit(1);
  }

  if(signal(SIGHUP, &Interrupt ) == SIG_ERR)
  {
    perror("signal(2) failed while setting up for SIGHUP");
    exit(1);
  }

  // setup the socket to listen on
  if((player_sock = 
      create_and_bind_socket(&listener,1,global_playerport,SOCK_STREAM,200)) 
     == -1)
  {
    fputs("create_and_bind_socket() failed; quitting", stderr);
    exit(-1);
  }

  // create the client manager object.
  // it will start one reader thread and one writer thread and keep track
  // of all clients
  clientmanager = new ClientManager;

  // main loop: accept new connections and hand them off to the client
  // manager.
  for(;;) 
  {
    clientData = new CClientData(auth_key);

    /* block here */
    if((clientData->socket = accept(player_sock,(struct sockaddr *)NULL, 
                                    &sender_len)) == -1)
    {
      perror("accept(2) failed: ");
      exit(-1);
    }

    // make the socket non-blocking
    if(fcntl(clientData->socket, F_SETFL, O_NONBLOCK) == -1)
    {
      perror("fcntl() failed while making socket non-blocking. quitting.");
      exit(-1);
    }

    /* got conn */
    printf("** Player [port %d] client accepted on socket %d **\n", 
           global_playerport, clientData->socket);

    /* add it to the manager's list */
    clientmanager->AddClient(clientData);

  }

  /* don't get here */
  return(0);
}


