#ifndef FUNCTIONS__H
#define FUNCTIONS__H

#define PERIOD 10 //10sec

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

Client create_client(int x, int y);

TrashBin create_trash_bin(double volume, double volume_max_trash_bag, TrashType type);
void setTID(TriPoint* triPoint, pthread_t pid_TriCenter);
void setTriCenter(Truck* truck, TriCenter* center);

TriPoint create_tri_point(int x, int y);
TriCenter create_tri_center(Truck* trucks, int nbTrucks, int period, int x, int y, TriPoint* triPoints, int nbTriPoint);
Truck create_truck(double volume);

void put_trash_bag(Client* client);
TrashBag generate_trash(Client* client);
void send_truck(TriCenter* center);
void wake_up_truck(Truck* truck);

void* thread_client(void* data);
void* thread_tri_center(void* data);
void* thread_tri_point(void* data);

TriPoint* findClosestTriPoint(int x, int y);

void add_tri_point_to_truck(TriPoint* point, Truck* truck);
int compute_distance_squared_Truck_TriPoint(Truck* truck, TriPoint* point);
int compute_distance_squared(int x1, int y1, int x2, int y2);

int find_closest_truck_from_tri_point(TriCenter* center, TriPoint* point);
void check_full_tri_point(int sig) ;
void signal_tri_point_full(TriCenter* center, TriPoint* point);
void* thread_truck(void* data);

#endif
