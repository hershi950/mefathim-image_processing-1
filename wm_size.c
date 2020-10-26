#include <windows.h>
#include "img.h"
#include "imp1.h"

static void _show_scroll_bar (HWND hWnd, struct State *state, int orientation);
static void _hide_scroll_bar(HWND hWnd, struct State* state, int orientation);

LRESULT imp1_wm_size(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	struct State *state = IMP1_STATE(hWnd);
	if (!state)
		return 0;

	if (!state->img) {
		_hide_scroll_bar(hWnd, state, SB_HORZ);
		_hide_scroll_bar(hWnd, state, SB_VERT);
		return 0;
	}

	int width = LOWORD(lParam);
	int height  = HIWORD(lParam);
	int imgWidth, imgHeight;
	img_dim(&imgWidth, &imgHeight, state->img);

	if (imgWidth <= width) {
		_hide_scroll_bar(hWnd, state, SB_HORZ);
		return 0;
	}
	if (imgHeight <= height) {
		_hide_scroll_bar(hWnd, state, SB_VERT);
		return 0;
	}

	SCROLLINFO info;
	memset (&info, 0, sizeof(SCROLLINFO));
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_RANGE | SIF_POS;
	info.nMin = 0;

	if (imgWidth > width) {
		info.nMax = imgWidth - width;
		SetScrollInfo(hWnd, SB_HORZ, &info, true);
		_show_scroll_bar(hWnd, state, SB_HORZ);
	}

	if (imgHeight > height) {
		info.nMax = imgHeight - height;
		SetScrollInfo(hWnd, SB_VERT, &info, true);
		_show_scroll_bar(hWnd, state, SB_VERT);
	}

	return 0;
}

#define LINE_INCREMENT 5
#define PAGE_INCREMENT 50 

LRESULT imp1_wm_scroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	struct State *state = IMP1_STATE(hWnd);
	if (!state || !state->img)
		return 0;

	// Initialize variables that make the difference between verical and horizintal scrolling
	// The code is identical, except these tow variables
	int bar;  // vertical or horizonta?
	int *offset; 
	if (message == WM_VSCROLL) {
		bar = SB_VERT;
		offset = &state->sb_vert;
	} else if (message == WM_HSCROLL) {
		bar = SB_HORZ;
		offset = &state->sb_horz;
	} else 
		return 0;

	// obtain current scrollbar information
	// -------------------------------------
	SCROLLINFO info;
	memset (&info, 0, sizeof(SCROLLINFO));
	info.cbSize = sizeof(info);
	info.fMask = SIF_POS | SIF_RANGE | SIF_TRACKPOS;
	if (!GetScrollInfo(hWnd, bar, &info))
		return 0;

	int pos = info.nPos;
	int range = info.nMax;

	switch (LOWORD(wParam)) {

		// User clicked the scroll bar shaft left of the scroll box. 
		case SB_PAGEUP: pos -= PAGE_INCREMENT; break;

		// User clicked the scroll bar shaft right of the scroll box. 
		case SB_PAGEDOWN: pos += PAGE_INCREMENT; break;

		// User clicked the left arrow. 
		case SB_LINEUP: pos -= LINE_INCREMENT; break;

		// User clicked the right arrow. 
		case SB_LINEDOWN: pos += LINE_INCREMENT; break;

		// User dragged/dragging the scroll box. 
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION: 	 pos = info.nTrackPos; break;
	}

	// make sure new position is within scrollbar range 
	if (pos < 0)
		pos = 0;
	if (pos >= range)
		pos = range - 1;


	// update the scroll box position and invalidate
	if (pos != info.nPos) {
		info.fMask = SIF_POS;
		info.nPos = *offset = pos;
		SetScrollInfo(hWnd, bar, &info, TRUE);
		InvalidateRect(hWnd, NULL, false);
	}
	return 0;
}


// show a scroll bar and record in state that it is shown
void _show_scroll_bar(HWND hWnd, struct State* state, int orientation)
{
	ShowScrollBar(hWnd, orientation, TRUE);
	if (orientation == SB_HORZ)
		state->isScrollbarHorz = true;
	else
		state->isScrollbarVert = true;
}


// hide a scroll bar and record in state that it is shown
void _hide_scroll_bar(HWND hWnd, struct State* state, int orientation)
{
	ShowScrollBar(hWnd, orientation, FALSE);
	if (orientation == SB_HORZ)
		state->isScrollbarHorz = false;
	else
		state->isScrollbarVert = false;
}


