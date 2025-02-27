

#include <sys/socket.h>


typedef struct _ip_addr {
  union {
    int ip4;
  } u_addr;
  int type;
  int st;
  struct sockaddr_in6 sin6;
} ip_addr_t;


//extern const ip_addr_t ip_addr_any;
//extern const ip_addr_t ip_addr_broadcast;

/** IP_ADDR_ can be used as a fixed/const ip_addr_t
 *  for the IPv4 wildcard and the broadcast address
 */
#define IP_ADDR_ANY         (&ip_addr_any)
#define IP_ADDR_BROADCAST   (&ip_addr_broadcast)
/** IP4_ADDR_ can be used as a fixed/const ip4_addr_t
 *  for the wildcard and the broadcast address
 */
#define IP4_ADDR_ANY        (ip_2_ip4(&ip_addr_any))
#define IP4_ADDR_BROADCAST  (ip_2_ip4(&ip_addr_broadcast))
