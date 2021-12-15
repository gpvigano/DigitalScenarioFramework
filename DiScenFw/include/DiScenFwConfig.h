/*
--------------------------------------------------------------------
 Digital Scenario Framework
  by Giovanni Paolo Vigano', 2021
--------------------------------------------------------------------

 Distributed under the MIT Software License.
 See http://opensource.org/licenses/MIT
*/

#pragma once

#ifdef _WIN32
#   ifdef DISCENFW_EXPORT
#       define DISCENFW_API __declspec(dllexport)
#   elif defined(DISCENFW_STATIC)
#       define DISCENFW_API
#   else
#       define DISCENFW_API __declspec(dllimport)
#       define DISCENFW_IMPORT
#   endif
#elif __GNUC__ >= 4
#   define DISCENFW_API __attribute__((visibility("default")))
#else
#   define DISCENFW_API
#endif


#if defined(DISCENFW_EXPORT) || defined(DISCENFW_STATIC)
#   define DISCENFW_DLL_PROTECTED public:
#else
#   define DISCENFW_DLL_PROTECTED protected:
#endif

/*!
@page discenfw_howto How to use this library
The main header file to be included by a client C++ module is
DigitalScenarioFramework.h, while if the client module uses a
different language (C, C#, ...), the reference header files are
DiScenApi.h and DiScenXp.h (or DiScenFw.h to include both of them).
This library is provided as a dynamic linking library (see
@ref discenfw_implementation for further details).
*/

/*!
@page discenfw_implementation Implementation details
This library is provided as a dynamic linking library, but different
configurations can be created in the project to link it as a static library.
@section dynamic_linkage Dynamic linkage
Linking this library as a dynamic linking library, it must be granted
that memory allocations and deallocations are done in the same module.
The DigitalScenarioFramework methods provide a safe memory management:
parameters are passed by copy (for basic C++ types) or as read-only
references (for STL types and complex types); method results are
returned by copy or as read-only references.
Anyway, for the implementation of the library itself, or whether the
library is statically linked, a proper macro in class declarations
(@ref DISCENFW_DLL_PROTECTED) enables public access to the class
members following the macro, otherwise (dynamically linked library),
it declares the following section as protected, thus preventing public
access from other DLL client applications or modules.

@subsection DISCENFW_DLL_PROTECTED
This macro enables public or protected access to the following class members
until the next access modifier, according to the library link mode.
For the implementation of the library itself, or whether the library
is statically linked, the macro is expanded as @c public:, otherwise
(dynamically linked library) it is expanded as @c protected:, preventing
public access from other DLL client applications or modules.

@section external_dependecies External dependencies
This library depends on other external libraries:
@li boost: https://www.boost.org/
@li rapidjson: https://github.com/miloyip/rapidjson/
@li gpvulc: https://github.com/gpvigano/gpvulc

Anyway these dependencies are needed only for compilation.
Once compiled, this library does not have dependecies on external
header files or libraries. Of course, if a plugin is needed for a
cyber system (CyberSystemPlugin), the corresponding DLL must be
available (see CyberSystemPlugin and CyberSystemLink).
*/
