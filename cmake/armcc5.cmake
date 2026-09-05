# 指定目标系统为 Generic，表示固件运行在没有 Windows/Linux 操作系统的裸机平台上。
set(CMAKE_SYSTEM_NAME Generic)

# 指定目标处理器体系为 ARM；真正的 Cortex-M3 参数会在后面的编译选项中设置。
set(CMAKE_SYSTEM_PROCESSOR ARM)

# 在 PATH 等搜索位置中查找 ninja，并把完整路径保存到 NINJA_EXECUTABLE；找不到时停止配置。
find_program(NINJA_EXECUTABLE NAMES ninja REQUIRED)

# 查找 ARMCC 5 的 C 编译器 armcc，并把完整路径保存到 ARMCC_EXECUTABLE。
find_program(ARMCC_EXECUTABLE NAMES armcc REQUIRED)

# 查找 ARMCC 5 的汇编器 armasm，并把完整路径保存到 ARMASM_EXECUTABLE。
find_program(ARMASM_EXECUTABLE NAMES armasm REQUIRED)

# 查找 ARMCC 5 的链接器 armlink，并把完整路径保存到 ARMLINK_EXECUTABLE。
find_program(ARMLINK_EXECUTABLE NAMES armlink REQUIRED)

# 查找 ARMCC 5 的静态库归档器 armar，并把完整路径保存到 ARMAR_EXECUTABLE。
find_program(ARMAR_EXECUTABLE NAMES armar REQUIRED)

# 查找 ARMCC 5 的映像转换工具 fromelf，并把完整路径保存到 FROMELF_EXECUTABLE。
find_program(FROMELF_EXECUTABLE NAMES fromelf REQUIRED)

# 告诉 CMake 使用找到的 Ninja 执行构建规则，并将路径作为 FILEPATH 类型缓存变量保存。
set(CMAKE_MAKE_PROGRAM "${NINJA_EXECUTABLE}" CACHE FILEPATH "Ninja executable")

# 告诉 CMake 使用找到的 armcc.exe 编译 C 源文件。
set(CMAKE_C_COMPILER "${ARMCC_EXECUTABLE}")

# 告诉 CMake 使用找到的 armasm.exe 汇编启动文件等汇编源文件。
set(CMAKE_ASM_COMPILER "${ARMASM_EXECUTABLE}")

# 把找到的 armlink.exe 交给 CMake 内置的 ARMCC 支持模块，用于最终链接。
set(CMAKE_ARMCC_LINKER "${ARMLINK_EXECUTABLE}")

# 把找到的 armar.exe 交给 CMake 内置的 ARMCC 支持模块，用于创建静态库。
set(CMAKE_ARMCC_AR "${ARMAR_EXECUTABLE}")

# 保存 fromelf.exe 的路径，供 CMakeLists.txt 在链接成功后生成 HEX 和 BIN。
set(CMAKE_FROMELF "${FROMELF_EXECUTABLE}" CACHE FILEPATH "ARM image conversion tool")

# 初始化 C 编译器公共参数，使 armcc 为 Cortex-M3 生成代码。
set(CMAKE_C_FLAGS_INIT "--cpu Cortex-M3")

# 初始化汇编器公共参数，使 armasm 按 Cortex-M3 指令集汇编启动文件。
set(CMAKE_ASM_FLAGS_INIT "--cpu Cortex-M3")

# 让 CMake 的编译器可用性测试生成静态库，避免检测阶段因缺少启动文件和 scatter 文件而链接失败。
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
