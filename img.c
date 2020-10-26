#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "img_.h"
#include "trap.h"

//#include "lzw.h"


// ----------------------------------------------------------------
// destruct the content of an image. The iamge struct can be reused
// ----------------------------------------------------------------
IMG *img_dtor(IMG *img)
{
	if (img) {
		if (img->color)
			free(img->color);
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
	imgGrey->grey = imgGrey->color = grey;
	imgGrey->nPlanes = 1;
	imgGrey->nBytes = imgColored->nPxls * sizeof(*grey);


	// Populate the grey plane
	for (unsigned n = imgColored->nPxls; n > 0; n--) 
		*grey++ = (*red++ + *green++ + *blue++ + IMG_COLOR_PLANES / 2) / IMG_COLOR_PLANES;
	
	return imgGrey;
}


 /* ================================================================= *\
|					Obtain image dimensions							    |
 \* ================================================================= */
void img_dim (int *width, int *height, const IMG *img)
{
	*width = *height = 0;	
	if (!img)
		return;
	*width = img->width;
	*height = img->height;
}


// -----------------------------------------------------------------------------
// duplicate an image. The call also duplicates the color plane(s), if they exist
// -----------------------------------------------------------------------------
IMG *img_dup(IMG *img)
{
	if (!img) 
		return img;

	IMG_COLOR *color = malloc(img->nBytes);
	IMG *I = malloc(sizeof(IMG));
	if (!color || !I) {
		if (color)
			free(color);
		if (I)
			free(I);
		return (IMG*)0;
	}

	memcpy (I, img, sizeof(IMG));
	I->color = color;
	memcpy (I->color, img->color, img->nBytes);
	switch (img->nPlanes) {
		case 1:
			I->grey = I->color;
			break;
		case IMG_COLOR_PLANES:
			I->red = I->color;
			I->green = I->red + I->width * I->height;
			I->blue = I->green + I->width * I->height;
			break;
		default:
			fatal_error();
	}
	return I;
}



 
// ------------------------------------------------------------------------------------------
// Rotate an image by 'angle' degrees. See header
// ------------------------------------------------------------------------------------------
IMG *img_rotate(IMG *img, int angle, _Bool newImage)
{
	if (newImage)
		img = img_dup(img);
	if (!img)
		return img;
	switch (img->nPlanes) {

		case 1:
			plane_rotate(img->grey, img->width, img->height, angle);
			break;

		case IMG_COLOR_PLANES:
			plane_rotate(img->red,   img->width, img->height, angle);
			plane_rotate(img->green, img->width, img->height, angle);
			plane_rotate(img->blue,  img->width, img->height, angle);
			break;

		default:
			fatal_error();
	}

	return img;
}


