#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "data_structure.h"
#include "fonction.h"

#define MAX_X 1000
#define MAX_Y 1000
#define CENTER_PERIOD 10


Client** clients = NULL;
TriPoint** triPoints = NULL;
pthread_t** threads = NULL;

TriCenter* center = NULL;

int nbClient = 0, nbTriPoint = 0, nbThreads;

void onKill(int sig) {
	printf("\n\nCleaning...\n");
	int i, j;

	printf("Terminaison des threads et libération\n");
	for(i = 0 ; i < nbThreads ; ++i) {
		pthread_cancel(*threads[i]);
		free(threads[i]);
	}
	
	tri_point_full = NULL;
	pthread_mutex_destroy(tri_point_full_mutex);

	printf("Libération des clients\n");
	for(i = 0 ; i < nbClient ; i++) {
		if(clients[i]->trash != NULL)
			free(clients[i]->trash);

		clients[i]->trash = NULL;
		free(clients[i]);
	}
	printf("Libération des points de tri\n");
	for(i = 0 ; i < nbTriPoint ; ++i) {
		printf("Libération des poubelles pour le point %d\n", i);
		for(j = 0 ; j < triPoints[i]->nbBins ; ++j) {
			if(triPoints[i]->bins[j] != NULL) {			
				pthread_mutex_destroy(triPoints[i]->bins[j]->mutex);
				free(triPoints[i]->bins[j]);
			}
		}
		pthread_mutex_destroy(triPoints[i]->mutex);
		free(triPoints[i]);
	}

	printf("Libération des différents tableau\n");
	free(threads);
	free(clients);
	free(triPoints);
	free(center);

	printf("Everything clear !\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
	int i;
	
	if(argc != 3) {
		printf("Wrong number of parameters.\n");
		printf("Usage : ./lo41 nbClient nbTriPoint\n");
		return EXIT_FAILURE;
	} else {
		nbClient = atoi(argv[1]);
		nbTriPoint = atoi(argv[2]);
		if(nbClient < 0 || nbTriPoint < 0) {
			printf("nbClient and nbTriPoint must be positif !\n");
			return EXIT_FAILURE;
		}
	}
	
	// Initialising data
	
	tri_point_full_mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(tri_point_full_mutex, NULL);
	tri_point_full = NULL;

	// Initialize randomizer
	srand(time(NULL));
	
	// Creating clients
	printf("Création de %d clients\n", nbClient);
	clients = (Client**) malloc(sizeof(Client*) * nbClient);
	if(clients == NULL) {
		printf("Erreur lors de l'initialisation du tableau de client\n");
		return EXIT_FAILURE;
	}
	
	for(i = 0 ; i<nbClient ; ++i) {
		clients[i] = create_client(rand()%MAX_X, rand()%MAX_Y);
		if(clients[i] == NULL) {
			printf("Erreur lors de l'initialisation du %dème client\n", i);
			return EXIT_FAILURE;
		}
	}
	
	// Creating tri points
	triPoints = (TriPoint**) malloc(sizeof(TriPoint*) * nbTriPoint);
	printf("Création de %d points de tri\n", nbTriPoint);
	if(triPoints == NULL) {
		printf("Erreur lors de l'initialisation du tableau de point de tri\n");
		return EXIT_FAILURE;
	}
	for(i = 0 ; i<nbTriPoint ; ++i) {
		triPoints[i] = create_tri_point(rand()%MAX_X, rand()%MAX_Y);
		if(triPoints[i] == NULL) {
			printf("Erreur lors de l'initialisation du %dème point de tri\n", i);
			return EXIT_FAILURE;
		}
	}
	
	// Assigning tri point to clients
	printf("Assignation des points de tri aux clients\n");
	for(i = 0 ; i<nbClient ; ++i) {
		assign_tri_point(clients[i], triPoints, nbTriPoint);
	}
	
	// Creating tri center
	printf("Création du centre de tri\n");
	center = create_tri_center(CENTER_PERIOD, rand()%MAX_X, rand()%MAX_Y, triPoints, nbTriPoint);
	if(center == NULL) {
		printf("Erreur lors de l'initialisation du centre de tri\n");
		return EXIT_FAILURE;
	}

	// Creating threads
	nbThreads = nbClient + nbTriPoint + 1;
	printf("Création de %d threads\n", nbThreads);
	threads = (pthread_t**) malloc(sizeof(pthread_t*) * nbThreads);
	if(threads == NULL) {
		printf("Erreur lors de l'initialisation du tableau de thread\n");
		return EXIT_FAILURE;
	}
	for(i = 0 ; i<nbThreads ; ++i) {
		threads[i] = (pthread_t*) malloc(sizeof(pthread_t));
		if(threads[i] == NULL) {
			printf("Erreur lors de l'initialisation du %dème thread\n", i);
		}
	}

	printf("\n\n*******************************************************\n\n");
	// Launching client threads
	for(i = 0 ; i < nbClient ; ++i) {
		pthread_create(threads[i], NULL, thread_client, (void*) clients[i]);
	}
	
	// Launching tri points threads
	for(i = 0 ; i < nbTriPoint ; ++i) {
		pthread_create(threads[i + nbClient], NULL, thread_tri_point, (void*) triPoints[i]);
	}
	
	// Launching tri center thread
	pthread_create(threads[nbThreads - 1], NULL, thread_tri_center, (void*) center);

	signal(SIGINT, onKill);
	
	pthread_join(*threads[nbThreads - 1], NULL);

	return EXIT_SUCCESS;
}
