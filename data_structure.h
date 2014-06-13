

typedef enum trashtype {
	WASTE,
	GLASS,
	PAPER
} TrashType;

typedef enum mode {
	KEY,
	BAC,
	KEY_BAC
} Mode;

typedef struct client {
	Mode mode;
	TrashBag* trash;
	int nbTrash;
	int x, y;
	TriPoint* point;
	
	int nbPerson;
	
	double period;
} Client;

typedef struct trashbag {
	double volume;
	TrashType type;
} TrashBag;

typedef struct trashbin {
	double volume;
	double volume_max_trash_bag;
	double current_volume;
	TrashType type;
	
	int mutex;
} TrashBin;

typedef struct tripoint {
	TrashBin* bins;
	TrashBag* free;
	int nbBins;
	int x, y;
	int nbFree;
	
	int mutex;
} TriPoint;

typedef struct tricenter {
	Truck* trucks;
	int nbTrucks;
	
	int period;
	int x, y;
	
	TriPoint* triPoints;
	int nbTriPoint;
} TriCenter;

typedef struct truck {
	double volume;
	TriCenter* center;
	TriPoint* triPoints;
	int nbTriPoint;
} Truck;
