# BSP组件需要编译的源码目录；CMake会在LED目录中查找源文件。
set(src_dirs
            LED)

# 对其他组件公开的头文件搜索目录，因此main.c可以直接#include "led.h"。
set(include_dirs
            LED)

# 本组件依赖ESP-IDF的driver组件，GPIO驱动由该组件提供。
set(requires
            driver)

# 将上面整理的源码、头文件目录和依赖注册为一个ESP-IDF组件。
idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

# 为本组件增加编译选项：快速浮点优化、O3优化，以及特定格式警告设置。
component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)
