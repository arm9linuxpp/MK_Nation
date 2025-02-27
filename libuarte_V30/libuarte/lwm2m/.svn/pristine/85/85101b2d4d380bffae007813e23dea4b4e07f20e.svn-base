
#ifndef VI_TS_H

#define VI_TS_H

#include <stdint.h>
#include <time.h>

const char *getTs(void);

int vi_GetPort(void);


void viFailureLog(char *);
void viDebugLog(char *msg);
int main_wakaama(void);
int upd_wakaama(int one,int two, int three, const char *data);


void viPlattformTick50ms(void);
time_t viPlattformTicks(void);


// LWM2M
void doNextStep(void);
void xxx_onUdsReceive(uint8_t *b, int len);


#endif
