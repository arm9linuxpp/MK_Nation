
#ifndef VI_FAKE_SOCKET
#define VI_FAKE_SOCKET 1


#define AF_INET 4

#define AF_INET6 6

#define  socklen_t int

typedef struct sockaddr_in6 {
    int sin6_addr;
    int sin6_port;
    int sin6_family;
    int st;
};


typedef struct sockaddr {
	int sa_family;
        int sin_port;
        char sa_data[10];
};
#endif

/*



typedef struct sockaddr_storage {
	int a;
	
};

*/
