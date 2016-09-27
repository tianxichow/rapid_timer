
#include <stdio.h>
#include <unistd.h>

#include "../src/rapid_timer.h"

int timer_action(const void* data, timer_id id) {
    (*(int*)data)++;
    return 0;
}

int stop_test(int scheme_id, int accuracy, int activation) {
    
    int ret = 0;
    struct timeval now;
    struct timeval interval;
    timer_id* id; 
    int tick_timers = 0;

    gettimeofday(&now, NULL);

    rapid_timer* rt = rapid_timer_init(scheme_id, accuracy, NULL, 0, PROCESS_PERSIST, &now);

    if (NULL == rt) {
        printf("rapid_timer_init failed\n");
        return -1;
    }

    id = (timer_id*)malloc(sizeof(timer_id) * activation);

    if (NULL == id) {
        printf("malloc timer id failed\n");
        return -1;
    }

    gettimeofday(&now, NULL);

    int i;
    
    for (i = 0; i < activation; ++i) {

        interval.tv_sec = i;
        interval.tv_usec = 0;

        ret = rapid_timer_start(rt, &now, &interval, ONESHOT, timer_action, &tick_timers, id + i);

        if (0 != ret) {
            return 1;
        }       

    }

    for (i = 0; i < activation; ++i) {

        if (i % 2 == 0) {
            continue;
        }

        ret = repid_timer_stop(rt, id[i]);

        if (0 != ret) {
            return 1;
        }       
    }    

    for (i = 0; i < activation + 2; ++i) {
        sleep(1);
        gettimeofday(&now, NULL);
        ret = repid_timer_tick(rt, &now);

        if (0 != ret) {
            printf("repid_timer_tick failed\n");
            return 1;
        }
    }

    if (tick_timers != (activation % 2 == 0 ? activation/2 : (activation + 1) /2 )) {
        return -1;
    }

    return 0;
}

int main() {

    int ret = 0;

    printf("stop test begin:\n");

    ret = stop_test(UNSORTED_LIST, 1, 5);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "unsorted_list", 1, ret == 0? "ok":"failed");

    ret = stop_test(UNSORTED_LIST, 10 * 1000000, 10);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "unsorted_list", 10 * 1000000, ret == 0? "ok":"failed");

    ret = stop_test(SORTED_LIST, 1, 5);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "sorted_list", 1, ret == 0? "ok":"failed");

    ret = stop_test(SORTED_LIST, 10 * 1000000, 10);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "sorted_list", 10 * 1000000, ret == 0? "ok":"failed");

    ret = stop_test(RB_TREE, 1, 5);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "rb_tree", 1, ret == 0? "ok":"failed");

    ret = stop_test(RB_TREE, 10 * 1000000, 10);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "rb_tree", 10 * 1000000, ret == 0? "ok":"failed");

    ret = stop_test(WHEEL_UNSORTED_LIST, 1, 5);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "wheel_unsorted_list", 1, ret == 0? "ok":"failed");

    ret = stop_test(WHEEL_UNSORTED_LIST, 10 * 1000000, 10);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "wheel_unsorted_list", 10 * 1000000, ret == 0? "ok":"failed");

    ret = stop_test(WHEEL_SORTED_LIST, 1, 5);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "wheel_sorted_list", 1, ret == 0? "ok":"failed");

    ret = stop_test(WHEEL_SORTED_LIST, 10 * 1000000, 10);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "wheel_sorted_list", 10 * 1000000, ret == 0? "ok":"failed");

    ret = stop_test(HIERARCHICAL_WHEEL, 1, 5);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "hierarchical_wheel", 1, ret == 0? "ok":"failed");

    ret = stop_test(HIERARCHICAL_WHEEL, 10 * 1000000, 10);
    printf("stop test: %s accuracy %d microsecond %s\n", 
            "hierarchical_wheel", 10 * 1000000, ret == 0? "ok":"failed");    



    return 0;
}
