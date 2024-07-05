# Audio

## Including

To add this library to your project, simply add these two lines to your *CMakeLists.txt*:
```cmake
add_subdirectory(path/to/Audio)
target_link_libraries(${PROJECT_NAME} PRIVATE Cool::Audio)
```

Then include it as:
```cpp
#include <Audio/Audio.hpp>
```
<br/>

**IMPORTANT**: On MacOS, in order for your application to be able to access the microphone (when installing your app on end-users machines), you need to add this in your *Info.plist* file:
```xml
<key>NSMicrophoneUsageDescription</key>
<string>This app requires microphone access because [your reason here].</string>
```
See [the documentation about this](https://developer.apple.com/documentation/bundleresources/information_property_list/protected_resources/requesting_authorization_for_media_capture_on_macos?language=objc).

## Running the tests

Simply use "tests/CMakeLists.txt" to generate a project, then run it.<br/>
If you are using VSCode and the CMake extension, this project already contains a *.vscode/settings.json* that will use the right CMakeLists.txt automatically.
