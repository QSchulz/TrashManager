
// Add a tri point to the specified truck rout (thread safe)
void add_tri_point_to_truck(TriPoint* point, Truck* truck) {
	P(truck->mutex);
	truck->triPoints[truck->nbTriPoint] = point;
	truck->nbTriPoint++;
	V(truck->mutex);
}

// Compute the square of the distance between a truck and a tri point (not thread safe)
int compute_distance_squared(Truck* truck, TriPoint* point) {
	return compute_distance_squared(truck->x, truck->y, point->x, point->y);
}


// Compute the square of the distance between two points (not thread safe)
int compute_distance_squared(int x1, int y1, int x2, int y2) {
	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)
}


// Return the index of the closest truck of the tri center from the tri point specified (thread safe)
int find_closest_truck_from_tri_point(TriCenter* center, TriPoint* point) {
	
	Truck* trucks = center->trucks;
	int i;
	
	// Locking each trucks
	for(i = 0 ; i<center->nbTrucks ; ++i) {
		P(center->trucks[i].mutex);
	}
	
	int index = 0;
	int dist_min = compute_distance_squared(center->trucks[0], point);
	
	for(i = 1 ; i<center->nbTrucks ; ++i) {
		int dist = compute_distance_squared(center->trucks[i], point);
		if(dist < dist_min) {
			dist_min = dist;
			index = i;
		}
		
		// Unlocking the truck as soon as possible
		V(center->trucks[i].mutex);
	}
	
	return index;
}

// Signal to the specified tri center that the specified tri point is full (thread safe)
void signal_tri_point_full(TriCenter* center, TriPoint* point) {
	int truck_index = find_closest_truck_from_tri_point(center, point);
	Truck* truck = center->trucks[truck_index];
	
	
	// Checking if the truck is on the road or not
	if(truck->nbTriPoint == 0) {
		add_tri_point_to_truck(point, center->trucks[truck_index]);
		// Waking the truck
		// TODO : How to wake up a thread ?
		wake_up_truck(truck);
	} else {
		add_tri_point_to_truck(point, center->trucks[truck_index]);
	}
	
	
}

// A thread associated with a truck passed in data, (normally thread safe)
void* thread_truck(void* data) {
	Truck* truck = (Truck*) data;
	
	while(truck->running == 1) {
		int i=0;
		for(i = 0 ; i<truck->nbTriPoint ; ++i) {
			
			// We lock the truck for the time of this treatment 
			P(truck->mutex);
			TriPoint* point = truck->triPoints[i];
			
			int startx = truck->x;
			int starty = truck->y;
			
			// We take the position of the tri point, stock in temporary variable to release the point
			P(point->mutex);
			int destx = point->x;
			int desty = point->y;
			V(point->mutex);
			
			// Get the length of the travel in meters
			int travel_length = sqrt( (destx - startx) * (destx - startx) + (desty - starty) * (desty - starty) );
			// Supposing riding at 8m/s
			travel_length = travel_length / 8;
			
			// Release the truck
			V(truck->mutex);
			
			// posing the truck for the time of the travel
			sleep(travel_length);
			
			// Once arrived we lock again the truck
			P(truck->mutex);
			
			// Updating the position
			truck->x = destx;
			truck->y = desty;
			
			// We lock the bin
			P(point->mutex);
			
			// Emptying each trash bin
			int j;
			for(j = 0 ; j<point->nbBins ; ++j) {
				P(point->bins[j].mutex);
				point->bins[j].current_volume = 0;
				V(point->bins[j].mutex);
			}
			
			// Removing the free bags
			for(j = 0 ; j<point->nbFree ; ++j) {
				free(point->free[j]);
			}
			point->nbFree = 0;
			
			// Releasing the tri point
			V(point->mutex);
			
			
		}
		
		// Going back to the tri center
		int destx = truck->center.x;
		int desty = truck->center.y;
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

void* thread_tri_center(void* data) {
	TriCenter* center = (TriCenter*) data;
	
	while(center->running == 1) {
		sleep(center->period);
		
		// Waking up every truck
		send_truck(center);
		
		
	}
	
}