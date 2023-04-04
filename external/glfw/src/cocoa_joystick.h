// Copyright (c) 2006-2023. Minh Nguyen
// All rights reserved.

#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>

#define _GLFW_PLATFORM_JOYSTICK_STATE         _GLFWjoystickNS ns
#define _GLFW_PLATFORM_LIBRARY_JOYSTICK_STATE struct { int dummyJoystick; }

#define _GLFW_PLATFORM_MAPPING_NAME "Mac OS X"
#define GLFW_BUILD_COCOA_MAPPINGS

// Cocoa-specific per-joystick data
//
typedef struct _GLFWjoystickNS
{
    IOHIDDeviceRef      device;
    CFMutableArrayRef   axes;
    CFMutableArrayRef   buttons;
    CFMutableArrayRef   hats;
} _GLFWjoystickNS;


void _glfwInitJoysticksNS(void);
void _glfwTerminateJoysticksNS(void);

