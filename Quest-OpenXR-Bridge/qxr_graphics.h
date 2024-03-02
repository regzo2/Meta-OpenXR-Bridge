#pragma once
#ifndef Q_GRAPHICS
#define Q_GRAPHICS
#include "pch.h"

#define XR_USE_GRAPHICS_API_D3D11

#include <d3d11.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#if defined(_WIN32)
extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; // force NVIDIA GPU.
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001; // force AMD GPU.
}
#endif

enum qxrGraphicsType {
	D3D11 = 1,
	D3D12 = 2,
	OpenGL = 10,
	OpenGLEX = 11,
	Vulcan = 20
};

XrResult qxrCreateGraphicsBind(XrInstance instance, XrSystemId systemId, void **xrGraphicsBinder);
bool qxrFreeGraphicsDevice();

#endif
