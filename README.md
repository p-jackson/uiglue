# uiglue

Two-way databinding library for Windows. The primary motivation is to facilitate
the [MVVM](http://en.wikipedia.org/wiki/Model_View_ViewModel) design pattern.

## Build

uiglue requires a C++11 compiler and CMake to generate the project files.

For example, to generate a 64-bit project using Visual Studio 2013 and the
November 2013 CTP run:

    $ cmake ..\path\to\source -G "Visual Studio 12 Win64" -T CTP_Nov2013

## curt

This includes a small Win32 wrapper called curt. The purpose of this wrapper is
to provide RAII for handle types and handle exceptions.
