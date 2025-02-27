
#include <unistd.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    printf("XXXXX\r\n");
  return len;
}

 char *buffer = "                                                      ";


size_t getline (char **string, size_t *n, void *stream) {
  *string = buffer;
   printf("XXXX3X\r\n");
  return 0;
}
// NO_FAKE_UDS
// handle_command(lwm2mH, commands, line);
//                         result = connection_handle_packet(connP, buffer, numBytes);

