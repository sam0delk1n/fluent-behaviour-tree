#===============================================================================================================================
#
# This file is part of the sam0delk1n's Conventional Tools (smdCT).
#
# Copyright © 2018 sam0delk1n <skipper05ster@gmail.com>.
# All rights reserved.
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
#
#===============================================================================================================================

cmake_minimum_required( VERSION 3.10.2 )

message( "\nWIN32=${WIN32}" )
message( "UNIX=${UNIX}" )
message( "\nMINGW=${MINGW}" )
message( "MSVC=${MSVC}" )
message( "CMAKE_COMPILER_IS_GNUCXX=${CMAKE_COMPILER_IS_GNUCXX}" )
message( "\nCMAKE_GENERATOR=${CMAKE_GENERATOR}" )
message( "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" )
message( "\nCMAKE_C_COMPILER=${CMAKE_C_COMPILER}" )
message( "CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}\n" )

set( CMAKE_CXX_STANDARD          17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS        OFF )

add_definitions( "-DUNICODE" "-D_UNICODE" )

if( CMAKE_GENERATOR MATCHES "MinGW Makefiles" OR CMAKE_GENERATOR MATCHES "Unix Makefiles" )
    add_compile_options( "-Wall" "-Wextra" "-Wshadow" "-pedantic" "-m64" "-march=x86-64" "-mfpmath=sse" )
    #add_compile_options( "-flto"/"-fno-lto" "-Wl,--strip-all" "-fverbose-asm" "-masm=intel" )
    #add_compile_options( "-static" "-static-libgcc" "-static-libstdc++" "-shared-libgcc" )
elseif( CMAKE_GENERATOR MATCHES "Visual Studio 15 2017 Win64" )
    add_compile_options( "/W4" )
endif()

file( GLOB SOURCE "examples/*.cpp" )
add_executable( "examples" ${SOURCE} )

file( GLOB SOURCE "sanity-check/*.cpp" )
add_executable( "fbt-check" ${SOURCE} )
