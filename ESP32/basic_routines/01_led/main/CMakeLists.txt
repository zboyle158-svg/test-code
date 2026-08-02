# 将main.c注册为ESP-IDF组件的源文件。
# INCLUDE_DIRS "."表示该组件可以从当前目录查找头文件。
idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")
