# PHP single purpose testing executables

This repository contains source codes for various executables that may be used for unit tests of PHP scripts.

## Usage

Before starting PHPUnit, you must compile the binaries using CMake. See below for the instructions or check out `*-build.*` scripts.

### Compilation

In order to allow any [build-automation utility](https://en.wikipedia.org/wiki/Build_automation) this package is shipped with a simple [CMakeLists.txt](https://cmake.org/cmake/help/latest/manual/cmake.1.html#generate-a-project-buildsystem) file. It is configured to output compiled binaries in a `bin` folder at the root of this package. You have to make sure you have a native build environments already installed on your machine ([Visual Studio](https://en.wikipedia.org/wiki/Microsoft_Visual_Studio), [make](https://en.wikipedia.org/w/index.php?title=Make_software), [Xcode](https://en.wikipedia.org/wiki/Xcode), ...)

After getting this repository, `cd` to it and [generate the project buildsystem](https://cmake.org/cmake/help/latest/manual/cmake.1.html#generate-a-project-buildsystem).

```shell
mkdir build
cd build
cmake ..
```

Then compile the executables by [building the project](https://cmake.org/cmake/help/latest/manual/cmake.1.html#build-a-project) from this `build` directory.

```shell
cmake --build .
```

To prevent debug informations from being generated you may add the `--config Release` option.

```shell
cmake --build . --config Release
```
