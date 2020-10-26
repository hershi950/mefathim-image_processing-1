// imagep1.cpp : Defines the entry point for the application.
//

#include "imp1.h"
#include "framework.h"
#include "imagep1.h"
#define nullptr NULL

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
//WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
//WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
static ATOM        _register_class(HINSTANCE hInstance, wchar_t *className);
static BOOL        _init_instance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
//    lstrcpyW (szWindowClass, IMP1_WINDOW_CLASS);
//    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
//    LoadStringW(hInstance, IDC_IMAGEP1, szWindowClass, MAX_LOADSTRING);
    _register_class(hInstance, IMP1_WINDOW_CLASS);

    // Perform application initialization:
    if (!_init_instance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IMAGEP1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM _register_class(HINSTANCE hInstance, wchar_t *className)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = sizeof(void *);
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IMAGEP1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_IMAGEP1);
    wcex.lpszClassName  = className; //szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL _init_instance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(IMP1_WINDOW_CLASS, IMP1_WINDOW_TITLE, WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT imp1_wm_command(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT imp1_wm_create(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT imp1_wm_destory(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT imp1_wm_paint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT imp1_wm_scroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT imp1_wm_size(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {

        case WM_CREATE:
            return imp1_wm_create(hWnd, message, wParam, lParam);

        case WM_COMMAND:
            return imp1_wm_command (hWnd, message, wParam, lParam);

        case WM_SIZE:
            return imp1_wm_size(hWnd, message, wParam, lParam);

        case WM_PAINT:
            return imp1_wm_paint(hWnd, message, wParam, lParam);

        case WM_DESTROY:
            return imp1_wm_destory(hWnd, message, wParam, lParam);

        case WM_VSCROLL:
        case WM_HSCROLL:
            return imp1_wm_scroll(hWnd, message, wParam, lParam);

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
