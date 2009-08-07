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
 * robot_params.h
 *
 * ActivMedia robot parameters, automatically generated by saphconv.tcl from
 * Saphira parameter files:
 *    /Users/gerkey/Desktop/params/params/amigo-sh.p
 *    /Users/gerkey/Desktop/params/params/amigo.p
 *    /Users/gerkey/Desktop/params/params/arnl.p
 *    /Users/gerkey/Desktop/params/params/default-arnl.p
 *    /Users/gerkey/Desktop/params/params/default-sonarnl.p
 *    /Users/gerkey/Desktop/params/params/p2at.p
 *    /Users/gerkey/Desktop/params/params/p2at8+.p
 *    /Users/gerkey/Desktop/params/params/p2at8.p
 *    /Users/gerkey/Desktop/params/params/p2ce.p
 *    /Users/gerkey/Desktop/params/params/p2d8+.p
 *    /Users/gerkey/Desktop/params/params/p2d8.p
 *    /Users/gerkey/Desktop/params/params/p2de.p
 *    /Users/gerkey/Desktop/params/params/p2df.p
 *    /Users/gerkey/Desktop/params/params/p2dx.p
 *    /Users/gerkey/Desktop/params/params/p2it.p
 *    /Users/gerkey/Desktop/params/params/p2pb.p
 *    /Users/gerkey/Desktop/params/params/p2pp.p
 *    /Users/gerkey/Desktop/params/params/p3at-sh.p
 *    /Users/gerkey/Desktop/params/params/p3at.p
 *    /Users/gerkey/Desktop/params/params/p3atiw-sh.p
 *    /Users/gerkey/Desktop/params/params/p3dx-sh.p
 *    /Users/gerkey/Desktop/params/params/p3dx.p
 *    /Users/gerkey/Desktop/params/params/patrolbot-sh.p
 *    /Users/gerkey/Desktop/params/params/peoplebot-sh.p
 *    /Users/gerkey/Desktop/params/params/perfpb+.p
 *    /Users/gerkey/Desktop/params/params/perfpb.p
 *    /Users/gerkey/Desktop/params/params/pion1m.p
 *    /Users/gerkey/Desktop/params/params/pion1x.p
 *    /Users/gerkey/Desktop/params/params/pionat.p
 *    /Users/gerkey/Desktop/params/params/powerbot-sh.p
 *    /Users/gerkey/Desktop/params/params/powerbot.p
 *    /Users/gerkey/Desktop/params/params/psos1m.p
 *    /Users/gerkey/Desktop/params/params/psos1x.p
 *    /Users/gerkey/Desktop/params/params/psos43m.p
 *    /Users/gerkey/Desktop/params/params/sonarnl.p
 *    /Users/gerkey/Desktop/params/params/wheelchair-sh.p
*/

#ifndef _ROBOT_PARAMS_H
#define _ROBOT_PARAMS_H


void initialize_robot_params(void);

#define PLAYER_NUM_ROBOT_TYPES 36


typedef struct
{
  double x;
  double y;
  double th;
} sonar_pose_t;


typedef struct
{
  double x;
  double y;
  double th;
  double length;
  double radius;
} bumper_def_t;


typedef struct
{
  int AdjustNumSamplesFlag; //
  int AlignAngle; //
  int AlignSpeed; //
  double AngleConvFactor; //
  int AngleIncrement; //
  int CenterAwayCost; //
  const char* Class;
  int ClearOnFail; //
  int CollisionRange; //
  double DiffConvFactor; //
  double DiscardThreshold; //
  int DistConvFactor; //
  double DistanceWt; //
  int DrivingRotAccel; //
  int DrivingRotDecel; //
  int DrivingRotVelMax; //
  int DrivingTransAccel; //
  int DrivingTransDecel; //
  int DrivingTransNegVelMax; //
  int DrivingTransVelMax; //
  int EmergencyMaxTransDecel; //
  int FailedTh; //
  int FailedX; //
  int FailedY; //
  int FastSpeed; //
  int FrontBumpers; //
  int FrontClearance; //
  int FrontPaddingAtFastSpeed; //
  int FrontPaddingAtSlowSpeed; //
  int FuseAllSensors; //
  int GoalAngleTol; //
  int GoalDistanceTol; //
  int GoalOccupiedFailDistance; //
  int GoalRotAccel; //
  int GoalRotDecel; //
  int GoalRotSpeed; //
  int GoalSpeed; //
  double GoalSwitchTime; //
  int GoalTransAccel; //
  int GoalTransDecel; //
  int GoalUseEncoder; //
  int GridRes; //
  double GyroScaler; //
  int HasMoveCommand; //
  int HeadingRotAccel; //
  int HeadingRotDecel; //
  int HeadingRotSpeed; //
  double HeadingWt; //
  int Holonomic; //
  int IRNum; //
  int IRUnit; //
  int IdleTimeTriggerTh; //
  int IdleTimeTriggerX; //
  int IdleTimeTriggerY; //
  double KDegPerDeg; //
  double KDegPerMm; //
  double KMmPerMm; //
  int LaserFlipped; //
  const char* LaserIgnore;
  const char* LaserPort;
  int LaserPossessed; //
  int LaserPowerControlled; //
  int LaserTh; //
  int LaserX; //
  int LaserY; //
  int LocalPathFailDistance; //
  const char* Map;
  double MarkOldPathFactor; //
  int MaxRVelocity; //
  int MaxRotSpeed; //
  int MaxSpeed; //
  int MaxVelocity; //
  int MinNumSamples; //
  int NewTableSensingIR; //
  int NforLinVelIncrements; //
  int NforRotVelIncrements; //
  int NumFrontBumpers; //
  int NumRearBumpers; //
  int NumSamples; //
  int NumSamplesAngleFactor; //
  int NumSplinePoints; //
  double ObsThreshold; //
  double OccThreshold; //
  int OneWayCost; //
  double PassThreshold; //
  double PeakFactor; //
  int PeakStdTh; //
  int PeakStdX; //
  int PeakStdY; //
  int PeturbTh; //
  int PeturbX; //
  int PeturbY; //
  int PlanEverytime; //
  double PlanFreeSpace; //
  double PlanRes; //
  int Qtt; //
  int Qxx; //
  int Qyy; //
  double RangeConvFactor; //
  int RearBumpers; //
  int RecoverOnFail; //
  int ReflectorMatchDist; //
  int ReflectorMaxAngle; //
  int ReflectorMaxRange; //
  int ReflectorVariance; //
  int RequestEncoderPackets; //
  int RequestIOPackets; //
  int Resistance; //
  int RobotDiagonal; //
  int RobotLength; //
  int RobotLengthFront; //
  int RobotLengthRear; //
  int RobotRadius; //
  int RobotWidth; //
  int RotAccel; //
  int RotDecel; //
  int RotVelMax; //
  int SecsToFail; //
  double SensorBelief; //
  int SettableAccsDecs; //
  int SettableVelMaxes; //
  int SideClearanceAtFastSpeed; //
  int SideClearanceAtSlowSpeed; //
  int SlowSpeed; //
  int SmoothWindow; //
  int SonarAngleRes; //
  int SonarAperture; //
  double SonarBetaMax; //
  double SonarBetaMin; //
  int SonarIncidenceLimit; //
  double SonarLambdaF; //
  double SonarLambdaR; //
  int SonarMaxRange; //
  int SonarMinLineSize; //
  int SonarNum; //
  int SonarRangeRes; //
  int SonarSigma; //
  int SplineDegree; //
  int StallRecoverDuration; //
  int StallRecoverRotation; //
  int StallRecoverSpeed; //
  int StdTh; //
  int StdX; //
  int StdY; //
  const char* Subclass;
  int SuperMaxTransDecel; //
  int SwitchToBaudRate; //
  int TableSensingIR; //
  int TransAccel; //
  int TransDecel; //
  int TransVelMax; //
  int Triangulate; //
  double TriangulateScoreThreshold; //
  int TriggerAngle; //
  int TriggerDistance; //
  int TriggerTime; //
  int TriggerTimeEnabled; //
  int UseCollisionRangeForPlanning; //
  int UseEStop; //
  int UseLaser; //
  int UseReflectorCenters; //
  int UseSonar; //
  int Vel2Divisor; //
  int VelConvFactor; //
  double VelocityWt; //
  sonar_pose_t sonar_pose[32];
  bumper_def_t bumper_geom[12];
} RobotParams_t;


extern RobotParams_t PlayerRobotParams[];


#endif
