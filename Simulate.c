/*
 * Simulate.c
 *
 *  Created on: 30-Jan-2013
 *      Author: Rajan
 */

#include "Simulate.h"

int main(int argc, char* argv[]) {

	elevator* elevator1 = newElevator();
	elevator* elevator2 = newElevator();

	while(!kbhit() || getch() != 'q') {

		// Generate number of groups arriving in next interval
		int numGroups = getNumberOfGroups();

		// Generate time of each group and process
		// Generated times are in increasing order
		int time_left = LOOPTIME;

		while(numGroups != 0) {
			int passed_time = rand()%time_left;

			// TODO: Move the elevators
			moveElevator(elevator1, passed_time);
			moveElevator(elevator2, passed_time);

			group* grp = newRandomGroup();
			elevator* chosenElevator = chooseElevator(elevator1, elevator2, grp->from, grp->to);
			if(grp->from > grp->to) llist_append(chosenElevator->downDestinations, grp);
			else llist_append(chosenElevator->upDestinations, grp);



			numGroups--;
		}

	}

	return 0;
}


elevator* newElevator() {
	elevator* temp = (elevator*) malloc(sizeof(elevator));
	temp->currentFloor = 0;
	temp->maxDown = 0;
	temp->maxUp = 0;
	temp->downDestinations = (LList*)malloc(sizeof(LList)*MAX_DESTINATIONS);
	temp->upDestinations = (LList*)malloc(sizeof(LList)*MAX_DESTINATIONS);
	int i;
	temp->direction = 0;
	temp->totalWeight = 0;
	return temp;
}

int getNumberOfGroups() {
	int random = rand() % 10;
	switch(random) {
	case 1:
	case 2:
	case 3:
	case 4: return 0;
	case 5:
	case 6:
	case 7: return 1;
	case 8:
	case 9: return 2;
	case 0: return 3;
	}
	return 0;
}

group* newRandomGroup() {
	int i;
	group* grp = (group*)malloc(sizeof(group));
	grp->numberOfPeople = getRandomSize();
	grp->from = rand() % FLOORS;
	grp->to = rand() % FLOORS;
	while(grp->to == grp->from) grp->to = rand() % FLOORS;
	grp->direction = (grp->from - grp->to)/abs(grp->from - grp->to);
	grp->Patience = (rand() % (MAX_WAIT - MIN_WAIT)) + MIN_WAIT;
	grp->weight = 0;
	for(i=0; i<grp->numberOfPeople; ++i) grp->weight += (rand() % (MAX_WEIGHT - MIN_WEIGHT)) + MIN_WEIGHT;
	return grp;
}

int getRandomSize() {
	int random = rand() % 10;
	switch(random) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5: return 1;
	case 6:
	case 7:
	case 8: return 2;
	case 9:
	case 0: return 3;
	}
	return 0;
}

elevator* chooseElevator( elevator* el1, elevator* el2, int from, int to ) {
	return abs(el1->currentFloor - from) < abs(el2->currentFloor - from) ? el1 : el2;
}

void moveElevator(elevator* el, int time) {
	if(el->direction == 1) {
		float gap = getNext(el->upDestinations, el->currentFloor) - el->currentFloor;


		/*float gap = el->maxUp - el->currentFloor;
		int time_used = gap * FLOORTIME;
		if(time_used > time)
			el->currentFloor += time*1.0/FLOORTIME;
		else {
			time = time - time_used;
			if(el->maxDown < el->maxUp) {
				el->direction = -1;
				el->maxUp = el->maxDown;
			}
		}*/
	}
}

int getNext(LList* dests, float floor) {
	return 0;
}
