
#import "DGCloud.h"



int SocketConnectedToAddress(NSData *data){
    struct sockaddr_storage server_addr;
    [data getBytes:&server_addr length:data.length];
    
    // Get a socket.
    int sockfd = socket(server_addr.ss_family, SOCK_STREAM, 0);
    
    if (sockfd == -1) {
        #ifdef DEBUG
        perror("socket");
        #endif
        return sockfd;
    }
    
    int err = connect(sockfd, (struct sockaddr *)&server_addr, server_addr.ss_len);
    
    if (err == -1) {
        #ifdef DEBUG
        printf("-- connect() error\n");
        #endif
        close(sockfd);
        sockfd = -1;
    }
    
    return sockfd;
}


// *************************
//         DGCloud
// *************************

@implementation DGCloud

- (id)init{
    self = [super init];
    services = [[NSMutableArray alloc] init];
    serviceBrowser = [[NSNetServiceBrowser alloc] init];
    searchLoop = [NSRunLoop currentRunLoop];

    return self;
}

- (int)searchForServers{
    fflush(stdout); // Print any pending messages to stdout
    shouldKeepSearching = YES;
    
    [serviceBrowser setDelegate:self];
    [serviceBrowser searchForServicesOfType:@"_davegrohl._tcp" inDomain:@""];
    [serviceBrowser scheduleInRunLoop:searchLoop forMode:NSDefaultRunLoopMode];
    
    while (shouldKeepSearching && [searchLoop runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]]);
    
    [serviceBrowser removeFromRunLoop:searchLoop forMode:NSDefaultRunLoopMode];
    
    [self resolveServers];
    
    
    return [services count];
}

- (void)resolveServers{
    shouldKeepResolving = YES;
    resolveCount = 0;
    
#ifdef DEBUG
    printf("Resolving %ld services...\n", [services count]);
#endif
    
    resolveLoop = [NSRunLoop currentRunLoop];
    
    for (int i = 0; i < [services count]; i++){
        [[services objectAtIndex:i] setDelegate:[DGCloud self]];
        [[services objectAtIndex:i] resolveWithTimeout:5.0];
        [[services objectAtIndex:i] scheduleInRunLoop:resolveLoop forMode:NSDefaultRunLoopMode];
    }
    
    printf("For loop ended....\n");
    
    while (shouldKeepResolving){
        [resolveLoop runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
     }
    
    printf("Stopped resolving...\n");
}

- (void)connect{
    int fd = -1;
    int numServers = [self serverCount];
    
    printf("-- Connecting to the cloud...\n");
    
    for (int i = 0; i < numServers; i++) {
        fd = SocketConnectedToAddress([[[[self services] objectAtIndex:i] addresses] objectAtIndex:0]);
        fileDescriptors.push_back(fd);
    }
    
}

- (int)serverCount{
    return [services count];
}

- (int)fdForServerNumber:(int)serverIndex{
    
    if (serverIndex < fileDescriptors.size()) {
        return fileDescriptors[serverIndex];
    } else {
        return -1;
    }
    
}

- (const char *)hostnameForServerNumber:(int)serverIndex{
    
    return [[[[self services] objectAtIndex:serverIndex] hostName] UTF8String];
}


// *************************
//   Delegates for Bonjour
// *************************


- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)browser
{
#ifdef DEBUG
    printf("netServiceBrowserWillSearch called\n");
#endif
}

- (void)netServiceBrowserDidStopSearch:(NSNetServiceBrowser *)browser
{
#ifdef DEBUG
    printf("netServiceBrowserDidStopSearch called\n");
#endif
    
    shouldKeepSearching = NO;
    
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
             didNotSearch:(NSDictionary *)errorDict
{
    printf("netServiceBrowser:didNotSearch: called\n");
}
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
           didFindService:(NSNetService *)aNetService
               moreComing:(BOOL)moreComing
{
#ifdef DEBUG
    printf("netServiceBrowser:didFindService:moreComing: called\n\t%s\n", [[aNetService description] UTF8String]);
#endif
    
    [services addObject:aNetService];
    
    if (!moreComing){
        [browser stop];
    }
}
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
         didRemoveService:(NSNetService *)aNetService
               moreComing:(BOOL)moreComing
{
    printf("netServiceBrowser:didRemoveService:moreComing: called\n");
    [services removeObject:aNetService];
}

- (void)netServiceDidResolveAddress:(NSNetService *)netService
{
    resolveCount++;
#ifdef DEBUG
    printf("Resolved %d of %ld -- %s\n", resolveCount, [services count], [[[netService addresses] description] UTF8String]);
#endif
    [netService stop];
    
    if (resolveCount == [services count]){
        shouldKeepResolving = NO;
    }
}

// Sent if resolution fails
- (void)netService:(NSNetService *)netService
     didNotResolve:(NSDictionary *)errorDict
{
    printf("Didn't resolve\n");
}


@synthesize services;
@synthesize serviceBrowser;

@end
