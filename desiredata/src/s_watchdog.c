/* Copyright (c) 1997-2000 Miller Puckette.
* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in this distribution.  */

/* This file is compiled into the separate program, "pd-watchdog," which
tries to prevent Pd from locking up the processor if it's at realtime
priority.  Linux only.  Invoked from s_inter.c. */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int happy = 1;
    while (1) {
        struct timeval timout;
        fd_set readset;
        if (happy) {
            timout.tv_sec = 5;
            timout.tv_usec = 0;
        } else {
            timout.tv_sec = 2;
            timout.tv_usec = 0;
        }
        FD_ZERO(&readset);
        FD_SET(0, &readset);
        select(1, &readset, 0, 0, &timout);
        if (FD_ISSET(0, &readset)) {
            char buf[100];
            happy = 1;
            if (read(0, &buf, 100) <= 0) return 0;
            continue;
        }
        happy = 0;
        kill(getppid(), SIGHUP);
        fprintf(stderr, "watchdog: signaling pd...\n");
    }
}
