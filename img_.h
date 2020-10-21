

#ifndef __img__h
#define __img__h

#ifndef __img_h
#include "img.h"
#endif

#define BMP IMG


#define IMG_ERR int
#define IMG_ERR_OK 0
#define IMG_ERR_ALLOC -1
#define IMG_ERR_READ -2
#define IMG_ERR_UK -3

#define IMG_COLOR_PLANES 3

#define IMG_COLOR short
#define IMG_COLOR_DEPTH 15 

#define _SHIFT 7


struct _img {
	union {
		int size;
		BITMAPINFO		bitmapInfo;
		BITMAPINFOHEADER infoHeader;
		BITMAPV5HEADER v5;
	} header;
	int nPlanes;		// number of color planes
//	int colorDepth;     // number of bits per color
//	int					bprl;		/* Bytes per raster line */
	int			width, height;
	IMG_COLOR* red, * green, * blue, * grey;
	IMG_COLOR* pxl;
	unsigned   nPxls; // total number of pixels in image
	unsigned   nBytes; // total number of bytes in image
};





BMP* bmp_dup(HDC, BMP*, RECT*);
void bmp_unload(BMP*);
void bmp_dim(int*, int*, BMP*);
HBITMAP	bmp_2_hdl(BMP*);


// -------------------------------------------------------------------------------------------------
// Construct a gray scale bitmap from a bmp. If successful, a new HBITMAP is returned. Otherwise, NULL
// The grey scale is 8-bits index to a palette
// -------------------------------------------------------------------------------------------------
HBITMAP bmp_to_dib_grey(HDC hdc, BMP* bmp);


#endif