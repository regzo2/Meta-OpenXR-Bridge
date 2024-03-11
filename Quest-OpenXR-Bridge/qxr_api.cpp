#include "pch.h"
#include "qxr_xr_interface.h"
#include "qxr_eye_tracker.h"
#include "qxr_face_tracker.h"
#include "qxr_api.h"

#include <stdio.h>

XrFaceExpressionWeights2FB xrExp = { XR_TYPE_FACE_EXPRESSION_WEIGHTS2_FB };
XrEyeGazesFB xrGazes = { XR_TYPE_EYE_GAZES_FB };

qxrResult InitializeSession() {
	return CreateXrSession();
}

bool CloseSession() {
	XrResult result = xrEndSession(session);
	return result;
}

bool CreateFaceTracker() {
	XrResult result = qxrCreateFaceTracker(instance, systemId, session);
	return result == XR_SUCCESS;
}

bool DestroyFaceTracker() {
	return qxrDestroyFaceTracker(instance) != XR_SUCCESS;
}

bool GetFaceData(FaceWeightsFB *expressions) {

	xrExp.weights = expressions->weights;
	xrExp.weightCount = XR_FACE_EXPRESSION2_COUNT_FB;
	xrExp.confidences = expressions->confidences;
	xrExp.confidenceCount = XR_FACE_CONFIDENCE2_COUNT_FB;
	xrExp.time = expressions->time;

	XrResult result = qxrUpdateFaceTracker(&xrExp);
	if (result != XR_SUCCESS)
		return false;

	return xrExp.isValid;
}

bool CreateEyeTracker() {
	return qxrCreateEyeTracker(instance, systemId, session, worldSpace) == XR_SUCCESS;
}

bool DestroyEyeTracker() {
	return qxrDestroyEyeTracker(instance);
}

bool GetEyeData(EyeGazesFB *gazes) {

	XrResult result = qxrUpdateEyeTracker(&xrGazes);
	printf("Eye Result: %i", result);
	if (result != XR_SUCCESS) 
		return false;
	gazes->gaze[0].orientation = xrGazes.gaze[0].gazePose.orientation;
	gazes->gaze[1].orientation = xrGazes.gaze[1].gazePose.orientation;
	gazes->time = xrGazes.time;
	return xrGazes.gaze[0].isValid | xrGazes.gaze[1].isValid;
}

int main() {

	int result;
	result = InitializeSession();
	if (result != 0) {
		printf("Error creating session");
		return -1;
	}

	result = CreateFaceTracker();
	if (result != 0) {
		return -1;
	}

	result = CreateEyeTracker();
	if (result != 0) {
		return -1;
	}

	FaceWeightsFB expressions{};
	EyeGazesFB gazes{};

	while (1) {
		printf("\nFaceGet: %i", (int)GetFaceData(&expressions));
		printf("\nResult: JawDrop: %f", expressions.weights[XR_FACE_EXPRESSION2_JAW_DROP_FB]);
		printf("\nEyeGet: %i", (int)GetEyeData(&gazes));
		printf("\nResult: Eye Left Quat: %f %f %f %f", gazes.gaze[0].orientation.x, 
													   gazes.gaze[0].orientation.y, 
													   gazes.gaze[0].orientation.w, 
													   gazes.gaze[0].orientation.z);
	}

	DestroyEyeTracker();
	DestroyFaceTracker();

	CloseSession();
	xrDestroySession(session);
}
