
#include "imp1.h"


LRESULT imp1_wm_paint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // map to the device independent bitmap that is about to be displayed.
    // if it does not exist yet, create it now. If you fail, don't paint
    struct State* state = IMP1_STATE(hWnd);
    if (!state || !state->img)
        return  0;
    if (!state->dib)
        state->dib = img_dib(NULL, state->img);
    if (!state->dib)
        return 0;

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HANDLE bmpCur = SelectObject(hdcMem, state->dib);

    int imgWidth, imgHeight;
    img_dim(&imgWidth, &imgHeight, state->img);

    BitBlt(hdc, 0, 0, imgWidth - state->sb_horz, imgHeight - state->sb_vert, hdcMem, state->sb_horz, state->sb_vert, SRCCOPY);

#if 0
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = rect.right;
    int height = rect.bottom;
    double ratiox = (double)width / imgWidth;
    double ratioy = (double)height / imgHeight;
    double ratio = ratiox < ratioy ? ratiox : ratioy;


    SetStretchBltMode(hdc, HALFTONE);
    StretchBlt(hdc, 0, 0, (int)(imgWidth * ratio), (int)(imgHeight * ratio), hdcMem, 0, 0, imgWidth, imgHeight, SRCCOPY),
        //    BitBlt(hdc, 0, 0, bmp->width, bmp->height, dcMem, 0, 0, SRCCOPY);
#endif

    SelectObject(hdcMem, bmpCur);
    DeleteObject(hdcMem);

    EndPaint(hWnd, &ps);
    return 0;
}

