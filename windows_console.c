#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>

static FILE *_f_err = (FILE *)0;
static FILE *_f_inp = (FILE *)0;
static FILE *_f_out = (FILE *)0;

void windows_console(int max_lines)
{
    if (_f_err || _f_inp || _f_out)
        return;

    int fd;

	
	if (!AllocConsole()) // allocate a console for this app
        return;

	// set the screen buffer to be big enough to let us scroll text
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = max_lines;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    // redirect unbuffered STDOUT to the console
//    stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
//    hConHandle = _open_osfhandle(stdHandle, _O_TEXT);
    fd = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    _f_out = _fdopen(fd, "w"); 
    fprintf (_f_out, "helooooooooooo\n"); fflush(_f_out);
//    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);

    // redirect unbuffered STDIN to the console
    fd = _open_osfhandle((intptr_t)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
    _f_inp = _fdopen(fd, "r");
//    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);

    // redirect unbuffered STDERR to the console
    fd = _open_osfhandle((intptr_t)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
    _f_err = _fdopen(fd, "w");
//    *stderr = *fp;
    setvbuf(stderr, NULL, _IONBF, 0);

    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
//    ios::sync_with_stdio();
}


int windows_console_printf(const char* format, ...)
{
    if (!_f_out)
        return 0;

    va_list va;
    va_start(va, format);
    return vfprintf(_f_out, format, va);
}


int window_console_scanf(const char* format, ...)
{
    if (!_f_inp)
        return 0;

    va_list va;
    va_start(va, format);
    return vfscanf(_f_inp, format, va);
}

int windows_console_getchar()
{
    return _f_inp ? fgetc(_f_inp) : EOF;
}



//#include <comutil.h>
char *open_file_dialog(char path[], int zPath, HWND hWnd)
{
    OPENFILENAME ofn;       // common dialog box structure
    TCHAR szFile[260] = { 0 };       // if using TCHAR macros

    // Initialize OPENFILENAME
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"bitmap\0*.bmp\0"; //_T("All\0*.*\0Text\0*.TXT\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        int len = (int)wcslen(ofn.lpstrFile);
        if (len < zPath) {
            char *s = (char *)ofn.lpstrFile + sizeof(ofn.lpstrFile[0]) * (len - 1);
            path[len] = '\0';
            while (len--) {
                path[len] = *s;
                s -= sizeof(ofn.lpstrFile[0]);
            }
            return path;
        }
    }
    return (char *)0;
}