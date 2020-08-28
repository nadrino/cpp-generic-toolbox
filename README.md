![](resources/logo/logo.png)

## Description

C++ Generic Toolbox is a **header-only** namespace which aims at being included in any cpp project. This toolbox gathers a collection of useful functions which I got tired of copy pasting over my different projects...

## Features

- `std::string` management tools
- Filesystem/IO tools
- Hardware tools (like RAM info)
- Conversion tools
- Displaying tools (like a simple progress bar)


## Showcase

![](resources/screenshot/screenExample.png)

## Requirements

- **C++11 or higher is required to use this toolbox.**

## Getting Setup

You can add cpp-generic-toolbox to your existing project as a submodule for example. In this guide we assume that your project is using CMake. In your CMakeLists.txt, add the following lines:

```cmake
include_directories(path/to/simple-cpp-logger/include)
```

Now you can include the header in your source file:

```c++
#include <GenericToolbox.h>
```
