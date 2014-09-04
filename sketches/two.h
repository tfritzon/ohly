#ifndef TWO_H
#define TWO_H

#define LISTSIZE 2048
#define LOOPS 10000000

#include <sys/time.h>
#define TSETUP struct timeval t;double t1,t0;struct timezone tz;tz.tz_minuteswest=0;tz.tz_dsttime=0
#define T0 t.tv_sec=0.0;gettimeofday(&t, &tz); t0=t.tv_sec+(t.tv_usec/1000000.0)
#define T1 t.tv_sec=0.0;gettimeofday(&t, &tz); t1=t.tv_sec+(t.tv_usec/1000000.0)
#define PT0 printf("%lf (%ld, %d)\n", t0, t.tv_sec, t.tv_usec)
#define PT1 printf("%lf (%ld, %d)\n", t1, t.tv_sec, t.tv_usec)
#define PDT gettimeofday(&t, &tz); printf("Time spent: %f\n", t1-t0)

#endif
