/*******************************************************************************
 *
 * Copyright (c) 2013, 2014, 2015 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *******************************************************************************/

#include <liblwm2m.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>
#include "vi_lwm2m.h"

void * lwm2m_malloc(size_t s)
{
    return malloc(s);
}

void lwm2m_free(void * p)
{
    free(p);
}

char * lwm2m_strdup(const char * str)
{
    if (!str) {
      return NULL;
    }

    const int len = strlen(str) + 1;
    char * const buf = lwm2m_malloc(len);

    if (buf) {
      memset(buf, 0, len);
      memcpy(buf, str, len - 1);
    }

    return buf;
}


int lwm2m_strncmp(const char * s1,
                     const char * s2,
                     size_t n)
{
    return strncmp(s1, s2, n);
}

int SGA_stricmp(const char *a, const char *b) {
    int ca, cb;
    do {
        ca = * (unsigned char *)a;
        cb = * (unsigned char *)b;
        ca = tolower(toupper(ca));
        cb = tolower(toupper(cb));
        a++;
        b++;
    } while (ca == cb && ca != '\0');
    return ca - cb;
}

int lwm2m_strcasecmp(const char * str1, const char * str2) {
    return SGA_stricmp(str1, str2);
}


static time_t gs_ViFakeTime = 0;

void viPlattformTick50ms(void)
{
  gs_ViFakeTime+=50;
}
time_t viPlattformTicks(void)
{
  return gs_ViFakeTime;
}

time_t lwm2m_gettime(void)
{
#ifndef FAKE_SEC_SINCE

  time_t ret = viPlattformTicks()/1000; // in sec.

#else
    // changes here are problematic because if one session is ok. reconnect only after 3 min!
    time_t ret = time(NULL);
//     ret-= 1;
//ret-= 5;   // OK after 5 min pause
//ret-= 10;  // OK after 5 min pause
//ret-= 20;  // OK after 5 min pause
//     ret-= 60;// OK after 5 min pause
//    ret-= 1738920182;


#endif

  char msg[255];
  sprintf(msg,"lwm2m_gettime %ld",ret);
  viFailureLog(msg);

    return ret;
}


int lwm2m_seed(void) {    
    return viPlattformTicks()/50;
}

void lwm2m_printf(const char * format, ...)
{
    char msg[255];
    va_list ap;
    va_start(ap, format);
    vsprintf( msg, format, ap);
    va_end(ap);
    viDebugLog(msg);
}
