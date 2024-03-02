#pragma once
#ifndef QXR_EYE_TRACKER_H
#define QXR_EYE_TRACKER_H

#include <openxr/openxr.h>
#include <openxr/fb_eye_tracking_social.h>

extern XrEyeGazesFB gazes;

XrResult qxrCreateEyeTracker(XrInstance instance, XrSystemId systemId, XrSession session, XrSpace space);
XrResult qxrUpdateEyeTracker(XrEyeGazesFB *data);
bool qxrDestroyEyeTracker(XrInstance instance);

#endif
