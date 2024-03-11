#pragma once

#ifndef QXR_API
#define QXR_API

#include "pch.h"
#include "qxr_xr_interface.h"
#include "qxr_eye_tracker.h"
#include "qxr_face_tracker.h"
#include <stdio.h>

enum qxrResult {
	SUCCESS,
	RUNTIME_MISSING,
	RUNTIME_FAILURE,
	RUNTIME_FEATURE_UNAVAILABLE,
	INSTANCE_CREATE_FAILURE,
	SESSION_CREATE_FAILURE,
	GRAPHICS_BIND_FAILURE,
	SYSTEM_GET_FAILURE,
	PROPERTY_GET_FAILURE,
	SPACE_CREATE_FAILURE,
	STEREO_VIEW_UNSUPPORTED
};

struct FaceWeightsFB {
	float weights[XR_FACE_EXPRESSION2_COUNT_FB];
	float confidences[XR_FACE_CONFIDENCE2_COUNT_FB];
	XrTime time;
};

struct EyeGazesFB {
	XrPosef gaze[2];
	XrTime time;
};

extern "C" {
	__declspec(dllexport) qxrResult InitializeSession();
	__declspec(dllexport) bool CloseSession();

	__declspec(dllexport) bool CreateFaceTracker();
	__declspec(dllexport) bool DestroyFaceTracker();
	__declspec(dllexport) bool GetFaceData(FaceWeightsFB *expressions);

	__declspec(dllexport) bool CreateEyeTracker();
	__declspec(dllexport) bool DestroyEyeTracker();
	__declspec(dllexport) bool GetEyeData(EyeGazesFB *gazes); 
}

#endif
