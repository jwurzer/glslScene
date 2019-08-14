glslScene
=========

glslScene is a value tool that supports learning and training of GLSL programming. It provides an environment for testing and developing GLSL shader programs. Loads any type of shaders (frag/vertex/…), images as textures and geometries. It will reload automatically resources on changes. Support of multiple framebuffers and rendering passes for postprocessing.

Supported OS: Windows, Linux

Main features of glslScene:
 * Hot Reloading
 * GUI to edit uniform values on the fly
 * Entity-Component-System
 * OpenGL version can be configured
 * One scene project file (scene.tml) to configure context properties, resources, scenes and rendering passes.
 * Easy to handle and switch between multiple projects. Each scene project has its own subdirectory.
 * Open Source

Screenshots
-----------

![Screenshot from the example scene_001_c_triangles](doc/screenshot-01.png?raw=true)

![Screenshot from the example scene_002_b_3D](doc/screenshot-02.png?raw=true)

Using glslScene
---------------
The individual scene projects are located in the scenes directory. Each scene project has its own subdirectory. The name of the folder is the project name. The project to be used is defined in the file scenes/selection.tml. Each scene project must have the configuration file scene.tml. Several resources, scenes and rendering passes can be created in this scene file. The project folder should also contain all other resources. For example the images for the textures and the source files of the shader programs.

For the scenes/selection.tml file and scene.tml files the Tine Markup Language is used. See: [TML specification](doc/tml-specification.md)

For more informations about the creation and configuration of a glslScene project see [Creation and configuration of a scene.tml file](doc/scene-config.md).

