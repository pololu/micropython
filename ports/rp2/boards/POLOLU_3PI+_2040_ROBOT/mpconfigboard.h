// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Pololu 3pi+ 2040 Robot"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (15 * 1024 * 1024)

#define MICROPY_HW_USB_MSC (1)
#define MICROPY_HW_USB_VID (0x1FFB)
#define MICROPY_HW_USB_PID (0x2043)

#define MICROPY_BANNER_MACHINE MICROPY_HW_BOARD_NAME
