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
	
	client.period = rand()%4+2; //Period for a recreation of the trashbag. [2;5] seconds
	return client;
}

TrashBin create_trash_bin(double volume, double volume_max_trash_bag, TrashType type){
	TrashBin trashBin;
	
	trashBin.volume = volume;
	trashBin.volume_max_trash_bag = volume_max_trash_bag;
	trashBin.type = type;
	
	pthread_mutex_init(&trashBin.mutex, NULL);
	
	return trashBin;
}	

TriPoint create_tri_point(int x, int y){
	TriPoint triPoint;
	pthread_mutex_init(&triPoint.mutex, NULL);
	//TODO
}
TriCenter create_tri_center(int nbTrucks, int period, int x, int y, TriPoint* triPoints, int nbTriPoint);

Truck create_truck(double volume, TriCenter* center) {
	Truck truck;
	
	truck.volume = volume;
	
	truck.center = center;
	truck.triPoints = NULL;
	
	truck.x = center->x;
	truck.y = center->y;
	
	pthread_mutex_init(&truck.mutex, NULL);

	return truck;
}

void put_trash_bag(Client* client){
	int i=0;
	pthread_mutex_lock(&client->point->mutex);
	int nbBins=client->point->nbBins;
	TrashBin* bins = client->point->bins;
	pthread_mutex_unlock(&client->point->mutex);
	
	while (i<nbBins){
		pthread_mutex_lock(&bins[i].mutex);
		if (bins[i].volume_max_trash_bag >= client->trash->volume &&
			bins[i].current_volume + client->trash->volume <= bins[i].volume
				&& bins[i].type == client->trash->type
				)
			break;
		pthread_mutex_unlock(&bins[i].mutex);
		i++;
	}
	if (i<nbBins){
		bins[i].current_volume += client->trash->volume;
		pthread_mutex_unlock(&bins[i].mutex);
	}
	else{
		pthread_mutex_lock(&client->point->free->mutex);
		client->point->free->volume += client->trash->volume;
		pthread_mutex_unlock(&client->point->free->mutex);	
	}
	if (bins->volume * VOLUME_ALERT <= bins->current_volume)
		//TODO Send signal to its TriPoint
		client->point;
}	

TrashBag generate_trash(Client* client){
	TrashBag trashBag;
	trashBag.type = rand()%3; //Avoid "ANY" type of wastes
	if (client->mode == BAC)
		trashBag.volume = 30;
	else if (client->mode == KEY){
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
	else{
		if (rand()%2 == 0)
			trashBag.volume = 30;
		else{
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
	}
	return trashBag;
}

void send_truck(TriCenter* center){
	
}

void wake_up_truck(Truck* truck){

}

void *thread_client(void* data){
	Client* client = (Client*) data;
	while(1){
		*(client->trash) = generate_trash(client);
		put_trash_bag(client);
		sleep(client->period);
	}
}

void* thread_tri_center(void* data){
	TriCenter* triCenter = (TriCenter*) data;
	time_t currentTime;
	time_t start;
	while(1){
		currentTime = 0;
		start = time(NULL);
		
		do{
			sleep(triCenter->period - currentTime);
			currentTime = time(NULL) - start;		
		}while((int)(currentTime / triCenter->period) == 0);
		send_truck(triCenter);
	}
}

TriPoint findClosestTriPoint(int x, int y){
	int i=0, index=-1;
	float min_distance = -1, distance;
	for (; i<nbTriPoints; i++)
	{
		distance = compute_distance_squared(x, y, triPoints[i].x, triPoints[i].y);
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
	return triPoints[index];
}


// Add a tri point to the specified truck rout (thread safe)
void add_tri_point_to_truck(TriPoint* point, Truck* truck) {
	pthread_mutex_lock(&truck->mutex);
	truck->triPoints[truck->nbTriPoint] = point;
	truck->nbTriPoint++;
	pthread_mutex_unlock(&truck->mutex);
}

// Compute the square of the distance between a truck and a tri point (not thread safe)
int compute_distance_squared_Truck_TriPoint(Truck* truck, TriPoint* point) {
	return compute_distance_squared(truck->x, truck->y, point->x, point->y);
}


// Compute the square of the distance between two points (not thread safe)
int compute_distance_squared(int x1, int y1, int x2, int y2) {
	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}


// Return the index of the closest truck of the tri center from the tri point specified (thread safe)
int find_closest_truck_from_tri_point(TriCenter* center, TriPoint* point) {
	
	Truck* trucks = center->trucks;
	int i;
	
	// Locking each trucks
	for(i = 0 ; i<center->nbTrucks ; ++i) {
		pthread_mutex_lock(&center->trucks[i].mutex);
	}
	
	int index = 0;
	int dist_min = compute_distance_squared_Truck_TriPoint(&center->trucks[0], point);
	
	for(i = 1 ; i<center->nbTrucks ; ++i) {
		int dist = compute_distance_squared_Truck_TriPoint(&center->trucks[i], point);
		if(dist < dist_min) {
			dist_min = dist;
			index = i;
		}
		
		// Unlocking the truck as soon as possible
		pthread_mutex_unlock(&center->trucks[i].mutex);
	}
	
	return index;
}

// Signal to the specified tri center that the specified tri point is full (thread safe)
void signal_tri_point_full(TriCenter* center, TriPoint* point) {
	int truck_index = find_closest_truck_from_tri_point(center, point);
	Truck* truck = &center->trucks[truck_index];
	
	
	// Checking if the truck is on the road or not
	if(truck->nbTriPoint == 0) {
		add_tri_point_to_truck(point, &center->trucks[truck_index]);
		// Waking the truck
		wake_up_truck(truck);
	} else {
		add_tri_point_to_truck(point, &center->trucks[truck_index]);
	}
	
	
}

// A thread associated with a truck passed in data, (normally thread safe)
void* thread_truck(void* data) {
	Truck* truck = (Truck*) data;
	
	while(1) {
		int i=0;
		for(i = 0 ; i<truck->nbTriPoint ; ++i) {
			
			// We lock the truck for the time of this treatment 
			pthread_mutex_lock(&truck->mutex);
			TriPoint* point = truck->triPoints[i];
			
			int startx = truck->x;
			int starty = truck->y;
			
			// We take the position of the tri point, stock in temporary variable to release the point
			pthread_mutex_lock(&point->mutex);
			int destx = point->x;
			int desty = point->y;
			pthread_mutex_unlock(&point->mutex);
			
			// Get the length of the travel in meters
			int travel_length = sqrt( (destx - startx) * (destx - startx) + (desty - starty) * (desty - starty) );
			// Supposing riding at 8m/s
			travel_length = travel_length / 8;
			
			// Release the truck
			pthread_mutex_unlock(&truck->mutex);
			
			// posing the truck for the time of the travel
			sleep(travel_length);
			
			// Once arrived we lock again the truck
			pthread_mutex_lock(&truck->mutex);
			
			// Updating the position
			truck->x = destx;
			truck->y = desty;
			
			// We lock the bin
			pthread_mutex_lock(&point->mutex);
			
			// Emptying each trash bin
			int j;
			for(j = 0 ; j<point->nbBins ; ++j) {
				pthread_mutex_lock(&point->bins[j].mutex);
				point->bins[j].current_volume = 0;
				pthread_mutex_unlock(&point->bins[j].mutex);
			}
			
			//Deleting illegal trashes.
			point->free->current_volume = 0;
			
			// Releasing the tri point
			pthread_mutex_unlock(&point->mutex);
			
			
		}
		
		// Going back to the tri center
		int destx = truck->center->x;
		int desty = truck->center->y;
		int startx = truck->x;
		int starty = truck->y;
		
		// Get the length of the travel in meters
		int travel_length = sqrt( (destx - startx) * (destx - startx) + (desty - starty) * (desty - starty) );
		// Supposing riding at 8m/s
		travel_length = travel_length / 8;
		sleep(travel_length);
		
		// Delete the route
		truck->nbTriPoint = 0;
		
		// Waiting for new order
		wait();
		
	}
}
