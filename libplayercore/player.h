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
/********************************************************************
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ********************************************************************/

/*
 * Desc: Player communication packet structures and codes
 * CVS:  $Id$
 */


#ifndef PLAYER_H
#define PLAYER_H

/* Include values from the configure script */
#include "playerconfig.h"

/** @ingroup libplayercore
 * @defgroup message_basics Messaging basics
 * Interface-independent message types, sizes, units, address structures, etc.
 */

/** @ingroup message_basics
 * @defgroup message_constants Miscellaneous constants
 * Maximum message lengths, etc.
 * @{ */
/** The largest possible message */
#define PLAYER_MAX_MESSAGE_SIZE 8388608 /*8MB*/
/** Maximum payload in a message */
#define PLAYER_MAX_PAYLOAD_SIZE (PLAYER_MAX_MESSAGE_SIZE - sizeof(player_msghdr_t))
/** Maximum length for a driver name */
#define PLAYER_MAX_DRIVER_STRING_LEN 64
/** The maximum number of devices the server will support. */
#define PLAYER_MAX_DEVICES             256
/** Default maximum length for a message queue */
#define PLAYER_MSGQUEUE_DEFAULT_MAXLEN 1024
/** String that is spit back as a banner on connection */
#define PLAYER_IDENT_STRING    "Player v."
/** Length of string that is spit back as a banner on connection */
#define PLAYER_IDENT_STRLEN 32
/** Length of authentication key */
#define PLAYER_KEYLEN       32
/** @} */

/** @ingroup message_basics
 * @defgroup message_types Message types
 * The Player message types
 */
/** @ingroup message_types
 * @{ */

/** A data message.  Such messages are asynchronously published from
devices, and are usually used to reflect some part of the device's state.
*/
#define PLAYER_MSGTYPE_DATA      1

/** A command message.  Such messages are asynchronously published to
devices, and are usually used to change some aspect of the device's state. */
#define PLAYER_MSGTYPE_CMD       2

/** A request message.  Such messages are published synchronously to
devices, usually to get or set some aspect of the device's state that is
not available in data or command messages.  Every request message gets
a response message (either PLAYER_MSGTYPE_RESP_ACK or
PLAYER_MSGTYPE_RESP_NACK). */
#define PLAYER_MSGTYPE_REQ       3

/** A positive response message.  Such messages are published in response
to a PLAYER_MSGTYPE_REQ.  This message indicates that the underlying driver
received, interpreted, and processed the request.  Any requested data is in
the body of this response message. */
#define PLAYER_MSGTYPE_RESP_ACK  4

/** A synch message.  Only used in @ref PLAYER_DATAMODE_PULL mode.
Sent at the end of the set of messages that are sent in response to a
@ref PLAYER_PLAYER_REQ_DATA request. */
#define PLAYER_MSGTYPE_SYNCH     5

/** A negative response message.  Such messages are published in response
to a PLAYER_MSGTYPE_REQ.  This messages indicates that the underlying
driver did not process the message.  Possible causes include: the driver's
message queue was full, the driver failed to interpret the request, or the
the driver does not support the request.   This message will have no data
in the body.*/
#define PLAYER_MSGTYPE_RESP_NACK 6

/** @} */


/** @ingroup message_basics
 * @defgroup message_codes Interface codes
 * An integer code is assigned to each interface.  See @ref interfaces for
 * detailed descriptions of each interface.
 */

/** @ingroup message_codes
 * @{ */

//#define PLAYER_NULL_CODE           256 // /dev/null analogue
//#define PLAYER_PLAYER_CODE         1   // the server itself
//#define PLAYER_POWER_CODE          2   // power subsystem
//#define PLAYER_GRIPPER_CODE        3   // gripper
//#define PLAYER_POSITION2D_CODE     4   // device that moves about in the plane
//#define PLAYER_SONAR_CODE          5   // fixed range-finder
//#define PLAYER_LASER_CODE          6   // scanning range-finder
//#define PLAYER_BLOBFINDER_CODE     7   // visual blobfinder
//#define PLAYER_PTZ_CODE            8   // pan-tilt-zoom unit
//#define PLAYER_AUDIO_CODE          9   // audio I/O
//#define PLAYER_FIDUCIAL_CODE       10  // fiducial detector
//#define PLAYER_SPEECH_CODE         12  // speech I/O
//#define PLAYER_GPS_CODE            13  // GPS unit
//#define PLAYER_BUMPER_CODE         14  // bumper array
//#define PLAYER_DIO_CODE            20  // digital I/O
//#define PLAYER_AIO_CODE            21  // analog I/O
//#define PLAYER_IR_CODE             22  // IR array
//#define PLAYER_WIFI_CODE           23  // wifi card status
//#define PLAYER_LOCALIZE_CODE       25  // localization
//#define PLAYER_MCOM_CODE           26  // multicoms
//#define PLAYER_POSITION3D_CODE     30  // 3-D position
//#define PLAYER_SIMULATION_CODE     31  // simulators
//#define PLAYER_BLINKENLIGHT_CODE   33  // blinking lights
//#define PLAYER_NOMAD_CODE          34  // Nomad robot
//#define PLAYER_CAMERA_CODE         40  // camera device
//#define PLAYER_MAP_CODE            42  // get a map
//#define PLAYER_PLANNER_CODE        44  // 2D motion planner
//#define PLAYER_LOG_CODE            45  // log read/write control
//#define PLAYER_MOTOR_CODE          47  // motor interface
//#define PLAYER_JOYSTICK_CODE       49  // Joytstick
//#define PLAYER_SPEECH_RECOGNITION_CODE  50  // speech recognition
//#define PLAYER_OPAQUE_CODE         51  // plugin interface
//#define PLAYER_POSITION1D_CODE     52  // 1-D position
//#define PLAYER_ACTARRAY_CODE       53  // Actuator Array interface
//#define PLAYER_LIMB_CODE           54  // Limb interface
//#define PLAYER_GRAPHICS2D_CODE     55  // Graphics2D interface
//#define PLAYER_RFID_CODE           56  // RFID reader interface
//#define PLAYER_WSN_CODE            57  // Wireless Sensor Networks interface
//#define PLAYER_GRAPHICS3D_CODE     58  // Graphics3D interface
//#define PLAYER_HEALTH_CODE	   59  // Statgrab Health interface
//#define PLAYER_IMU_CODE            60  // Inertial Measurement Unit interface
//#define PLAYER_POINTCLOUD3D_CODE   61  // 3-D point cloud
//#define PLAYER_RANGER_CODE         62  // Range sensor device
/** @} */

/** @ingroup message_basics
 * @defgroup message_strings Interface string names
 * Used in configuration file parsing and console output, each interface is
 * assigned a string name. See @ref interfaces for
 * detailed descriptions of each interface.
 */

/** @ingroup message_strings
 * @{ */

//#define PLAYER_ACTARRAY_STRING        "actarray"
//#define PLAYER_AIO_STRING             "aio"
//#define PLAYER_AUDIO_STRING           "audio"
//#define PLAYER_BLINKENLIGHT_STRING    "blinkenlight"
//#define PLAYER_BLOBFINDER_STRING      "blobfinder"
//#define PLAYER_BUMPER_STRING          "bumper"
//#define PLAYER_CAMERA_STRING          "camera"
//#define PLAYER_DIO_STRING             "dio"
//#define PLAYER_GRIPPER_STRING         "gripper"
//#define PLAYER_FIDUCIAL_STRING        "fiducial"
//#define PLAYER_GPS_STRING             "gps"
//#define PLAYER_GRAPHICS2D_STRING      "graphics2d"
//#define PLAYER_GRAPHICS3D_STRING      "graphics3d"
//#define PLAYER_IMU_STRING             "imu"
//#define PLAYER_POINTCLOUD3D_STRING    "pointcloud3d"
//#define PLAYER_HEALTH_STRING	      "health"
//#define PLAYER_IR_STRING              "ir"
//#define PLAYER_JOYSTICK_STRING        "joystick"
//#define PLAYER_LASER_STRING           "laser"
//#define PLAYER_LIMB_STRING            "limb"
//#define PLAYER_LOCALIZE_STRING        "localize"
//#define PLAYER_LOG_STRING             "log"
//#define PLAYER_MAP_STRING             "map"
//#define PLAYER_MCOM_STRING            "mcom"
//#define PLAYER_MOTOR_STRING           "motor"
//#define PLAYER_NOMAD_STRING           "nomad"
//#define PLAYER_NULL_STRING            "null"
//#define PLAYER_OPAQUE_STRING          "opaque"
//#define PLAYER_PLANNER_STRING         "planner"
//#define PLAYER_PLAYER_STRING          "player"
//#define PLAYER_POSITION1D_STRING      "position1d"
//#define PLAYER_POSITION2D_STRING      "position2d"
//#define PLAYER_POSITION3D_STRING      "position3d"
//#define PLAYER_POWER_STRING           "power"
//#define PLAYER_PTZ_STRING             "ptz"
//#define PLAYER_RFID_STRING            "rfid"
//#define PLAYER_SIMULATION_STRING      "simulation"
//#define PLAYER_SONAR_STRING           "sonar"
//#define PLAYER_SPEECH_STRING          "speech"
//#define PLAYER_SPEECH_RECOGNITION_STRING  "speech_recognition"
//#define PLAYER_WIFI_STRING            "wifi"
//#define PLAYER_WSN_STRING             "wsn"
//#define PLAYER_RANGER_STRING          "ranger"

/** @} */

/** @ingroup message_basics
 * @defgroup address_structs Address structures
 * %Device and message address structures.
 * @{ */

/** @brief A device address.

 Devices are identified by 12-byte addresses of this form. Some of the
 fields are transport-dependent in their interpretation. */
typedef struct player_devaddr
{
  /** The "host" on which the device resides.  Transport-dependent. */
  uint32_t host;
  /** The "robot" or device collection in which the device resides.
      Transport-dependent */
  uint32_t robot;
  /** The interface provided by the device; must be one of PLAYER_*_CODE */
  uint16_t interf;
  /** Which device of that interface */
  uint16_t index;
} player_devaddr_t;

/** @brief Generic message header.

 Every message starts with this header.*/
typedef struct player_msghdr
{
  /** Device to which this message pertains */
  player_devaddr_t addr;
  /** Message type; must be one of PLAYER_MSGTYPE_* */
  uint8_t type;
  /** Message subtype; interface specific */
  uint8_t subtype;
  /** Time associated with message contents (seconds since epoch) */
  double timestamp;
  /** For keeping track of associated messages.  Transport-specific. */
  uint32_t seq;
  /** Size in bytes of the payload to follow */
  uint32_t size;
} player_msghdr_t;
/** @} */

/** @ingroup message_basics
 * @defgroup utility_structs General-purpose message structures.
 * These structures often appear inside other structures.
 * @{ */

/** @breif A null structure for parsing completeness */
typedef struct player_null
{
} player_null_t;

/** @brief A point in the plane */
typedef struct player_point_2d
{
  /** X [m] */
  double px;
  /** Y [m] */
  double py;
} player_point_2d_t;


/** @brief A point in 3D space */
typedef struct player_point_3d
{
  /** X [m] */
  double px;
  /** Y [m] */
  double py;
  /** Z [m] */
  double pz;
} player_point_3d_t;


/** @brief An angle in 3D space */
typedef struct player_orientation_3d
{
  /** roll [rad] */
  double proll;
  /** pitch [rad] */
  double ppitch;
  /** yaw [rad] */
  double pyaw;
} player_orientation_3d_t;

/** @brief A pose in the plane */
typedef struct player_pose2d
{
  /** X [m] */
  double px;
  /** Y [m] */
  double py;
  /** yaw [rad] */
  double pa;
} player_pose2d_t;

/** @brief A pose in space */
typedef struct player_pose3d
{
  /** X [m] */
  double px;
  /** Y [m] */
  double py;
  /** Z [m] */
  double pz;
  /** roll [rad] */
  double proll;
  /** pitch [rad] */
  double ppitch;
  /** yaw [rad] */
  double pyaw;
} player_pose3d_t;

/** @brief A rectangular bounding box, used to define the size of an object */
typedef struct player_bbox2d
{
  /** Width [m] */
  double sw;
  /** Length [m] */
  double sl;
} player_bbox2d_t;

/** @brief A rectangular bounding box, used to define the size of an object */
typedef struct player_bbox3d
{
  /** Width [m] */
  double sw;
  /** Length [m] */
  double sl;
  /** Height [m] */
  double sh;
} player_bbox3d_t;

/** @brief A line segment, used to construct vector-based maps */
typedef struct player_segment
{
  /** Endpoints [m] */
  double x0;
  /** Endpoints [m] */
  double y0;
  /** Endpoints [m] */
  double x1;
  /** Endpoints [m] */
  double y1;
} player_segment_t;

/** @brief A rectangular bounding box, used to define the origin and bounds of an object.
* It is expected that x0 is less than x1 and y0 is less than y1. The points (x0,y0) and (x1,y1)
* represent opposite sides of the rectangle.
*/
typedef struct player_extent2d
{
  /** Origin x [m] */
  double x0;
  /** Origin y [m] */
  double y0;
  /** Endpoints [m] */
  double x1;
  /** Endpoints [m] */
  double y1;
} player_extent2d_t;

/** @brief A color descriptor */
typedef struct player_color
{
  /** Alpha (transparency) channel */
  uint8_t alpha;
  /** Red color channel */
  uint8_t red;
  /** Green color channel */
  uint8_t green;
  /** Blue color channel */
  uint8_t blue;
} player_color_t;

/** @brief A boolean variable, 0 for false anything else for true */
typedef struct player_bool
{
  /** state */
  uint8_t state;
} player_bool_t;

/** @} */

/**
@ingroup message_basics
@defgroup units Units
Standard units used in Player messages.

In the interest of using MKS units (http://en.wikipedia.org/wiki/Mks) the
internal message structure will use the following unit base types.

Base units
- kilogram [kg]
- second   [s]
- meter    [m]
- ampere   [A]
- radian   [rad]
- watt     [W]
- degree Celcsius [C]
- hertz    [Hz]
- decibel  [dB]
- volts    [V]

@note see float.h and limits.h for the limits of floats and integers on your
system

*/


/**
@ingroup message_basics
@defgroup capabilities Capabilities
Querying driver capabilities.

All drivers will respond to the universal request subtype, PLAYER_CAPABILTIES_REQ.

This request takes a data structure that defines the message type and subtype of the
capability you wish to query. The driver will respond with a NACK if the capability
is not supported and an ACK if it is.

The HANDLE_CAPABILITY_REQUEST macro (from driver.h) can be used to make this process
simpler, an example call would be something like this at the start of ProcessMessage

HANDLE_CAPABILITY_REQUEST (position_id, resp_queue, hdr, data, PLAYER_MSGTYPE_REQ, PLAYER_CAPABILTIES_REQ);
HANDLE_CAPABILITY_REQUEST (position_id, resp_queue, hdr, data, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_VEL);

*/

#define PLAYER_CAPABILTIES_REQ 255

/** @brief Structure containing a single capability request */
typedef struct player_capabilities_req
{
  /** The type of the requested capability (i.e. PLAYER_MSGTYPE_REQ). */
  uint32_t type;
  /** The subtype of the requested capability (i.e. PLAYER_ACTARRAY_SPEED_REQ. */
  uint32_t subtype;
} player_capabilities_req_t;


/**
@ingroup message_basics
@defgroup propbags Property Bags
Querying driver properties

*/

#define PLAYER_GET_INTPROP_REQ 254
#define PLAYER_SET_INTPROP_REQ 253
#define PLAYER_GET_DBLPROP_REQ 252
#define PLAYER_SET_DBLPROP_REQ 251
#define PLAYER_GET_STRPROP_REQ 250
#define PLAYER_SET_STRPROP_REQ 249

/** @brief Request to get an integer property */
typedef struct player_intprop_req
{
	/** The property key's length */
	uint32_t key_count;
	/** The property key */
	char *key;
	/** The property value */
	int32_t value;
} player_intprop_req_t;

/** @brief Request to get a double property */
typedef struct player_dblprop_req
{
	/** The property key's length */
	uint32_t key_count;
	/** The property key */
	char *key;
	/** The property value */
	double value;
} player_dblprop_req_t;

/** @brief Request to get a string property */
typedef struct player_strprop_req
{
	/** The property key's length */
	uint32_t key_count;
	/** The property key */
	char *key;
	/** The property's length */
	uint32_t value_count;
	/** The property value */
	char *value;
} player_strprop_req_t;

// /////////////////////////////////////////////////////////////////////////////
//
//             Here starts the alphabetical list of interfaces
//                       (please keep it that way)
//
// /////////////////////////////////////////////////////////////////////////////

/**
@ingroup libplayercore
@defgroup interfaces Interface specifications

All Player communication occurs through <i>interfaces</i>, which specify
the syntax and semantics for a set of messages. See the tutorial @ref
tutorial_devices for a discussion of what an interface is.

Below are the details.  For each interface, the following is given:
- Relevant constants (size limits, etc.)
- %Message subtypes:
  - Data subtypes : codes for each kind of data message defined by the interface
  - Command subtypes : codes for each kind of command message define by
  the interfce.
  - Request/reply subtypes: codes for each kind of request/reply message
  defined by the interface.  Also specified are the interaction semantics,
  such as when to send a null request and when to expect a null response.
  A "null" request or response is a zero-length message.
- Utility structures : structures that appear inside messages.
- %Message structures:
  - Data message structures : data messages that can be published via
  this interface.
  - Command message structures : command messages that can be sent to
  this interface.
  - Request/reply message structures : request messages that can be sent
  to this interface and reply messages that can be expected from it.

It can be the case that a given message can be sent as data or in response
to a request.  A common example is geometry.  For many devices geometry
is fixed and so need only be requested once.  For others geometry may
change dynamically and so the device will publish it periodically.

@todo
  - Normalize subtype names (PLAYER_PTZ_REQ_GEOM vs PLAYER_POSITION2D_REQ_GET_GEOM)
*/

#include <libplayercore/player_interfaces.h>

#endif /* PLAYER_H */
