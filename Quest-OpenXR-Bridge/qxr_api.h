#pragma once

#ifndef QXR_API
#define QXR_API

#include "pch.h"
#include "qxr_xr_interface.h"
#include "qxr_eye_tracker.h"
#include "qxr_face_tracker.h"
#include <stdio.h>

struct FaceWeightsFB {
	float weights[XR_FACE_EXPRESSION2_COUNT_FB];
	float confidences[XR_FACE_CONFIDENCE2_COUNT_FB];
};

struct EyeGazesFB {
	XrPosef gaze[2];
};

extern "C" {
	__declspec(dllexport) int InitializeSession();
	__declspec(dllexport) int CloseSession();

	__declspec(dllexport) int CreateFaceTracker();
	__declspec(dllexport) int DestroyFaceTracker();
	__declspec(dllexport) int GetFaceData(FaceWeightsFB *expressions);

	__declspec(dllexport) int CreateEyeTracker();
	__declspec(dllexport) int DestroyEyeTracker();
	__declspec(dllexport) int GetEyeData(EyeGazesFB *gazes); 
}

#endif
