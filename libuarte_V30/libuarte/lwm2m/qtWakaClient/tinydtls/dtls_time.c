/*******************************************************************************
 *
 * Copyright (c) 2011, 2012, 2013, 2014, 2015 Olaf Bergmann (TZI) and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v. 1.0 which accompanies this distribution.
 *
 * The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Olaf Bergmann  - initial API and implementation
 *
 *******************************************************************************/

/**
 * @file dtls_time.c
 * @brief Clock Handling
 */

#include "tinydtls.h"
#include "dtls_time.h"

#ifdef _MSC_VER
#include <time.h>
#include <windows.h>
#endif

#ifdef WITH_CONTIKI
clock_time_t dtls_clock_offset;

void
dtls_clock_init(void) {
  clock_init();
  dtls_clock_offset = clock_time();
}

void
dtls_ticks(dtls_tick_t *t) {
  *t = clock_time();
}

#elif defined(RIOT_VERSION)

dtls_tick_t dtls_clock_offset;

void
dtls_clock_init(void) {
  dtls_clock_offset = ztimer_now(ZTIMER_MSEC);
}

void
dtls_ticks(dtls_tick_t *t) {
  *t = ztimer_now(ZTIMER_MSEC) - dtls_clock_offset;
}

#elif defined(WITH_ZEPHYR)

void
dtls_clock_init(void) {
}

void
dtls_ticks(dtls_tick_t *t) {
  *t = k_uptime_get();
}

#elif defined(WITH_LWIP)

void
dtls_clock_init(void) {
}

void
dtls_ticks(dtls_tick_t *t) {
  *t = sys_now();
}

#elif defined(WITH_POSIX) || defined(IS_WINDOWS)

time_t dtls_clock_offset;

void
dtls_clock_init(void) {
#ifdef HAVE_TIME_H
  dtls_clock_offset = time(NULL);
#else
#  ifdef __GNUC__
  /* Issue a warning when using gcc. Other prepropressors do 
   *  not seem to have a similar feature. */ 
#   warning "cannot initialize clock"
#  endif
  dtls_clock_offset = 0;
#endif
}



void dtls_ticks(dtls_tick_t *t) {
#ifdef HAVE_SYS_TIME_H
  struct timeval tv;
  gettimeofday(&tv, NULL);
  *t = (tv.tv_sec - dtls_clock_offset) * DTLS_TICKS_PER_SECOND 
    + (tv.tv_usec * DTLS_TICKS_PER_SECOND / 1000000);

#elif defined(_MSC_VER)

  SYSTEMTIME current_time;
  GetSystemTime(&current_time);
  *t = (current_time.wSecond - dtls_clock_offset) * DTLS_TICKS_PER_SECOND 
    + (current_time.wMilliseconds * DTLS_TICKS_PER_SECOND / 1000);

#else
*t = viPlattformTicks();

#endif
}

#endif /* ! CONTIKI && ! RIOT_VERSION && ! WITH_ZEPHYR && ! WITH_LWIP && ! WITH_POSIX */
