/*
 * verification_functions.c
 *
 *  Created on: 13.11.2014
 *      Author: andreas.meier
 */

#include <stdio.h>
#include "verification_functions.h"
#include "msp430/msp430-spi.h"
#include "UC8156.h"
#include "UC8156_MTP.h"
#include "waveform.h"
#include "config.h"

void check_temperature_sensor()
{
	u8 return_value;

	return_value = 	spi_read_command_1param(0x07); //read temperature value register
	fprintf(stderr, "R07h = %xh\n", return_value);
	return_value = 	spi_read_command_1param(0x08); //read temperature value register
	fprintf(stderr, "temperature (R08h) = %dd\n", return_value);

	spi_write_command_1param(0x07, 0x09); //trigger internal temperature sensor read
	mdelay(170);
 	while (spi_read_command_1param(0x15)!=0); // check status register bit for temperature sensor
	return_value = 	spi_read_command_1param(0x07); //read temperature value register
	fprintf(stderr, "R07h = %xh\n", return_value);

	return_value = 	spi_read_command_1param(0x08);
	fprintf(stderr, "temperature (R08h) = %dd\n", return_value);
}

void read_waveform_LUT()
{
	u8 buffer[120];
	spi_read_command_and_bulk_data(0x1c, buffer, 120);
}

void read_and_print_MTP()
{
	u8 return_value;

	int i=0,j=0;
	fprintf(stderr, "\n");
	for(i=0;i<30;i++)
	{
		fprintf(stderr, "%2x: ", i*4+j);
		while(j<4)
		{
			return_value = read_MTP_address(i*4+j);
			fprintf(stderr, "%2x ", return_value);
			j++;
		}
		fprintf(stderr, "\n");
		j=0;
	}
}

void drift_test(u8 *waveform_p)
{
	UC8156_HVs_on();

	send_drift_test_image();
	UC8156_update_display(FULL_UPDATE);

	send_drift_test_image();
	UC8156_set_Vcom(3600);
	UC8156_send_waveform(waveform_long_null);
	UC8156_update_display(FULL_UPDATE);

	send_drift_test_image();
	//UC8156_set_Vcom(4000);
	UC8156_send_waveform(waveform_p);
	spi_write_command_4params(0x0c, 0x00, 80, GATE_LINES_MAX-GATE_LINES, GATE_LINES_MAX-1); // Panel resolution setting --> SOURCE_E needs to be SOURCELINE instead of SOURCELINE-1 for 180x100, don't know why
	spi_write_command_3params(0x18, 0x40, 0x02, 0x34); //BPCOM=GND, TPCOM=Hi-Z after update, gate_out=VGH after update
	//spi_write_command_4params(0x0c, 0x00, SOURCE_LINES/2/8*8-1, GATE_LINES_MAX-GATE_LINES, GATE_LINES_MAX-1); // Panel resolution setting --> SOURCE_E needs to be SOURCELINE instead of SOURCELINE-1 for 180x100, don't know why
	UC8156_update_display(FULL_UPDATE);

	spi_write_command_4params(0x0c, 0x00, SOURCE_LINES, GATE_LINES_MAX-GATE_LINES, GATE_LINES_MAX-1); // Panel resolution setting --> SOURCE_E needs to be SOURCELINE instead of SOURCELINE-1 for 180x100, don't know why

	UC8156_HVs_off();

}

void send_drift_test_image()
{
	  u8 image_data[PIXEL_COUNT/4];
	  int i;

	  for(i=0;i<PIXEL_COUNT/4/2;i++)
		  image_data[i]=0x00;
	  for(i=PIXEL_COUNT/4/2;i<PIXEL_COUNT/4;i++)
		  image_data[i]=0xff;

	  UC8156_send_image_data(image_data);
}

void check_GPIOs()
{
//	GPIO output verification
	spi_write_command_1param(0x09, 0xf0); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf1); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf2); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf4); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf8); //configure GPIO's for output
}


void verify_update_modes()
{
	//show_image("240x160/13_240~1.PGM", FULL_UPDATE);

	show_image("4GL.PGM", 0x20 | 0x01);

	white_update();

	spi_write_command_4params(0x0d, 0x00, 0xef, 0x10, 0x9f);
	show_image("4GL.PGM", 0x20 | 0x01);

	spi_write_command_4params(0x0d, 0x10, 0xef, 0x00, 0x9f);
	show_image("4GL.PGM", 0x20 | 0x01);
}

void verify_ckbd_SOO_0()
{
	alt_source_SOO_0();
	mdelay(2000);
	clear_display(); // initialize display with 2 white updates
	checkerboard_SOO_0();
}

void register_dump()
{
	int i;
	u8 *return_value;

	for(i=0;i<0x20;i++)
	{
		return_value=spi_read_command_4params(i);
		fprintf(stderr, "%02x: %02x%02x%02x%02x\n", i, *(return_value+3), *(return_value+2), *(return_value+1), *(return_value+0));
	}
}

void print_register_value(u8 reg, u8 count)
{
	int i;

	u8 *values_p = (u8 *)malloc(count);
	spi_read_command_and_bulk_data(reg, values_p, count);

	fprintf(stderr, "Reg0x%02xh: ", reg);
	for(i=0; i<count; i++)
		fprintf(stderr, "%02x ", *(values_p+i));
	fprintf(stderr, "\n");

	free(values_p);
}



void power_measurement()
{
	//debug power consumption
	spi_write_command_2params(0x04, 0x11, 0x34);
	//spi_read_command_2params(0x04);
	spi_write_command_2params(0x02, 0x25, 0x88); // set Vgate to +17V/-25V
	spi_write_command_2params(0x06, 0xda, 0xaa); // set timing to LAT=105, S2G+G2S=5

	while(1)
	{
		//checkerboard_SOO_0();
		alt_gate_SOO_0();
		alt_gate_SOO_0();
		checkerboard_SOO_0();
		checkerboard_SOO_0();
		show_image("240x160/13_240~1.PGM", FULL_UPDATE);
		show_image("240x160/13_240~1.PGM", FULL_UPDATE);
	}
}

void measure_vcom()
{
#define MEAS_TIME 5 //in seconds
#define MEAS_RESOLUTION 100 // in ms
#define MEAS_COUNT MEAS_TIME * 1000 / MEAS_RESOLUTION

	u8 meas_value[MEAS_COUNT];
	int i;
	u8 *return_value;

	spi_write_command_4params(0x18, 0x40, 0x02, 0x24, 0x05); //TPCOM always Hi-Z

	UC8156_HVs_on();

	fprintf(stderr,"Reg[19h]=0x%2x\n\n", ((MEAS_TIME * 50 / 24)<<2) | 0x01);
	spi_write_command_1param(0x19, ((MEAS_TIME * 50 / 24)<<2) | 0x01);

	for (i=0; i<MEAS_COUNT; i++)
	{
		mdelay(MEAS_RESOLUTION);
		return_value = spi_read_command_2params(0x1a);
		meas_value[i] = return_value[0];
	}

	for (i=0; i<MEAS_COUNT; i++)
	{
		fprintf(stderr, "Vkb[%dms] = %d - %f\n", (i+1)*MEAS_RESOLUTION, meas_value[i], meas_value[i]*0.03);
	}
}

void WF_type2_update_verification()
{
spi_write_command_1param(0x07, 0x00);
	spi_write_command_1param(0x07, 20);

	u8 R44h_temp;
	spi_read_command(0x44, &R44h_temp, 1);
	spi_write_command_1param(0x44, R44h_temp | 0x40);

	while(1)
	{
		spi_write_command_1param(0x40, 0x00);
		white_update();

		spi_write_command_1param(0x40, 0x02);
		white_update();
	}
}

void RAM_window_test()
{
u8 image_data[30*30/4];

u8 x_s=20;
u8 y_s=10;
u8 x_e=50;
u8 y_e=40;

spi_write_command_4params(0x0d, x_s, x_e-1, GATE_LINES_MAX-y_e, GATE_LINES_MAX-y_s-1); // RAM window setup
//spi_write_command_2params(0x0e, 0x00, GATE_LINES-1); //start Y from 159d/9fh, related to R0fh/DEM setting

UC8156_send_image_data(image_data);

UC8156_HVs_on();
UC8156_update_display_full();
UC8156_HVs_off();

	x_s=50;
	y_s=50;
	x_e=65;
	y_e=65;

	  spi_write_command_4params(0x0d, x_s, x_e-1, GATE_LINES_MAX-y_e, GATE_LINES_MAX-y_s-1); // RAM window setup

	  UC8156_send_image_data(image_data);

	  UC8156_HVs_on();
	  UC8156_update_display_full();
	  UC8156_HVs_off();

		x_s=90;
		y_s=70;
		x_e=120;
		y_e=90;

		  spi_write_command_4params(0x0d, x_s, x_e-1, GATE_LINES_MAX-y_e, GATE_LINES_MAX-y_s-1); // RAM window setup

		  UC8156_send_image_data(image_data);

		  UC8156_HVs_on();
		  UC8156_update_display_full();
		  UC8156_HVs_off();
}
