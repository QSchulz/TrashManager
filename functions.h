#ifndef FUNCTIONS__H
#define FUNCTIONS__H

#include <pthread.h>

Client create_client(int x, int y);

TrashBin create_trash_bin(double volume, double volume_max_trash_bag, TrashType type);

TriPoint create_tri_point(int x, int y);
TriCenter create_tri_center(int nbTrucks, int period, int x, int y, TriPoint* triPoints, int nbTriPoint);
Truck create_truck(double volume, TriCenter* center);

void put_trash_bag(Client* client);
TrashBag generate_trash(Client* client);
void send_truck(TriCenter* center);
void wake_up_truck(Truck* truck);

void* thread_client(void* data);
void* thread_tri_center(void* data);

TriPoint* findClosestTriPoint(int x, int y);

void add_tri_point_to_truck(TriPoint* point, Truck* truck);
int compute_distance_squared_Truck_TriPoint(Truck* truck, TriPoint* point);
int compute_distance_squared(int x1, int y1, int x2, int y2);

int find_closest_truck_from_tri_point(TriCenter* center, TriPoint* point);
void signal_tri_point_full(TriCenter* center, TriPoint* point);
void* thread_truck(void* data);

#endif
