#ifndef DAVEGROHL_INCLUDE_DGSERVER_H_
#define DAVEGROHL_INCLUDE_DGSERVER_H_


#import <iostream>

// #import "DGController.h"
#import "DGTypes.h"
// #import "DGCracker.h"

static in_port_t kPortNumber;
static const int kAcceptQueueSizeHint = 8;

int parseTraffic(DGPacket *packet, int fd);
int server(int port);
static void AcceptClientFromSocket(int listenFd);


#endif // DAVEGROHL_INCLUDE_DGSERVER_H_