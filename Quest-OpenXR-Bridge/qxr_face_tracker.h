#pragma once
#ifndef Q_FACE_TRACKER_H
#define Q_FACE_TRACKER_H

#include <openxr/openxr.h>
#include <openxr/fb_face_tracking2.h>

XrResult qxrCreateFaceTracker(XrInstance instance, XrSystemId systemId, XrSession session);
XrResult qxrUpdateFaceTracker(XrFaceExpressionWeights2FB *expressions);
bool qxrDestroyFaceTracker(XrInstance instance); 

#endif
