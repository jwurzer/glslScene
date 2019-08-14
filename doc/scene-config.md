Scene Configuration
===================

The scene config file uses the TML. See: [TML specification](tml-specification.md)

Configuration for a scene is splitt in four sections/objects:

 * **context**
 * **resources**
 * **scenes**
 * **rendering**

A simple scene file for a triangle looks like this:

	context
		rendering-api = opengl 2.1

	resources
		mesh
			id = MESH1
			vertex-layout
				x       y       r       g       b       a
			triangles
				0.0     0.0     1.0     0.0     0.0     1.0
				1.0     0.0     0.0     1.0     0.0     1.0
				0.0     1.0     0.0     0.0     1.0     1.0
		shader
			id = SHADER1
			vertex = shaders/triangle.vert
			fragment = shaders/triangle.frag

	scenes
		scene
			id = SCENE-ONE
			entity
				shader-component
					shader-program-id = SHADER1
				mesh-component
					mesh-id = MESH1

	rendering
		render-pass
			framebuffer-id = default
			clear-color = 0.5 0.5 0.5 1.0
			scene-id = SCENE-ONE

Specification
=============

Short info to the specification format:

Inside `<>` (e.g. `<id-name>`) means that it must be replaced by a value (number or text, etc.).

Inside `[]` means that its optional. E.g `[<id-name>]` means that no value is also allowed.

context
=======

The **context** section is specified as follow:

	context
		rendering-api = opengl [es] [<version>] [<profile>]

`opengl` must be always the first element for `rendering-api`.

`es` is optional. Must be used for OpenGL ES.

`<version>` is optional. Must be a floating point number like 3.3

`<profile>` is optional. Must be `core` or `compatibility`.

resources
=========

The **resources** can define different resources. Supported resources are **texture**, **mesh**, **framebuffer** and **shader**.

The **resources** section is specified as follow:

	resources
		mesh
			id = MESH1
			... informations for mesh resource ... (see mesh)
		shader
			id = SHADER1
			... informations for shader resource ... (see shader)
		texture
			id = TEX-SMILEY1
			... informations for texture resource ... (see texture)
		framebuffer
			id = FB-OFF-SCREEN
			... informations for framebuffer resource ... (see framebuffer)
		texture
			id = TEX-TWO
			... informations for texture resource ... (see texture)

		... more resources can follow ...
		...
		...

The order/sequence of the resources inside the scene file can be random.

texture
-------
A texture resource is specified as follow:

	texture
		id = <id-name>               (must exist)
		filename = <filename>        (must exist)
		mipmap = <boolean>           (must exist)
		min-filter = <filter>        (must exist)
		mag-filter = <filter>        (must exist)

`<id-name>` must be a text. Simple a number is not ok!

`<filename>` must be filename. The filename should be relative to the image file. E.g. `textures/smiley.png`

`<filter>` can be `linear` or `nearest`.

shader
------

A shader resource is specified as follow:

	shader
		id = <id-name>                          (must exist)
		<shader-type> = <shader-filename>       (optional, can be used multiple times in series)
		uniform <type> <var-name> = <special-keyword>                    (optional, multiple times)
		uniform int <var-name> = <ivalue>                                (optional, multiple times)
		uniform float <var-name> = <fvalue>                              (optional, multiple times)
		uniform vec2 <var-name> = <fvalue> <fvalue>                      (optional, multiple times)
		uniform vec3 <var-name> = <fvalue> <fvalue> <fvalue>             (optional, multiple times)
		uniform vec4 <var-name> = <fvalue> <fvalue> <fvalue> <fvalue>    (optional, multiple times)
		uniform sampler2d <var-name> = <texture-unit-index>              (optionalm, mult. t., texture unit 0 - 7)
		attribute <var-name> = <component-offset> <component-count>


`<id-name>` must be a text. Simple a number is not ok!

`<shader-type>` must be one of the following: `compute`, **`vertex`**, `tess-control`, `tess-evaluation`, `geometry`, **`fragment`**

`<shader-filename>` must be the filename for the specified shader-type. E.g. `shaders/effect.vert`

`<var-name` must be the same as in the shader file.

`<type>` can be `int`, `float`, `vec2`, `vec3`, `vec4`, `sampler2d`, `mat4`, `mat4x4`. `mat4` or `mat4x4` can only be used with a special keyword as value which is a matrix.

`<special-keyword>` can be one of the following:

 * `time`: Time in seconds since start
 * `relative-time <ref-time-sec>`: Time relative to a reference time. The unit of `<ref-time-sec>` are seconds.
 * `delta-time`: Time between the last and the current frame.
 * `mouse-pos-factor`: Mouse position in perentage. Left/Buttom is 0.0/0.0. Right/Top is 1.0/1.0.
 * `mouse-pos-pixel`: Mouse position in pixels. Left/Buttom is 0/0. Right/Top is the window size.
 * `viewport-pos-pixel`: First two used values of glViewport. Are always 0.0/0.0 at glslScene.
 * `viewport-size-pixel`: width and height values of glViewport.
 * `view-size`: Calculate width and height of glOrtho. If glPerspective is used then this value is not useful.
 * `view-ratio`: Ratio of width/height from view-size. If vec2 is used as type then the second is always 1.0.
 * `projection-matrix`: Projection matrix
 * `view-matrix`: View matrix
 * `model-matrix`: Model matrix
 * `model-view-matrix`: Model View matrix. Same as View matrix * Model matrix.
 * `entity-matrix`: Matrix of the current entity. If no matrix is used for the entity then its an identity matrix.
 * `mvp-matrix`: Model View Projection matrix. Same as Projection matrix * View matrix * Model matrix.

`<component-offset>` and `<component-count>` specify the offset and count of an vertex attribute. E.g. If a vertex has a position (x, y) and a color (rgba) then the attributes for this shader would look like:

	attribute aPosition = 0 2
	attribute aColor = 2 4

The attribute type is only necessary in the shader source file but not here. Using vec3 as attribute type in the source code but only use a count of 2 is no problem. The 3 component will be automatic be 0.

mesh
----
A mesh in glslScene corresponds to a draw call. A mesh is internally a vertex array or VBO (and VAO). Depends on the used opengl version (see context section).

A mesh is specified as follow:

	mesh
		id = MESH1
		vertex-layout      (or custom-vertex-layout)
			x	y	[z]	[s0	t0]	[s1	t1]	[[r	g	b]	a]
		triangles
			... values of 1. vertex of 1. triangle ...
			... values of 2. vertex of 1. triangle ...
			... values of 3. vertex of 1. triangle ...
			... values of 1. vertex of 2. triangle ...
			... values of 2. vertex of 2. triangle ...
			... values of 3. vertex of 2. triangle ...
			... values for more triangles ...

An example would be:

	mesh
		id = MESH1
		vertex-layout
			x	y	s0	t0	s1	t1	r	g	b	a
		triangles
			0	0	0	0	0	0	1	0	0	1
			100	0	1	0	1	0	0	1	0	1
			0	100	0	1	0	1	0	0	1	1
			100	0	1	0	1	0	0	1	0	1
			100	100	1	1	1	1	1	1	0	1
			0	100	0	1	0	1	0	0	1	1

`<id-name>` must be a text. Simple a number is not ok!

`vertex-layout` defines the layout of the vertices. A vertex must always have a position. x and y must exist. z is optional. Then none, one or more s,t texture coordinates can follow. Must have the name `s<N>`, `t<N>` (N can be 0 - 7). The color is optional. If a color is used then r, g, b or r, g, b, a must be used in this order.

If `custom-vertex-layout` is used instead of `vertex-layout` then other additional components can follow at the end of a vertex layout specification. In this case **all** components of the vertex must be specified by the shader as attributes. No automatic attribute binding is done.

framebuffer
-----------
A framebuffer is only useful if multi-pass rendering is used (see rendering section). A framebuffer is used for offsceen rendering. The *default framebuffer* (for the window) always exist. No extra entry inside the resources is necessary (or possible) for the default framebuffer.

	framebuffer
		id = <id-name>                            (must exist)
		size = <width> <height>  or  window-size  (must exist)

`<id-name>` must be a text. Simple a number is not ok!

`<width>` and `<height>` define the size of the framebuffer in pixels. Or instead of values for width and height also the keyword `window-size` can be use. If `window-size` is used and the window is resized then also the framebuffer is automatic resized. window-size is currently not implemented :-( **TODO: implement window-size**


scenes
======

The **scenes** section can have one or more scenes. Each scene must have a `id` attribute. A scene must have at least one entity. A scene can have one or more entities. The **scenes** section is specified as follow:

	scenes
		scene
			id = <id-name>                            (must-exist)
			entity                                    (must exist)
			... one or more entities can follow ...
		scene
			id = <id-name>                            (must-exist)
			entity                                    (must exist)
			... one or more entities can follow ...
		scene
			id = <id-name>                            (must-exist)
			entity                                    (must exist)
			... one or more entities can follow ...

A example with one scene:

	scenes
		scene
			id = SCENE_A
			entity
				transform-component
					translate 100 -60
					scale 2.0 2.0
				shader-component
					shader-program-id = SHADER1
				mesh-component
					mesh-id = MESH1

scene
-----

A scene is specified as follow:

	scene
		id = <id-name>                            (must-exist)
		entity                                    (must exist)
		... one or more entities can follow ...


`<id-name>` must be a text. Simple a number is not ok!

For create a `entity` see entity in the next section.

entity
------

glslScene use a entity-component-system. A entity is like a game object. This entity can have following components:

 * logic-component
 * transform-component
 * texture-component
 * shader-component
 * mesh-component

Each component can exist exactly once inside a entity. E.g. two mesh-component inside one entity is not possible.

The components **transform-component**, **texture-component** and **shader-component** are inherit to there child entities (if the entity has children).

A entity is specified as follow. The different attributes of a entity must have **exactly this order/sequence!!!**

	entity
		id = <id-name>            (OPTIONAL, doesn't have to exist like the other id's)
		name = <name>             (optional, doesn't have to be unique)
		active = <boolean>        (optional, default is true)
		logic-component           (optional, can be used once)
		transform-component       (optional, can be used once)
		texture-component         (optional, can be used once)
		shader-component          (optional, can be used once)
		mesh-component            (optional, can be used once)
		entity                    (optional, can be used multiple times)


`id` is optional. If used then `<id-name>` must be a text. Simple a number is not ok!

`name` is optional. `<name>` must be a text. Doesn't have to be unique.

`active` is optional. If used `<boolean>` must be `true` or `false`.

`<...>-component`: Each component is optional. For more informations about the components see the sections below.

`entity` is optional. A entity can have multiple children. And these children can also have children. And so on...

logic-component
---------------

A logic-component can have one or more logic's. Each logic must be programmed in C++ and the config-loader must be extended by this new logic. Only for special cases a logic-component is needed.

Currently only one logic exist. It's the *glsl-sandbox-logic*. If the glsl-sandbox-logic should be used then it looks like this:

	logic-component
		glsl-sandbox-logic


transform-component
-------------------

A transform-component is needed to transform the mesh and/or the children of its entity. If the entity has children then this transform-component is also applied to its children (and to their sub-children if they also have children).

A transform-component supports the following transformations:

 * translate
 * scale
 * rotate

Inside one transform-component this transformations can be used **multiple times** in **different order/sequence**.

A transform-component is specified as follow:

	transform-component
		translate <x> <y>
		translate <x> <y> <z>
		scale <scale-x> <scale-y>
		scale <scale-x> <scale-y> <center-x> <center-y>
		scale <scale-x> <scale-y> <scale-z>
		scale <scale-x> <scale-y> <scale-z> <center-x> <center-y> <center-z>
		rotate-degree <angle>
		rotate-degree <angle> <center-x> <center-y>
		rotate-degree <angle> <rot-axis-x> <rot-axis-y> <rot-axis-z>
		rotate-radian <angle>
		rotate-radian <angle> <center-x> <center-y>
		rotate-radian <angle> <rot-axis-x> <rot-axis-y> <rot-axis-z>

texture-component
-----------------

A texture-component is needed to **bind** one or more **textures** to one or more **texture units**. If the entity has children then this texture-component is also applied to its children (and to their sub-children if they also have children).

	texture-component
		tex-id-unit-0 = <res-id-name-or-null>         (optional)
		tex-id-unit-1 = <res-id-name-or-null>         (optional)
		tex-id-unit-2 = <res-id-name-or-null>         (optional)
		tex-id-unit-3 = <res-id-name-or-null>         (optional)
		tex-id-unit-4 = <res-id-name-or-null>         (optional)
		tex-id-unit-5 = <res-id-name-or-null>         (optional)
		tex-id-unit-6 = <res-id-name-or-null>         (optional)
		tex-id-unit-7 = <res-id-name-or-null>         (optional)

`<res-id-name-or-null>` must be the id-name of the texture or framebuffer or must be `null`. If a texture from the texture unit (which is binded by the parent entity) can be unbind for this entity with `null`.

shader-component
----------------

A shader-component is needed to **bind** a **shader program**. This shader (shader program) is used for the mesh (Each mesh is a draw call). If the entity has children then this shader is also binded to its children (and to their sub-children if they also have children).

	shader-component
		shader-id = <res-id-name-or-null>

`<res-id-name-or-null>` must be the id-name of the shader or must be `null`. If a shader is binded by the parent entity than this shader can be unbind for this entity with `null`.

`shader-id`: Instead of using the attribute name `shader-id` also the attribute name `shader-program-id` is allowed.

mesh-component
--------------
A mesh-component defines which mesh resource should be drawn (used for the draw call). A mesh is **not** inherit to its children.

	mesh-component
		mesh-id = <res-id-name>

`<res-id-name>` is the id-name of a mesh resource.

rendering
=========

The **rendering** section defines the different rendering passes. At least one render pass must exist. The last render pass must always render into the *default framebuffer* (= window).

The **rendering** section is specified as follow:

	rendering
		render-pass
			... frame buffer id , clear color and scene id informations ...
		render-pass
			... frame buffer id , clear color and scene id informations ...
		... more render passes can exist ...

An example for a single render-pass for the default framebuffer:

	rendering
		render-pass
			framebuffer-id = default
			clear-color = 0.5 0.5 0.5 1.0
			scene-id = SCENE-ONE

render-pass
-----------


	render-pass
		framebuffer-id = <res-id-name>    or   default    (must exist, default for default framebuffer)
		clear-color = <r> <g> <b> <a>                     (must exist)
		projection-matrix                                 (optional)
		view-matrix                                       (optional)
		scene-id = <scene-id-name>                        (must exist)
		depth-test = <boolean>                            (optional, default: false)

`framebuffer-id` must exist. `<res-id-name>` must be the id-name of the framebuffer resource or `default` for the default framebuffer.

`clear-color` must exist. `<r>` `<g>` `<b>` `<a>` must be floating values between 0.0 and 1.0 for each color channel.

`projection-matrix` is optional.

`view-matrix` is optional.

`scene-id` must exist. `<scene-id-name>` must be the id-name of the scene.

`depth-test` is optional. Default is `false`. If depth-test should be made (with Z-buffer) then it must be `true`.

projection-matrix
-----------------

Only one attribute name (`ortho`, `perspective` or `frustom`) is allowed to be used.

	projection-matrix
		ortho = <left> <right> <bottom> <top> <nearVal> <farVal> [<format-mode>]
		ortho = window-size
		ortho = window-size-center
		perspective = <fovy-radian> <aspect> <zNear> <zFar>
		perspective = <fovy-radian> window-ratio <zNear> <zFar>
		perspective-degree = <fovy-degree> <aspect> <zNear> <zFar>
		perspective-degree = <fovy-degree> window-ratio <zNear> <zFar>
		frustum = <left> <right> <bottom> <top> <zNear> <zFar>

`<format-mode>` for `ortho` is optinal.

`<format-mode>` can be: `stretch`, `shrink-to-fit`, `width-fit`, `height-fit`, `crop`

For perspective `window-ratio` can be used instead of define a value for `<aspect>` to use the correct window aspect ratio.

Example:

	projection-matrix
		ortho = -500 500 -500 500 -500 500 shrink-to-fit


view-matrix
-----------

view-matrix is specified as follow. `look-at` and/or `transform` must exist if view-matrix is used.

	view-matrix
		look-at = <eyeX> <eyeY> <eyeZ> [<centerX> <centerY> <centerZ> [<upX> <upY> <upZ>]]
		transform
			... transformations, syntax same as for transform-component ...

If no `<centerX>` `<centerY>` `<centerZ>` is used then the center point 0/0/0 is used.

If no `<upX>` `<upY>` `<upZ>` is used then an automatic calculated up vector is used.

Example:

	view-matrix
		look-at = 0.003 0.0 0.2

Example:

	view-matrix
		transform
			translate 300 0
			scale 2 2
Example:

	view-matrix
		transform
			translate -200 0


Scene Configuration Overview
============================

Overview:

	context
		rendering-api = opengl [es] [<version>] [<profile>]

	resources
		texture
			id = <id-name>               (must exist)
			filename = <filename>        (must exist)
			mipmap = <boolean>           (must exist)
			min-filter = <filter>        (must exist)
			mag-filter = <filter>        (must exist)

		shader
			id = <id-name>                          (must exist)
			<shader-type> = <shader-filename>       (optional, can be used multiple times in series)
			uniform <type> <var-name> = <special-keyword>                    (optional, multiple times)
			uniform int <var-name> = <ivalue>                                (optional, multiple times)
			uniform float <var-name> = <fvalue>                              (optional, multiple times)
			uniform vec2 <var-name> = <fvalue> <fvalue>                      (optional, multiple times)
			uniform vec3 <var-name> = <fvalue> <fvalue> <fvalue>             (optional, multiple times)
			uniform vec4 <var-name> = <fvalue> <fvalue> <fvalue> <fvalue>    (optional, multiple times)
			uniform sampler2d <var-name> = <texture-unit-index>              (optionalm, mult. t., texture unit 0 - 7)
			attribute <var-name> = <component-offset> <component-count>
		mesh
			id = MESH1
			vertex-layout      (or custom-vertex-layout)
				x	y	[z]	[s0	t0]	[s1	t1]	[[r	g	b]	a]
			triangles
				... values of 1. vertex of 1. triangle ...
				... values of 2. vertex of 1. triangle ...
				... values of 3. vertex of 1. triangle ...
				... values of 1. vertex of 2. triangle ...
				... values of 2. vertex of 2. triangle ...
				... values of 3. vertex of 2. triangle ...
				... values for more triangles ...
		framebuffer
			id = <id-name>                            (must exist)
			size = <width> <height>  or  window-size  (must exist)

	scenes
		scene
			id = <id-name>                            (must-exist)
			entity                                    (must exist)
				id = <id-name>            (OPTIONAL, doesn't have to exist like the other id's)
				name = <name>             (optional, doesn't have to be unique)
				active = <boolean>        (optional, default is true)
				logic-component           (optional, can be used once)
				transform-component       (optional, can be used once)
					translate <x> <y>
					translate <x> <y> <z>
					scale <scale-x> <scale-y>
					scale <scale-x> <scale-y> <center-x> <center-y>
					scale <scale-x> <scale-y> <scale-z>
					scale <scale-x> <scale-y> <scale-z> <center-x> <center-y> <center-z>
					rotate-degree <angle>
					rotate-degree <angle> <center-x> <center-y>
					rotate-degree <angle> <rot-axis-x> <rot-axis-y> <rot-axis-z>
					rotate-radian <angle>
					rotate-radian <angle> <center-x> <center-y>
					rotate-radian <angle> <rot-axis-x> <rot-axis-y> <rot-axis-z>
				texture-component         (optional, can be used once)
					tex-id-unit-0 = <res-id-name-or-null>         (optional)
					tex-id-unit-1 = <res-id-name-or-null>         (optional)
					tex-id-unit-2 = <res-id-name-or-null>         (optional)
					tex-id-unit-3 = <res-id-name-or-null>         (optional)
					tex-id-unit-4 = <res-id-name-or-null>         (optional)
					tex-id-unit-5 = <res-id-name-or-null>         (optional)
					tex-id-unit-6 = <res-id-name-or-null>         (optional)
					tex-id-unit-7 = <res-id-name-or-null>         (optional)
				shader-component          (optional, can be used once)
					shader-id = <res-id-name-or-null>
				mesh-component            (optional, can be used once)
					mesh-id = <res-id-name>
				entity                    (optional, can be used multiple times)
			... one or more entities can follow ...
		scene
			id = <id-name>                            (must-exist)
			entity                                    (must exist)
			... one or more entities can follow ...
		scene
			id = <id-name>                            (must-exist)
			entity                                    (must exist)
			... one or more entities can follow ...

	rendering
		render-pass
			framebuffer-id = <res-id-name>    or   default    (must exist, default for default framebuffer)
			clear-color = <r> <g> <b> <a>                     (must exist)
			projection-matrix                                 (optional)
				ortho = <left> <right> <bottom> <top> <nearVal> <farVal> [<format-mode>]
				ortho = window-size
				ortho = window-size-center
				perspective = <fovy-radian> <aspect> <zNear> <zFar>
				perspective = <fovy-radian> window-ratio <zNear> <zFar>
				perspective-degree = <fovy-degree> <aspect> <zNear> <zFar>
				perspective-degree = <fovy-degree> window-ratio <zNear> <zFar>
				frustum = <left> <right> <bottom> <top> <zNear> <zFar>
			view-matrix                                       (optional)
				look-at = <eyeX> <eyeY> <eyeZ> [<centerX> <centerY> <centerZ> [<upX> <upY> <upZ>]]
				transform               (syntax same as for transform-component)
					translate <x> <y>
					translate <x> <y> <z>
					scale <scale-x> <scale-y>
					scale <scale-x> <scale-y> <center-x> <center-y>
					scale <scale-x> <scale-y> <scale-z>
					scale <scale-x> <scale-y> <scale-z> <center-x> <center-y> <center-z>
					rotate-degree <angle>
					rotate-degree <angle> <center-x> <center-y>
					rotate-degree <angle> <rot-axis-x> <rot-axis-y> <rot-axis-z>
					rotate-radian <angle>
					rotate-radian <angle> <center-x> <center-y>
					rotate-radian <angle> <rot-axis-x> <rot-axis-y> <rot-axis-z>
			scene-id = <scene-id-name>                        (must exist)
			depth-test = <boolean>                            (optional, default: false)
		render-pass
			... frame buffer id , clear color and scene id informations ...
		render-pass
			... frame buffer id , clear color and scene id informations ...

