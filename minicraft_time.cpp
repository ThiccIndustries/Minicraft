/*
 * Created by MajesticWaffle on 4/29/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

Time* g_time = new Time;


void (* tick_callback)();
double time_since_tick = 0;

int time_timer_purge();

void time_update_time(double glfw_time){
    g_time -> delta = glfw_time - g_time -> global;
    g_time -> global = glfw_time;

    if(time_since_tick >= 1.0 / TIME_TPS) {
        g_time->tick++;
        tick_callback();

        time_since_tick = 0;
    }
    else
        time_since_tick += g_time -> delta;
}

int time_get_framerate(){
   return (int)(1.0 / g_time -> delta);
}

Timer* time_timer_start(long duration){

    Timer* t = new Timer;

    t -> duration = duration;
    t -> starting_tick = g_time -> tick;


    return t;
}

bool time_timer_finished(Timer*& t){

    if(t == nullptr)
        return false;

    //timer is finished
    if(g_time -> tick >= (t -> starting_tick + t -> duration)){
        time_timer_cancel(t);
        return true;
    }else
        return false;

}

void time_set_tick_callback(void (*callback_function)()){
    tick_callback = callback_function;
}


void time_timer_cancel(Timer*& t){
    if(t == nullptr)
        return;

    std::cout << "Timer #" << t << " ended." << std::endl;
    delete t;
    t = nullptr;

}