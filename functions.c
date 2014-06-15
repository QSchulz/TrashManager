#include "data_structure.h"
#include "functions.h"
#include <math.h>

Client create_client(int x, int y){
	Client client;
	
	client.mode = rand()%3; //Choose a random enum value for the mode
	
	client.trash = (TrashBag*) malloc(sizeof(TrashBag));
	client.nbTrash = 0;
	
	client.x = x;
	client.y = y;		
	
	client.point = (TriPoint*) malloc(sizeof(TriPoint));
	*(client.point) = findClosestTriPoint(x, y);
	
	client.nbPerson = rand()%5+1; //Randomize the number of persons in the family
	
	client.period = rand()%3+2; //Period for a recreation of the trashbag. [2;5] seconds
	return client;
}

//Currently unused
TrashBag create_trash_bag(double volume, TrashType type){
	TrashBag trashBag;
	
	trashBag.volume = volume;
	trashBag.type = type;
	
	return trashBag;
}

TrashBin create_trash_bin(double volume, double volume_max_trash_bag, TrashType type){
	TrashBin trashBin;
	
	trashBin.volume = volume;
	trashBin.volume_max_trash_bag = volume_max_trash_bag;
	trashBin.type = type;
	
	return trashBin;
}	

TriPoint create_tri_point(int x, int y);
TriCenter create_tri_center(int nbTrucks, int period, int x, int y, TriPoint* triPoints, int nbTriPoint);
Truck create_truck(double volume, TriCenter* center);

void put_trash_bag(Client* client){
	//TODO Mutex?
	//TODO try to find one bin amongst all that has enough remainding volume
	TrashBin* bins = client->point->bins;
	if (bins->volume_max_trash_bag>=client->trash->volume &&
			bins->current_volume + client->trash->volume <= bins->volume
//				&& bins->type == client->trash->type
				)
		bins->current_volume += client->trash->volume;
	if (bins->volume * VOLUME_ALERT <= bins->current_volume)
		//Send signal to its TriPoint
		client->point;
}	

TrashBag generate_trash(Client* client){
	TrashBag trashBag;
	if (client->mode == BAC)
		trashBag.volume = 30;
	else{
		//TODO Randomize on KEY_BAC?
		switch(client->nbPerson){
			case 1:
				trashBag.volume = 80;
				break;
			case 2:
				trashBag.volume = 120;
				break;
			case 3:
			case 4:
				trashBag.volume = 180;
				break;
			default:
				trashBag.volume = 240;
				break;
		}
	}
	return trashBag;
}

void send_truck(TriCenter* center);

void *thread_client(void* data){
	Client* client = (Client*) data;
	while(1){
		*(client->trash) = generate_trash(client);
		put_trash_bag(client);
		sleep(client->period);
	}
}

void pick_up_trash(TriCenter* triCenter){}

void* thread_tri_center(void* data){
	TriCenter* triCenter = (TriCenter*) data;
	time_t currentTime;
	while(1){
		currentTime = time(NULL);
		do{
			sleep(currentTime % triCenter->period);
			currentTime = time(NULL);
		}while((int)(currentTime / triCenter->period) == 0);
		pick_up_trash(triCenter);
	}
}

//TODO Don't we have to return a pointer?
TriPoint findClosestTriPoint(int x, int y){
	int i=0, index=-1;
	float min_distance = -1, distance;
	//TODO Mutex?
	for (; i<nbTriPoints; i++)
	{
		distance = distanceBetween(x, y, triPoints[i].x, triPoints[i].y);
		if (min_distance == -1)
		{
			min_distance = distance;
			index = i;
		}
		else if(min_distance > distance)
		{
			min_distance = distance;
			index = i;
		}
	}	
	//TODO Check if index is not -1 ???
	return triPoints[index];
}

float distanceBetween(int x1, int y1, int x2, int y2){
	int distanceOnXSquare = (x1-x2)*(x1-x2), distanceOnYSquare = (y1-y2)*(y1-y2);
	return sqrt(distanceOnXSquare + distanceOnYSquare);
}
