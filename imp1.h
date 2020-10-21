
#pragma once

// image processing 1 
#include <stdbool.h>

#ifndef __imp1_h
#define __imp1_h

#ifndef __img_h
#include "img.h"
#endif

#ifndef __path_h
#include "path.h"
#endif

#define IMP1_WINDOW_CLASS L"IMP1_WINDOWS_CLASS"
#define IMP1_WINDOW_TITLE L"Mefathim Image Processing"


// The menu on the window
#define IMP1_CMD_FILE_ 100
#define IMP1_CMD_FILE_OPEN (IMP1_CMD_FILE_ + 1)
#define IMP1_CMD_FILE_NAME (IMP1_CMD_FILE_ + 2)
#define IMP1_CMD_FILE_z    (IMP1_CMD_FILE_ + 99)

#define IMP1_CMD_VIEW 200
#define IMP1_CMD_ROTATE_CLOCK (IMP1_CMD_VIEW + 1)
#define IMP1_CMD_ROTATE_ANTI (IMP1_CMD_VIEW + 2)


#define IMP1_IMAGES 10 // number of known images

#define IMP1_STATE(hWnd) (struct State *)GetWindowLongPtr(hWnd, 0) 


// the state describing which  image is displayed in the Window and how

struct State {
    IMG *img;     // currently displayed image or nulll
    HBITMAP dib; // dib associated with image.

    _Bool isScrollbarVert, isScrollbarHorz; // vertical or horizonatl scroll bars are shown?
    int sb_vert; int sb_horz; // scroll bar vertical and horizontal offset

    char path[IMP1_IMAGES][PATH_MAX]; // history of paths of loaded file
    int  time[IMP1_IMAGES]; // 'time' when path was last displayed
    int iPath; // currently displayed path
    int nPaths;//number of paths in the array
};


#endif
