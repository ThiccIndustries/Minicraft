//
// Created by trevorskupien on 4/27/21.
//

#include "minicraft.h"

bool k_keys[GLFW_KEY_LAST + 1];
bool m_keys[GLFW_KEY_LAST + 1]; //GLFW_MOUSE_BUTTON_LAST = 7, plenty of mice have more than that

int k_actions[GLFW_KEY_LAST + 1];
int m_actions[GLFW_KEY_LAST + 1];

double m_posx = 0;
double m_posy = 0;

void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    k_keys[key]     = action != GLFW_RELEASE;
    k_actions[key]  = action;
}

void input_mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    m_keys[button]      = action != GLFW_RELEASE;
    m_actions[button]   = action;
}

void input_moues_position_callback(GLFWwindow* window, double xpos, double ypos){
    m_posx = xpos;
    m_posy = ypos;
}

void input_register_callbacks(GLFWwindow* window){
    glfwSetKeyCallback(window, input_key_callback);
    glfwSetMouseButtonCallback(window, input_mouse_button_callback);
    glfwSetCursorPosCallback(window, input_moues_position_callback);
}

bool input_get_key(int keycode)         { return k_keys[keycode]; }
bool input_get_button(int keycode)      { return m_keys[keycode]; }
bool input_get_key_down(int keycode)    { return k_actions[keycode] == GLFW_PRESS; }
bool input_get_mouse_down(int keycode)  { return m_actions[keycode] == GLFW_PRESS; }

void input_poll_input(){
    //Reset all actions
    for(int i = 0; i <= GLFW_KEY_LAST; i++){
        m_actions[i] = -1;
        k_actions[i] = -1;
    }

    glfwPollEvents();
}