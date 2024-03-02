# Meta OpenXR Bridge
>A lightweight wrapper for Meta's OpenXR APIs.

## Features
* Simple API: Straightforward and easy-to-use API for integrating Meta's OpenXR extensions.
* Easy Integration: Seamlessly integrate facial and eye tracking functionalities into your applications.

## Supported OpenXR Features
Currently, Meta OpenXR Bridge supports the following features:

* fb_face_tracking2: Face tracking functionality.
* fb_eye_tracking_social: Eye tracking functionality.

## Usage
To use Meta OpenXR Bridge:

Call CreateSession(), which attempts to create an OpenXR session with the supported extensions exposed. If the runtime does not support the extensions, it will fail.

After the session successfully is created, you can call other functions provided by Meta OpenXR Bridge to utilize the supported features.

## To Do
* Support for Other Graphics APIs: Currently, Meta OpenXR Bridge only supports D3D11. Support for other graphics APIs will be added in the future.
* Enhancements to Session Management: Basic session creation and application lifetime management are implemented, but improvements are planned for future updates. It is currently only designed to run extensions in a background environment (unfocused/idle)

## License
Meta OpenXR Bridge is licensed under the MIT License.
