#ifndef CHLOCK_H
#define CHLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

static struct timeval *tm_last = NULL;

/**
 * @description: print time in a default mode
 * @param -
 * @return -
 */
void ChlockPrintTimeDefault() {
    struct timeval tm_now;

    gettimeofday(&tm_now, NULL);

    struct tm *p_local_tm;
    p_local_tm = localtime(&tm_now.tv_sec);
    printf("Time: %04d-%02d-%02d %02d:%02d:%02d.%06ld\n", p_local_tm->tm_year + 1900, p_local_tm->tm_mon + 1,
           p_local_tm->tm_mday, p_local_tm->tm_hour, p_local_tm->tm_min, p_local_tm->tm_sec, tm_now.tv_usec);

    return;
}

void ChlockPrintTime(const char *info, const int mode) {
    struct timeval tm_now;

    gettimeofday(&tm_now, NULL);

    struct tm *p_local_tm;
    p_local_tm = localtime(&tm_now.tv_sec);
    printf("%s", info);
    // print interval
    printf("  :  ");
    if (mode == 0) {
        // second mode
        printf("%02d:%02d\n", p_local_tm->tm_min, p_local_tm->tm_sec);
    } else if (mode == 1) {
        // milisecond mode
        printf("%02d:%02d.%06ld\n", p_local_tm->tm_min, p_local_tm->tm_sec, tm_now.tv_usec);
    }
}

void ChlockSetInterval() {
    if (tm_last != NULL) {
        printf("Last tag is covered\n");
        free(tm_last);
    }
    tm_last = (struct timeval *)malloc(sizeof(struct timeval));
    gettimeofday(tm_last, NULL);
    printf("New tag is set\n");
    return;
}

int ChlockTimevalSub(struct timeval *result, struct timeval *x, struct timeval *y) {
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

void ChlockGetInterval(int mode) {
    if (tm_last == NULL) {
        printf("Time since last tag does not exists\n");
        return;
    }
    struct timeval tm_now, interval;
    gettimeofday(&tm_now, NULL);
    ChlockTimevalSub(&interval, &tm_now, tm_last);
    struct tm *p_local_tm;
    p_local_tm = localtime(&interval.tv_sec);
    printf("Time since last tag: ");
    // print interval
    printf("  :  ");
    if (mode == 0) {
        // second mode
        printf("%02d:%02d\n", p_local_tm->tm_min, p_local_tm->tm_sec);
    } else if (mode == 1) {
        // milisecond mode
        printf("%02d:%02d.%06ld\n", p_local_tm->tm_min, p_local_tm->tm_sec, tm_now.tv_usec);
    }
    gettimeofday(tm_last, NULL);
}

void ChlockFreeInterval() {
    printf("Time tag released\n");
    free(tm_last);
    return;
}

#endif