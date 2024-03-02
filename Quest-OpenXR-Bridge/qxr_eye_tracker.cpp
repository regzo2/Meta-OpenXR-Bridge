#include "pch.h"

#include <openxr/openxr.h>
#include <openxr/fb_eye_tracking_social.h>
#include "qxr_eye_tracker.h"
#include <iostream>

XrEyeTrackerFB eyeTracker;
PFN_xrGetEyeGazesFB getGazes;
XrSpace space;

XrResult qxrCreateEyeTracker(XrInstance instance, XrSystemId systemId, XrSession session, XrSpace worldSpace) {

    if (instance == NULL || systemId == NULL || session == NULL || worldSpace == NULL)
        return XR_ERROR_INITIALIZATION_FAILED;

    printf("\nGetting ET properties from system: \n");

    XrSystemEyeTrackingPropertiesFB etProp = { XR_TYPE_SYSTEM_EYE_TRACKING_PROPERTIES_FB };
    etProp.supportsEyeTracking = true;

    PFN_xrGetSystemProperties pfnGetSystemProperties;
    XrResult result = xrGetInstanceProcAddr(instance, "xrGetSystemProperties", (PFN_xrVoidFunction*)&pfnGetSystemProperties);
    if (result != XR_SUCCESS)
        return result;
    XrSystemProperties systemProps{ XR_TYPE_SYSTEM_PROPERTIES, &etProp };

    if (pfnGetSystemProperties(instance, systemId, &systemProps) == XR_SUCCESS) {
        // Check if eye tracking is supported
        if (etProp.supportsEyeTracking == XR_TRUE) {
            printf("    Eye tracking is supported.\n");
        }
        else {
            printf("    Eye tracking is not supported.\n");
        }
    }

    // Declare function pointer typedefs for extension functions
    PFN_xrCreateEyeTrackerFB pfnCreateEyeTracker;

    // Get instance function addresses for extension functions
    result = xrGetInstanceProcAddr(instance, "xrCreateEyeTrackerFB", (PFN_xrVoidFunction*)&pfnCreateEyeTracker);
    if (result != XR_SUCCESS) {
        printf("Failed to retrieve function address for xrCreateEyeTrackerFB.\n");
        return result;
    }

    printf("\nCreating Eye Tracker: ");

    // Create a face tracker for default set of facial expressions.
    XrEyeTrackerCreateInfoFB createInfo{ XR_TYPE_EYE_TRACKER_CREATE_INFO_FB };
    result = pfnCreateEyeTracker(session, &createInfo, &eyeTracker);
    if (result != XR_SUCCESS) {
        printf("Failed to create eye tracker.");
        return result;
    }

    // Get function pointer for xrGetFaceExpressionWeights2FB.
    result = xrGetInstanceProcAddr(
        instance,
        "xrGetEyeGazesFB",
        reinterpret_cast<PFN_xrVoidFunction*>(
            &getGazes));

    if (result != XR_SUCCESS) {
        printf("Failed to create GetEyeDatas.");
        return result;
    }

    space = worldSpace;

    return result;
}

XrEyeGazesInfoFB gazesInfo{XR_TYPE_EYE_GAZES_INFO_FB};

XrResult qxrUpdateEyeTracker(XrEyeGazesFB *data) {

    //XrFrameState frameState; // previously returned from xrWaitFrame
    //const XrTime time = frameState.predictedDisplayTime;

    gazesInfo.baseSpace = space;

    XrResult result = getGazes(
        eyeTracker,
        &gazesInfo,
        data);

    return result;
}

bool qxrDestroyEyeTracker(XrInstance instance) {

    if (eyeTracker == NULL)
        return false;

    PFN_xrDestroyEyeTrackerFB destroyEye;
    XrResult result = xrGetInstanceProcAddr(
        instance,
        "xrDestroyEyeTrackerFB",
        reinterpret_cast<PFN_xrVoidFunction*>(&destroyEye));

    if (result != XR_SUCCESS) 
        return false;

    destroyEye(eyeTracker);

    return true;
}