## How to build Digital Scenario Framework ##

Projects for Microsoft [Visual Studio] 2015 and [Code::Blocks] 20 (IDE [version 20.03](https://sourceforge.net/projects/codeblocks/files/Binaries/20.03/Windows/) *mingw-setup*) are provided, but the code should be compliant with any compiler supporting C++ 14.
Using newer versions of [Visual Studio] should work (successfully tested with [Visual Studio] 2017).
[Code::Blocks] projects are configured to build with [gcc] compiler (8.1 included [MinGW] setup of [Code::Blocks]).

The [Visual Studio] 2015 projects are configured to support 8.1 target platform for backward compatibility, but this can be easily updated (upgrading to [Visual Studio] 2022 and above require to change the target platform).

**Warning for Code::Blocks:** probably it is not possible to link 32 bit libraries with the default Code::Blocks/mingw 64 bit installation, 32 bit libraries are not found.
The toolchain installed along with Code::Blocks-mingw (64 bit) cannot build 32 bit.
[You need to install the 32 bit toolchain yourself and point CodeBlocks to use that one to build for 32 bit.](https://forums.codeblocks.org/index.php/topic,23817.msg162483.html#msg162483)


### Dependencies

All libraries are linked **statically** when building the framework libraries.

After external dependencies are build (see [this document](./DEPENDENCIES.md) for details), the folder `deps` will include a subset of the external libraries needed to build this project.

Please note that the external dependencies are not needed to use the compiled framework libraries, header files and libraries binaries are enough.

### Framework and examples

The [Visual Studio] solution and [Code::Blocks] workspace to build all the projects (excluding dependencies) are in the root folder of this repository.


[Visual Studio]: https://visualstudio.microsoft.com/
[Code::Blocks]: https://www.codeblocks.org/
