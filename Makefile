
all: build compile

build:
	-mkdir build
ifeq ($(OS),Windows_NT)
	rem visual studio 2017 (version number 15.0)
	cd build && cmake -G "Visual Studio 15" ..
	rem xcopy "res" "build\res\" /s/h/e/k/f/c
	xcopy "lib\SDL2-2.0.10\lib\x86\*.dll" "build" /s/h/e/k/f/c
	xcopy "lib\glew-2.1.0\bin\Release\Win32\*.dll" "build" /s/h/e/k/f/c
else
	cd build; cmake ..
endif

compile:
ifeq ($(OS),Windows_NT)
	@echo ""
	@echo "Open the project with Visual Studio and compile it ;-)"
else
	make -C build
endif

clean:
ifeq ($(OS),Windows_NT)
	-rmdir build /s /q
else
	rm -rf build
endif

