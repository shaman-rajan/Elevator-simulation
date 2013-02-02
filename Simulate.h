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
#define WEIGHT_LIMIT 400
#define FLOORTIME 3
#define LOOPTIME 10
#define FLOORS 5
#define MAX_WAIT 20
#define MIN_WAIT 10
#define MAX_WEIGHT 90
#define MIN_WEIGHT 60
#define MIN_STOP 5
#define MAX_STOP 10
#define MAX_DESTINATIONS 100

/*
 * Structures
 */

struct _elevator {
	int direction;
	float currentFloor;
	int maxUp;
	int maxDown;
	LList* upDestinations;
	LList* downDestinations;
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
int getNext(LList* dests, float floor);


#endif /* SIMULATE_H_ */
