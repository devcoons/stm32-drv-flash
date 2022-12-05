# FLASH INFO

This document contains information about the microcontrollers' flash memory organization

- [NUCLEO-L552ZE-Q](#NUCLEO-L552ZE-Q)
- [NUCLEO-H745ZI-Q](#NUCLEO-H745ZI-Q)
- [NUCLEO-U575ZI-Q](#NUCLEO-U575ZI-Q)

# NUCLEO-L552ZE-Q

- 512KB memory
- 2 banks
- Memory organized in **pages** 
  The main memory contains **128 pages**.
  The page size is 2 KB, each page consisting of 8 rows of 256 bytes.
- Data width is 64-bit 
- Supports **page erase**, **bank erase**, **mass erase**

| **Flash Area** | **Flash Memory Address**     | **Size** | **Name** |
| :--:           |            :--:              |  :--:    |   :--:   |
| *Bank 1*       |  0x0800_0000 - 0x0800_07FF   | 2KB      |  Page 0  |
|                |...                           |   ...    |  ...     |
|                | 0x0803_F800 - 0x0803_FFFF    | 2KB      | Page 127 |
| *Bank 2*       |  0x0800_4000 - 0x0800_07FF   | 2KB      |  Page 0  |
|                |...                           |   ...    |  ...     |
|                | 0x0807_F800 - 0x0807_FFFF    | 2KB      | Page 127 |
|||||
|*Non-secure*         | 0x0BF9_0000 - 0x0BF9_3FFF   | 16KB    | System Memory|
|*information block*  | 0x0BFA_0000 - 0x0BFA_01FF   | 512 B   | OTP area     |
|||||
|*Secure*                | 0x0FF8_0000 - 0x0FF8_1FFF   | 8KB    | RSS|
| *information block*    | 0x0FF8_2000 - 0x0FF8_27FF   | 2KB   | RSS library     |


|**Operation**|**Granurality**|
| :--:        | :--:          |
| Programming | 8-Byte        |
| Erase       | Mass, Bank, Page |



## NUCLEO-H745ZI-Q

- 2MB memory
- 2 banks
- Memory organized in **sectors** 
  The main memory contains **16 sectors** (8 each bank)
  The sector size is 128 KB
- **266-bit** Flash words 
- The Flash interface can be accessed by Double-word (64bits), by Single-word (32 bits), by Half-word (16 bits) or byByte (8 bits).
- Supports **sector erase**, **bank erase**, **mass erase**
- A program or erase operation can be executed on Bank 1 while another program or erase operation is executed on Bank 2.
- The address mapping of the user Flash memory of each bank can be swapped

| **Flash Area** | **Flash Memory Address**     | **Size** | **Name**    |
| :--:           |            :--:              |  :--:    |   :--:      |
|  *Bank 1*      |                              |          |             |
|                |                              |          |             |
| *Main Memory*  |  0x0800_0000 - 0x0801_FFFF   | 128KB    | Sector 0    |
|                |...                           |   ...    |  ...        |
|                | 0x080E_0000 - 0x080F_FFFF    | 128KB    | Sector 7    |
| *System Flash* | 0x1FF0_0000 - 0x1FF1_FFFF    | 128KB    |System Memory|
|                |                              |          |             |
|  *Bank 2*      |                              |          |             |
|                |                              |          |             |
| *Main Memory*  |  0x0810_0000 - 0x0811_FFFF   | 128KB    | Sector 0    |
|                |...                           |   ...    |  ...        |
|                | 0x0812_0000 - 0x0813_FFFF    | 128KB    | Sector 7    |
| *System Flash* | 0x1FF4_0000 - 0x1FF5_FFFF    | 128KB    |System Memory|

- TypeErase = FLASH_TYPEERASE_SECTORS; (lines 88 & 106)
- Need to change flash_erase_info.Page to flash_erase_info.Sector (lines 90 & 107);
- Need to change flash_erase_info.NbPages to flash_erase_info.NbSectors (lines 91 & 109);
- Need to change FLASH_PAGE_SIZE to FLASH_SECTOR_SIZE (line 64);
- TypeProgram argument to flash write is FLASH_TYPEPROGRAM_FLASHWORD
- **M4 is mapped in 0x08100000 (bank 2) and M7 in 0x08000000 (bank 1)**

## NUCLEO-U575ZI-Q

- 2MB memory
- 2 banks
- Memory organized in **pages** 
  The main memory contains **128 pages**.
  The page size is 8 KB
- Supports **page erase**, **bank erase**, **mass erase**

| **Flash Area** | **Flash Memory Address**     | **Size** | **Name** |
| :--:           |            :--:              |  :--:    |   :--:   |
| *Bank 1*       | 0x0800_0000 - 0x0800_1FFF   | 8KB      |  Page 0  |
|                |...                           |   ...    |  ...     |
|                |  0x080F_E000 - 0x080F_FFFF    | 8KB      | Page 127 |
| *Bank 2*       |  0x0810_0000 - 0x0810_1FFF   | 8KB      |  Page 0  |
|                |...                           |   ...    |  ...     |
|                | 0x081F_E000 - 0x081F_FFFF    | 8KB      | Page 127 |
|||||
|*Non-secure*         | 0x0BF9_0000 - 0x0BF9_3FFF   | 32KB    | System Memory|
|*information block*  | 0x0BFA_0000 - 0x0BFA_01FF   | 512 B   | OTP area     |


|**Operation**|**Granurality**|
| :--:        | :--:          |
| Programming | Standard: Quad-word programming (4 x 32-bit )        |
|             |Burst: 8 quad-words programming|
| Erase       | Mass, Bank, Page |
