
cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(_UFW_UHEAP_INIT_ OFF)

function(UHEAP_INIT TARGET)
    if(NOT _UFW_UHEAP_INIT_)
        message(STATUS "UHEAP: Heap init")
        file(GLOB_RECURSE __L_HEAP_SRC  RELATIVE ${PROJECT_SOURCE_DIR} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/heap/uheap.*" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/uheap_allocator.h")
        file(GLOB_RECURSE __L_HEAP_HOOKS_SRC  RELATIVE ${PROJECT_SOURCE_DIR} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/heap/_uheap_hooks.c")
        file(GLOB_RECURSE __L_HEAP_OPTIONS  RELATIVE ${PROJECT_SOURCE_DIR} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/uheap_opt.h")
        message(STATUS "UHEAP INIT:${__L_HEAP_SRC} ${__L_HEAP_HOOKS_SRC} ${__L_HEAP_OPTIONS}")
        target_sources(${TARGET} PUBLIC ${__L_HEAP_HOOKS_SRC} ${__L_HEAP_OPTIONS} ${__L_HEAP_SRC})
        target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_FUNCTION_LIST_DIR})
        target_compile_definitions(${TARGET} PUBLIC UFW_USING_UHEAP_ALLOCATION)
        set(_UFW_UHEAP_INIT_ ON PARENT_SCOPE)
     else()
         message(STATUS "UHEAP: Heap already inited!")
     endif()
endfunction()

function(UHEAP_NEW TARGET)
    message(STATUS "UHEAP_NEW invoked")
    #
    if(NOT _UFW_UHEAP_INIT_)
        uheap_init(${TARGET})
        set(_UFW_UHEAP_INIT_ ON PARENT_SCOPE)
    endif()
    #
    file(GLOB_RECURSE __L_NEW_SRC  RELATIVE ${PROJECT_SOURCE_DIR} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/heap/uheap_new.cpp")
    message(STATUS "UHEAP NEW:${__L_NEW_SRC}")
    target_compile_definitions(${TARGET} PUBLIC UHEAP_OVERRIDES_NEW=1)
    target_sources(${TARGET} PUBLIC ${__L_NEW_SRC})
endfunction()

 function(UHEAP_MALLOC TARGET)
     message(STATUS "UHEAP_MALLOC invoked")
     #
     if(NOT _UFW_UHEAP_INIT_)
         uheap_init(${TARGET})
         set(_UFW_UHEAP_INIT_ ON PARENT_SCOPE)
     endif()
     #
     file(GLOB_RECURSE __L_CINTERFACE_SRC  RELATIVE ${PROJECT_SOURCE_DIR} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/heap/*_wrapper.*")
     file(GLOB_RECURSE __L_MALLOC_SRC  RELATIVE ${PROJECT_SOURCE_DIR} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/heap/udynmemory.*")
     message(STATUS "UHEAP MALLOC:${__L_CINTERFACE_SRC} ${__L_MALLOC_SRC}")

     target_compile_definitions(${TARGET} PUBLIC UHEAP_WRAPS_MALLOC)
     target_sources(${TARGET} PUBLIC ${__L_CINTERFACE_SRC} ${__L_MALLOC_SRC})
 endfunction()

# Must init heap and add wrappers to reent versions of C allocation functions
# function(UHEAP_NEWLIB_MALLOC TARGET)
#     message(STATUS "UHEAP_NEWLIB_MALLOC invoked")
#     #
#     if(NOT _UFW_UHEAP_INIT_)
#         uheap_init(${TARGET})
#         set(_UFW_UHEAP_INIT_ ON PARENT_SCOPE)
#     endif()

#     file(GLOB_RECURSE __L_CINTERFACE_SRC  RELATIVE ${PROJECT_SOURCE_DIR} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/heap/*_wrapper.*")
#     file(GLOB_RECURSE __L_MALLOC_SRC  RELATIVE ${PROJECT_SOURCE_DIR} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/udynmemory.*")
#     message(STATUS "UHEAP NEWLIB:${__L_CINTERFACE_SRC} ${__L_MALLOC_SRC}")
#     target_sources(${TARGET} PUBLIC ${__L_CINTERFACE_SRC} ${__L_MALLOC_SRC})
# endfunction()
