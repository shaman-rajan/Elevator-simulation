/*
 * Simulate.c
 *
 *  Created on: 30-Jan-2013
 *      Author: Rajan
 */

#include <math.h>
#include "Simulate.h"

int main(int argc, char* argv[]) {

	elevator* elevator1 = newElevator();
	elevator* elevator2 = newElevator();

	int debug_count = 0;

	while(!kbhit() || getch() != 'q') {
		printf("%d",debug_count);
		debug_count++;
		// Generate number of groups arriving in next interval
		int numGroups = getNumberOfGroups();

		// Generate time of each group and process
		// Generated times are in increasing order
		int time_left = LOOPTIME;

		while(numGroups != 0) {
			int passed_time = time_left > 0 ? rand()%time_left : 0;

			// Move the elevators
			moveElevator(elevator1, passed_time);
			moveElevator(elevator2, passed_time);

			// Generate group and choose which elevator serves it
			group* grp = newRandomGroup();
			elevator* chosenElevator = chooseElevator(elevator1, elevator2, grp->from, grp->to);
			if(chosenElevator->direction == 0) {
				if(chosenElevator->currentFloor > grp->from) {
					chosenElevator->direction = -1;
					chosenElevator->downStops[grp->from] = 1;
				} else {
					chosenElevator->direction = 1;
					chosenElevator->upStops[grp->from] = 1;
				}

			}

			if(grp->direction > 0) {
				chosenElevator->upStops[grp->from] = 1;
				llist_append(chosenElevator->waitingGroups[grp->from][1], grp);
			} else {
				chosenElevator->downStops[grp->from] = 1;
				llist_append(chosenElevator->waitingGroups[grp->from][0], grp);
			}

			time_left -= passed_time;
			numGroups--;
		}

		moveElevator(elevator1, time_left);
		moveElevator(elevator2, time_left);

	}

	return 0;
}


elevator* newElevator() {
	elevator* temp = (elevator*) malloc(sizeof(elevator));
	temp->currentFloor = 0;
	temp->pendingActionTime = 0;
	int i; for(i=0; i<FLOORS; ++i) {
		temp->downStops[i] = temp->upStops[i] = 0;
		temp->insideElevator[i] = llist_new();
		temp->waitingGroups[i][0] = llist_new();
		temp->waitingGroups[i][1] = llist_new();
	}
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
	grp->direction = (grp->to - grp->from)/abs(grp->from - grp->to);
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
	// TODO: Write pseud algo here

	return abs(el1->currentFloor - from) < abs(el2->currentFloor - from) ? el1 : el2;
}

void moveElevator(elevator* el, int time) {

	// In case the elevator is stopped at some floor, waiting for doors to close
	if(el->pendingActionTime > time) {
		el->pendingActionTime -= time;
		return;
	} else {
		time -= el->pendingActionTime;
		el->pendingActionTime = 0;
	}

	while(1) {
		if(el->direction == 1) {
			int next_floor = getNextUp(el->upStops, el->currentFloor);
			if(next_floor >= 0) {

				// In case no more groups want to go above the current floor,
				// elevator now changes direction. It will at least go till the floor
				// required by group waiting to go down.
				if(next_floor < el->currentFloor) {
					el->downStops[next_floor] = 1;
					el->direction = -1;
					continue;
				}

				// Inter-floor time
				float gap = next_floor - el->currentFloor;
				time -= gap * FLOORTIME;
				if(time <= 0) {
					el->currentFloor = el->currentFloor + (time + gap * FLOORTIME) * 1.0 / FLOORTIME;
					return;
				}
				el->currentFloor = next_floor;
				el->upStops[next_floor] = 0;

				// At floor time
				time -= getOnAndOff(el, next_floor);
				if(time <= 0) {
					el->pendingActionTime = -time;
					return;
				}

			} else {
				int i;
				for(i=0; i<FLOORS; ++i) {
					if(el->downStops[i] == 1) {
						el->direction = -1;
						continue;
					}
				}
				el->direction = 0;
				return;
			}
		} else if(el->direction == -1) {
			int next_floor = getNextDown(el->downStops, el->currentFloor);
			if(next_floor >= 0) {

				// In case no more groups want to go above the current floor,
				// elevator now changes direction. It will at least go till the floor
				// required by group waiting to go down.
				if(next_floor > el->currentFloor) {
					el->upStops[next_floor] = 1;
					el->direction = 1;
					continue;
				}

				// Inter-floor time
				float gap =  el->currentFloor - next_floor;
				time -= gap * FLOORTIME;
				if(time <= 0) {
					el->currentFloor = el->currentFloor - (time + gap * FLOORTIME) * 1.0 / FLOORTIME;
					return;
				}
				el->currentFloor = next_floor;
				el->downStops[next_floor] = 0;

				// At floor time
				time -= getOnAndOff(el, next_floor);
				if(time <= 0) {
					el->pendingActionTime = -time;
					return;
				}

			} else {
				int i;
				for(i=0; i<FLOORS; ++i) {
					if(el->upStops[i] == 1) {
						el->direction = 1;
						continue;
					}
				}
				el->direction = 0;
				return;
			}
		} else return;

	}
}

int getNextUp(int dests[], float floor) {
	int next = ceil(floor);
	do {
		if(dests[next] == 1) return next;
		next = (next + 1) % FLOORS;
	} while(next != ceil(floor));
	return -1;
}

int getNextDown(int dests[], float floor) {
	int next = floor;
	do {
		if(dests[next] == 1) return next;
		next = next - 1;
		if(next < 0) next = 4;
	} while(next != (int)floor);
	return -1;
}

int getOnAndOff(elevator* el, int floor) {
	int count = 0;

	// All the groups who get off here will go out
	Node* temp = el->insideElevator[floor]->head->next;
	while(temp != NULL) {
		count += ((group*)temp->data)->numberOfPeople;
		el->totalWeight -= ((group*)temp->data)->weight;
		temp = temp->next;
	}
	llist_clear_memory(el->insideElevator[floor]);

	// If elevator going down, then only those groups should get in.
	if(el->direction == -1) {
		temp = el->waitingGroups[floor][0]->head->next;
		while(temp != NULL) {
			llist_append(el->insideElevator[((group*)temp->data)->to], (group*)temp->data);
			count += ((group*)temp->data)->numberOfPeople;
			el->totalWeight += ((group*)temp->data)->weight;
			temp = temp->next;
		}
		llist_clear_save_nodes(el->waitingGroups[floor][0]);
	}

	// Next, for elevator going up.
	else if(el->direction == 1) {
		temp = el->waitingGroups[floor][1]->head->next;
		while(temp != NULL) {
			llist_append(el->insideElevator[((group*)temp->data)->to],(group*)temp->data);
			count += ((group*)temp->data)->numberOfPeople;
			el->totalWeight += ((group*)temp->data)->weight;
			temp = temp->next;
		}
		llist_clear_save_nodes(el->waitingGroups[floor][1]);
	}

	if(el->totalWeight > WEIGHT_LIMIT) {
		// TODO: Some code here
	}

	return count * LIFT_ENTRY_TIME;
}
