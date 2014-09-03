#include <stdio.h>

//#include "image-data.h"
#include "FatFs/ff.h"
#include "types.h"
#include "pnm-utils.h"
#include "utils.h"

#define FILE_BUFFER_LENGTH 128

// global file system information used by FatFs
static FATFS Sd_Card;

// init SD-card
int sdcard_init(void)
{
	f_chdrive(0);
	return (f_mount(0,&Sd_Card) == FR_OK ? 0 : -EACCES);
}

// helper function
u8 charToHex(char upper, char lower)
{
	int uneg=48, lneg=48;

	if(upper >= 'A' && upper <='Z')
		uneg = 'A'-10;
	else if(upper >= 'a' && upper <='z')
		uneg = 'a'-10;

	if(lower >= 'A' && lower <='Z')
			lneg = 'A'-10;
	else if(lower >= 'a' && lower <='z')
		lneg = 'a'-10;

	u8 fu = (upper -uneg)<<4 | (lower-lneg);
	return fu;
}

/*Parses char values to integer*/
int parsevalue(char* str, int start, int length)
{
	int i;
	int pot=1;
	int ret=0;

	for (i = start+length-1; i >= start; i--)
	{
		if(str[i] >'9' | str[i] < '0')
			return -1;

		ret += (str[i]-'0')*pot;
		switch(pot)
		{
		case 1:
			pot=10;
			break;
		case 10:
			pot = 100;
			break;
		case 100:
			pot = 1000;
			break;
		}
	}
	return ret;
}

// loads waveform from SD-card
int sdcard_load_waveform(const char *path, u8 *formdata)
{
	FIL fl;
	u8 buff=1;
	u8 val[2];
	int returnval= -1;
	UINT count=0;
	int z = 0;
	bool isStart= false;
	if(f_open(&fl,path,FA_READ) == FR_OK)
	{
		while(z<90)
		{
			f_read(&fl,&buff,1,&count);

			if(!isStart && buff=='C')
			{
				f_read(&fl,&val,2,&count);
				isStart = val[0] =='3' & val[1] =='2';
			}


			if(isStart && buff=='D')
			{
				f_read(&fl,&val,2,&count);
				formdata[z] = charToHex(val[0],val[1]);
				z++;
			}
		}
		f_close(&fl);
	}


	return returnval;
}

// reads Vcom value from text-file on SD-card
int sdcard_load_vcom(const char *filename, int *vcom_mv_value)
{
	FIL fVcom;
	char buff[4];
	int val;
	if (f_open(&fVcom, filename, FA_READ) != FR_OK) {
				//LOG("Failed to open slideshow text file [%s]", SLIDES_PATH);
				return -1;
			}

	while(parser_read_file_line(&fVcom,buff,4))
	{
		val = parsevalue(buff,0,4);

		*vcom_mv_value = val;
	}
	return 1;
}

// reads image data from PMG image file - part of load_image function
static int read_image_data(FIL *f, u8 *image)
{
	u8 data[FILE_BUFFER_LENGTH];
	//u8 packed_data[IMAGE_BUFFER_LENGTH];
	size_t count;

	do {
		if (f_read(f, data, FILE_BUFFER_LENGTH, &count) != FR_OK)
			return -1;

		pack_4bpp(data, image, count);

		image+=count/4;

	} while (count);

	return 0;
}

/* Reads an image from SD Card */
int sdcard_load_image(const char *image_name, u8 *image_data)
{
	FIL image_file;
	struct pnm_header hdr;
	int ret;

	if (f_open(&image_file, image_name, FA_READ) != FR_OK) {
		//LOG("Failed to open image file");
		return -1;
	}

	ret = pnm_read_header(&image_file, &hdr);

	if (ret < 0) {
		//LOG("Failed to parse PGM header");
		goto err_close_file;
	}

	ret = read_image_data(&image_file, image_data);

err_close_file:
	f_close(&image_file);

	return ret;
}
