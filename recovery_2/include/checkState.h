#ifndef CHECKSTATE_H
#define CHECKSTATE_H

#include "functions.h"
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>


float MAX_ALTITUDE = 0;

float DROGUE_ALTITUDE = 0;

int apogeeCounter = 0;

// This checks that we have started ascent
// If we have a positive 20 metres displacement upwards
int checkInFlight(float altitude)
{
  float displacement = altitude - BASE_ALTITUDE;
  if (displacement > MAX_GROUND_STATE_DEVIATION)
  {
    return IN_FLIGHT_STATE;
  }
  else
  {
    return PRE_FLIGHT_GROUND_STATE;
  }
}

// This checks that we have reached apogee and begun descent
// we check if previous altitude is greater than current altitude. To be certain, this has to be true at least 30 times
int checkForApogee(float velocity, float currentAltitude, float previousAltitude)
{
  if (currentAltitude < previousAltitude)
  {
    apogeeCounter++;
  }
  if (apogeeCounter == MIN_APOGEE_COUNT)
  {
    MAX_ALTITUDE = currentAltitude;
    return BALLISTIC_DESCENT_STATE;
  }
  else
  {
    return IN_FLIGHT_STATE;
  }
}

// Deploys drogue parachute if we moved down 20 metres below apogee
int AttemptDeployDrogueChute(float altitude)
{
  float displacement = MAX_ALTITUDE - altitude;
  if (displacement > DROGUE_DEPLOYMENT_MAX_DISPLACEMENT)
  {
    DROGUE_ALTITUDE = altitude;
    // Fires drogue ejection charge
    ejection(DROGUE_EJECTION_PIN);
    return DROGUE_CHUTE_DESCENT_STATE;
  }
  else
  {
    return BALLISTIC_DESCENT_STATE;
  }
}

// Deploys Main parachute if we moved down 500 metres below drogue
int AttemptDeployMainChute(float altitude)
{
  float displacement = DROGUE_ALTITUDE - altitude;
  if (displacement > MAIN_DEPLOYMENT_MAX_DISPLACEMENT)
  {
    // Fires main ejection charge
    ejection(MAIN_EJECTION_PIN);
    return MAIN_CHUTE_DESCENT_STATE;
  }
  else
  {
    return DROGUE_CHUTE_DESCENT_STATE;
  }
}

// This checks that we have reached the ground
// detects landing of the rocket
int checkGround(float currentAltitude, float previousAltitude)
{
  float difference = abs(currentAltitude - previousAltitude);
  if (difference < MAX_GROUND_STATE_DEVIATION)
  {
    return POST_FLIGHT_GROUND_STATE;
  }
  else
  {
    return MAIN_CHUTE_DESCENT_STATE;
  }
}

// Updates the state-machine state
// We check if rocket has launched to move from PRE_FLIGHT_GROUND_STATE to IN_FLIGHT_STATE
// We check if we have reached apogee to move to BALLISTIC_DESCENT_STATE
// We deploy drogue parachute to move to DROGUE_CHUTE_DESCENT_STATE
// We deploy main parachute to move to MAIN_CHUTE_DESCENT_STATE
// We check if we have reached the ground to move to POST_FLIGHT_GROUND_STATE
int checkState(float currentAltitude, float previousAltitude, float velocity, float acceleration, int state)
{
  switch (state)
  {
  case PRE_FLIGHT_GROUND_STATE:
    return checkInFlight(currentAltitude);
  case IN_FLIGHT_STATE:
    return checkForApogee(velocity, currentAltitude, previousAltitude);
  case BALLISTIC_DESCENT_STATE:
    return AttemptDeployDrogueChute(currentAltitude);
  case DROGUE_CHUTE_DESCENT_STATE:
    return AttemptDeployMainChute(currentAltitude);
  case MAIN_CHUTE_DESCENT_STATE:
    return checkGround(currentAltitude, previousAltitude);
  case POST_FLIGHT_GROUND_STATE:
    return POST_FLIGHT_GROUND_STATE;
  default:
    return checkInFlight(currentAltitude);
  }
}

#endif