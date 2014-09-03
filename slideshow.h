#ifndef SLIDESHOW_H_
#define SLIDESHOW_H_

#include "types.h"

int show_image(const char *image);
int slideshow_run(const char *path, slideshow_cb_t callback);

#endif
