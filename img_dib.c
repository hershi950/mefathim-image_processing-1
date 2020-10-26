
#include <stdint.h>

#include "img_.h"


uint32_t _byte_to_mask(byte b);



// ----------------------------------
// construct HBITMAP for a gey bitmap
// ----------------------------------
static HBITMAP _grey_dib(HDC hdc, const IMG *img)
{
	if (img->nPlanes != 1 || !img->grey)
		return (HBITMAP)0;

	unsigned char _info[sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 256];
	memset(&_info, 0, sizeof(_info));
	BITMAPINFO* info = (BITMAPINFO*)_info;
	BITMAPINFOHEADER* header = &info->bmiHeader;
	RGBQUAD* rgbquad = info->bmiColors;


	// Fill the header of the bitmapinfo struct
	// ----------------------------------------
	header->biSize = sizeof(BITMAPINFOHEADER);
	header->biWidth = img->width;
	header->biHeight = img->height;
	header->biPlanes = 1;
	header->biBitCount = 8;
	header->biClrUsed = 256;
	header->biCompression = BI_RGB;
	header->biSizeImage = 0;

	// fill the colors table with grey shades
	for (int i = 0; i < 256; i++)
		rgbquad[i].rgbRed = rgbquad[i].rgbGreen = rgbquad[i].rgbBlue = i;


	// allocate the bitmap
	int bprl = (img->width + sizeof(uint32_t) - 1) / sizeof(uint32_t) * sizeof(uint32_t);
	byte* bits = malloc(bprl * img->height);
	if (!bits)
		return (HBITMAP)0;

	// fill the bits
	byte *b = bits;
	IMG_COLOR *grey = img->grey;
	int rasterLines = img->height;
	for (int r = 0; r < rasterLines; r++) {
		for (int n = img->width; n > 0; n--)
			*b++ = *grey++ >> _SHIFT;
		b = bits + r * bprl;
	}

	HBITMAP dib = CreateDIBitmap(hdc, &info->bmiHeader, CBM_INIT, bits, info, DIB_RGB_COLORS);
	free(bits);
	return dib;
}



// -------------------------------------------
// cosntruct a HBITMAP for a 24 bit RGB bitmap
// -------------------------------------------
HBITMAP _colored_dib_24(HDC hdc, const IMG *img)
{
	BITMAPINFO info;
	memset(&info, 0, sizeof(info));


	// Fill the header of the bitmapinfo struct
	// ----------------------------------------
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biHeight = img->height;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biSize = sizeof(BITMAPV5HEADER);
//	info.bV5SizeImage = img->width * img->height;
	info.bmiHeader.biWidth = img->width;

	// allocate the bitmap bits
	int bprl = img->width * IMG_COLOR_PLANES;
	int tmp = bprl % sizeof(int);
	if (tmp)
		bprl += sizeof(uint32_t) - tmp;
	byte *bits = malloc(bprl * img->height);
	if (!bits)
		return (HBITMAP)0;


	byte *b = (byte*)bits;
	IMG_COLOR* red = img->red;
	IMG_COLOR* green = img->green;
	IMG_COLOR* blue = img->blue;
	int rasterLines = img->height;
	for (int r = 0; r < rasterLines; r++) {
		for (int n = img->width; n > 0; n--) {
			*b++ = *blue++ >> _SHIFT;
			*b++ = *green++ >> _SHIFT;
			*b++ = *red++ >> _SHIFT;
		}
		b = bits + r * bprl;
	}


	HBITMAP dib = CreateDIBitmap(hdc, (BITMAPINFOHEADER*)&info, CBM_INIT, bits, (BITMAPINFO*)&info, DIB_RGB_COLORS);
	free(bits);
	return dib;
}



// ----------------------------------------
// Cosntruct a HBITMAP for a 32 RGBA bitmap
// ----------------------------------------
HBITMAP _colored_dib_32(HDC hdc, const IMG *img)
{
	BITMAPV5HEADER v5;
	memset(&v5, 0, sizeof(v5));


	// Fill the header of the bitmapinfo struct
	// ----------------------------------------
	v5.bV5AlphaMask = _byte_to_mask(3);
	v5.bV5BitCount = 32;
	v5.bV5BlueMask = _byte_to_mask(2);
	v5.bV5Compression = BI_BITFIELDS;
	v5.bV5CSType = LCS_sRGB;
	v5.bV5GreenMask = _byte_to_mask(1);
	v5.bV5Height = img->height;
	//	v5.bV5Intent      = ...;
	v5.bV5Planes = 1;
	v5.bV5RedMask = _byte_to_mask(0);
	v5.bV5Size = sizeof(BITMAPV5HEADER);
	v5.bV5SizeImage = img->width * img->height;
	v5.bV5Width = img->width;

	// allocate the bitmap
	const int bprl = img->width * sizeof(uint32_t); // bytes per raster line
	byte* bits = malloc(bprl * img->height);
	if (!bits)
		return (HBITMAP)0;


	byte *b = (byte*)bits;
	IMG_COLOR *red = img->red;
	IMG_COLOR *green = img->green;
	IMG_COLOR *blue = img->blue;
	for (unsigned n = img->nPxls; n > 0; n--/*, b += sizeof(uint32_t)*/) {
		*b++ = *red++ >> _SHIFT;
		*b++ = *green++ >> _SHIFT;
		*b++ = *blue++ >> _SHIFT;
		*b++ = 0xFF;
	}


	HBITMAP dib = CreateDIBitmap(hdc, (BITMAPINFOHEADER*)&v5, CBM_INIT, bits, (BITMAPINFO*)&v5, DIB_RGB_COLORS);
	free(bits);
	return dib;
}



// ----------------------------------------------------------------------------------
// allocate HBITMAP and populate the bits. 
// If the image is colored, a 24 bits per color bitmap is generated.
// If the image is grey, a paletted 8 bits bitmap is generated.
// in case of failure (HBITMAP)0 is returned
// ----------------------------------------------------------------------------------
HBITMAP img_dib(HDC hdc, const IMG *img)
{
	if (!img)
		return NULL;

	HDC hdcNew;
	if (!hdc)
		hdcNew = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	else
		hdcNew = hdc;

	HBITMAP dib = NULL;
	switch (img->nPlanes) {
		case IMG_COLOR_PLANES: 
			dib = _colored_dib_24(hdcNew, img); break;
		case 1: 
			dib = _grey_dib(hdcNew, img); break;
	}

	if (!hdc)
		DeleteDC(hdcNew);

	return dib;
}

static unsigned const __endian = 0x03020100;
static byte* _endian = (byte*)&__endian;
unsigned _byte_to_mask(byte b)
{
	static unsigned mask[sizeof(uint32_t)] = { 0xFF, 0xFF00, 0xFF0000, 0xFF000000 };

	if (b < 0 || b >= sizeof(uint32_t))
		return 0;

	return mask[_endian[b]];
}
