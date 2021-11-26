project(MinimalSfmlAsSubmodule)
cmake_minimum_required(VERSION 3.18.1)

macro( cl_set_output_directories PATH )
    message( STATUS "Output directories set to ${PATH}" )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PATH}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PATH}/bin )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PATH}/lib )
    foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
        string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
        set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
        set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
        set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
    endforeach()
endmacro()

if (MSVC)
    set( CompilerFlags CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE )
    foreach( CompilerFlag ${CompilerFlags} )
        string(REGEX REPLACE "/MDd|/MTd|/MD|/MT|/W0|/W3" "" ${CompilerFlag} ${${CompilerFlag}})
    endforeach()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /utf-8")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /utf-8")
endif()

cl_set_output_directories(${CMAKE_BINARY_DIR}/out)

set(HEADERS
)

set(SOURCES
    src/main.cpp
)

add_executable(demo ${SOURCES} ${HEADERS})
target_include_directories(demo PUBLIC include)

# SFML
if (WIN32)
    set( BUILD_SHARED_LIBS FALSE )
    set( SFML_STATIC_LIBRARIES TRUE )
    set( SFML_USE_STATIC_STD_LIBS TRUE )
    target_compile_definitions( demo PUBLIC -DSFML_STATIC )

    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set (PLATFORM_TYPE "x86")
    else()
        set (PLATFORM_TYPE "x64")
    endif()
    add_custom_target(
        Multimedia-SFML-deps
        COMMENT
        "SFML runtime dependencies copied to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/SFML/extlibs/bin/${PLATFORM_TYPE}/openal32.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    add_dependencies(demo Multimedia-SFML-deps)
endif()
add_subdirectory( third_party/SFML )
target_link_libraries(demo sfml-network)
target_link_libraries(demo sfml-audio)
target_link_libraries(demo sfml-graphics)
target_link_libraries(demo sfml-window)
target_link_libraries(demo sfml-system)
target_include_directories( demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SFML/include )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Greatmatis_-_Crossroads.flac
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Greatmatis_-_Drama_Initiative.flac
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
