@echo off

:: First clear the CMake cache:
if exist bin\\CMakeCache.txt del /f bin\\CMakeCache.txt
if exist bin\\CMakeFiles rd /s /q "bin\\CMakeFiles

:: Then generate all the build files:
cmake -S build/ -B bin -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ^
	  -DBUILD_TYPE="Debug" ^
	  -DBUILD_ENABLE_LOGS=1 ^
	  -DPROJECT_NAME="CV" ^
	  -DPROJECT_ENABLE_LOGS=1 ^
	  -DPROJECT_ENGINE_BACKEND="Raylib"
