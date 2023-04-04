// Copyright (c) 2006-2023. Minh Nguyen
// All rights reserved.

#define _GLFW_PLATFORM_LIBRARY_TIMER_STATE _GLFWtimerPOSIX posix

#include <stdint.h>


// POSIX-specific global timer data
//
typedef struct _GLFWtimerPOSIX
{
    GLFWbool    monotonic;
    uint64_t    frequency;
} _GLFWtimerPOSIX;


void _glfwInitTimerPOSIX(void);

