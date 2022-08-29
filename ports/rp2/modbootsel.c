// *************************************************************************
// *                                                                       *
// *    MicroPython 'bootsel' module                                       *
// *                                                                       *
// *************************************************************************
// *                                                                       *
// *    This module allows access to the 'BOOTSEL' button                  *
// *                                                                       *
// *************************************************************************
// *                                                                       *
// *    .button()                                                          *
// *                                                                       *
// *************************************************************************

#define PRODUCT "BOOTSEL Button Interfacing"
#define PACKAGE "bootsel"
#define PROGRAM "modbootsel.c"
#define VERSION "0.00"
#define CHANGES "0000"
#define TOUCHED "0000-00-00 00:00:00"
#define LICENSE "MIT Licensed"
#define DETAILS "https://opensource.org/licenses/MIT"

// .--------------------------------------------------------------------------.
// |    MIT Licence                                                           |
// `--------------------------------------------------------------------------'

// Copyright 2021, "Hippy"

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// .-----------------------------------------------------------------------.
// |    MicroPython, Pico C SDK and TinyUSB includes                       |
// `-----------------------------------------------------------------------'

#include "py/runtime.h"

#include "pico/stdlib.h"
#include "py/objstr.h"

#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"

// .-----------------------------------------------------------------------.
// |    Set Binary Information to show added module                        |
// `-----------------------------------------------------------------------'

#if ENABLE_OWN_MODULE_BOOTSEL

BI_AM_ADD("bootsel")
BI_AM_TXT("BOOTSEL button access")

#endif

// *************************************************************************
// *                                                                       *
// *    C functionality                                                    *
// *                                                                       *
// *************************************************************************

bool __no_inline_not_in_flash_func(get_bootsel_button)() {
    const uint CS_PIN_INDEX = 1;
    uint32_t flags = save_and_disable_interrupts();
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);
    for (volatile int i = 0; i < 1000; ++i);
    bool button_state = !(sio_hw->gpio_hi_in & (1u << CS_PIN_INDEX));
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);
    restore_interrupts(flags);
    return button_state;
}

// *************************************************************************
// *                                                                       *
// *    MicroPython interface                                              *
// *                                                                       *
// *************************************************************************

STATIC const MP_DEFINE_STR_OBJ(PRODUCT_string_obj, PRODUCT);
STATIC const MP_DEFINE_STR_OBJ(PACKAGE_string_obj, PACKAGE);
STATIC const MP_DEFINE_STR_OBJ(PROGRAM_string_obj, PROGRAM);
STATIC const MP_DEFINE_STR_OBJ(VERSION_string_obj, VERSION);
STATIC const MP_DEFINE_STR_OBJ(CHANGES_string_obj, CHANGES);
STATIC const MP_DEFINE_STR_OBJ(TOUCHED_string_obj, TOUCHED);
STATIC const MP_DEFINE_STR_OBJ(LICENSE_string_obj, LICENSE);
STATIC const MP_DEFINE_STR_OBJ(DETAILS_string_obj, DETAILS);

// .-----------------------------------------------------------------------.
// *    .button()                                                          |
// `-----------------------------------------------------------------------'

STATIC uint repeat  = 0;
STATIC uint timeout = 0;
STATIC uint result  = 0;

STATIC mp_obj_t bootsel_button(size_t n_args, const mp_obj_t *args) {
    if(n_args == 1) {
        if(args[0] == mp_const_none) { repeat = 0; }
        else                         { repeat = mp_obj_get_int(args[0]); }
        if(timeout > repeat) {
            timeout = repeat;
        }
    }
    if(timeout == 0) {
        result = get_bootsel_button();
        timeout = repeat;
    } else {
        timeout--;
    }
    return MP_OBJ_NEW_SMALL_INT(result);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bootsel_button_obj, 0, 1,
                                    bootsel_button);

// *************************************************************************
// *                                                                       *
// *    MicroPython module definition                                      *
// *                                                                       *
// *************************************************************************

STATIC const mp_rom_map_elem_t bootsel_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bootsel)    },

    { MP_ROM_QSTR(MP_QSTR_PRODUCT),  MP_ROM_PTR(&PRODUCT_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_PACKAGE),  MP_ROM_PTR(&PACKAGE_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_PROGRAM),  MP_ROM_PTR(&PROGRAM_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_VERSION),  MP_ROM_PTR(&VERSION_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_CHANGES),  MP_ROM_PTR(&CHANGES_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_TOUCHED),  MP_ROM_PTR(&TOUCHED_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_LICENSE),  MP_ROM_PTR(&LICENSE_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_DETAILS),  MP_ROM_PTR(&DETAILS_string_obj) },

    { MP_ROM_QSTR(MP_QSTR_button),   MP_ROM_PTR(&bootsel_button_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bootsel_module_globals,
                            bootsel_module_globals_table);

// .-----------------------------------------------------------------------.
// *    MicroPython integration                                            |
// `-----------------------------------------------------------------------'

const mp_obj_module_t mp_module_bootsel = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&bootsel_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_bootsel, mp_module_bootsel);

// *************************************************************************
// *                                                                       *
// *    End of 'bootsel' module                                            *
// *                                                                       *
// *************************************************************************