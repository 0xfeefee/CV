@echo off

:: First clear the CMake cache:
if exist bin\CMakeCache.txt del /f bin\CMakeCache.txt

:: Then generate all the build files:
cmake -S build/ -B bin -G Ninja ^
	  -DBUILD_TYPE=Debug ^
	  -DBUILD_COMPILER="MSVC" ^
	  -DBUILD_ENABLE_LOGS=1 ^
	  -DPROJECT_NAME="CV" ^
	  -DPROJECT_ENABLE_LOGS=1 ^
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=1
