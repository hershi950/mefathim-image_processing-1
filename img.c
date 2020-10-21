#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "img_.h"

//#include "lzw.h"


// ----------------------------------------------------------------
// destruct the content of an image. The iamge struct can be reused
// ----------------------------------------------------------------
IMG *img_dtor(IMG *img)
{
	if (img) {
		if (img->pxl)
			free(img->pxl);
		memset(img, 0, sizeof(IMG));
	}
	return img;
}


// -------------------------------------------------------------------
// delete an allocated image. All color planes are deallocated as well
// -------------------------------------------------------------------
void img_delete(IMG *img)
{
	img_dtor(img);
	if (img)
		free(img);
}



// --------------------------------------------------------------------
// Generate a new image that has only a grey plane from a colored image
// Return a newly allocated image or null in case of failure.
// --------------------------------------------------------------------
IMG *img_to_grey(const IMG *imgColored)
{
	if (!imgColored || !imgColored->nPxls)
		return (IMG *)0;
	if (imgColored->nPlanes != IMG_COLOR_PLANES) 
		return (IMG *)0;

	// Map to color planes and allocate the grey plane
	IMG_COLOR *red   = imgColored->red;
	IMG_COLOR *green = imgColored->green;
	IMG_COLOR *blue  = imgColored->blue;
	IMG_COLOR *grey  = malloc(imgColored->nPxls * sizeof(*grey));
	if (!grey)
		return (IMG *)0;

	// Allocate a new IMG
	IMG* imgGrey = malloc(sizeof(IMG));
	if (!imgGrey) {
		free(grey);
		return (IMG *)0;
	}

	// fill the new image struct. These are adjustments to the colored image members
	memcpy (imgGrey, imgColored, sizeof(IMG));
	imgGrey->red = imgGrey->green = imgGrey->blue = (IMG_COLOR *)0;
	imgGrey->grey = imgGrey->pxl = grey;
	imgGrey->nPlanes = 1;
	imgGrey->nBytes = imgColored->nPxls * sizeof(*grey);


	// Populate the grey plane
	for (unsigned n = imgColored->nPxls; n > 0; n--) 
		*grey++ = (*red++ + *green++ + *blue++ + IMG_COLOR_PLANES / 2) / IMG_COLOR_PLANES;
	
	return imgGrey;
}


 /* ================================================================= *\
|					Obtain image dimensions							|
 \* ================================================================= */
void img_dim (int *width, int *height, const IMG *img)
{
	*width = *height = 0;	
	if (!img)
		return;
	*width = img->width;
	*height = img->height;
}


 
