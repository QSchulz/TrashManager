#include "data_structure.h"
#include "functions.h"

#include <stdlib.h>
#include <stdio.h>

#define MAX_X 1000
#define MAX_Y 1000
#define TRI_POINTS_PER_TRI_CENTER 10
#define NB_TRUCKS_PER_TRI_CENTER TRI_POINTS_PER_TRI_CENTER/5
#define min(x, y) x<y ? x : y


TriPoint* triPoints = NULL;
int nbTriPoints = 0;

TriCenter* triCenters = NULL;
int nbTriCenters = 0;

pthread_t* threads=NULL;
Client* clients=NULL;
int nbClients=0;

Truck* trucks = NULL;
int nbTrucks=0;

TriPoint** full_tri_points = NULL;
int nb_full_tri_points = 0;
pthread_mutex_t full_tri_points_mutex;

int numClient=0;

void onKill(int sig) {
	int i=0;
	//Stop threads
	for (i=0; i<nbClients+nbTriCenters+nbTrucks+nbTriPoints; i++)
		pthread_cancel(threads[i]);
	//Free pointers and mutexes in structures
	for (i=0; i<nbTriPoints; i++){
		pthread_mutex_destroy(&triPoints[i].mutex);
		pthread_cond_destroy(&triPoints[i].cond);
		free(triPoints[i].bins);
	}
	for (i=0; i<nbTrucks; i++)
		free(trucks[i].triPoints);
		
	pthread_mutex_destroy(&full_tri_points_mutex);
		
	//Free global lists
	free(clients);
	free(triCenters);
	free(triPoints);
	free(threads);
	free(trucks);

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
	
	int i=0,j=0;
	
	// Initialising the full tri point message queue
	full_tri_points = (TriPoint**) malloc(sizeof(TriPoint*) * nbTriPoints);
	for(i = 0 ; i<nbTriPoints ; ++i) {
		full_tri_points[i] = NULL;
	}
	pthread_mutex_init(&full_tri_points_mutex, NULL);
	
	triPoints = (TriPoint*) malloc(sizeof(TriPoint)*nbTriPoints);
	for (i=0; i<nbTriPoints; i++)
		triPoints[i] = create_tri_point(rand()%MAX_X, rand()%MAX_Y);
		
	nbTriCenters = nbTriPoints/TRI_POINTS_PER_TRI_CENTER+1;
	nbTrucks = NB_TRUCKS_PER_TRI_CENTER*nbTriCenters;
	trucks = (Truck*) malloc(sizeof(Truck)*nbTrucks);
	for (i=0; i<nbTrucks; i++)
		trucks[i] = create_truck(TRUCK_VOLUME);
		
	triCenters = (TriCenter*) malloc(sizeof(TriCenter)*nbTriCenters);
	for (i=0; i<nbTriCenters; i++){
		triCenters[i] = create_tri_center(&trucks[i*NB_TRUCKS_PER_TRI_CENTER], NB_TRUCKS_PER_TRI_CENTER, PERIOD, rand()%MAX_X, rand()%MAX_Y, &triPoints[i*TRI_POINTS_PER_TRI_CENTER], min(TRI_POINTS_PER_TRI_CENTER, nbTriPoints - i*TRI_POINTS_PER_TRI_CENTER));
		for (j=0; j<NB_TRUCKS_PER_TRI_CENTER; j++)
			setTriCenter(&trucks[i*NB_TRUCKS_PER_TRI_CENTER+j], &triCenters[i]);
	}
		
	clients = (Client*) malloc(sizeof(Client)*nbClients);
	for (i=0;i<nbClients; i++)
		clients[i] = create_client(rand()%MAX_X, rand()%MAX_Y);
	
	//Threads' creation
	threads = (pthread_t*) malloc(sizeof(pthread_t)*(nbClients + nbTriCenters + nbTrucks + nbTriPoints));
	
	for (i=0; i<nbClients; i++)
		pthread_create(&threads[i], NULL, thread_client, (void*) &clients[i]);
			
	for (i=0; i<nbTriCenters; i++){
		pthread_create(&threads[i+nbClients], NULL, thread_tri_center, (void*) &triCenters[i]);
		for (j=0; j<TRI_POINTS_PER_TRI_CENTER; j++)
			setTID(&triPoints[i*TRI_POINTS_PER_TRI_CENTER+j], threads[i+nbClients]);
	}
		
	for (i=0; i<nbTrucks; i++)
		pthread_create(&threads[i+nbClients+nbTriCenters], NULL, thread_truck, (void*) &trucks[i]);
	
	for (i=0; i<nbTriPoints; i++)
		pthread_create(&threads[i+nbClients+nbTriCenters+nbTrucks], NULL, thread_tri_point, (void*) &triPoints[i]);
		
	
	signal(SIGKILL, onKill);
	
	for (i=0; i<nbClients+nbTriCenters+nbTrucks+nbTriPoints; i++)
		pthread_join(threads[i], NULL);
	
	return 0;
}
