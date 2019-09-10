glslScene
=========

glslScene is a tool that supports learning and training of GLSL programming. It provides an environment for testing and developing GLSL shader programs. Loads any type of shaders (frag/vertex/…), images as textures and geometries. It will reload automatically resources on changes. Support of multiple framebuffers and  multiple rendering passes for postprocessing.

Supported platforms: Windows, Linux

Main features of glslScene:
 * Hot Reloading
 * GUI to edit uniform values on the fly
 * C Scripting support with PicoC interpreter
 * Entity-Component-System
 * OpenGL version can be configured
 * One scene project file (scene.tml) to configure context properties, resources, scenes and rendering passes.
 * Easy to handle and switch between multiple projects. Each scene project has its own subdirectory.
 * Open Source, permissive free software license. See: [LICENSE](LICENSE)

Screenshots
-----------

![Screenshot from the example scene_001_c_triangles](doc/screenshot-01.png?raw=true)

![Screenshot from the example scene_002_b_3D](doc/screenshot-02.png?raw=true)

Using glslScene
---------------
The individual scene projects are located in the scenes directory. Each scene project has its own subdirectory. The name of the folder is the project name. The project to be used is defined in the file scenes/selection.tml. Each scene project must have the configuration file scene.tml. Several resources, scenes and rendering passes can be created in this scene file. The project folder should also contain all other resources. For example the images for the textures and the source files of the shader programs.

For the scenes/selection.tml file and scene.tml files the Tine Markup Language is used. See: [TML specification](doc/tml-specification.md)

For more informations about the creation and configuration of a glslScene project see [Creation and configuration of a scene.tml file](doc/scene-config.md).

glslScene binaries
------------------
Windows builds: Binaries for Windows can be downloaded from the [releases](https://github.com/jwurzer/glslScene/releases) section. Simple download glslScene-win-vX-X.zip extract it and start glslscene-rel.exe (release build) or glslscene.exe (debug build) from the bin subdirectory.

Compile glslScene
-----------------

### Compile for Linux (Debian / Ubuntu)

Update list of available packages for Debian/Ubuntu (instead of **apt-get** the command **apt** can be used)

	$ sudo apt-get update

Install dependencies

	$ sudo apt-get install git make cmake g++ libsdl2-dev libglew-dev libglm-dev libreadline-dev

Checkout source code and submodules

	$ git clone https://github.com/jwurzer/glslScene.git
	$ cd glslScene
	glslScene$ git submodule update --init --recursive

Compile per make with Makefile

The Makefile creates a build subdirectory, calls cmake for you and compiles it.
Instead of using the Makefile (which is in this project only a helper script for using CMake), cmake can be used directly. If you want to use cmake look into the Makefile to see the cmake build commands.

	glslScene$ make

Start glslScene

	glslScene$ build/glslScene

### Compile for Windows with Visual Studio

##### Requirements

* git for Windows

* Visual Studio 2017

* cmake (e.g. Windows installer version)

  Add CMake to system PATH

* Make for Windows

  http://gnuwin32.sourceforge.net/packages/make.htm
  
  http://gnuwin32.sourceforge.net/downlinks/make.php (Installer)
  
  Add the install path inclusive bin subdirectory to PATH.
  e.g. add the following to PATH:
  C:\Program Files (x86)\GnuWin32\bin

##### Checkout the source code (using the cmd)

	> git clone https://github.com/jwurzer/glslScene.git
	> cd glslScene
	glslScene> git submodule update --init --recursive

##### Necessary Libraries: SDL, GLM and GLEW

Create a subdirectory **lib** at glslScene (direct inside of glslScene).

Download the following libraries into the lib directory:

* SDL library - https://www.libsdl.org/release/SDL2-devel-2.0.10-VC.zip
* GLM library - https://github.com/g-truc/glm/releases/download/0.9.9.6/glm-0.9.9.6.zip
* GLEW library - https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip/download

Extract `SDL2-devel-2.0.10-VC.zip`, `glm-0.9.9.6.zip` and `glew-2.1.0-win32.zip` direct into the **lib** directory.
The CMakeLists.txt except this paths.

After extraction the hierachy of the lib folder **must** look like this:

	glslScene\lib               (contains the zip files and the sub directories glew-2.1.0, glm and SDL-2.0.10)
	glslScene\lib\glew-2.1.0    (contains LICENSE.txt and the sub dirs bin, doc, include and lib)
	glslScene\lib\glew-2.1.0\bin
	glslScene\lib\glew-2.1.0\doc
	glslScene\lib\glew-2.1.0\include
	glslScene\lib\glew-2.1.0\lib
	glslScene\lib\glm           (contains some files and the sub dirs doc, glm, test and util)
	glslScene\lib\glm\doc
	glslScene\lib\glm\glm
	glslScene\lib\glm\test
	glslScene\lib\glm\util
	glslScene\lib\SDL2-2.0.10   (contains some txt files and the sub dirs docs, include and lib)
	glslScene\lib\SDL2-2.0.10\docs
	glslScene\lib\SDL2-2.0.10\include
	glslScene\lib\SDL2-2.0.10\lib

##### Create Visual Studio solution

Open a command line (cmd) go into the glslScene directory and enter the command make

	glslScene> make

This creates a subdirectory build and calls CMake to create a solution inside the build directory.
After this the message `"Open the project with Visual Studio and compile it ;-)"` should be printed.

##### Compile glslScene

Go into the build directory open the solution glslScene.sln. In Visual Studio there are three projects: ALL_BUILD, glslScene and ZERO_CHECK. Select the glslScene project as Startup project. Compile the project glslScene und run it.

License
-------
Most parts of glslScene are dual-licensed in public domain and zlib license. Some other parts (e.g. external used libraries) are under different licenses (e.g. ImGui library under MIT License, GLEW library under modified BSD License).

The scene projects of glslScene (scenes subdirectory) can have its own licenses.
If no own license exists in the directory of the specific scene project or
no other license information exists, then the corresponding scene project is also
under the public domain or zlib license (dual license).

For more information about the licences in glslScene see the [LICENSE](LICENSE) file.

External libraries
------------------
glslScene use the following libraries: SDL, GLM, stb_image, ImGui, PicoC, GLEW or optional glad, some classes from SFML.
