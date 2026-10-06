cmake_minimum_required(VERSION 3.20)
project(avp2_stubbins LANGUAGES CXX RC)

# AvP2 and its engine are 32-bit; a 64-bit DLL can't load into lithtech.exe.
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "AVP2 Mod Stub must be built as Win32: cmake -S . -B build -A Win32")
endif()

# Settings shared by the two code DLLs (cshell.dll and object.lto).
function(stub_code_dll target)
    set_target_properties(${target} PROPERTIES
        # Static CRT, so nothing extra (VC++ redistributable) needs installing.
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    # Our engine interface headers (include/), not the LithTech SDK.
    target_include_directories(${target} PRIVATE "${CMAKE_SOURCE_DIR}/include")
    target_compile_definitions(${target} PRIVATE
        WIN32
        _WINDOWS
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
    )
    target_compile_options(${target} PRIVATE /W3 /EHsc /GR /permissive-)
endfunction()

# Every DLL: keep only the PDB file name in the binary, not the full build path
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /PDBALTPATH:%_PDB%")

# cshell.dll
add_library(cshell SHARED
    src/client/ClientShell.cpp
)
stub_code_dll(cshell)
target_include_directories(cshell PRIVATE res/cres)
target_link_libraries(cshell PRIVATE user32)

# object.lto - the server shell and object classes.
add_library(object SHARED
    src/server/ServerShell.cpp
    src/server/ObjectList.cpp
)
stub_code_dll(object)
set_target_properties(object PROPERTIES SUFFIX ".lto")

# cres.dll / sres.dll - resource-only DLLs, no code.
add_library(cres SHARED res/cres/cres.rc)
add_library(sres SHARED res/sres/sres.rc)
foreach(res_dll cres sres)
    set_target_properties(${res_dll} PROPERTIES LINKER_LANGUAGE CXX)
    target_link_options(${res_dll} PRIVATE /NOENTRY)
endforeach()
