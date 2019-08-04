glslScene for Windows:
----------------------

requirements:
* git for Windows
* Visual Studio 2017
* cmake (e.g. Windows installer version)
  Add CMake to system PATH
  If cmake can't find MSBuild.exe then add the correct path to PATH.
  e.g. C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\amd64
* Make for Windows
  http://gnuwin32.sourceforge.net/packages/make.htm
  http://gnuwin32.sourceforge.net/downlinks/make.php (Installer)
  Add the install path inclusive bin subdirectory to PATH.
  e.g. add the following to PATH:
  C:\Program Files (x86)\GnuWin32\bin
* SDL library
  Download https://www.libsdl.org/release/SDL2-devel-2.0.10-VC.zip



Using SDL under Windows:

For SDL download SDL2-devel-2.0.8-VC.zip
from https://www.libsdl.org/download-2.0.php

For SDL_image download SDL2_image-devel-2.0.3-VC.zip
from https://www.libsdl.org/projects/SDL_image/

Copy the header files from SDL2-devel-2.0.8-VC.zip to
C:\Development\Library\include\SDL2
Copy the lib directory to C:\Development\Library\
Libs are under C:\Development\Library\lib\x86 and
C:\Development\Library\lib\x64

Copy the header files (should be only one) from
SDL2_image-devel-2.0.3-VC.zip to
C:\Development\Library\include\SDL2
And the libs from x86 and x64 to the correct
lib directories (C:\Dev...\lib\x86 and ...\x64)

Create the visual studio solution file with the Makefile
(GNU Make for Windows must be installed).
The Makefile creates a build directory and uses cmake to
create the VS solution.
Additionally the "res" directory is copied into the "build"
directory and all dll-Files from C:\Development\Library\Libs\x86
also copied into the "build" directory.

