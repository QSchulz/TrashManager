#include "data_structure.h"
#include "functions.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


TriPoint* triPoints = NULL;
int nbTriPoints = 0;

TriCenter* triCenters = NULL;
int nbTriCenters = 0;

pthread_mutex_t mutexTriPoints;
pthread_cond_t condTriPoints;

void onInterrupt2(int sig) {
	printf("Second alarm received\n");
}

void onInterrupt(int sig) {
	printf("Alarm received\n");
	signal(SIGALRM, onInterrupt2);
	alarm(2);
	sleep(10 - 5);
}

int main(int argc, char* argv[]) {
	srand(time(NULL)); //DO NOT DELETE
	pthread_mutex_init(&mutexTriPoints, NULL); //DO NOT DELETE
	
	signal(SIGALRM, onInterrupt);
	
	alarm(5);
	sleep(10);
	printf("Hello World !\n");
	
	return 0;
}
