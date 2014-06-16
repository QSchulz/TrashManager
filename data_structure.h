#ifndef DATA_STRUCTURE__H
#define DATA_STRUCTURE__H

#include <stdlib.h>

//Currently unused
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
	//currently unused
	TrashType type;
} TrashBag;

typedef struct trashbin {
	double volume;
	double volume_max_trash_bag;
	double current_volume;
	//currenlty unused
	TrashType type;
	
	int mutex;
} TrashBin;

typedef struct tripoint {
	TrashBin* bins;
	//currently unused
	int nbBins;
	
	TrashBin* free;
	//currently unused
	int nbFree;
	
	int x, y;
	
	int mutex;
} TriPoint;

typedef struct triCenter {
	struct truck* trucks;
	int nbTrucks;
	
	int period;
	int x, y;
	
	TriPoint* triPoints;
	int nbTriPoint;
} TriCenter;

typedef struct truck {
	double volume;
	
	struct triCenter* center;
	
	TriPoint** triPoints;
	int nbTriPoint;
	
	int x, y;
	
	int mutex;
} Truck;

typedef struct client {
	Mode mode;
	//Pointer really needed? Each client has a different trash :)
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
extern int mutexTriPoints;

extern TriCenter* triCenters;
extern int nbTriCenters;


#define VOLUME_ALERT 0.8


#endif
