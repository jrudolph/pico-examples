if (TARGET tinyusb_device)
    add_executable(hello_usb
            hello_usb.c
            )
    
    pico_generate_pio_header(hello_usb ${CMAKE_CURRENT_LIST_DIR}/ps2.pio)
    pico_generate_pio_header(hello_usb ${CMAKE_CURRENT_LIST_DIR}/clocked_input.pio)

    #pico_set_binary_type(TARGET_NAME no_flash)
    # Pull in our pico_stdlib which aggregates commonly used features
    target_link_libraries(hello_usb pico_stdlib hardware_pio hardware_i2c hardware_spi)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(hello_usb 1)
    pico_enable_stdio_uart(hello_usb 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(hello_usb)

    # add url via pico_set_program_url
    example_auto_set_url(hello_usb)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()
