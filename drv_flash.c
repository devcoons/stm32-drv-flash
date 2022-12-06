/*!
	@file   drv_flash.c
	@brief  <brief description here>
	@t.odo	-
	---------------------------------------------------------------------------

	MIT License
	Copyright (c) 2019 Ioannis Deligiannis

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/******************************************************************************
* Preprocessor Definitions & Macros
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/

#include "drv_flash.h"

#ifdef HAL_IWDG_MODULE_ENABLED
#include "iwdg.h"
#endif

/******************************************************************************
* Enumerations, structures & Variables
******************************************************************************/

static FLASH_EraseInitTypeDef flash_erase_info;
static volatile uint8_t flash_write_error = 0;
static volatile uint32_t flash_write_cur_address = 0;
static volatile uint64_t flash_write_data = 0;
static volatile uint8_t flash_write_data_arr[128];

/******************************************************************************
* Declaration | Static Functions
******************************************************************************/

static uint32_t get_flash_sector(uint32_t address);

/******************************************************************************
* Definition  | Static Functions
******************************************************************************/

static uint32_t get_flash_sector(uint32_t address)
{

#if defined(FLASH_SECTOR_SIZE)
	return address < (FLASH_BASE + FLASH_BANK_SIZE) ?
			(address - FLASH_BASE) / FLASH_SECTOR_SIZE
			:
			(address - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_SECTOR_SIZE;
#elif defined(FLASH_PAGE_SIZE)
	return address < (FLASH_BASE + FLASH_BANK_SIZE) ?
			(address - FLASH_BASE) / FLASH_PAGE_SIZE
			:
			(address - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
#else
#error "Must be rewritten"
#endif

}

static uint32_t get_flash_bank(uint32_t address)
{
	return address < (FLASH_BASE + FLASH_BANK_SIZE) ? FLASH_BANK_1 : FLASH_BANK_2;
}
/******************************************************************************
* Definition  | Public Functions
******************************************************************************/

uint32_t get_flash_sector_address(uint32_t address)
{
	return FLASH_BASE + (get_flash_sector(address)*FLASH_SECTION_SIZE);
}

i_status flash_erase(uint32_t addr_from, uint32_t addr_to)
{
	uint32_t flash_sector_error = 0;
	uint32_t first_sector 		= get_flash_sector(addr_from);
	uint32_t nb_sectors 		= get_flash_sector(addr_to) - first_sector + 1;

	if(first_sector == 0xFFFFFFFF)
		return I_FAILED;

	flash_erase_info.Banks         = get_flash_bank(addr_from);
	#if defined(FLASH_TYPEERASE_SECTORS)
	flash_erase_info.TypeErase     = FLASH_TYPEERASE_SECTORS;
	flash_erase_info.Sector        = first_sector;
	flash_erase_info.NbSectors     = nb_sectors;
	#elif defined(FLASH_TYPEERASE_PAGES)
	flash_erase_info.TypeErase     = FLASH_TYPEERASE_PAGES;
	flash_erase_info.Page        = first_sector;
	flash_erase_info.NbPages     = nb_sectors;
	#else
	#error "Must rewrite this function"
	#endif

	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&flash_erase_info, &flash_sector_error);
	HAL_FLASH_Lock();

	return flash_sector_error == 0xFFFFFFFF ? I_OK : I_FAILED;
}

i_status flash_erase_nb(uint32_t addr_from, uint32_t nbp)
{
	uint32_t flash_sector_error = 0;
	if(addr_from == 0xFFFFFFFF || addr_from>FLASH_ADDR_END)
		return I_FAILED;

	flash_erase_info.Banks         = get_flash_bank(addr_from);
	#if defined(FLASH_TYPEERASE_SECTORS)
	flash_erase_info.TypeErase     = FLASH_TYPEERASE_SECTORS;
	flash_erase_info.Sector        = get_flash_sector(addr_from);;
	flash_erase_info.NbSectors     = nbp;
	#elif defined(FLASH_TYPEERASE_PAGES)
	flash_erase_info.TypeErase     = FLASH_TYPEERASE_PAGES;
	flash_erase_info.Page        = get_flash_sector(addr_from);;
	flash_erase_info.NbPages     = nbp;
	#else
	#error "Must rewrite this function"
	#endif

	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&flash_erase_info, &flash_sector_error);
	HAL_FLASH_Lock();

	return flash_sector_error == 0xFFFFFFFF ? I_OK : I_FAILED;
}

i_status flash_write(uint32_t address, uint8_t* data_array, uint32_t data_array_sz)
{
	flash_write_error = 0;

	if((address % 8) != 0)
		return I_MEMUNALIGNED;

	#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
	#endif

	HAL_FLASH_Unlock();

#if defined(STM32H745xx) || defined(STM32H743xx)
	uint32_t _data_array_sz = data_array_sz/(8*4);
	for(uint32_t i = 0; i< _data_array_sz; i++)
	{
		flash_write_error = 0;
		flash_write_cur_address = address+(i*(8*4));
		memmove(flash_write_data_arr,&data_array[i*(8*4)],(8*4));
		while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, flash_write_cur_address,(uint32_t)flash_write_data_arr) != HAL_OK)
		{
			HAL_FLASH_Lock();
			flash_write_error++;
			osDelay(5);
			HAL_FLASH_Unlock();
			if(flash_write_error>5)
				break;
		}
	}

#elif defined(FLASH_TYPEPROGRAM_FLASHWORD)
	uint32_t _data_array_sz = data_array_sz/4;
	for(uint32_t i = 0; i< _data_array_sz; i++)
	{
		flash_write_error = 0;
		flash_write_cur_address = address+(i*4);
		memmove((uint64_t*)&flash_write_data,&data_array[i*4],4);
		while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, flash_write_cur_address,flash_write_data) != HAL_OK)
		{
			HAL_FLASH_Lock();
			flash_write_error++;
			osDelay(5);
			HAL_FLASH_Unlock();
			if(flash_write_error>5)
				break;
		}
	}

#elif defined(FLASH_TYPEPROGRAM_DOUBLEWORD)
	uint32_t _data_array_sz = data_array_sz/8;
	for(uint32_t i = 0; i< _data_array_sz; i++)
	{
		flash_write_error = 0;
		flash_write_cur_address = address+(i*8);
		memmove((uint64_t*)&flash_write_data,&data_array[i*8],8);
		while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_write_cur_address,flash_write_data) != HAL_OK)
		{
			HAL_FLASH_Lock();
			flash_write_error++;
			osDelay(5);
			HAL_FLASH_Unlock();
			if(flash_write_error>5)
				break;
		}
	}

#else

#endif


	HAL_FLASH_Lock();

	#ifdef HAL_IWDG_MODULE_ENABLED
	HAL_IWDG_Refresh(&hiwdg);
	#endif

	return flash_write_error == 0 ? I_OK : I_FAILED;
}

/******************************************************************************
* EOF - NO CODE AFTER THIS LINE
******************************************************************************/
