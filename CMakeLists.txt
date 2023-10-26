cmake_minimum_required(VERSION 3.10)

project(Terrim)
set(CMAKE_BUILD_TYPE Debug)  # Ensure we're compiling with optimizations
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_UNUSED_VARIABLE_CHECKS ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
# set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-auto-import")


# 添加源文件列表
set(SOURCE_FILES
        src/cstring.c
        src/format.c
        src/header.c
        src/tiles.c
        src/pixels.c
        src/lodepng.c
)




# 添加动态库 (.dll)
add_library(TerrimDLL SHARED ${SOURCE_FILES}
        include/format.h
        src/main.c)
target_include_directories(TerrimDLL PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(TerrimDLL PRIVATE )

# 添加静态库 (.a)
add_library(TerrimStatic STATIC ${SOURCE_FILES})
target_include_directories(TerrimStatic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(TerrimStatic PRIVATE )

# 添加可执行文件
add_executable(Terrim src/main.c ${SOURCE_FILES})
set_target_properties(Terrim PROPERTIES CXX_Unused_Variable_Check ON)
target_include_directories(Terrim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Terrim PRIVATE )
target_compile_options(Terrim PRIVATE -O3)




# 添加一个后置步骤来运行可执行文件
#add_custom_command(
#        TARGET main
#        POST_BUILD
#        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/main
#)

# enable_testing() # 启用测试

# add_executable(tests ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_async.c ) # 添加测试文件

# target_link_libraries(tests  Threads::Threads) # 链接 tcolor 库

# add_test(NAME test_async COMMAND tests) # 添加测试