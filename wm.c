
/*
* the code in this file processes Windows message on behalf of the Window procecdure 
* associated with the winodow class IMP1_WINDOW_CLASS defined in imp1.h
*/
#include <windows.h>
#include "imp1.h"
//#include "imagep1.h"
#include "path.h"
#include "str_util.h"


#include "img.h"


#pragma warning(disable : 4996)

static IMG *_cmd_open(HWND hWnd, struct State *state, const char path[]);
static IMG *_cmd_reopen(HWND hWnd, struct State *state, int iPath);
static void _cmd_rotate(HWND hWnd, struct State* state);


// process commands issued by the user
LRESULT imp1_wm_command(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    struct State *state = IMP1_STATE(hWnd);
    if (!state)
        return 0;

    int i;
    char path[PATH_MAX];
    WORD cmd = LOWORD(wParam);
    switch (cmd) {

        case IMP1_CMD_FILE_OPEN:
            if (open_file_dialog(path, sizeof(path), hWnd)) {
                if (_cmd_open(hWnd, state, path))
                    InvalidateRect(hWnd, NULL, TRUE);
                break;
            }


        case IMP1_CMD_ROTATE_ANTI:
            state->rotation += 90;
            _cmd_rotate(hWnd, state);
            break;

        case IMP1_CMD_ROTATE_CLOCK:
            state->rotation -= 90;
            _cmd_rotate(hWnd, state);
            break;


//    case IDM_ABOUT:
//        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//        break;

//        case IDM_EXIT:
//            DestroyWindow(hWnd);
//            break;

        default:
            if (cmd >= IMP1_CMD_FILE_NAME && cmd < IMP1_CMD_FILE_z) {
                i = cmd - IMP1_CMD_FILE_NAME;
                if (i >= 0 && i < state->nPaths)
                    if (_cmd_reopen(hWnd, state, i)) {
                        InvalidateRect(hWnd, NULL, TRUE);
                        break;
                    }
            }
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}




LRESULT imp1_wm_create(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static struct State state;
    memset (&state, 0, sizeof(struct State));
    state.iPath = -1;
    SetWindowLongPtr(hWnd, 0, (LONG_PTR)&state);

    HMENU hFile = CreatePopupMenu();
    AppendMenu(hFile, 0, IMP1_CMD_FILE_OPEN, L"&open...");

    HMENU hView = CreatePopupMenu();
    AppendMenu(hView, 0, IMP1_CMD_ROTATE_CLOCK, L"rotate clockwise");
    AppendMenu(hView, 0, IMP1_CMD_ROTATE_ANTI, L"rotate anti clockwise");
    AppendMenu(hView, 0, IMP1_CMD_ROTATE_ANTI, L"&zoom");

    HMENU hMenu = CreateMenu();
    AppendMenu(hMenu, MF_POPUP,  (UINT_PTR)hFile, L"File");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hView, L"View");
    SetMenu(hWnd, hMenu);

    windows_console(300);
    _cmd_open(hWnd, &state, "c:/dev/mefathim/C-course/imageprocessing/imagep1/IMG-20181031-WA0000.bmp");
    return DefWindowProc(hWnd, message, wParam, lParam);
}


LRESULT imp1_wm_destory(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    SetWindowLongPtr(hWnd, 0, (LONG_PTR) NULL);
    PostQuitMessage(0);
    return 0;
}



// Re/Open an image file. Change image content, dib and menu
static int _now = 0;
static void _build_menu(HWND hWnd, struct State * state);
static IMG *_load(struct State* state, const char path[]);
IMG *_cmd_open(HWND hWnd, struct State *state, const char path[])
{
    IMG *img = _load(state, path);
    if (!img)
        return img;

    // Look if we already have this path in the array of paths
    int iPath = -1;
    for (int i=0; i < state->nPaths; i++)
        if (!strcmp(path, state->path[i])) {
            iPath = i;
            break;
        }

    // if it is a new path, add it to the array. 
    // If array is full, least recently accessed path is evicted
    _now++;
    if (iPath < 0) {

        // select which path to evict
        if (state->nPaths >= IMP1_IMAGES) {
            int dif;
            dif = _now - state->time[0];
            iPath = 0;
            for (int i = 1; i < IMP1_IMAGES; i++) 
                if (_now - state->time[i] > dif) {
                    dif = _now - state->time[1];
                    iPath = i;
                }
        } else
            iPath = state->nPaths++;

        strcpy(state->path[iPath], path);
    }

    state->iPath = iPath;
    state->time[iPath] = _now;


    _build_menu(hWnd, state);
    return img;
}



// reopen an image that is already in the list of files
static IMG *_cmd_reopen(HWND hWnd, struct State* state, int iPath)
{
    if (iPath < 0 || iPath >= state->nPaths)
        return(IMG *)0;
    IMG *img = _load(state, state->path[iPath]);
    if (!img)
        return img;

    state->time[iPath] = ++_now;
    state->iPath = iPath;
    _build_menu(hWnd, state);
    return img;
}


// load image from file
static IMG *_load(struct State *state, const char path[])
{
    IMG *img = img_load(path);
    if (!img)
        return img;
    if (state->img)
        img_delete(state->img);
    state->img = img;

    // generate a DIB for this image
    if (state->dib)
        DeleteObject(state->dib);
    state->dib = (HBITMAP)0;
    return img;
}



// build the "file" menu
static void _build_menu(HWND hWnd, struct State* state)
{
    HMENU hMenu = GetMenu(hWnd);
    if (!hMenu)
        return;

    HMENU hFile = CreatePopupMenu();
    AppendMenu(hFile, 0, IMP1_CMD_FILE_OPEN, L"&open");
    AppendMenu(hFile, MF_SEPARATOR, 0, L"");
    for (int i = 0; i < state->nPaths; i++) {
        char tmp[PATH_MAX];
        pathCopyNameExt(tmp, sizeof(tmp), state->path[i]);
        wchar_t wtmp[PATH_MAX];
        str_2_wstr(wtmp, PATH_MAX, tmp);
        UINT flags = 0;
        if (i == state->iPath)
            flags |= MF_CHECKED;
        AppendMenu(hFile, flags, IMP1_CMD_FILE_NAME + i, wtmp);
    }
    DeleteMenu(hMenu, 0, MF_BYPOSITION);
    InsertMenu(hMenu, 0, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hFile, L"File");
}


static void _cmd_rotate(HWND hWnd, struct State *state)
{
    const int iPath = state->iPath;
    if (iPath < 0 || iPath >= state->nPaths)
        return;
    _load(state, state->path[iPath]);
    img_rotate(state->img, state->rotation, false);
    InvalidateRect(hWnd, NULL, true);
}
