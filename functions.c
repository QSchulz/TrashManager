#include "data_structure.h"

Client create_client(int x, int y);

TrashBag create_trash_bag(double volume, TrashType type);
TrashBin create_trash_bin(double volume, double volume_max_trash_bag, TrashType type);

TriPoint create_tri_point(int x, int y);
TriCenter create_tri_center(int nbTrucks, int period, int x, int y, TriPoint* triPoints, int nbTriPoint);
Truck create_truck(double volume, TriCenter* center);

void put_trash_bag(Client* client);
void send_truck(TriCenter* center);

void* thread_client(void* data);
void* thread_tri_center(void* data);
