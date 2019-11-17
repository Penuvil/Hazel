@echo off

pushd %VULKAN_SDK%\Lib
pushd %VULKAN_SDK%\shaderc

mkdir build
pushd %VULKAN_SDK%\shaderc\build
cmake -G "Visual Studio 15 Win64" ..
cmake --build . --config Debug

popd
popd
mkdir Debug
copy %VULKAN_SDK%\shaderc\build\libshaderc\Debug\shaderc_combined.lib Debug\
copy v*.* Debug\
popd
PAUSE