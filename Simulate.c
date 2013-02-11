/*
 * Simulate.c
 *
 *  Created on: 30-Jan-2013
 *      Author: Rajan
 */

#include <math.h>
#include "Simulate.h"

int main(int argc, char* argv[]) {

	FILE* fp = fopen("../elevator_log.txt", "w");

	fprintf(fp, " E1:\n");
	elevator* elevator1 = newElevator();
	fprintf(fp, " E2:\n");
	elevator* elevator2 = newElevator();

	int debug_count = 0;

	while(!kbhit() || getch() != 'q') {
		debug_count++;

		// Generate number of groups arriving in next interval
		int numGroups = getNumberOfGroups();

		// Generate time of each group and process
		// Generated times are in increasing order
		int time_left = LOOPTIME;

		while(numGroups != 0) {
			int passed_time = time_left > 0 ? rand()%time_left : 0;

			// Move the elevators
			moveElevator(elevator1, passed_time, fp);
			moveElevator(elevator2, passed_time, fp);
			time_left -= passed_time;

			// Generate group and choose which elevator serves it
			group* grp = newRandomGroup();

			fprintf(fp, "%ds Elevator1 - (%0.2f,%d) Elevator2 - (%0.2f,%d) Group - %d->%d\n",
					debug_count*LOOPTIME-time_left,
					elevator1->currentFloor,
					elevator1->direction,
					elevator2->currentFloor,
					elevator2->direction,
					grp->from,
					grp->to);

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

			numGroups--;
		}

		fprintf(fp, " E1:\n");
		moveElevator(elevator1, time_left, fp);
		fprintf(fp, " E2:\n");
		moveElevator(elevator2, time_left, fp);

	}

	fclose(fp);
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
	case 2: return 0;
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

	int direction = (to-from) / abs(to-from);
	if(direction == 1) {
		if(el1->upStops[from] == 1)
			return el1;
		else if(el2->upStops[from] == 1)
			return el2;
	} else {
		if(el1->downStops[from] == 1)
			return el1;
		else if(el2->downStops[from] == 1)
			return el2;
	}

	// TODO: Write pseud algo here
	return abs(el1->currentFloor - from) < abs(el2->currentFloor - from) ? el1 : el2;
}

void moveElevator(elevator* el, int time, FILE* fp) {

	// In case the elevator is stopped at some floor, waiting for doors to close
	if(el->pendingActionTime > time) {
		fprintf(fp, "	%ds used in waiting for people to enter/exit at %0.2f\n", time, el->currentFloor);
		el->pendingActionTime -= time;
		return;
	} else {
		if(el->pendingActionTime > 0) fprintf(fp, "    %ds used in waiting for people to enter/exit at %0.2f\n", el->pendingActionTime, el->currentFloor);
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
				time -= (int)(gap * FLOORTIME);
				if(time < 0) {
					fprintf(fp, "    %ds used to move from %0.2f to ", time + (int)(gap * FLOORTIME), el->currentFloor);
					el->currentFloor = el->currentFloor + (time + gap * FLOORTIME) * 1.0 / FLOORTIME;
					fprintf(fp, "%0.2f\n", el->currentFloor);
					return;
				}

				fprintf(fp, "    %ds used to move from %0.2f to %d\n", (int)(gap * FLOORTIME), el->currentFloor, next_floor);

				el->currentFloor = next_floor;
				el->upStops[next_floor] = 0;

				// At floor time
				int timeout = getOnAndOff(el, next_floor);
				time -= timeout;
				if(time < 0) {
					fprintf(fp, "    %ds used for people to exte/exit at %d with pending %ds\n", time+timeout, next_floor, -time);
					el->pendingActionTime = -time;
					return;
				} else fprintf(fp, "    %ds used for people to exter/exit at %d\n", timeout, next_floor);

			} else {
				int i, flag = 0;
				for(i=0; i<FLOORS; ++i) {
					if(el->downStops[i] == 1) {
						el->direction = -1;
						flag = 1;
						break;
					}
				}
				if(flag == 0) {
					el->direction = 0;
					return;
				}
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
				time -= (int)(gap * FLOORTIME);
				if(time < 0) {
					fprintf(fp, "    %ds used to move from %0.2f to ", (int)time + (int)(gap * FLOORTIME), el->currentFloor);
					el->currentFloor = el->currentFloor - (time + gap * FLOORTIME) * 1.0 / FLOORTIME;
					fprintf(fp, "%0.2f\n", el->currentFloor);
					return;
				}

				fprintf(fp, "    %ds used to move from %0.2f to %d\n", (int)(gap * FLOORTIME), el->currentFloor, next_floor);

				el->currentFloor = next_floor;
				el->downStops[next_floor] = 0;

				int timeout = getOnAndOff(el, next_floor);
				time -= timeout;
				if(time < 0) {
					fprintf(fp, "    %ds used for people to get on/off at %d with pending %ds\n", time+timeout, next_floor, -time);
					el->pendingActionTime = -time;
					return;
				}  else fprintf(fp, "    %ds used for people to get on/off at %d\n", timeout, next_floor);

			} else {
				int i, flag = 0;
				for(i=0; i<FLOORS; ++i) {
					if(el->upStops[i] == 1) {
						el->direction = 1;
						flag = 1;
						break;
					}
				}
				if(flag == 0) {
					el->direction = 0;
					return;
				}
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
			el->downStops[((group*)temp->data)->to] = 1;
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
			el->upStops[((group*)temp->data)->to] = 1;
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
