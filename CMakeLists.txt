cmake_minimum_required(VERSION 3.16)
project(24300120181_李佳羲_大作业)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 SFML 路径
set(SFML_DIR "${CMAKE_SOURCE_DIR}/SFML-2.6.2")
link_directories(${SFML_DIR}/lib)

# 添加源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(main ${SOURCES})

# 编译选项
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 包含头文件目录
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/element
    ${CMAKE_CURRENT_SOURCE_DIR}/src/screen
    ${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/include
)


# 链接 SFML 动态库
target_link_libraries(main PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# 拷贝 SFML 的 DLL 到输出目录
file(GLOB SFML_DLLS "${SFML_DIR}/bin/*.dll")
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SFML_DLLS}
        $<TARGET_FILE_DIR:main>
)

# 拷贝资源文件夹
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:main>/assets
)
