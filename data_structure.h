#ifndef DATA_STRUCTURE__H
#define DATA_STRUCTURE__H

#include <stdlib.h>

typedef enum trashtype {
	WASTE=0,
	GLASS,
	PAPER,
	ANY
} TrashType;

typedef enum mode {
	KEY=0,
	BAC,
	KEY_BAC
} Mode;

typedef struct trashbag {
	double volume;

	TrashType type;
} TrashBag;

typedef struct trashbin {
	double volume;
	double volume_max_trash_bag;
	double current_volume;

	TrashType type;
	Mode mode;
	
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} TrashBin;

typedef struct tripoint {
	TrashBin* bins;

	int nbBins;
	
	TrashBin free;
	
	int x, y;
	
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	
	int signal_send;
	pthread_t tid_TriCenter;
} TriPoint;

typedef struct triCenter {
	struct truck* trucks;
	int nbTrucks;
	
	int period;
	int x, y;
	
	TriPoint* triPoints;
	int nbTriPoints;
} TriCenter;

typedef struct truck {
	//unused
	double volume;
	
	struct triCenter* center;
	
	TriPoint** triPoints;
	int nbTriPoints;
	
	int x, y;
	
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} Truck;

typedef struct client {
	Mode mode;

	TrashBag trash;
	int x, y;
	TriPoint* point;
	
	int nbPerson;
	
	int period;
	int numero;
} Client;

//Global lists with their size
extern TriPoint* triPoints;
extern int nbTriPoints;

extern TriCenter* triCenters;
extern int nbTriCenters;

extern TriPoint** full_tri_points;
extern int nb_full_tri_points;
extern pthread_mutex_t full_tri_points_mutex;

extern int numClient;
#define VOLUME_ALERT 0.8
#define TRASH_BIN_VOLUME 5000
#define TRASH_BIN_FREE_VOLUME 10000
#define TRUCK_VOLUME 100000


#endif
