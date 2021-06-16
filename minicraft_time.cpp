/*
 * Created by MajesticWaffle on 4/29/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

typedef struct ITimer{
    double duration;
    double start_stamp;
} ITimer;

Time* g_time = new Time;
ITimer* timers[TIME_MAX_TIMERS];

int time_timer_purge();

void time_update_time(double glfw_time){
    g_time -> delta = glfw_time - g_time -> global;
    g_time -> global = glfw_time;
}

int time_get_framerate(){
   return (int)(1.0 / g_time -> delta);
}

Timer time_timer_start(double duration){

    for(int i = 0; i < TIME_MAX_TIMERS; ++i){
        if(timers[i] == nullptr){
            timers[i] = new ITimer{duration, g_time -> global};

            std::cout << "Timer #" << i << " started." << std::endl;
            return i;
        }
    }

    //no available timer, attempt to purge orphans and try again
    if(time_timer_purge() > 0)
        return time_timer_start(duration);

    //No orphans were removed, cannot start timer
    std::cout << "here" << std::endl;
    error("No avail timers. No orphans.");
    return -1;
}

bool time_timer_finished(Timer t){

    if(timers[t] == nullptr)
        return false;

    //timer is finished
    if(g_time -> global >= (timers[t] -> start_stamp + timers[t] -> duration)){
        time_timer_cancel(t);
        std::cout << "Timer #" << t << " finished." << std::endl;
        return true;
    }else
        return false;

}

//Purge any timer which has been been completed for over TIME_TIMER_ORPHAN_THRESHOLD seconds without having its status checked.
//This is a last resort that can cause all sorts of problems if that timer is somehow still needed.

int time_timer_purge(){
    int count = 0;

    for(uint i = 0; i < TIME_MAX_TIMERS; i++){
        if(g_time -> global - (timers[i] -> start_stamp + timers[i] -> duration) >= TIME_TIMER_ORPHAN_THRESHOLD){
            time_timer_cancel(i);
            count++;
        }
    }

    std::cout << "Purged: " << count << " orphaned timers." << std::endl;
    return count;
}

void time_timer_cancel(Timer t){
    std::cout << "Timer #" << t << " ended." << std::endl;
    delete timers[t];
    timers[t] = nullptr;
}