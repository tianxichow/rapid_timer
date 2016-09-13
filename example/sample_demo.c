
#include <stdio.h>
#include <unistd.h>

#include "../src/rapid_timer.h"

int timer_action(const void* data) {
    printf("ding\n");
    return 0;
}

int main() {

    rapid_timer* rt = rapid_timer_init(WHEEL_UNSORTED_LIST, 0, NULL, 0, PROCESS_PERSIST);

    if (NULL == rt) {
        printf("rapid_timer_init failed\n");
        return -1;
    }

    printf("rapid_timer_init success\n");

    struct timeval now;
    struct timeval now1;
    struct timeval interval;
    timer_id id;

    gettimeofday(&now, NULL);
    int k = 1;
    while (k++ < 10000);
    gettimeofday(&now1, NULL);

    printf("%lu %u\n", now.tv_sec, now.tv_usec);
    printf("%lu %u\n", now1.tv_sec, now1.tv_usec);

    interval.tv_sec = 2;
    interval.tv_usec = 0;

    int ret = rapid_timer_start(rt, &now, &interval, REPEATE, timer_action, NULL, &id);

    if (0 != ret) {
        return 1;
    }

    printf("get timer id=%d\n", id);

    int i;

    for (i = 0; i < 10; ++i) {
        sleep(1);
        printf("tick %d\n", i);
        gettimeofday(&now, NULL);
        ret = repid_timer_tick(rt, &now);

        if (0 != ret) {
            printf("repid_timer_tick failed\n");
            return 1;
        }
    }

    

    return 0;
}
