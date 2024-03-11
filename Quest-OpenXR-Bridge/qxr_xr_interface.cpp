#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#define XR_USE_PLATFORM_WIN32

#include "qxr_graphics.h"
#include "qxr_xr_interface.h"

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <openxr/openxr_reflection.h>

#include <windows.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include "qxr_api.h"
using namespace std;

XrInstance instance;
XrSystemId systemId;
XrViewConfigurationType viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
XrSession session;
XrSpace worldSpace;

qxrResult CreateXrSession() {

    uint32_t extensionsCount = 0;
    if (xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionsCount, nullptr) != XR_SUCCESS) {
        return RUNTIME_MISSING;
    }

    vector<XrExtensionProperties> properties(extensionsCount, { XR_TYPE_EXTENSION_PROPERTIES });
    if (xrEnumerateInstanceExtensionProperties(nullptr, extensionsCount, &extensionsCount, properties.data()) != XR_SUCCESS) {
        return PROPERTY_GET_FAILURE;
    }

    vector<char*> extensions; // Declare and initialize the set to store available extensions

    vector<string> supportedExtensions{
        XR_KHR_D3D11_ENABLE_EXTENSION_NAME,
        XR_KHR_OPENGL_ENABLE_EXTENSION_NAME
    };

    vector<string> requiredExtensions{
        XR_FB_FACE_TRACKING2_EXTENSION_NAME,
        XR_FB_EYE_TRACKING_SOCIAL_EXTENSION_NAME
    };

    printf("\nExtensions Accessible: \n");
    for (const XrExtensionProperties& prop  : properties) {
        printf("%s\n", prop.extensionName);
    }

    // Check for supported extensions
    for (const XrExtensionProperties& prop : properties) {
        auto supported = std::find(supportedExtensions.begin(), supportedExtensions.end(), prop.extensionName);
        if (supported != supportedExtensions.end()) {
            extensions.push_back((char*)prop.extensionName);
        }
    }

    // Check for required extensions
    for (string required : requiredExtensions) {
        auto found = std::find(extensions.begin(), extensions.end(), required);
        if (found == extensions.end()) {
            printf("Required feature %s not available.", required.c_str());
            return RUNTIME_FEATURE_UNAVAILABLE;
        }
    }

    printf("\nExtensions Available: \n");
    for (char* ext : extensions) {
        printf("%s\n", ext);
    }

    XrResult result = XR_SUCCESS;

    XrInstanceCreateInfo createInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
    createInfo.next = NULL;
    createInfo.createFlags = 0;
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.enabledExtensionNames = extensions.data();
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
        return INSTANCE_CREATE_FAILURE;
    }

    if (instance == NULL)
        return INSTANCE_CREATE_FAILURE;

    printf("\nGetting XrSystem: ");

    XrSystemGetInfo systemInfo = { XR_TYPE_SYSTEM_GET_INFO };
    systemInfo.next = nullptr;
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    result = xrGetSystem(instance, &systemInfo, &systemId);
    if (result != XR_SUCCESS)
        return SYSTEM_GET_FAILURE;

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
        return STEREO_VIEW_UNSUPPORTED;
    }

    printf("\nBinding graphics: ");

    void* gfxBinding = nullptr;
    result = qxrCreateGraphicsBind(instance, systemId, &gfxBinding);
    if (result != XR_SUCCESS) {
        printf("Failed to bind graphics.");
        return GRAPHICS_BIND_FAILURE;
    }

    printf("\nCreating Session: ");

    XrSessionCreateInfo sessionCreateInfo = { XR_TYPE_SESSION_CREATE_INFO };
    sessionCreateInfo.next = gfxBinding;
    sessionCreateInfo.createFlags = 0;
    sessionCreateInfo.systemId = systemId;

    result = xrCreateSession(instance, &sessionCreateInfo, &session);
    if (result != XR_SUCCESS)
        return SESSION_CREATE_FAILURE;

    XrReferenceSpaceCreateInfo spaceInfo = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
    spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;

    XrQuaternionf orientation{ 0.0f, 0.0f, 0.0f, 1.0f };
    XrVector3f position{ 0.0f, 0.0f,0.0f };
    spaceInfo.poseInReferenceSpace.orientation = orientation;
    spaceInfo.poseInReferenceSpace.position = position;


    result = xrCreateReferenceSpace(session, &spaceInfo, &worldSpace);
    if (result != XR_SUCCESS) {
        printf("Failed to create a reference space.");
        return SPACE_CREATE_FAILURE;
    }

    return SUCCESS;
}

int CloseXrSession() {
    xrEndSession(session);
}
