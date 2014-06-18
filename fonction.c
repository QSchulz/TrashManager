#include "fonction.h"

Client* create_client(int x, int y) {
	Client* client = (Client*) malloc(sizeof(Client));
	
	client->mode = rand()%3;
	
	client->trash = (TrashBag*) malloc(sizeof(TrashBag));
	client->nbTrash = 0;
	
	client->x = x;
	client->y = y;
	
	client->point = NULL;
	client->nbPerson = rand()%5+1;
	
	client->period = rand()%4 + 2;
	return client;
}

TrashBin* create_trash_bin(double volume, double trash_bag_size, TrashType type, Mode mode) {
	TrashBin* bin = (TrashBin*) malloc(sizeof(TrashBin));

	if(bin == NULL) {
		printf("Erreur lors de la création d'une corbeille\n");
		exit(EXIT_FAILURE);
	}
	bin->volume = volume;
	bin->volume_max_trash_bag = trash_bag_size;
	bin->type = type;
	bin->mode = mode;
	
	bin->current_volume = 0;
	
	bin->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));

	pthread_mutex_init(bin->mutex, NULL);
	
	return bin;
}

TriPoint* create_tri_point(int x, int y) {
	TriPoint* point = (TriPoint*) malloc(sizeof(TriPoint));
	point->nbBins = 6;

	printf("Création de %d corbeilles de volume %d\n", point->nbBins, TRASH_BIN_VOLUME);
	point->bins = (TrashBin**) malloc(sizeof(TrashBin*) * point->nbBins);

	point->bins[0] = create_trash_bin(TRASH_BIN_VOLUME, 240, WASTE, BAC);
	point->bins[1] = create_trash_bin(TRASH_BIN_VOLUME, 240, GLASS, BAC);
	point->bins[2] = create_trash_bin(TRASH_BIN_VOLUME, 240, PAPER, BAC);
	
	point->bins[3] = create_trash_bin(TRASH_BIN_VOLUME, 240, WASTE, KEY);
	point->bins[4] = create_trash_bin(TRASH_BIN_VOLUME, 240, GLASS, KEY);
	point->bins[5] = create_trash_bin(TRASH_BIN_VOLUME, 240, PAPER, KEY);
	
	point->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(point->mutex, NULL);
	
	point->free = create_trash_bin(TRASH_BIN_FREE_VOLUME, 1000, ANY, BAC);
	
	point->x = x;
	point->y = y;

	return point;
}

TriCenter* create_tri_center(int period, int x, int y, TriPoint** triPoints, int nbTriPoint) {
	TriCenter* center = (TriCenter*) malloc(sizeof(TriCenter));
	
	center->period = period;
	center->x = x;
	center->y = y;
	center->triPoints = triPoints;
	center->nbTriPoints = nbTriPoint;
	
	center->current_point = NULL;

	return center;
}

TrashBag* generate_trash(Client* client) {
	TrashBag *trash = (TrashBag*) malloc(sizeof(TrashBag));
	trash->type = rand()%3;
	if(client->mode == BAC)
		trash->volume = 30;
	else if (client->mode == KEY) {
		switch(client->nbPerson) {
			case 1:
				trash->volume = 80;
				break;
			case 2:
				trash->volume = 120;
				break;
			case 3:
			case 4:
				trash->volume = 180;
				break;
			default:
				trash->volume = 240;
				break;
		}
	} else {
		if(rand() % 2 == 0) {
			trash->volume = 30;
		} else {
			switch(client->nbPerson) {
				case 1:
					trash->volume = 80;
					break;
				case 2:
					trash->volume = 120;
					break;
				case 3:
				case 4:
					trash->volume = 180;
					break;
				default:
					trash->volume = 240;
					break;
			}
		}
	}
	
	return trash;
}

void signal_center(int sig) {
	if(sig == SIGUSR1) {
		printf("Le signal de vidage d'urgence d'une poubelle à été reçu !\n");
		pthread_mutex_lock(tri_point_full_mutex);
		empty_tri_point(tri_point_full, center);
		pthread_mutex_unlock(tri_point_full_mutex);
		signal(SIGUSR1, signal_center);
	}
}

void empty_tri_point(TriPoint* point, TriCenter* center) {
	if(point != center->current_point && point != NULL) {
		// Lock the tri point
		pthread_mutex_lock(point->mutex);
		printf("Vidage du point de tri plein\n");
		// Empty each trash bin
		int j;
		for(j = 0 ; j < point->nbBins ; ++j) {
			pthread_mutex_lock(point->bins[j]->mutex);
			point->bins[j]->current_volume = 0;
			pthread_mutex_unlock(point->bins[j]->mutex);
		}
		
		// Empty the illegal deposit
		pthread_mutex_lock(point->free->mutex);
		point->free->current_volume = 0;
		pthread_mutex_unlock(point->free->mutex);
		
		// Simulating time emptying the bins
		sleep(1);
	
		pthread_mutex_unlock(point->mutex);
	}
}

void pick_up_trash(TriCenter* center) {
	int i;

	printf("Le centre de tri vide les différents point de tri\n");
	for(i = 0 ; i < center->nbTriPoints ; ++i) {
		center->current_point = center->triPoints[i];
		TriPoint* point = center->current_point;
		// Lock the tri point
		pthread_mutex_lock(point->mutex);
		printf("Vidage du point de tri %d\n", i);
		// Empty each trash bin
		int j;
		for(j = 0 ; j < point->nbBins ; ++j) {
			pthread_mutex_lock(point->bins[j]->mutex);
			point->bins[j]->current_volume = 0;
			pthread_mutex_unlock(point->bins[j]->mutex);
		}
		
		// Empty the illegal deposit
		pthread_mutex_lock(point->free->mutex);
		point->free->current_volume = 0;
		pthread_mutex_unlock(point->free->mutex);
		
		// Simulating time emptying the bins
		sleep(1);
	
		printf("Le point de tri %d est désormais vide\n", i);
		// Unlocking the tri point
		center->current_point = NULL;
		pthread_mutex_unlock(point->mutex);

		// Simulating time between two trash point
		sleep(2);
	}
}

void put_trash_bag(Client* client) {	
	if(client->trash != NULL) {
		TriPoint* point = client->point;
	
		// pthread_mutex_lock(point->mutex);
		int nbBins=point->nbBins;
		TrashBin** bins = client->point->bins;
		int i;
		for(i = 0 ; i < nbBins ; ++i) {
			pthread_mutex_lock(bins[i]->mutex);
			if((bins[i]->mode == client->mode || client->mode == KEY_BAC) && bins[i]->type == client->trash->type) {
				if(bins[i]->current_volume < bins[i]->volume) {
					bins[i]->current_volume += client->trash->volume;
					printf("Le client dépose une poubelle de %0.1fl de type %d dans la poubelle de type %d et de mode %d située aux coordonnées x:%d y:%d grâce au mode %d. Son volume est maintenant de %0.1f.\n", client->trash->volume, client->trash->type, bins[i]->type, bins[i]->mode, client->point->x, client->point->y, client->mode, bins[i]->current_volume);
					pthread_mutex_unlock(bins[i]->mutex);
					break;
				} else {
					if(client->mode == KEY_BAC && i < 3){}
					else {
						pthread_mutex_unlock(bins[i]->mutex);
						pthread_mutex_lock(point->free->mutex);
						point->free->current_volume += client->trash->volume;
						
						printf("Le client s'est fait avoir, les poubelles sont pleines. Il dépose une poubelle de %0.1fl de type %d devant le point de tri situé aux coordonnées x:%d y:%d. Le volume de déchets illégaux est maintenant de %0.1f.\n", client->trash->volume, client->trash->type, point->x, point->y, point->free->current_volume);
						pthread_mutex_unlock(point->free->mutex);
						break;
					}
						
				}
			}
			pthread_mutex_unlock(bins[i]->mutex);
		}
		
		//pthread_mutex_unlock(point->mutex);
	}
}

int dist_client_tri_point(Client* client, TriPoint* point) {
	return (client->x - point->x) * (client->y - point->y) + (client->y - point->y) * (client->y - point->y);
}

void assign_tri_point(Client* client, TriPoint** points, int nbPoint) {
	int i;
	int min = dist_client_tri_point(client, points[0]), index_min = 0;
	
	for(i = 1 ; i < nbPoint ; ++i) {
		int dist = dist_client_tri_point(client, points[i]);
		if(dist < min) {
			min = dist;
			index_min = i;
		}
	}
	
	client->point = points[index_min];
}

void* thread_client(void* data) {
	Client* client = (Client*) data;
	while(1) {
		client->trash = generate_trash(client);
		put_trash_bag(client);
		sleep(client->period);
	}
}

void* thread_tri_point(void* data) {
	TriPoint* point = (TriPoint*) data;
	int i;

	while(1) {
		// TODO: Do something
		pthread_mutex_lock(point->mutex);
		for(i = 0 ; i < point->nbBins ; ++i) {
			if(point->bins[i]->current_volume >= point->bins[i]->volume) {
				pthread_mutex_lock(tri_point_full_mutex);
				tri_point_full = point;
				pthread_mutex_unlock(tri_point_full_mutex);
				if(threads[nbThreads - 1] != NULL)
					pthread_kill(*threads[nbThreads - 1], SIGUSR1);
				
				break;
			}
		}
		pthread_mutex_unlock(point->mutex);
		usleep(500000);
	}
	
}

void* thread_tri_center(void* data) {
	TriCenter* center = (TriCenter*) data;
	time_t currentTime;
	time_t start;

	signal(SIGUSR1, signal_center);

	while(1) {
		currentTime = 0;
		start = time(NULL);
		
		do {
			sleep(center->period - currentTime);
			currentTime = time(NULL) - start;
		} while((int) (currentTime / center->period == 0));
		
		pick_up_trash(center);
	}
}
