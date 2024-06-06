# AssetPacker
Pack files into a compressed DATA.pck file.

## Building This Project
You will need Visual Studio 2022. Additionally, you will need zlib. The project is set up to work with zlib from this [repository](https://github.com/kiyolee/zlib-win-build). It will probably be easier to simply change the include and library directories to link to your local x64 version of zlib and to change the input file from libz-static.lib to whatever the lib file is named.

## Using This Library
This project requires C++ 17, and additionally targets Visual Studio 2022. It also depends on the zlib library. You must link to zlib when using this library in your own project.
