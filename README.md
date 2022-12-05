# FLASH DRIVER

FLASH driver reusable in different platform.

[Information about flash memories](https://github.com/energicamotor/stm32-drv-flash/blob/main/FLASH_INFO.md)

## Supported Hardware

- NUCLEO-H745ZI-Q
- NUCLEO-L552ZE-Q


### Functions Guide

- `flash_erase` : to erase a sector of the memory.
- `flash_erase_nb` : to erase a given number of pages.
- `flash_write` : to write on a sector of the memory.
- `get_flash_sector_address` : returns the address of the memory sector being used.

## How to use

- Include the header file `drv_flash.h`.
- Enable the FLASH in the `.ioc` file.

- use `flash_erase` to erase a sector of the memory from a specified address to another
- use `flash_erase_nb` to erase a given number of pages.
- use `flash_write` to write in the flash, starting from a given address, data saved in an array.

Base addresses of the pages are defined in [drv_flash.h](https://github.com/energicamotor/stm32-drv-flash/blob/main/drv_flash.h)
