#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#define XR_USE_PLATFORM_WIN32

#include "qxr_graphics.h"
#include "qxr_face_tracker.h"
#include "qxr_eye_tracker.h"
#include "qxr_xr_interface.h"

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <openxr/openxr_reflection.h>

#include <windows.h>
#include <iostream>
#include <vector>
#include <iterator>

using namespace std;

bool qIsExtensionSupported(const std::vector<XrExtensionProperties>& availableExtensions, const char* extensionName) {
    for (const auto& extension : availableExtensions) {
        if (strcmp(extension.extensionName, extensionName) == 0) {
            return true;
        }
    }
    return false;
}

XrInstance instance;
XrSystemId systemId;
XrViewConfigurationType viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
XrSession session;
XrSpace worldSpace;

int CreateXrSession() {

    uint32_t extensionsCount = 0;
    if (xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionsCount, nullptr) != XR_SUCCESS) {
        // If it fails this early that means there's no runtime installed
        return 4;
    }

    vector<XrExtensionProperties> properties(extensionsCount, { XR_TYPE_EXTENSION_PROPERTIES });
    if (xrEnumerateInstanceExtensionProperties(nullptr, extensionsCount, &extensionsCount, properties.data()) != XR_SUCCESS) {
        // Failed to enumerate instance extension properties
        return 3;
    }

    vector<string> AvailableExtensions; // Declare and initialize the set to store available extensions

    // Populate set with available extensions
    for (const XrExtensionProperties& prop : properties) {
        AvailableExtensions.push_back(prop.extensionName);
    }

    // Check if required extensions are supported
    vector<const char*> requiredExtensions {
        XR_KHR_D3D11_ENABLE_EXTENSION_NAME,
        XR_FB_FACE_TRACKING2_EXTENSION_NAME,
        XR_FB_EYE_TRACKING_SOCIAL_EXTENSION_NAME
    };

    XrResult result = XR_SUCCESS;

    XrInstanceCreateInfo createInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
    createInfo.next = NULL;
    createInfo.createFlags = 0;
    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.enabledExtensionNames = requiredExtensions.data();
    createInfo.enabledApiLayerCount = 0;
    createInfo.applicationInfo.applicationVersion = 1;
    createInfo.applicationInfo.engineVersion = 0;
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    strncpy_s(createInfo.applicationInfo.applicationName,
        "VRCFaceTracking Quest Pro Bridge",
        XR_MAX_APPLICATION_NAME_SIZE);
    strncpy_s(createInfo.applicationInfo.engineName,
        "VRCFaceTracking",
        XR_MAX_APPLICATION_NAME_SIZE);

    printf("\nCreating XrInstance: ");

    result = xrCreateInstance(&createInfo, &instance);
    if (result != XR_SUCCESS) {
        return -2;
    }

    if (instance == NULL) return -10;

    printf("\nGetting XrSystem: ");

    XrSystemGetInfo systemInfo = {XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.next = nullptr;
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    
    result = xrGetSystem(instance, &systemInfo, &systemId);
    if (result != XR_SUCCESS)
        return -3;

    printf("\nResolving stereo view.");

    uint32_t viewCount;
    vector<XrViewConfigurationType> viewConfigTypes;
    xrEnumerateViewConfigurations(instance, systemId, 0, &viewCount, nullptr);
    viewConfigTypes.resize(viewCount);
    // Fill the initial array with valid enum types (this will fail in the validation layer otherwise).
    for (XrViewConfigurationType& type : viewConfigTypes)
        type = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO;
    xrEnumerateViewConfigurations(instance, systemId, viewCount, &viewCount, viewConfigTypes.data());
    bool bSupportStereoView = false;

    for (XrViewConfigurationType viewConfigType : viewConfigTypes)
    {
        if (viewConfigType == viewConfiguration)
        {
            bSupportStereoView = true;
            break;
        }
    }

    if (!bSupportStereoView)
    {
        printf("Does not support stereo view type.");
        return -4;
    }

     printf("\nBinding graphics: ");

    void *gfxBinding = nullptr;
    qxrCreateGraphicsBind(instance, systemId, &gfxBinding);

     printf("\nCreating Session: ");

    XrSessionCreateInfo sessionCreateInfo = { XR_TYPE_SESSION_CREATE_INFO };
    sessionCreateInfo.next = gfxBinding;
    sessionCreateInfo.createFlags = 0;
    sessionCreateInfo.systemId = systemId;

    result = xrCreateSession(instance, &sessionCreateInfo, &session);
    if (result != XR_SUCCESS)
        return -5;

    XrReferenceSpaceCreateInfo spaceInfo = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
    spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;

    XrQuaternionf orientation{ 0.0f, 0.0f, 0.0f, 1.0f };
	XrVector3f position{ 0.0f, 0.0f,0.0f };
	spaceInfo.poseInReferenceSpace.orientation = orientation;
	spaceInfo.poseInReferenceSpace.position = position;


    result = xrCreateReferenceSpace(session, &spaceInfo, &worldSpace);
    if (result != XR_SUCCESS) {
        printf("DUMB");
        return -1;
    }

    return 0;
}

int CloseXrSession() {
    xrEndSession(session);
}
