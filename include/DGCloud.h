#ifndef DAVEGROHL_INCLUDE_DGCLOUD_H_
#define DAVEGROHL_INCLUDE_DGCLOUD_H_


#import <iostream>
#import <sys/socket.h>
#import <thread>
#import <vector>
#import <atomic>
#import <Foundation/Foundation.h>


int SocketConnectedToAddress(NSData *data);


@interface DGCloud : NSObject <NSNetServiceBrowserDelegate, NSNetServiceDelegate>
{
    NSRunLoop *searchLoop, *resolveLoop;
    std::atomic<bool> shouldKeepSearching;
    std::atomic<bool> shouldKeepResolving;
    int resolveCount;
    std::vector<int> fileDescriptors;
}

- (int)searchForServers;
- (void)resolveServers;
- (void)connect;
- (int)serverCount;
- (int)fdForServerNumber:(int)serverIndex;
- (const char *)hostnameForServerNumber:(int)serverIndex;


// Delegates for Bonjour
- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)browser;
- (void)netServiceBrowserDidStopSearch:(NSNetServiceBrowser *)browser;
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
             didNotSearch:(NSDictionary *)errorDict;
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
           didFindService:(NSNetService *)aNetService
               moreComing:(BOOL)moreComing;
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
         didRemoveService:(NSNetService *)aNetService
               moreComing:(BOOL)moreComing;

// Resolution methods
- (void)netServiceDidResolveAddress:(NSNetService *)netService;
- (void)netService:(NSNetService *)netService
     didNotResolve:(NSDictionary *)errorDict;


@property (readwrite, strong) NSMutableArray *services;
@property (readwrite, strong) NSNetServiceBrowser *serviceBrowser;

@end


#endif // DAVEGROHL_INCLUDE_DGCLOUD_H_