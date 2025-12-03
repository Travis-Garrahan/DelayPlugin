[How to build] (https://github.com/user-attachments/files/23893658/How.to.build.md)
**Windows** 

**Method 1: Download .exe from GitHub:**  
[https://github.com/Travis-Garrahan/DelayPlugin/releases/tag/Alpha](https://github.com/Travis-Garrahan/DelayPlugin/releases/tag/Alpha)

**Method 2: Build from source on Windows:**  
Prerequisites:

- CMake [https://cmake.org/download/](https://cmake.org/download/)   
- MSVC compiler/ dev tools

Run PowerShell as Admin:

- cd to project directory  
- Run cmake \-S . \-B build  
- Run cmake \--build build  
- cd to DelayPlugin-main/build/plugin/Simple\_Delay\_artefacts\\Debug\\Standalone\\  
- Run ‘Simple Delay.exe’, which will be located in this directory.

---

**MacOS**

**Method 1: Download app bundle from GitHub:**  
[https://github.com/Travis-Garrahan/DelayPlugin/releases/tag/MacOS\_alpha](https://github.com/Travis-Garrahan/DelayPlugin/releases/tag/MacOS_alpha)

\*\*This is not signed with an Apple Developer ID, to run:

1\. Open terminal  
2\. cd to directory where app was downloaded (ex: 'cd \~/Downloads')  
3\. run  'xattr \-rd com.apple.quarantine Simple\\ Delay.app'

You should then be able to open the app from finder.

**Method 2: Build from source on MacOS:**  
Prerequisites:

- CMake [https://cmake.org/download/](https://cmake.org/download/)  
- Clang/ gcc compiler


In Terminal:

- cd to project directory  
- Run cmake \-S . \-B build  
- Run cmake \--build build  
- cd to build/plugin/Simple\_Delay\_artefacts/Standalone/Simple\\ Delay.app/Contents/MacOS  
- Run ./Simple\\ Delay 

