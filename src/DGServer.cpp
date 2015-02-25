#import <arpa/inet.h>
#import <errno.h>
#import <netinet/in.h>
#import <netinet6/in6.h>
#import <stdbool.h>
#import <stdlib.h>
#import <string.h>
#import <sys/socket.h>
#import <sys/types.h>
#import <unistd.h>
#import <thread>
#import <Foundation/Foundation.h>

#import "DGController.h"
#import "DGServer.h"

bool connectionOpen;

@interface NetServiceDelegate : NSObject <NSNetServiceDelegate>
@end

@implementation NetServiceDelegate

- (void)netServiceWillPublish:(NSNetService *)netService{
    #ifdef DEBUG
    printf("-- netServiceWillPublish: called\n");
    #endif
}
- (void) netService: (NSNetService *) sender
didNotPublish: (NSDictionary *) errorDict{
    printf("-- netService:didNotPublish: called\n");
    exit(0);
}

@end // NetServiceDelegate



int parseTraffic(DGPacket *packet, int fd){
    crack_t params;
    hashData_t theHashData;
    DGPacket response;
    
    if (0 == strncmp(packet->message, msgHELLO, strlen(msgHELLO))) {
        #ifdef DEBUG
        printf("-- Received HELLO packet\n");
        #endif
        response.threads = [[NSProcessInfo processInfo] activeProcessorCount];
        write(fd, &response, sizeof(response));
    } else if (0 == strncmp(packet->message, msgCRACK, strlen(msgCRACK))) {
        #ifdef DEBUG
        printf("-- Received CRACK packet\n");
        #endif
        theHashData = packet->hashData;
        params = packet->crackData;
        params.distributed = false;
        
        
        std::thread masterWatcher(watchMaster, fd);
        
        DGController * theController = [[DGController alloc] init];
        [theController startCracking:&theHashData withOptions:&params];
        
        masterWatcher.join();
    }
    
    
    return 1;
}


int server(int port){
    connectionOpen = NO;
    int exitCode = EXIT_FAILURE;
    const bool useIPv6 = false;
    kPortNumber = port;
    
    #ifdef DEBUG
    printf("-- Packet size: %lu\n", sizeof(DGPacket));
    #endif
    
    // Get socket
    int fd;
    if (useIPv6) fd = socket(AF_INET6, SOCK_STREAM, 0);
    else fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (fd == -1) { 
        perror("*** socket");
        // goto cleanup;
        return EXIT_FAILURE;
    }
    
    // Reuse the address so stale sockets won't kill us
    int yes = 1;
    int result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (result == -1) {
        perror("*** setsockopt(SO_REUSEADDR)");
        // goto cleanup;
        return EXIT_FAILURE;
    }
    
    // Bind to an address and port
    union {
        struct sockaddr sa;
        struct sockaddr_in in;
        struct sockaddr_in6 in6;
    } address;
    
    if (useIPv6) {
        address.in6.sin6_len = sizeof(address.in6);
        address.in6.sin6_family = AF_INET6;
        address.in6.sin6_port = htons(kPortNumber);
        address.in6.sin6_flowinfo = 0;
        address.in6.sin6_addr = in6addr_any;
        address.in6.sin6_scope_id = 0;
    } else {
        address.in.sin_len = sizeof(address.in);
        address.in.sin_family = AF_INET;
        address.in.sin_port = htons(kPortNumber);
        address.in.sin_addr.s_addr = htonl(INADDR_ANY);
        memset(address.in.sin_zero, 0, sizeof(address.in.sin_zero));
    }
    
    result = bind(fd, &address.sa, address.sa.sa_len);
    
    if (result == -1) {
        perror("*** bind");
        return EXIT_FAILURE;
    }
    
     
    result = listen(fd, kAcceptQueueSizeHint);
    if (result == -1) {
        perror("*** listen");
        return EXIT_FAILURE;
    }
    
    // Publish on bonjour
    NSNetService *netService = [[NSNetService alloc] initWithDomain:@""
                                                               type:@"_davegrohl._tcp."
                                                               name:@""
                                                               port:kPortNumber];
    
    NetServiceDelegate *delegate = [[NetServiceDelegate alloc] init];
    netService.delegate = delegate;
    [netService publish];
    
    printf("-- Listening on port %d\n", (int)kPortNumber);
    
    while (1) {
        AcceptClientFromSocket(fd);
    }
    
    exitCode = EXIT_SUCCESS;
    
    close(fd);
    
    printf("Server() returned\n");
    return 0;
}



static void AcceptClientFromSocket(int listenFd) {
    
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    
    // Accept and get the remote address
    int clientFd = accept(listenFd, (struct sockaddr *)&addr, &addr_len);
    if (clientFd == -1) {
        perror("*** accept");
        return;
    }
    
    // Get the port and a pointer to the network address
    const void *net_addr = NULL;
    in_port_t port = 0;
    
    if (addr.ss_family == AF_INET){
        struct sockaddr_in *addr_in = (struct sockaddr_in *)&addr;
        net_addr = &addr_in->sin_addr;
        port = addr_in->sin_port;
    } else {
        struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)&addr;
        net_addr = &addr_in6->sin6_addr;
        port = addr_in6->sin6_port;
    }
    
    // Convert address to human-readable
    DGPacket packet;
    char buffer[4096];
    const char *name = inet_ntop(addr.ss_family, net_addr, buffer, sizeof(buffer));
    printf("-- %s:%d connected\n", name ? name : "(unknown)", ntohs(port));
    
    // Drain the socket
    while (1) {
        ssize_t read_count = read(clientFd, &packet, sizeof(packet));
        
        if (0 == read_count) {
            break; // EOF
        } else if (read_count == -1){
            perror("*** read");
            break; // Error
        } else {
            parseTraffic(&packet, clientFd);
        }
    }
    
    close(clientFd);
    puts("\n-- Connection closed");
    
}


