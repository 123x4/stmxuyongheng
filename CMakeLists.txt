# cmake --workflow --preset armcc5-full
# STM32_Programmer_CLI -c port=SWD -w ".\out\build\armcc5\STM32SX.hex" -v -rst
# 要求运行本工程的 CMake 版本不低于 3.20，并采用相应版本的规则行为。
cmake_minimum_required(VERSION 3.20)

# 创建名为 STM32SX 的工程，并启用 C 语言和汇编语言支持。
project(STM32SX LANGUAGES C ASM)

# 创建源文件列表变量 FIRMWARE_SOURCES，供后面的固件目标统一引用。
set(FIRMWARE_SOURCES
    # 应用程序入口和 I2C 业务代码。
    user/main.c
    # STM32 系统时钟初始化及 SystemInit 等函数的实现。
    user/system_stm32f10x.c
    # STM32 异常与中断处理函数，其中包含 SysTick_Handler。
    user/stm32f10x_it.c
    # 延时函数及系统节拍变量的实现。
    my_lib/delay.c
    # RCC 标准外设驱动，为时钟配置函数提供实现。
    std_periph_driver/src/stm32f10x_rcc.c
    # GPIO 标准外设驱动，为 GPIO 配置和读写函数提供实现。
    std_periph_driver/src/stm32f10x_gpio.c
    # I2C 标准外设驱动，为 I2C 配置和数据收发函数提供实现。
    std_periph_driver/src/stm32f10x_i2c.c
    # STM32F10x 中容量器件的启动文件，提供向量表和复位入口。
    startup/startup_stm32f10x_md.s
)

# 用上述源文件创建名为 STM32SX 的可执行目标；在这里它表示 MCU 程序映像。
add_executable(STM32SX ${FIRMWARE_SOURCES})

# 把 STM32SX 目标的输出后缀设为 .axf，使链接结果名为 STM32SX.axf。
set_target_properties(STM32SX PROPERTIES SUFFIX ".axf")

# 为 STM32SX 目标设置头文件搜索目录；PRIVATE 表示这些目录只属于该目标自身。
target_include_directories(STM32SX PRIVATE
    # STM32 芯片、CMSIS 和标准外设驱动头文件所在目录。
    ${CMAKE_CURRENT_SOURCE_DIR}/std_periph_driver/inc
    # 用户配置、系统初始化和中断头文件所在目录。
    ${CMAKE_CURRENT_SOURCE_DIR}/user
    # 自定义 delay.h 等头文件所在目录。
    ${CMAKE_CURRENT_SOURCE_DIR}/my_lib
)

# 为 STM32SX 的源文件添加预处理宏定义。
target_compile_definitions(STM32SX PRIVATE
    # 选择 STM32F10x 中容量器件配置，STM32F103C8 属于该类别。
    STM32F10X_MD
    # 启用 STM32 标准外设驱动库声明。
    USE_STDPERIPH_DRIVER
)

# 根据当前源文件的语言，为 STM32SX 添加不同的编译或汇编参数。
target_compile_options(STM32SX PRIVATE
    # 仅编译 C 文件时使用 C99、关闭优化并禁用多字节字符处理。
    $<$<COMPILE_LANGUAGE:C>:--c99;-O0;--no_multibyte_chars>
    # 仅汇编 ASM 文件时生成调试信息。
    $<$<COMPILE_LANGUAGE:ASM>:--debug>
)

# 为 STM32SX 的最终链接步骤设置 armlink 参数。
target_link_options(STM32SX PRIVATE
    # 指定链接目标处理器为 Cortex-M3。
    --cpu Cortex-M3
    # 指定 scatter 文件，由它安排 Flash、RAM 和向量表位置。
    --scatter ${CMAKE_CURRENT_SOURCE_DIR}/cmake/stm32f103c8.sct
    # 要求链接器生成内存映射报告。
    --map
    # 要求映射报告包含最终符号及其地址。
    --symbols
    # 要求报告程序大小以及被移除的未使用内容。
    --info sizes,unused
)

# 为 STM32SX 添加链接完成后的命令；只有 AXF 链接成功后才执行。
add_custom_command(TARGET STM32SX POST_BUILD
    # 调用工具链文件找到的 fromelf 程序。
    COMMAND ${CMAKE_FROMELF}
            # 要求 fromelf 生成合并的 Intel HEX 格式。
            --i32combined
            # 指定 HEX 输出文件位于 STM32SX.axf 所在目录。
            --output $<TARGET_FILE_DIR:STM32SX>/STM32SX.hex
            # 指定转换输入为 STM32SX 目标的完整 AXF 路径。
            $<TARGET_FILE:STM32SX>
    # 再次调用 fromelf，生成原始二进制文件。
    COMMAND ${CMAKE_FROMELF}
            # 选择原始 BIN 输出格式。
            --bin
            # 指定 BIN 输出文件位于 STM32SX.axf 所在目录。
            --output $<TARGET_FILE_DIR:STM32SX>/STM32SX.bin
            # 指定转换输入仍为刚链接成功的 STM32SX.axf。
            $<TARGET_FILE:STM32SX>
    # 告诉构建系统下面两个文件也是该构建步骤产生的附加结果。
    BYPRODUCTS
        # 声明生成的 Intel HEX 文件。
        ${CMAKE_CURRENT_BINARY_DIR}/STM32SX.hex
        # 声明生成的原始二进制文件。
        ${CMAKE_CURRENT_BINARY_DIR}/STM32SX.bin
    # 构建到这个步骤时，在终端显示下面的说明文字。
    COMMENT "Generating STM32SX.hex and STM32SX.bin"
    # 要求 CMake 为底层命令准确处理参数、引号和路径中的特殊字符。
    VERBATIM
)
