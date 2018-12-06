What is cpplibsocket?
---------------------

cpplibsocket is a very lightweight system socket RAII wrapper.

 - Written in a modern C++14 standard
 - 3-Clause BSD License
 - Supports both IPv4 and IPv6 standards
 - For now only supports TCP and UDP protocols

Dependencies
------------
 
To build cpplibsockets, you will need [CMake](https://cmake.org/) (>= 3.0).

Building cpplibsockets
----------------------
 
 - Linux:
   - `git clone ...`
   - `cd cpplibsockets`
   - `mkdir build && cd build`
   - `cmake --DCMAKE_BUILD_TYPE=Release ..`
  
 - Windows (Visual Studio 2017 64bit build):
   - `git clone ...`
   - `cd cpplibsockets`
   - `mkdir build && cd build`
   - `cmake.exe -G "Visual Studio 15 2017 Win64" ..`
   - `msbuild.exe cpplibsocket.sln /t:cpplibsocket /property:Configuration=Release /property:Platform=x64 /m /nologo /verbosity:normal`