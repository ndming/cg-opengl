// Copyright (c) 2016-2023. Minh Nguyen
// All rights reserved.

#include "internal.h"


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int _glfwPlatformPollJoystick(_GLFWjoystick* js, int mode)
{
    return GLFW_FALSE;
}

void _glfwPlatformUpdateGamepadGUID(char* guid)
{
}

