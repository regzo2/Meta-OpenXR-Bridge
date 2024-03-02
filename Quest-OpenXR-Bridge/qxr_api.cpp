#include "pch.h"
#include "qxr_xr_interface.h"
#include "qxr_eye_tracker.h"
#include "qxr_face_tracker.h"
#include "qxr_api.h"

#include <stdio.h>

XrFaceExpressionWeights2FB xrExp = { XR_TYPE_FACE_EXPRESSION_WEIGHTS2_FB };
XrEyeGazesFB xrGazes = { XR_TYPE_EYE_GAZES_FB };

int InitializeSession() {
	return CreateXrSession();
}

int CloseSession() {
	XrResult result = xrEndSession(session);
	return result;
}

int CreateFaceTracker() {
	XrResult result = qxrCreateFaceTracker(instance, systemId, session);
	if (result != XR_SUCCESS)
		return result;
}

int DestroyFaceTracker() {
	return qxrDestroyFaceTracker(instance);
}

int GetFaceData(FaceWeightsFB *expressions) {

	xrExp.weights = expressions->weights;
	xrExp.weightCount = XR_FACE_EXPRESSION2_COUNT_FB;
	xrExp.confidences = expressions->confidences;
	xrExp.confidenceCount = XR_FACE_CONFIDENCE2_COUNT_FB;

	XrResult result = qxrUpdateFaceTracker(&xrExp);
	if (result != XR_SUCCESS)
		return 0;

	return !xrExp.isValid;
}

int CreateEyeTracker() {
	return qxrCreateEyeTracker(instance, systemId, session, worldSpace);
}

int DestroyEyeTracker() {
	return qxrDestroyEyeTracker(instance);
}

int GetEyeData(EyeGazesFB *gazes) {

	XrResult result = qxrUpdateEyeTracker(&xrGazes);
	printf("Eye Result: %i", result);
	if (result != XR_SUCCESS) 
		return 0;
	gazes->gaze[0].orientation = xrGazes.gaze[0].gazePose.orientation;
	gazes->gaze[1].orientation = xrGazes.gaze[1].gazePose.orientation;
	return -1*(!xrGazes.gaze[0].isValid) | -2*(!xrGazes.gaze[1].isValid);
}

int main() {

	int result;
	result = InitializeSession();
	if (result != 0) {
		printf("Error creating session");
	}

	result = CreateFaceTracker();
	if (result != 0) {
		printf("Error creating Face Tracker");
			return -1;
	}

	result = CreateEyeTracker();
	if (result != 0) {
		printf("Error creating Eye Tracker");
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
