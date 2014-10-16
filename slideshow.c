#include <stdio.h>
#include <string.h>

#include "FatFs/ff.h"
#include "UC8156.h"
#include "read-sd.h"
#include "config.h"

/* FatFS only supports 8.3 filenames, and we work from the current directory so
   paths should be short... */
#define MAX_PATH_LEN 64

// loads image form SD-card and updates it on the display using REFRESH waveform
//int show_image(const char *image)
int show_image(const char *image, int mode, u16 delay_ms)
{
	  u8 image_data[PIXEL_COUNT/4];

	  if (sdcard_load_image(image, image_data))
		return -1;

	  UC8156_send_image_data(image_data);

	  UC8156_HVs_on();
	  UC8156_update_display(mode);
      UC8156_HVs_off();

      mdelay(delay_ms);

	return 0;
}

/** Slideshow callback function, called on each file found in a directory */
//int slideshow_run(const char *path, slideshow_cb_t callback)
int slideshow_run(const char *path, int mode, u16 delay_ms)
{
	DIR dir;
	FILINFO fno;
	char full_path[MAX_PATH_LEN];
	int result = 0;

	if (f_opendir(&dir, path) != FR_OK)
		return -1;

	do {
		if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
			break;

		/* skip directories */
		if ((fno.fname[0] == '.') || (fno.fattrib & AM_DIR))
			continue;

		/* .. and files without the PGM extension */
		if (!strstr(fno.fname, ".PGM"))
			continue;

		sprintf(full_path, "%s/%s", path, fno.fname);

		//result = callback(full_path);
		result = show_image(full_path, mode, delay_ms);

	} while (result >= 0);

	return result;
}
