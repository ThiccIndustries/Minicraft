/*
 * Created by MajesticWaffle on 4/29/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

Time* mTime = new Time;

void time_update_time(double glfw_time){
    mTime -> delta = glfw_time - mTime -> global;
    mTime -> global = glfw_time;
}

int time_get_framerate(){
   return (int)(1.0 / mTime -> delta);
}