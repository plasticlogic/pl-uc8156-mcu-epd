/*
 * solomon.c
 *
 * Author: andreas.meier
 */

#include <msp430.h>
#include <stdio.h>

#include "UC8156.h"
#include "msp430/msp430-spi.h"
#include "msp430/msp430-gpio.h"
#include "types.h"
#include "utils.h"

//global variables
u8 UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM = WAVEFORM_FROM_MTP;
extern u16 PIXEL_COUNT; //global variable
extern regSetting_t *REG_SETTINGS; //global variable
extern u8 NUMBER_OF_REGISTER_OVERWRITES; //global variable

// UC8156 hardware reset
void UC8156_hardware_reset()
{
	gpio_set_value_lo(PIN_RESET);
	mdelay(1);
	gpio_set_value_hi(PIN_RESET);
}

// waits for BUSY getting inactive = 1 (BUSY pin is low-active)
 unsigned int UC8156_wait_for_BUSY_inactive()
{
	unsigned long counter=0;
 	while (gpio_get_value(PIN_BUSY)==0) // BUSY loop
 	{
 		mdelay(1);
		counter++; // BUSY loop
 	}
 	return counter;
}

 // waits for BUSY getting inactive = 1 (BUSY pin is low-active)
 void UC8156_wait_for_BUSY_inactive_debug()
 {
 	printf("BUSY loop counter = %d\n", UC8156_wait_for_BUSY_inactive());
 }

// waits for Power_ON RDY
unsigned long UC8156_wait_for_PowerON_ready()
 {
 	unsigned long counter=0;
 	while (spi_read_command_1param(0x15)!=4)
 	{
  		mdelay(1);
 		counter++; // BUSY loop
  	}
 	return(counter);
 }

// waits for Power_ON RDY -> print-out counter
void UC8156_wait_for_PowerON_ready_debug()
 {
	unsigned long counter = UC8156_wait_for_PowerON_ready();
 	printf("PowerOn loop counter = %d\n", counter);
 }

// UC8156 change registers which need values different from power-up values
void UC8156_init_registers()
{
	int i;

	for (i=0; i<NUMBER_OF_REGISTER_OVERWRITES; i++)
	    spi_write_command((REG_SETTINGS+i)->addr, (REG_SETTINGS+i)->val, (REG_SETTINGS+i)->valCount);
}

// UC8156 HV power-on (enable charge pumps, execute power-on sequence for outputs)
void UC8156_HVs_on()
{
	u8 reg_value = spi_read_command_1param(0x03); //read power control setting register
	reg_value |= 0x11; //switch on CLKEN+PWRON bits
	spi_write_command_1param (0x03, reg_value); //write power control setting register --> switch on CLKEN+PWRON bits

	UC8156_wait_for_PowerON_ready();
//	UC8156_wait_for_PowerON_ready_debug();
}

// UC8156 power-off sequence
void UC8156_HVs_off()
{
	u8 reg_value = spi_read_command_1param(0x03); //read power control setting register
	reg_value &= ~0x01; //switch off PWRON bit
	spi_write_command_1param (0x03, reg_value); //write power control setting register
	UC8156_wait_for_BUSY_inactive();
	reg_value &= ~0x10; //switch off CLKEN bit
	spi_write_command_1param (0x03, reg_value);
}

u8 UC8156_read_RevID()
{
	return spi_read_command_1param(0x00);
}

// send Vcom value (in mV) to UC8156
void UC8156_set_Vcom(int Vcom_mv_value)
{
	u16 Vcom_register_value = (float)Vcom_mv_value/(float)30.0;
	spi_write_command_2params(0x1B, (u8)Vcom_register_value, (u8)((Vcom_register_value>>8)&0x03));
}

// send waveform to UC8156
void UC8156_send_waveform(u8 *waveform)
{
	spi_write_command_and_bulk_data(0x1C, waveform, WAVEFORM_LENGTH);
}

//send an image to UC8156 image data memory
void UC8156_send_image_data(u8 *image_data)
{
	spi_write_command_and_bulk_data(0x10, image_data, PIXEL_COUNT/4);
}

//send an image to UC8156 image data memory
void UC8156_send_image_data_area(u8 *image_data, int col_start, int col_size, int row_start, int row_size)
{
	spi_write_command_4params(0x0d, row_start*2, row_start*2+row_size*2-1, col_start/2, col_start/2+col_size/2-1);
	spi_write_command_2params(0x0e, row_start*2, col_start/2);

	spi_write_command_and_bulk_data(0x10, image_data, col_size*row_size/4);
}

//send any data to UC8156 image data memory --> e.g. used for MTP programming
void UC8156_send_data_to_image_RAM_for_MTP_program(u8 *waveform_data, size_t size)
{
	spi_write_command_and_bulk_data(0x10, waveform_data, size);
}

//send an repeated byte to the image buffer --> used to create a solid image like all white
void UC8156_send_repeated_image_data(u8 image_data)
{
	spi_write_command_byte_repeat(0x10, image_data, PIXEL_COUNT/4);
}

//update display using full update mode and wait for BUSY-pin low
void UC8156_update_display_full()
{
	spi_write_command_1param(0x14, 0x01);
	UC8156_wait_for_BUSY_inactive();
}

//update display and wait for BUSY-pin low
void UC8156_update_display(u8 update_mode, u8 waveform_mode)
{
	spi_write_command_1param(0x40, spi_read_command_1param(0x40) | waveform_mode);
	spi_write_command_1param(0x14, UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM | update_mode | 1 );
	//spi_write_command_1param(0x14, UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM | update_mode | 1 | 0x20); // test area update mode
	UC8156_wait_for_BUSY_inactive();
	//UC8156_wait_for_BUSY_inactive_debug();
}

void UC8156_update_display_with_power_on_off(u8 update_mode, u8 waveform_mode)
{
	  UC8156_HVs_on();
	  UC8156_update_display(update_mode, waveform_mode);
      UC8156_HVs_off();
}

void UC8156_show_image(u8 *image_data, u8 update_mode, u8 waveform_mode)
{
	  UC8156_send_image_data(image_data);

	  UC8156_update_display_with_power_on_off(update_mode, waveform_mode);
}

void UC8156_show_image_area(u8 *image_data, int col_start, int col_size, int row_start, int row_size, u8 update_mode, u8 waveform_mode)
{
	  UC8156_send_image_data_area(image_data, col_start, col_size, row_start, row_size);

	  UC8156_HVs_on();
	  UC8156_update_display(update_mode, waveform_mode);
      UC8156_HVs_off();
}

void measure_Vcom()
{
	u8 return_value;

	spi_write_command_4params(0x18, 0x40, 0x01,0x24, 0x05); //TPCOM=Hi-Z before update and during null-frame drive

	UC8156_HVs_on();
	return_value = 	spi_read_command_1param(0x15);
	printf("R15h after HV_on = %x\n", return_value);
	//spi_write_command_1param(0x19, 0x29); //trigger Vcom sensing with waiting time 5sec
	spi_write_command_1param(0x19, 0x81); //trigger Vcom sensing with waiting time 5sec


	int i;
	u8 value[60][2];
	for (i=0;i<60;i++)
	{
		spi_read_command_2params1(0x1a, &value[i][0]);
		mdelay(250);
	}

	UC8156_wait_for_BUSY_inactive(); // wait for power-up completed
	UC8156_HVs_off();

	for (i=0;i<60;i++)
	{
		printf("%f sec = %f V\n", i*0.25, value[i][0] * 0.03);
	}
}

void print_temperature_sensor_value()
{
	u8 return_value = spi_read_command_1param(0x08);
	printf("Temperatur sensor value read from Reg[08h] = 0x%x\n", return_value);
}

void print_current_VCOM()
{
	u8 return_value = spi_read_command_1param(0x1b);
	printf("Vcom read from Reg[1Bh] = 0x%x = %.3fV\n", return_value, return_value*0.03);
}

void UC8156_check_status_register(u8 expected_value)
{
	char error_message[30];

	u8 status_reg_value = spi_read_command_1param(0x15);

	#if DEBUG_PRINTOUTS
	printf("Status Register = %x\n", status_reg_value);
	#endif

	if (status_reg_value != expected_value) //check Status Register
	{
		sprintf(error_message, "Status Register not %x but %x.\n", expected_value, status_reg_value);
		abort_now(error_message, ABORT_UC8156_INIT);
	}
}

void UC8156_check_RevID()
{
	char error_message[30];
	u8 revID = UC8156_read_RevID();

//	printf("RevID = %x\n", revID);

	if (revID != 0x56)
	{
		sprintf(error_message, "RevID 0x56 not read correctly (%x).\n", revID);
		abort_now(error_message, ABORT_UC8156_INIT);
	}
}
