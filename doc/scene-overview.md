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


