/*
 * main.c
 *
 *  Created on: 15.01.2014
 *      Author: andreas.meier
 */

//#define DEBUG_PRINTOUTS

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "assert.h"
#include "FatFs/ff.h"
#include "msp430/msp430-spi.h"
#include "msp430/msp430-gpio.h"
#include "msp430/hal_pmm.h"
#include "Slideshow.h"
#include "UC8156.h"
#include "UC8156_MTP.h"
#include "read-sd.h"
#include "waveform.h"
#include "config.h"
#include "pattern.h"

void clear_display();

int _system_pre_init(void) {
 // stop WDT
 WDTCTL = WDTPW + WDTHOLD;
 // Perform C/C++ global data initialization
 return 1;
}

// setup clock to 20MHz
static void MSP430_init_clock(void)
{
	WDTCTL = WDTPW + WDTHOLD;				// Hold WDT
	SetVCore(PMMCOREV_3);					// Set VCore = 1.9V for 20MHz clock

	UCSCTL4 = SELM_4 | SELS_4 | SELA_1;		// MCLK = SMCLK = DCOCLKDIV = 20MHz /  ACLK = VLO = 10Khz
	UCSCTL3 |= SELREF_2;					// Set DCO FLL reference = REFO

	__bis_SR_register(SCG0);				// Disable the FLL control loop
	UCSCTL0 = 0x0000;						// Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_6;					// Select DCO range 30MHz operation
	UCSCTL2 = FLLD_1 + 609;					// Set DCO Multiplier for 20MHz
											// (N + 1) * FLLRef = Fdco
											// (609 + 1) * 32768 = 20MHz
											// Set FLL Div = fDCOCLK/2
	__bic_SR_register(SCG0);				// Enable the FLL control loop

	// Worst-case settling time for the DCO when the DCO range bits have been
	// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
	// UG for optimization.
	// 32 x 32 x 20 MHz / 32,768 Hz = 625000 = MCLK cycles for DCO to settle
	__delay_cycles(625000);

	// Loop until XT1,XT2 & DCO fault flag is cleared
	do {
		UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | XT1HFOFFG | DCOFFG);
												// Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG;                      // Clear fault flags
	} while (SFRIFG1 & OFIFG);
}

int main(void)
{
 	MSP430_init_clock();

	sdcard_init(); // initialize SD-card using FatFS
	spi_init(0,16); // initialize SPI interface towards the display

	gpio_init(); // initialize GPIO's used for display communication
	gpio_set_value_hi(PIN_3V3_ENABLE);
	mdelay(5);
	gpio_set_value_hi(PIN_RESET);

	UC8156_wait_for_BUSY_inactive(); // wait for power-up completed

	mdelay(5);
	UC8156_reset(); // UC8156 hardware reset
	UC8156_wait_for_BUSY_inactive(); // wait for RESET completed

	UC8156_init_registers();
	spi_write_command_1param(0x01, 0x10); //GVS=1 (p-mos, selected gate to VGL and non-selected gate to VGH) + SOO=0

	u8 status_reg = 	spi_read_command_1param(0x15);
	if (status_reg!=0) //check Status Register
	{
		fprintf(stderr, "Status Register not clear (%x).\n", status_reg);
		return 1;
	}
	#ifdef DEBUG_PRINTOUTS
	fprintf(stderr, "Status Register = %x\n", status_reg);
	#endif
	static u8 revID;
	revID = UC8156_read_RevID();
	if (revID!=0x56)
	{
		fprintf(stderr, "RevID 0x56 not read correctly (%x).\n", revID);
		return 1;
	}
	#ifdef DEBUG_PRINTOUTS
	fprintf(stderr, "RevID = %x\n", revID);
	#endif

//	write_waveform_library_to_MTP();

	//set Vcom
	UC8156_set_Vcom(2500);

	//read waveform from SD-card
	u8 waveform[WAVEFORM_LENGTH];
	//sdcard_load_waveform("waveform.bin", waveform);

//	UC8156_send_waveform(waveform);
	UC8156_send_waveform(waveform_default);

	clear_display(); // initialize display with 2 white updates

	//slideshow_run("240x160", WAVEFORM_FROM_MTP | FULL_UPDATE, 1000);
	slideshow_run("240x80", FULL_UPDATE, 1000);
	while(1);
}

void clear_display()
{
	u8 reg0fh_value = spi_read_command_1param(0x0f);

	spi_write_command_1param(0x0f, reg0fh_value|0x10); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	spi_write_command_1param(0x0f, reg0fh_value&(~0x10)); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
	UC8156_update_display(FULL_UPDATE);
	UC8156_HVs_off();
}

