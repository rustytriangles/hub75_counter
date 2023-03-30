set(OUTPUT_NAME hub75_counter)
add_executable(${OUTPUT_NAME} hub75_counter.cpp)

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})

target_link_libraries(${OUTPUT_NAME}
    pico_stdlib
    pico_multicore
    hardware_vreg
    hub75
)

pico_enable_stdio_usb(${OUTPUT_NAME} 1)
pico_enable_stdio_uart(${OUTPUT_NAME} 0)
