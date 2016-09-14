
#include <stdio.h>
#include <unistd.h>

#include "../src/rapid_timer.h"

int timer_action(const void* data) {
    printf("ding\n");
    return 0;
}

int main() {

    int ret = 0;

    rapid_timer* rt = rapid_timer_init(WHEEL_SORTED_LIST, 1, NULL, 0, PROCESS_PERSIST);

    if (NULL == rt) {
        printf("rapid_timer_init failed\n");
        return -1;
    }

    printf("rapid_timer_init success\n");

    struct timeval now;
    struct timeval interval;
    timer_id id;

    gettimeofday(&now, NULL);

    interval.tv_sec = 2;
    interval.tv_usec = 0;

    ret = rapid_timer_start(rt, &now, &interval, REPEATE, timer_action, NULL, &id);

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
