## NRT.CRT Project Note

### Overview

The goal of this project is to provide a lightweight, simple, clean utility for modular modern C++ application development. 
This project is intended to work under hosted environment, and should never be expected to work standalone. 
Due to shortage of man power, this library has a coverage limitations. 

### Meta library
Provides a interface for injecting class metadata into binary for runtime use.
Class load/unload hooks is provided

Currently we only grantee its behavior correctness for code which is compiled into 
executables and shared libraries with MSVC and GNU compatible compilers.

Currently implemented with type_info. If possible, use C++ standard introspection to
provide more information.

### System library
Provide unified interface for system functions such as shared library management,
timer querying, and an basic NEWorld block-based memory management interface.

### Object library
Defines NEWorld objects and facets with reversed reference table to implement
ordered initialization and finalization with dependency injection as middleware.

### Service library
Implemented using meta-info hooks, this library provides basic dependency injection
management and basic automatic service instance lifecycle management.
