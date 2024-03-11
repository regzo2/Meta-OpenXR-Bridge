#pragma once

#ifndef QXR_XR_INTERFACE
#define QXR_XR_INTERFACE

#include <openxr/openxr.h>
#include "qxr_api.h"

extern XrInstance instance;
extern XrSystemId systemId;
extern XrViewConfigurationType viewConfiguration;
extern XrSession session;
extern XrSpace worldSpace;

qxrResult CreateXrSession();

#endif
