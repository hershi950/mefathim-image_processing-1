#include <stdio.h>
#include <stdint.h>

#include "img_.h"



static IMG_ERR _load(IMG *img, FILE *f);
static IMG_ERR _load_bmp_v5(IMG *img, FILE *f);
static IMG_ERR _load_bmp_ih(IMG *img, BITMAPINFOHEADER* ih, FILE *f);
static     int _mask_to_byte(unsigned mask);
static IMG_ERR _my_read(void* buf, unsigned zBuf, FILE* f);


 /* --------------------------------------------------------------- *\
|             Allocate and construct an image from a file			  |
 \* --------------------------------------------------------------- */
IMG* img_load(const char path[])
{
	FILE *f;

	IMG img;
	memset(&img, 0, sizeof(IMG));

#pragma warning (push, 1)
#pragma warning(disable : 4996)
	if (!(f = fopen(path, "rb")))
#pragma warning (pop)
		return (IMG *)0;

	IMG_ERR err = _load(&img, f);
	fclose(f);

	if (err) {
		img_dtor(&img);
		return (IMG *)0;
	}

	IMG *ret = malloc(sizeof(IMG));
	if (ret)
		memcpy(ret, &img, sizeof(IMG));
	return ret;
}


#define	LZW_SIG	23120
#define	BMP_SIG	19778


static IMG_ERR _load(IMG *img, FILE *f)
{
	BITMAPFILEHEADER	FH;
	HANDLE hLZW = 0;
	BYTE* mLZW = 0;
	IMG_ERR err;


	/* Check type of file: BMP / LZWed */
	/* ------------------------------- */
	if (err = _my_read((char*)&FH.bfType, sizeof(FH.bfType), f))
		return err;
	if (FH.bfType != BMP_SIG/* && FH.bfType != LZW_SIG*/)
		return IMG_ERR_UK;


	/* If this is an LZWed file, expand it to its full extent in memory	*/
	/* ---------------------------------------------------------------- */
	if (FH.bfType == LZW_SIG) {
		//		hLZW = lzw_expand(hResInfo, fd);
		//		if (!hLZW)
		return IMG_ERR_UK;
		//		mLZW = (BYTE *) GlobalLock (hLZW);
	}


	/* Read the rest of the File Header (FH)  */
	/* -------------------------------------- */
	if (hLZW) {
		memcpy(&FH, mLZW, sizeof(FH));
		mLZW = mLZW + sizeof(FH);
	}
	else
		if (err = _my_read(((LPSTR)&FH) + sizeof(FH.bfType), sizeof(FH) - sizeof(FH.bfType), f))
			return err;

	// read information header
	// ------------------------
	int zIH = FH.bfOffBits - sizeof(FH);  // size of information header
	if (zIH > sizeof(img->header))
		return IMG_ERR_UK;
	if (err = _my_read(&img->header, zIH, f))
		return err;
	if (img->header.size != zIH)
		return IMG_ERR_UK;

	// Build according to information header type
	switch (zIH) {
		case sizeof(BITMAPINFOHEADER) : return _load_bmp_ih(img, &img->header.infoHeader, f);
			case sizeof(BITMAPV5HEADER) : return _load_bmp_v5(img, f);
	}
	return IMG_ERR_UK;
}


// -------------------------------------------------------
// Load a Windows .BMP file that has BITMAPV5HEADER header
// -------------------------------------------------------
static IMG_ERR _load_bmp_v5(IMG* img, FILE* f)
{
	IMG_ERR _f_err;
	BITMAPV5HEADER* v5 = &img->header.v5;
	if (v5->bV5Size != sizeof(BITMAPV5HEADER))
		return IMG_ERR_UK;

	// We support 32 bits per pixel RGBA color space without compression and without a palette
	if (v5->bV5BitCount != sizeof(int) * 8 ||   // 32 bits per pixel alphaRGB color space
		v5->bV5Compression != BI_BITFIELDS || // no compression
		v5->bV5CSType != LCS_sRGB ||          // RGBA color space
		v5->bV5ClrUsed)                       // no palette
		return IMG_ERR_UK;
	img->nPlanes = IMG_COLOR_PLANES;

	// Decode the byte position of the color components in a 32 bits int
	// -----------------------------------------------------------------
	int iRed = _mask_to_byte(v5->bV5RedMask);
	int iGreen = _mask_to_byte(v5->bV5GreenMask);
	int iBlue = _mask_to_byte(v5->bV5BlueMask);
	if (iRed < 0 || iBlue < 0 || iGreen < 0)
		return IMG_ERR_UK;
	if (iRed == iGreen || iGreen == iBlue || iRed == iBlue)
		return IMG_ERR_UK;

	img->width = v5->bV5Width;
	img->height = v5->bV5Height;
	img->nPxls = img->width * img->height;
	img->nBytes = img->nPxls * sizeof(int);
	if (img->nBytes != v5->bV5SizeImage)
		return IMG_ERR_UK;

	// allocate three color planes
	if (!(img->pxl = malloc(sizeof(*img->pxl) * img->nPxls * IMG_COLOR_PLANES)))
		return IMG_ERR_ALLOC;

	// allocate a buffer to read the entire file and read it
	unsigned* bits;
	if (!(bits = malloc(img->nPxls * sizeof(uint32_t))))
		return IMG_ERR_ALLOC;
	if (_f_err = _my_read(bits, img->nPxls * sizeof(unsigned), f))
		return _f_err;

	IMG_COLOR* red = img->red = img->pxl;
	IMG_COLOR* green = img->green = img->red + img->nPxls;
	IMG_COLOR* blue = img->blue = img->green + img->nPxls;

	unsigned* pxl = bits;
	for (unsigned n = img->nPxls; n > 0; n--, pxl++) {
		byte* b = (byte*)pxl;
		*red++ = b[iRed] << _SHIFT;
		*green++ = b[iGreen] << _SHIFT;
		*blue++ = b[iBlue] << _SHIFT;
	}

	//memcpy (bmp->pxl, bits, bmp->nBytes);

	free(bits);

	return 0;
}



// -------------------------------------------------------
// Load a Windows .BMP fie that has BITMPINFOHEADER header
// -------------------------------------------------------
static IMG_ERR _load_bmp_ih(IMG *img, BITMAPINFOHEADER *ih, FILE *f)
{
	IMG_ERR err;
	BITMAPINFOHEADER IH;
	memcpy(&IH, ih, sizeof(BITMAPINFOHEADER));


	// allocate bytes to read the bitmap and read
	// ------------------------------------------
	int bpp = (IH.biBitCount == 24) ? 3 : 0; // bits per pixel
	if (!bpp)
		return IMG_ERR_UK;
	byte* bits = malloc(IH.biWidth * IH.biHeight * bpp);
	if (!bits)
		return IMG_ERR_ALLOC;
	if (err = _my_read(bits, IH.biWidth * IH.biHeight * bpp, f)) {
		free(bits);
		return err;
	}


	// basic initilaization
	// --------------------
	img->width = IH.biWidth;
	img->height = IH.biHeight;
	img->nPxls = img->width * img->height;
	img->nPlanes = IMG_COLOR_PLANES;
	img->nBytes = img->nPxls * sizeof(*img->pxl) * IMG_COLOR_PLANES;


	// allocate BMP color planes
	// -------------------------
	img->pxl = malloc(img->nBytes);
	if (!img->pxl) {
		free(bits);
		return IMG_ERR_ALLOC;
	}


	// Populate the color planes
	byte* b = bits;
	IMG_COLOR* red = img->red = img->pxl;
	IMG_COLOR* green = img->green = img->red + img->width * img->height;
	IMG_COLOR* blue = img->blue = img->green + img->width * img->height;
	for (unsigned n = img->nPxls; n > 0; n--) {
		*blue++ = *b++ << _SHIFT;
		*green++ = *b++ << _SHIFT;
		*red++ = *b++ << _SHIFT;
	}

	free(bits);



#if 0
	int		nColors;		/* Number of colros in the color table of the map */
	RGBQUAD	quad[256];
	/* Build Color table */
/* ----------------- */
	if (!IH.biClrUsed)
		nColors = 1 << IH.biBitCount;
	else
		nColors = (int)IH.biClrUsed;
	if (nColors <= sizeof(quad) / sizeof(quad[0]))
		if (hLZW) {
			memcpy(quad, mLZW, sizeof(RGBQUAD) * nColors);
			mLZW = mLZW + (sizeof(RGBQUAD) * nColors);
		}
		else
			if (err = _my_read((char*)quad, sizeof(RGBQUAD) * nColors, f))
				return err;


	/* Compute size of image in bytes and allocate memory */
	/* -------------------------------------------------- */
	if ((bprl = _bprl((int)IH.biWidth, IH.biBitCount)) <= 0)
		return IMG_ERR_UK;
	if (IH.biSizeImage)
		zMem = IH.biSizeImage;
	else
		zMem = bprl * ((long)IH.biHeight);
	if (!(mMem = malloc(zMem)))
		IMG_ERR_ALLOC;



	/* Load the bits */
	/* ------------- */
	if (hLZW)
		for (; ;)
			if (zMem > (unsigned long)0xFFF0) {
				memcpy(mMem, mLZW, 0xFFF0);
				mMem = mMem + (unsigned)0xFFF0;
				mLZW = mLZW + (unsigned)0xFFF0;
				zMem = zMem - (unsigned long)0xFFF0;
			}
			else {
				memcpy(mMem, mLZW, (unsigned int)zMem);
				GlobalUnlock(hLZW);
				GlobalFree(hLZW);
				break;
			}
	else
		for (; ;)
			if (zMem > (unsigned long)0xFFF0) {
				if (err = _my_read((char*)mMem, 0xFFF0, f))
					return err;
				mMem = mMem + (unsigned)0xFFF0;
				zMem = zMem - (unsigned long)0xFFF0;
			}
			else {
				if (err = _my_read((char*)mMem, zMem, f))
					return err;
				break;
			}



	/* Create the IMG struct */
	/* --------------------- */
	bmp = (IMG*)malloc(sizeof(IMG));
	memcpy(&bmp->IH, &IH, sizeof(IH));
	if (nColors <= sizeof(quad) / sizeof(quad[0]))
		memcpy(bmp->quad, quad, sizeof(RGBQUAD) * nColors);
	bmp->bit = mMem;
	bmp->nColors = nColors;
	bmp->bprl = bprl;
#endif
	return IMG_ERR_OK;
}




static unsigned const __endian = 0x03020100;
static byte* _endian = (byte*)&__endian;

int _mask_to_byte(unsigned mask)
{
	int i = -1;
	switch (mask) {
	case 0xFF000000: i = 3; break;
	case 0x00FF0000: i = 2; break;
	case 0x0000FF00: i = 1; break;
	case 0x000000FF: i = 0; break;
	}
	if (i >= 0)
		i = _endian[i];
	return i;
}

static IMG_ERR _my_read(void* buf, unsigned zBuf, FILE* f)
{
	return fread(buf, 1, zBuf, f) != zBuf ? IMG_ERR_READ : 0;
}
