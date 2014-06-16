#ifndef DATA_STRUCTURE__H
#define DATA_STRUCTURE__H

#include <stdlib.h>

typedef enum trashtype {
	WASTE,
	GLASS,
	PAPER,
	ANY
} TrashType;

typedef enum mode {
	KEY,
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

	TrashBag* trash;
	//currently unused
	int nbTrash;
	int x, y;
	TriPoint* point;
	
	int nbPerson;
	
	double period;
} Client;

//Global lists with their size
extern TriPoint* triPoints;
extern int nbTriPoints;
extern pthread_mutex_t mutexTriPoints;
extern pthread_cond_t condTriPoints;

extern TriCenter* triCenters;
extern int nbTriCenters;


#define VOLUME_ALERT 0.8
#define TRASH_BIN_VOLUME 5000
#define TRASH_BIN_FREE_VOLUME 10000
#define TRUCK_VOLUME 100000


#endif
