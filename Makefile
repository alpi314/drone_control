build:
	cmake --preset drone_control_vcpkg
	cmake --build build

run: build
	./build/Debug/drone_control.exe