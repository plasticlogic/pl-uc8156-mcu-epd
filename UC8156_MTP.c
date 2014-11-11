/*
 * UC8156_MTP.c
 *
 *  Created on: 09.09.2014
 *      Author: andreas.meier
 */
#include <stdio.h>

#include "msp430/msp430-spi.h"
#include "UC8156.h"
#include "UC8156_MTP.h"

void write_Vcom_to_MTP(u16 Vcom_mv_value)
{
	u8 return_value;
	u16 address=0x04B9;

	static u8 v1[2];
	spi_read_command_2params1(0x02, v1);
	spi_write_command_2params(0x02, v1[0], 0x33);

	return_value = read_MTP_address(address);
	fprintf(stderr, "return_value = %x\n", return_value);

	UC8156_HVs_on();

	//switch internal VPP on (VSH)
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value|0x08);
	mdelay(100);

	//setup and start MTP program
	spi_write_command_2params(0x41, address&0xFF, (address>>8)&0xFF);

	u16 Vcom_register_value = (float)Vcom_mv_value/(float)30.0;

	spi_write_command_1param(0x42, (Vcom_register_value>>8)&0xFF);
	UC8156_wait_for_BUSY_inactive();

	spi_write_command_1param(0x42, Vcom_register_value&0xFF);
	UC8156_wait_for_BUSY_inactive();

	//switch internal VPP off
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value&(~0x08));
	mdelay(100);

	UC8156_HVs_off();

	spi_write_command_2params(0x02, v1[0], v1[1]);

	return_value = read_MTP_address(address);
	fprintf(stderr, "return_value = %x\n", return_value);
}

void write_waveform_to_MTP(u8 *waveform_lut)
{
	//set VSH to 9.5V to be used as VPP
	//spi_write_command_2params(0x02, 0x50, 0xFF); //set Vgate+Vsource
	static u8 reg02h_backup[2];
	spi_read_command_2params1(0x02, reg02h_backup);
	spi_write_command_2params(0x02, reg02h_backup[0], 0x33);

	//set SRAM access paramter
	spi_write_command_4params(0x0D, 0x00, 0xEF, 0x00, 0x9F); //set window to full size
	spi_write_command_2params(0x0E, 0x00, 0x00); //set WS start location to (0,0)
	u8 reg0fh_backup = spi_read_command_1param(0x0f);
	spi_write_command_1param(0x0f, 0x00); //set data entry mode

	//power-up default is [0x20, 0xE1]
	spi_write_command_2params(0x44, 0x20, 0xEC); //MTP option setting --> Vs=2.4V (new default)

	spi_write_command_2params(0x0e, 0x00, 0x00);
	//write to be programmed data into SRAM
	UC8156_send_data_to_image_RAM_for_MTP_program(waveform_lut, WAVEFORM_LENGTH);

	UC8156_HVs_on();

	//switch internal VPP on (VSH)
	u8 return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value|0x08);
	mdelay(100);

	//start MTP program
	spi_write_command_1param(0x40, 0x01);
	UC8156_wait_for_BUSY_inactive();

	//switch VPP off
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value&(~0x08));
	mdelay(100);

	UC8156_HVs_off();

	//reset register to previous values
	spi_write_command_2params(0x02, reg02h_backup[0], reg02h_backup[1]);
	spi_write_command_1param(0x0f, reg0fh_backup);

	return_value = read_MTP_address(0x0000);
	fprintf(stderr, "addr 0x00 = %x\n", return_value);
	return_value = read_MTP_address(0x0010);
	fprintf(stderr, "addr 0x10 = %x\n", return_value);
	return_value = read_MTP_address(0x0020);
	fprintf(stderr, "addr 0x20 = %x\n", return_value);
	return_value = read_MTP_address(0x0060);
	fprintf(stderr, "addr 0x60 = %x\n", return_value);

}

u8 read_MTP_address(const u16 address)
{
	spi_write_command_2params(0x41, address&0xFF, (address>>8)&0x07); // set MTP address
	//spi_read_command_2params(0x43);
	return spi_read_command_1param_1dummy(0x43);
}

void debug_complex_MTP_program()
{
	u8 return_value;

	return_value = read_MTP_address(0x0000);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0010);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0033);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0066);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0077);
	fprintf(stderr, "return_value = %x\n", return_value);

	//set VSH to 9.5V to be used as VPP
	//spi_write_command_2params(0x02, 0x50, 0xFF); //set Vgate+Vsource
	static u8 v1[2];
	spi_read_command_2params1(0x02, v1);
	spi_write_command_2params(0x02, v1[0], 0x33);

	//power-up default is [0x20, 0xE1]
	spi_write_command_2params(0x44, 0x20, 0xEC); //MTP option setting --> Vs=2.4V (new default)

	//write to be programmed data into SRAM
	UC8156_send_repeated_image_data(0x67);

	UC8156_HVs_on();

	//spi_write_command_4params(0x0D, 0x00, 0xEF, 0x00, 0x9F); //set window to full size
	//spi_write_command_2params(0x0E, 0x00, 0x00); //set WS start location to (0,0)
	//spi_write_command_1param(0x0F, 0x00); //set data entry mode

	//switch on VPP manually

	//switch internal VPP on (VSH)
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value|0x08);
	mdelay(100);

	//start MTP program
	spi_write_command_1param(0x40, 0x01);
	UC8156_wait_for_BUSY_inactive();

	//switch VPP off
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value&(~0x08));
	mdelay(100);

	UC8156_HVs_off();

	return_value = read_MTP_address(0x0000);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0010);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0033);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0066);
	fprintf(stderr, "return_value = %x\n", return_value);
	return_value = read_MTP_address(0x0077);
	fprintf(stderr, "return_value = %x\n", return_value);
}

void one_Byte_MTP_program(u16 address, u8 data)
{
	u8 return_value;

	static u8 v1[2];
	spi_read_command_2params1(0x02, v1);
	spi_write_command_2params(0x02, v1[0], 0x33);

	return_value = read_MTP_address(address);
	fprintf(stderr, "return_value = %x\n", return_value);

	UC8156_HVs_on();

	//switch internal VPP on (VSH)
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value|0x08);
	mdelay(100);
	//return_value = spi_read_command_1param(0x03);
	//fprintf(stderr, "return_value = %x\n", return_value);

	//setup and start MTP program
	spi_write_command_2params(0x41, address&0xFF, (address>>8)&0xFF);
	spi_write_command_1param(0x42, data);
	UC8156_wait_for_BUSY_inactive();

	//switch internal VPP off
	return_value = spi_read_command_1param(0x03);
	spi_write_command_1param(0x03, return_value&(~0x08));
	mdelay(100);

	UC8156_HVs_off();

	spi_write_command_2params(0x02, v1[0], v1[1]);

	return_value = read_MTP_address(address);
	fprintf(stderr, "return_value = %x\n", return_value);
}

