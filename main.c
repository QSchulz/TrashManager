#include "data_structure.h"
#include "functions.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#define MAX_X 1000
#define MAX_Y 1000
#define TRI_POINTS_PER_TRI_CENTER 10
#define NB_TRUCKS_PER_TRI_CENTER TRI_POINTS_PER_TRI_CENTER/5
#define PERIOD 20000 //20sec

TriPoint* triPoints = NULL;
int nbTriPoints = 0;

TriCenter* triCenters = NULL;
int nbTriCenters = 0;

pthread_t* threads=NULL;
Client* clients=NULL;
int nbClients=0;

//Pourquoi ?
pthread_mutex_t mutexTriPoints;
pthread_cond_t condTriPoints;

void onKill(int sig) {
	int i=0;
	for (i=0;i<nbClients; i++)
		pthread_cancel(threads[i]);
	for (i=0;i<nbClients; i++){
	}
	/*
	for (i=0; i<nbTriCenters; i++)
		pthread_cancel(threads[i+nbClients]);
	for (i=0; i<nbTriCenters; i++){
	}*/
	
	for(i=0; i<nbTriPoints; i++){
		pthread_mutex_destroy(&triPoints[i].mutex);
		pthread_cond_destroy(&triPoints[i].cond);
		free(triPoints[i].bins);
	}
	free(clients);
	free(triCenters);
	free(triPoints);
	free(threads);
	//TODO Free IPCs and pointers
	exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
	if (argc != 3)
	{
		printf("Wrong call. Please use: ./lo41 [nbClients] [nbTriPoints]\n");
		return EXIT_FAILURE;
	}
	srand(time(NULL));
	nbClients = atoi(argv[1]);
	nbTriPoints = atoi(argv[2]);
	
	if (nbClients <= 0 || nbTriPoints <=0)
	{
		printf("Parameters have to be greater than 0.\n");
		return EXIT_FAILURE;
	}
	
	int i=0;
	
	triPoints = (TriPoint*) malloc(sizeof(TriPoint)*nbTriPoints);
	for (i=0; i<nbTriPoints; i++)
		triPoints[i] = create_tri_point(rand()%MAX_X, rand()%MAX_Y);
		
	//TODO Trucks?	
		
	triCenters = (TriCenter*) malloc(sizeof(TriCenter)*(nbTriPoints/TRI_POINTS_PER_TRI_CENTER));
	nbTriCenters = nbTriPoints/TRI_POINTS_PER_TRI_CENTER;
	for (i=0; i<nbTriCenters; i++)
		triCenters[i] = create_tri_center(NB_TRUCKS_PER_TRI_CENTER, PERIOD, rand()%MAX_X, rand()%MAX_Y, &triPoints[i*TRI_POINTS_PER_TRI_CENTER], TRI_POINTS_PER_TRI_CENTER);
		
	
	clients = (Client*) malloc(sizeof(Client)*nbClients);
	for (i=0;i<nbClients; i++)
		clients[i] = create_client(rand()%MAX_X, rand()%MAX_Y);
	
	//pthread_mutex_init(&mutexTriPoints, NULL);
	
	threads = (pthread_t*) malloc(sizeof(pthread_t)*(nbClients + nbTriCenters));
	
	for (i=0; i<nbClients; i++)
		pthread_create(&threads[i], NULL, thread_client, (void*) &clients[i]);
	
	//TODO Trucks?
	
	for (i=0; i<nbTriCenters; i++)
		pthread_create(&threads[i+nbClients], NULL, thread_tri_center, (void*) &triCenters[i]);
		
	
	signal(SIGKILL, onKill);
	
	for (i=0; i<nbClients; i++)
		pthread_join(threads[i], NULL);
	
	
	return 0;
}
