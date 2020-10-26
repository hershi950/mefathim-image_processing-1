


#if 0
static	HANDLE	   rle_2_non(IMG* bmp);
/* ==================================================================== *\
|						Convert a loaded .IMG file into a handle. 					|
 \* ==================================================================== */
HBITMAP	img_2_hdl(bmp)
IMG* bmp;
{
	HDC		hDC;
	IMG		cpy;
	BYTE	mBits;
	HBITMAP	hBitMap;


	if (!bmp)
		return ((HBITMAP)0);
	memcpy(&cpy, bmp, sizeof(IMG));


	/* Decompress the bitmap, if needed */
	/* -------------------------------- */
	if (cpy.IH.biCompression)
		mBits = rle_2_non(&cpy);
	else
		mBits = cpy.bit;


	/* Create the handle to the bitmap */
	/* ------------------------------- */
	hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	hBitMap = CreateDIBitmap(hDC, &cpy.IH, CBM_INIT, GlobalLock(hBits),
		(LPBITMAPINFO)&cpy, DIB_RGB_COLORS);

	/* Clean up */
	/* -------- */
	DeleteDC(hDC);
	if (bmp->IH.biCompression)
		GlobalFree(mBits);

	return (hBitMap);
}
#endif

/* ----------------------------------------------------------------- *\
|				Compute number of bytes per raster line 				|
 \* ----------------------------------------------------------------- */
static int _bprl(int Width, int BitCount)
{
	int	bprl;

	switch (BitCount) {
	case 4: bprl = (Width / 2) + (Width % 2); break;
	case 8: bprl = Width; break;
	default:
		abort();
	}
	if (bprl % 4)
		bprl = bprl + 4 - (bprl % 4);
	return (bprl);
}


#if 0
/* ----------------------------------------------------------------- *\
|		Decompress a Run Lenth Encoding (RLE) type 1 .IMG memory		|
 \* ----------------------------------------------------------------- */
static	HANDLE	rle_2_non(IMG* bmp)
{
	int		n;
	long		zMem;
	BYTE* s;
	BYTE* t;
	HANDLE 	hMem;			/* Handle to allocated memory */

	/* Compute the size of the bitmap after decompression, */
	/* allocate memory and lock bits in memory.				 */
	/* --------------------------------------------------- */
	zMem = ((long)img_bprl((int)bmp->IH.biWidth, bmp->IH.biBitCount)) * bmp->IH.biHeight;
	hMem = GlobalAlloc(GMEM_MOVEABLE, zMem);
	t = (BYTE*)GlobalLock(hMem);

	s = (BYTE*)GlobalLock(bmp->hBits);


	/* Decompress */
	/* ---------- */
	for (; ;)
		if (*s) {
			n = *s++;
			memset(t, *s, n);
			s++;
			t = t + n;
		}
		else {
			s++;
			n = *s++;
			if (!n)
				break;
			memcpy(t, s, n);
			t = t + n;
			s = s + n;
		}


	bmp->IH.biCompression = 0;
	bmp->IH.biSizeImage = zMem;
	return (hMem);
}
#endif


