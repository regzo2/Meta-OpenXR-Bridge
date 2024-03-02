#pragma once

#ifndef QXR_XR_INTERFACE
#define QXR_XR_INTERFACE

#include <openxr/openxr.h>

extern XrInstance instance;
extern XrSystemId systemId;
extern XrViewConfigurationType viewConfiguration;
extern XrSession session;
extern XrSpace worldSpace;

int CreateXrSession();

#endif
