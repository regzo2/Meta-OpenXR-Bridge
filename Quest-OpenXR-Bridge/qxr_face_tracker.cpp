#include "pch.h"

#include <openxr/openxr.h>
#include <openxr/fb_face_tracking2.h>
#include "qxr_face_tracker.h"
#include <stdio.h>

bool faceActive = false;
XrFaceTracker2FB faceTracker;
PFN_xrGetFaceExpressionWeights2FB getWeights;
XrFaceExpressionWeights2FB fbExpressions = {XR_TYPE_FACE_EXPRESSION_WEIGHTS2_FB};

XrResult qxrCreateFaceTracker(XrInstance instance, XrSystemId systemId, XrSession session) {

    if (instance == NULL || systemId == NULL || session == NULL)
        return XR_ERROR_INITIALIZATION_FAILED;

    printf("\nGetting FT properties from system: \n");

    XrSystemFaceTrackingProperties2FB ftProp = { XR_TYPE_SYSTEM_FACE_TRACKING_PROPERTIES2_FB };

    PFN_xrGetSystemProperties pfnGetSystemProperties;
    XrResult result = xrGetInstanceProcAddr(instance, "xrGetSystemProperties", (PFN_xrVoidFunction*)&pfnGetSystemProperties);
    if (result != XR_SUCCESS)
        return result;

    XrSystemProperties systemProps{ XR_TYPE_SYSTEM_PROPERTIES, &ftProp };
    result = pfnGetSystemProperties(instance, systemId, &systemProps);

    if (result == XR_SUCCESS) {
        // Check if visual face tracking is supported
        if (ftProp.supportsVisualFaceTracking == XR_TRUE) {
            printf("    Visual face tracking is supported.\n");
        }
        else return result;

        // Check if audio face tracking is supported
        if (ftProp.supportsAudioFaceTracking == XR_TRUE) {
            printf("    Audio face tracking is supported.\n");
        }
        else {
            printf("    Audio face tracking is not supported.\n");
            return result;
        }
    }
    else {
        printf(" Failed to get system properties.\n");
    }

    PFN_xrCreateFaceTracker2FB pfnCreateFaceTracker2FB;
    result = xrGetInstanceProcAddr(instance, "xrCreateFaceTracker2FB",
                             reinterpret_cast<PFN_xrVoidFunction*>(
                             &pfnCreateFaceTracker2FB));
    if (result != XR_SUCCESS) {
        printf("Failed to create PFN");
        return result;
    }
    printf("pizza");
    printf("\nCreating Face Tracker object");

    XrFaceTrackerCreateInfo2FB createInfo{ XR_TYPE_FACE_TRACKER_CREATE_INFO2_FB };
    createInfo.faceExpressionSet = XR_FACE_EXPRESSION_SET2_DEFAULT_FB;
    createInfo.requestedDataSourceCount = 2;
    XrFaceTrackingDataSource2FB dataSources[2] = {
        XR_FACE_TRACKING_DATA_SOURCE2_VISUAL_FB,
        XR_FACE_TRACKING_DATA_SOURCE2_AUDIO_FB };
    createInfo.requestedDataSources = dataSources;

    result = pfnCreateFaceTracker2FB(session, &createInfo, &faceTracker);
    if (result != XR_SUCCESS) {
        printf("Failed to create face tracker");
        return result;
    }

    // Get function pointer for xrGetFaceExpressionWeights2FB.
    result = xrGetInstanceProcAddr(
        instance,
        "xrGetFaceExpressionWeights2FB",
        reinterpret_cast<PFN_xrVoidFunction*>(
            &getWeights));
    if (result != XR_SUCCESS)
        printf("Failed to create weights PFN");

    faceActive = true;
    return result;
}

XrFaceExpressionInfo2FB expressionInfo{ XR_TYPE_FACE_EXPRESSION_INFO2_FB };

XrResult qxrUpdateFaceTracker(XrFaceExpressionWeights2FB *expressions) {

    if (!faceActive) return XR_ERROR_INITIALIZATION_FAILED;

    XrResult result = getWeights(
        faceTracker,
        &expressionInfo,
        expressions);

    return result;
}


bool qxrDestroyFaceTracker(XrInstance instance) {
    
    if (faceTracker == NULL) 
        return false;

    PFN_xrDestroyFaceTracker2FB pfnDestroyFaceTracker;
    XrResult result = xrGetInstanceProcAddr(
        instance,
        "xrDestroyFaceTracker2FB",
        reinterpret_cast<PFN_xrVoidFunction*>(&pfnDestroyFaceTracker));

    if (result != XR_SUCCESS)
        return false;
    
    pfnDestroyFaceTracker(faceTracker);

    return true;
}
