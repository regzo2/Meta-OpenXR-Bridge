#include "pch.h"
#ifndef XR_USE_GRAPHICS_API_D3D11
#define XR_USE_GRAPHICS_API_D3D11
#endif

#ifndef XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_GRAPHICS_API_OPENGL
#endif

#include <d3d11.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <stdio.h>

bool (*qxrFreeGraphics)();

bool qxrFreeD3D11Device() {
    return true;
}


XrResult qxrCreateD3D11Bind(XrInstance instance, XrSystemId systemId, void **xrGraphicsBinder) {

    XrGraphicsBindingD3D11KHR *d3d11Binding = new XrGraphicsBindingD3D11KHR(); // Allocate memory
    d3d11Binding->type = XR_TYPE_GRAPHICS_BINDING_D3D11_KHR;
    d3d11Binding->next = nullptr;

    XrGraphicsRequirementsD3D11KHR requirements;
    requirements.type = XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR;
    requirements.next = nullptr;

    PFN_xrGetD3D11GraphicsRequirementsKHR pfnD3D11Req;
    XrResult result = xrGetInstanceProcAddr(instance, "xrGetD3D11GraphicsRequirementsKHR", (PFN_xrVoidFunction*)&pfnD3D11Req);
    if (result != XR_SUCCESS)
        return result;

    result = pfnD3D11Req(instance, systemId, &requirements);
    if (result != XR_SUCCESS)
        return result;
    
    printf("\nCreating D3D device: ");

    HRESULT hResult = D3D11CreateDevice(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0x00,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &d3d11Binding->device,
        NULL,
        NULL);

    if (FAILED(hResult)) {
        printf("Failed to create D3D11 device.");
        delete d3d11Binding; // Cleanup on failure
        return XR_ERROR_GRAPHICS_DEVICE_INVALID;
    }

    *xrGraphicsBinder = d3d11Binding; // Update pointer value
    qxrFreeGraphics = &qxrFreeD3D11Device; // point free fcn to free d3d11 device.

    return XR_SUCCESS;
}

XrResult qxrCreateGraphicsBind(XrInstance instance, XrSystemId systemId, void** xrGraphicsBinder) {

    return qxrCreateD3D11Bind(instance, systemId, xrGraphicsBinder);

}
