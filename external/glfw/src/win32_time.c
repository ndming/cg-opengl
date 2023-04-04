// Copyright (c) 2006-2023. Minh Nguyen
// All rights reserved.

#include "internal.h"


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

// Initialise timer
//
void _glfwInitTimerWin32(void)
{
    QueryPerformanceFrequency((LARGE_INTEGER*) &_glfw.timer.win32.frequency);
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

uint64_t _glfwPlatformGetTimerValue(void)
{
    uint64_t value;
    QueryPerformanceCounter((LARGE_INTEGER*) &value);
    return value;
}

uint64_t _glfwPlatformGetTimerFrequency(void)
{
    return _glfw.timer.win32.frequency;
}

