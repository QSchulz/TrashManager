#ifndef FONCTION_H
#define FONCTION_H

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "data_structure.h"

Client* create_client(int x, int y);
TrashBin* create_trash_bin(double volume, double trash_bag_size, TrashType type, Mode mode);
TriPoint* create_tri_point(int x, int y);
TriCenter* create_tri_center(int period, int x, int y, TriPoint** triPoints, int nbTriPoint);
TrashBag* generate_trash(Client* client);
void pick_up_trash(TriCenter* center);
void put_trash_bag(Client* client);
void signal_center(int sig);
void empty_tri_point(TriPoint* point, TriCenter* center);
void* thread_client(void* data);
void* thread_tri_point(void* data);
void* thread_tri_center(void* data);

void assign_tri_point(Client* client, TriPoint** points, int nbPoint);

pthread_mutex_t* tri_point_full_mutex;
TriPoint* tri_point_full;
extern TriCenter* center;
extern pthread_t** threads;
extern int nbThreads;

#endif
