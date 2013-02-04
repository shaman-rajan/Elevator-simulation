/*
 * Simulate.h
 *
 *  Created on: 30-Jan-2013
 *      Author: Rajan
 */

#ifndef SIMULATE_H_
#define SIMULATE_H_

/*
 * Includes
 */
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "LList.h"

/*
 * Constants
 */


#define WEIGHT_LIMIT 400		// Elevator weight limit
#define FLOORTIME 3				// Floor to floor time
#define LOOPTIME 10				// 1 loop in the program
#define FLOORS 5				// number of floors
#define MAX_WAIT 20				// Patience limits
#define MIN_WAIT 10				// Patience limits
#define MAX_WEIGHT 90			// Max weight of a person
#define MIN_WEIGHT 60			// Min weight of a person
#define MIN_STOP 5				// Stop time limit at a floor
#define MAX_STOP 10				// Stop time limit at a floor
#define MAX_DESTINATIONS 100	// Array size of elevator destinations
#define LIFT_ENTRY_TIME 1		// Time 1 person takes to get in or out of lift

/*
 * Structures
 */

struct _elevator {
	int direction;
	float currentFloor;
	int pendingActionTime;
	int upStops[FLOORS];
	int downStops[FLOORS];
	// Group lists
	LList* insideElevator[FLOORS];
	LList* waitingGroups[FLOORS];
	int totalWeight;
};
typedef struct _elevator elevator;

struct _group {
	int numberOfPeople;
	int Patience;			// In seconds
	int weight;
	int from;
	int to;
	int direction;
};
typedef struct _group group;

/*
 * Functions
 */
elevator* newElevator();
int getNumberOfGroups();
group* newRandomGroup();
int getRandomSize();
elevator* chooseElevator( elevator* el1, elevator* el2, int from, int to);
void moveElevator(elevator* el, int time);
int getNextUp(int dests[], float floor);
int getNextDown(int dests[], float floor);
int getOnAndOff(elevator* el, int floor);


#endif /* SIMULATE_H_ */
