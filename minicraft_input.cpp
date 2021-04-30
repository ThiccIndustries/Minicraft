/*
 * Created by MajesticWaffle on 4/27/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

bool g_k_keys[GLFW_KEY_LAST + 1];
bool g_m_keys[GLFW_KEY_LAST + 1]; //GLFW_MOUSE_BUTTON_LAST = 7, plenty of mice have more than that

int g_k_actions[GLFW_KEY_LAST + 1];
int g_m_actions[GLFW_KEY_LAST + 1];

Coord2d g_m_pos{0, 0};

void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    g_k_keys[key]     = action != GLFW_RELEASE;
    g_k_actions[key]  = action;
}

void input_mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    g_m_keys[button]      = action != GLFW_RELEASE;
    g_m_actions[button]   = action;
}

void input_mouse_position_callback(GLFWwindow* window, double xpos, double ypos){
    g_m_pos.x = xpos;
    g_m_pos.y = ypos;
}

void input_register_callbacks(GLFWwindow* window){
    glfwSetKeyCallback(window, input_key_callback);
    glfwSetMouseButtonCallback(window, input_mouse_button_callback);
    glfwSetCursorPosCallback(window, input_mouse_position_callback);
}

bool input_get_key(int keycode)         { return g_k_keys[keycode]; }
bool input_get_button(int keycode)      { return g_m_keys[keycode]; }
bool input_get_key_down(int keycode)    { return g_k_actions[keycode] == GLFW_PRESS; }
bool input_get_mouse_down(int keycode)  { return g_m_actions[keycode] == GLFW_PRESS; }

void input_poll_input(){
    //Reset all actions
    for(int i = 0; i <= GLFW_KEY_LAST; i++){
        g_m_actions[i] = -1;
        g_k_actions[i] = -1;
    }

    glfwPollEvents();
}