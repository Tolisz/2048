// 2048.cpp : Defines the entry point for the application.
//


#include "framework.h"
#include "2048.h"
#include <time.h>

#include <iostream>
#include <fstream>
using namespace std;

//#include <winresrc.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

RECT GlobWRect;
RECT GlobURect;

HWND GlobhWndParent;
HWND GlobhWndChild;

int pole = 70;
int odstep = 15;

int field[4][4] = { {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1} };

int WhatAnimate[4][4] = {{-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}};
int AnimateLeft = 0;    // 6
int HowManyTimeRefresh = 4;
int Rozmiar[5] = { 3, 5, 7, 5, 3 };

int xpoj = -1;
int ypoj = -1;
int Pojawienie[5] = { -50, -40, -30, -20, -10 };


bool gamestarted = false;
bool keydown = false;
int counter = 0;

int EndGameValue = 2048;

bool gamewin = false;
bool gamelose = false;

bool ifRefreshed = true;

string GlobalFile = "save.txt";

// My Function
void RysujPole(HWND hWnd, HDC hdc, HDC memDC, int numer, int i, int j);
void ObslugaWASD(WPARAM wParam);
void WindowsRefresh();
void NumberGenerator();
void DownShift(bool &haveToChange);
void UpShift(bool& haveToChange);
void LeftShift(bool& haveToChange);
void RightShift(bool& haveToChange);
void CheckIfWin();
void CheckIfLose();

void WriteToFile(string filename);
void ReadFromFile(string filename);

void RysujPolePowiekszone(HWND hWnd, HDC hdc, HDC memDC, int numer, int i, int j, int k);
void AnimateUpShift();
void AnimateDownShift();
void AnimateLeftShift();
void AnimateRightShift();

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
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

    srand(time(NULL));
    ReadFromFile(GlobalFile);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY2048, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    
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


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(CreateSolidBrush(RGB(250, 247, 238)));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY2048);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   POINT s = { 100, 100 };  // Koordynaty gdzie pojawi się przestrzeń klijenta.
   POINT l = { 4 * pole + 5 * odstep, 5 * pole + 6 * odstep };  // szyrokość i wysokość przestrzeni klijenta.
   RECT rc;
   rc.left = s.x; rc.top = s.y;
   rc.right = l.x + s.x; rc.bottom = l.y + s.y;
   RECT rcCopy = rc;

   GlobURect = rc;
   AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);
   GlobWRect = rc;

   
   HWND hWnd = CreateWindowW(szWindowClass,
                            szTitle,
                            WS_OVERLAPPEDWINDOW ,       
                            rc.left,
                            rc.top, 
                            rc.right - rc.left,
                            rc.bottom - rc.top,
                            nullptr,
                            nullptr,
                            hInstance,
                            nullptr);

   if (!hWnd)
      return FALSE;

   GlobhWndParent = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   
   rc = rcCopy;
   AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX, TRUE);
   HWND hWndChild = CreateWindowW(szWindowClass,
       szTitle,
       WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX,
       rc.left,
       rc.top,
       rc.right - rc.left,
       rc.bottom - rc.top,
       hWnd,
       nullptr,
       hInstance,
       nullptr);

   if (!hWndChild)
       return FALSE;


   SetWindowLong(hWndChild, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
   SetLayeredWindowAttributes(hWndChild, 0, (255 * 100) / 100, LWA_ALPHA);
   
   
   GlobhWndChild = hWndChild;
   
   ShowWindow(hWndChild, nCmdShow);
   UpdateWindow(hWndChild);

   return TRUE;
}

/*
const int AbufSize = 256;
TCHAR Abuf[AbufSize];

_stprintf_s(buf, bufSize, _T("dziawam"));
SetWindowText(hWnd, buf);
*/

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            
            HMENU hmenu = GetMenu(hWnd);
            
            MENUITEMINFO mi8;
            MENUITEMINFO mi16;
            MENUITEMINFO mi64;
            MENUITEMINFO mi2048;

            mi8.cbSize = sizeof(MENUITEMINFO);
            mi8.fMask = MIIM_STATE;
            mi16.cbSize = sizeof(MENUITEMINFO);
            mi16.fMask = MIIM_STATE;
            mi64.cbSize = sizeof(MENUITEMINFO);
            mi64.fMask = MIIM_STATE;
            mi2048.cbSize = sizeof(MENUITEMINFO);
            mi2048.fMask = MIIM_STATE;

            GetMenuItemInfo(hmenu, ID_GOAL_8, FALSE, &mi8);
            GetMenuItemInfo(hmenu, ID_GOAL_16, FALSE, &mi16);
            GetMenuItemInfo(hmenu, ID_GOAL_64, FALSE, &mi64);
            GetMenuItemInfo(hmenu, ID_GOAL_2048, FALSE, &mi2048);

            switch (wmId)
            {
                case IDM_NEWGAME:
                    gamestarted = false;
                    gamewin = false;
                    gamelose = false;

                    WindowsRefresh();
                    break;

                case ID_GOAL_8:
                    if (mi8.fState == MFS_UNCHECKED) {
                        mi8.fState = MFS_CHECKED;
                        mi16.fState = MFS_UNCHECKED;
                        mi64.fState = MFS_UNCHECKED;
                        mi2048.fState = MFS_UNCHECKED;
                        EndGameValue = 8;
                    }
                    break;

                case ID_GOAL_16:
                    if (mi16.fState == MFS_UNCHECKED)
                    {
                        mi8.fState = MFS_UNCHECKED;
                        mi16.fState = MFS_CHECKED;
                        mi64.fState = MFS_UNCHECKED;
                        mi2048.fState = MFS_UNCHECKED;
                        EndGameValue = 16;
                    }
                    break;

                case ID_GOAL_64:
                    if (mi64.fState == MFS_UNCHECKED)
                    {
                        mi8.fState = MFS_UNCHECKED;
                        mi16.fState = MFS_UNCHECKED;
                        mi64.fState = MFS_CHECKED;
                        mi2048.fState = MFS_UNCHECKED;
                        EndGameValue = 64;
                    }
                    break;

                case ID_GOAL_2048:
                    if (mi2048.fState == MFS_UNCHECKED)
                    {
                        mi8.fState = MFS_UNCHECKED;
                        mi16.fState = MFS_UNCHECKED;
                        mi64.fState = MFS_UNCHECKED;
                        mi2048.fState = MFS_CHECKED;
                        EndGameValue = 2048;
                    }
                    break;

                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }

            SetMenuItemInfo(hmenu, ID_GOAL_8, FALSE, &mi8);
            SetMenuItemInfo(hmenu, ID_GOAL_16, FALSE, &mi16);
            SetMenuItemInfo(hmenu, ID_GOAL_64, FALSE, &mi64);
            SetMenuItemInfo(hmenu, ID_GOAL_2048, FALSE, &mi2048);

        }
        break;

    case WM_PAINT:
        {
            
            WriteToFile(GlobalFile);
            
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            HDC memDC = CreateCompatibleDC(hdc);
            
            RECT tlo;
            tlo.bottom = tlo.right = 1000;
            tlo.left = tlo.top = 0;

            HBRUSH brush = CreateSolidBrush(RGB(250, 247, 238));
            FillRect(hdc, &tlo, brush);
            DeleteObject(brush);

            // Rysowanie planszy do gry
            HBITMAP bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
            HBITMAP oldbitmap = (HBITMAP)SelectObject(memDC, bitmap);
            
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    BitBlt(hdc, odstep + j*(pole+odstep), (pole+2*odstep) + i*(pole + odstep), pole, pole, memDC, 0, 0, SRCCOPY);
                }
            }

            SelectObject(memDC, oldbitmap);
            DeleteObject(bitmap);

            bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
            oldbitmap = (HBITMAP)SelectObject(memDC, bitmap);
            BitBlt(hdc, odstep, odstep, 4 * pole + 3 * odstep, pole, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, oldbitmap);
            DeleteObject(bitmap);
            // Koniec rysowania planszy 


            // Początek nowej gry
            if (!gamestarted)
            {
                gamestarted = true;
                gamewin = false;
                gamelose = false;
                counter = 0;

                for (int i = 0; i < 4; i++)
                    for (int j = 0; j < 4; j++)
                        field[i][j] = -1;

                field[rand()%4][rand()%4] = 2;
            }


            // ANIMACJA
            if (AnimateLeft == 0)
            {
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        RysujPole(hWnd, hdc, memDC, field[i][j], odstep + j * (pole + odstep), (pole + 2 * odstep) + i * (pole + odstep));
                    }
                }

                for (int i = 0; i < 4; i++)
                    for (int j = 0; j < 4; j++)
                        WhatAnimate[i][j] = -1;
            }
            else {
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (i == xpoj && j == ypoj)
                        {
                            RysujPolePowiekszone(hWnd, hdc, memDC, 2, odstep + j * (pole + odstep), (pole + 2 * odstep) + i * (pole + odstep), Pojawienie[AnimateLeft]);
                            continue;
                        }

                        if (WhatAnimate[i][j] == -1)
                            RysujPole(hWnd, hdc, memDC, field[i][j], odstep + j * (pole + odstep), (pole + 2 * odstep) + i * (pole + odstep));
                        else
                            RysujPolePowiekszone(hWnd, hdc, memDC, field[i][j], odstep + j * (pole + odstep), (pole + 2 * odstep) + i * (pole + odstep), Rozmiar[AnimateLeft]);
                    }
                }
                
                AnimateLeft--;
                
                xpoj = -1;
                ypoj = -1;
            }


            // Rysujemy counter
            const int bufSize = 64;
            TCHAR buf[bufSize];

            _stprintf_s(buf, bufSize, _T("%d"), counter);

            RECT rc;
            rc.top = odstep;
            rc.left = odstep;
            rc.right = 4 * (odstep + pole);
            rc.bottom = odstep + pole;

            HFONT font = CreateFont(
                -MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                0, // Width
                0, // Escapement
                0, // Orientation
                FW_BOLD, // Weight
                false, // Italic
                FALSE, // Underline
                0, // StrikeOut
                EASTEUROPE_CHARSET, // CharSet
                OUT_DEFAULT_PRECIS, // OutPrecision
                CLIP_DEFAULT_PRECIS, // ClipPrecision
                DEFAULT_QUALITY, // Quality
                DEFAULT_PITCH | FF_SWISS, // PitchAndFamily
                _T(" Verdana ")); // Facename

            HFONT oldFont = (HFONT)SelectObject(hdc, font);
            
            SetBkColor(hdc, RGB(204, 192, 174));
            SetTextColor(hdc, RGB(255, 255, 255));
            //SelectObject(hdc, oldColor);

            DrawText(hdc, buf, (int)_tcslen(buf), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, oldFont);
            DeleteObject(font);

            //Rysujemy komunikat że wygraliśmy lub przegraliśmy
            if (gamewin)
            {

                bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BT_WIN));
                oldbitmap = (HBITMAP)SelectObject(memDC, bitmap);

                BLENDFUNCTION bf;
                bf.BlendOp = AC_SRC_OVER;
                bf.BlendFlags = 0;
                bf.SourceConstantAlpha = 127;
                bf.AlphaFormat = 0; // AC_SRC_ALPHA;

                AlphaBlend(hdc, 0, 0, 355, 440, memDC, 0, 0, 355, 440, bf);
                
                SelectObject(hdc, oldbitmap);
                DeleteObject(bitmap);
            }
            if (gamelose)
            {
                bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BT_LOSE));
                oldbitmap = (HBITMAP)SelectObject(memDC, bitmap);

                BLENDFUNCTION bf;
                bf.BlendOp = AC_SRC_OVER;
                bf.BlendFlags = 0;
                bf.SourceConstantAlpha = 127;
                bf.AlphaFormat = 0;// AC_SRC_ALPHA;

                AlphaBlend(hdc, 0, 0, 355, 440, memDC, 0, 0, 355, 440, bf);

                SelectObject(hdc, oldbitmap);
                DeleteObject(bitmap);
            }

            DeleteDC(memDC);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_KEYDOWN:
    {
        if (gamewin || gamelose)
            break;

        if (!keydown)
        {
            keydown = true;
            ObslugaWASD(wParam);
            WindowsRefresh();
            
            CheckIfWin();
            if (gamewin)    break;
            
            CheckIfLose();
            if (gamelose)   break;
        }
    }
    break;

    case WM_KEYUP:
    {
        keydown = false;
        //WindowsRefresh();
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* i = (MINMAXINFO*)lParam;
        
        // Maksymalny rozmiar okna w trybie "Na cały ekran"
        
        i->ptMaxSize.x = GlobWRect.right - GlobWRect.left;
        i->ptMaxSize.y = GlobWRect.bottom - GlobWRect.top;

        // Ustalamy "TrackSize" nie pozwalający zmieniać rozmiar okna
        i->ptMaxTrackSize.x = GlobWRect.right - GlobWRect.left;
        i->ptMaxTrackSize.y = GlobWRect.bottom - GlobWRect.top;
        
        i->ptMinTrackSize.x = GlobWRect.right - GlobWRect.left;
        i->ptMinTrackSize.y = GlobWRect.bottom - GlobWRect.top;
    }
    break;

    case WM_MOVE:
    {
        int cx = GetSystemMetrics(SM_CXSCREEN) / 2;
        int cy = GetSystemMetrics(SM_CYSCREEN) / 2;

        RECT nrc;
        GetWindowRect(hWnd, &nrc);

        int x1 = nrc.right;
        int y1 = nrc.bottom;

        int x2 = cx + (cx - x1);
        int y2 = cy + (cy - y1);
        
        if (hWnd == GlobhWndParent)
            MoveWindow(GlobhWndChild, x2, y2, GlobWRect.right - GlobWRect.left, GlobWRect.bottom - GlobWRect.top, TRUE);
        else if (hWnd == GlobhWndChild)
            MoveWindow(GlobhWndParent, x2, y2, GlobWRect.right - GlobWRect.left, GlobWRect.bottom - GlobWRect.top, TRUE);

        RECT p, c;
        GetWindowRect(GlobhWndParent, &p);
        GetWindowRect(GlobhWndChild, &c);

        if ( (c.left < p.left && p.left < c.right && p.top < c.top && c.top < p.bottom) ||
             (p.left < c.left && c.left < p.right && c.top < p.top && p.top < c.bottom) || 
             (c.left < p.left && p.left < c.right && c.top < p.top && p.top < c.bottom) ||
             (p.left < c.left && c.left < p.right && p.top < c.top && c.top < p.bottom)
           )
        {
            SetLayeredWindowAttributes(GlobhWndChild, 0, (255 * 50) / 100, LWA_ALPHA);
        }
        else
        {
            SetLayeredWindowAttributes(GlobhWndChild, 0, (255 * 100) / 100, LWA_ALPHA);
        }
    }
    break;

    case WM_CREATE:
    {
        SetTimer(hWnd, 12, 70, NULL);
    }
    break;

    case WM_TIMER:
    {
        if (wParam == 12 && AnimateLeft != 0)
        {
            WindowsRefresh();
            ifRefreshed = false;
        }
        else if (AnimateLeft == 0 && ifRefreshed == false)
        {
            /*
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    WhatAnimate[i][j] = -1;
            */
            ifRefreshed = true;
            WindowsRefresh();
        }
    }
    break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


/*
*   MY FUNKCIONS 
*/

void RysujPolePowiekszone(HWND hWnd, HDC hdc, HDC memDC, int numer, int i, int j, int k)
{
    HBITMAP bitmap, oldbitmap;

    switch (numer)
    {
    case 2:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_2)); break;
    case 4:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_4)); break;
    case 8:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_8)); break;
    case 16:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_16)); break;
    case 32:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_32)); break;
    case 64:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_64)); break;
    case 128:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_128)); break;
    case 256:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_256)); break;
    case 512:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_512)); break;
    case 1024:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_1024)); break;
    case 2048:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_2048)); break;
    default:    return;
    }

    oldbitmap = (HBITMAP)SelectObject(memDC, bitmap);
    StretchBlt(hdc, i - k, j - k, pole + 2*k, pole + 2*k, memDC, 0, 0, pole, pole, SRCCOPY);

    SelectObject(memDC, oldbitmap);
    DeleteObject(bitmap);
}

void RysujPole(HWND hWnd, HDC hdc, HDC memDC, int numer, int i, int j)
{
    HBITMAP bitmap, oldbitmap;

    switch (numer)
    {
    case 2:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_2)); break;
    case 4:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_4)); break;
    case 8:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_8)); break;
    case 16:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_16)); break;
    case 32:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_32)); break;
    case 64:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_64)); break;
    case 128:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_128)); break;
    case 256:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_256)); break;
    case 512:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_512)); break;
    case 1024:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_1024)); break;
    case 2048:     bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POLE_2048)); break;
    default:    return;
    }

    oldbitmap = (HBITMAP)SelectObject(memDC, bitmap);
    BitBlt(hdc, i, j, pole, pole, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldbitmap);
    DeleteObject(bitmap);
}

void ObslugaWASD(WPARAM wParam)
{
    bool haveToChange = false;

    switch (wParam)
    {
    case 'W':  // W - góra
        
        UpShift(haveToChange);

        // Konkatenacja takich samych liczb
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 3; i++)
            {
                if (field[i][j] != -1 && field[i + 1][j] != -1 && field[i][j] == field[i + 1][j])
                {
                    field[i][j] *= 2;
                    field[i+1][j] = -1;
                    haveToChange = true;

                    counter += field[i][j];

                    WhatAnimate[i][j] = field[i][j];
                }
            }
        }
        
        AnimateUpShift();

        UpShift(haveToChange); // Już skonkatenowane liczby poruszamy do 'W' - góry 
        if (haveToChange) NumberGenerator();

        break;

    case 'A':  // A - lewo

        LeftShift(haveToChange);

        // Konkatenacja takich samych liczb
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (field[i][j] != -1 && field[i][j+1] != -1 && field[i][j] == field[i][j+1])
                {
                    field[i][j] *= 2;
                    field[i][j+1] = -1;
                    haveToChange = true;

                    counter += field[i][j];

                    WhatAnimate[i][j] = field[i][j];
                }
            }
        }

        AnimateLeftShift();

        LeftShift(haveToChange); // Już skonkatenowane liczby poruszamy w 'A' - lewo 
        if (haveToChange) NumberGenerator();

        break;

    case 'D':  // D

        RightShift(haveToChange);

        // Konkatenacja takich samych liczb
        for (int i = 0; i < 4; i++)
        {
            for (int j = 2; j >= 0; j--)
            {
                if (field[i][j] != -1 && field[i][j + 1] != -1 && field[i][j] == field[i][j + 1])
                {
                    field[i][j + 1] *= 2;
                    field[i][j] = -1;
                    haveToChange = true;

                    counter += field[i][j + 1];

                    WhatAnimate[i][j + 1] = field[i][j + 1];
                }
            }
        }

        AnimateRightShift();

        RightShift(haveToChange);// Już skonkatenowane liczby poruszamy w 'D' - prawo 
        if (haveToChange) NumberGenerator();

        break;

    case 'S':  // S - dół
        
        DownShift(haveToChange);

        // Konkatenacja takich samych liczb
        for (int j = 0; j < 4; j++)
        {
            for (int i = 2; i >= 0; i--)
            {
                if (field[i][j] != -1 && field[i + 1][j] != -1 && field[i][j] == field[i + 1][j])
                {
                    field[i + 1][j] *= 2;
                    field[i][j] = -1;
                    haveToChange = true;

                    counter += field[i + 1][j];

                    WhatAnimate[i + 1][j] = field[i + 1][j];
                }
            }
        }

        AnimateDownShift();

        DownShift(haveToChange); // Już skonkatenowane liczby poruszamy do 'S' - Dołu
        if (haveToChange) NumberGenerator(); // Generujemy na wolnym miejscu planszy nowy obiekt

        break;
    }
}

void WindowsRefresh()
{
    InvalidateRect(GlobhWndChild, NULL, FALSE);
    InvalidateRect(GlobhWndParent, NULL, FALSE);
}

void NumberGenerator()
{
    bool isFULL = true;
    int buf[2][16] = { -1 }; // 0 - i; 1 - j
    int s = 0;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (field[i][j] == -1)
            {
                isFULL = false;
                buf[0][s] = i;
                buf[1][s] = j;
                s++;
            }
        }
    }

    if (isFULL)
    {
        return;
    }

    int k = rand() % s;
    
    field[buf[0][k]][buf[1][k]] = 2;
    
    xpoj = buf[0][k];
    ypoj = buf[1][k];
}


// Funkcje odpowiadające za poruszenie płaszczyzną.
void DownShift(bool& haveToChange)
{
    if (!haveToChange)
        for (int j = 0; j < 4; j++)
            for (int i = 0; i < 3; i++)
                if (field[i + 1][j] == -1 && field[i][j] != -1)
                    haveToChange = true;

    for (int j = 0; j < 4; j++)
    {
        int buf[4] = { -1, -1, -1, -1 };
        int s = 3;

        for (int i = 3; i >= 0; i--)
            if (field[i][j] != -1)
                buf[s--] = field[i][j];

        for (int i = 0; i < 4; i++)
            field[i][j] = buf[i];
    }
}
void UpShift(bool& haveToChange)
{
    if (!haveToChange)
        for (int j = 0; j < 4; j++)
            for (int i = 0; i < 3; i++)
                if (field[i][j] == -1 && field[i+1][j] != -1)
                    haveToChange = true;

    for (int j = 0; j < 4; j++)
    {
        int buf[4] = { -1, -1, -1, -1 };
        int s = 0;

        for (int i = 0; i < 4; i++)
            if (field[i][j] != -1)
                buf[s++] = field[i][j];

        for (int i = 0; i < 4; i++)
            field[i][j] = buf[i];
    }
}
void LeftShift(bool& haveToChange)
{
    if (!haveToChange)
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 3; j++)
                if (field[i][j] == -1 && field[i][j + 1] != -1)
                    haveToChange = true;

    for (int i = 0; i < 4; i++)
    {
        int buf[4] = { -1, -1, -1, -1 };
        int s = 0;

        for (int j = 0; j < 4; j++)
            if (field[i][j] != -1)
                buf[s++] = field[i][j];

        for (int j = 0; j < 4; j++)
            field[i][j] = buf[j];
    }
}
void RightShift(bool& haveToChange)
{
    if (!haveToChange)
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 3; j++)
                if (field[i][j] != -1 && field[i][j + 1] == -1)
                    haveToChange = true;

    for (int i = 0; i < 4; i++)
    {
        int buf[4] = { -1, -1, -1, -1 };
        int s = 3;

        for (int j = 3; j >= 0; j--)
            if (field[i][j] != -1)
                buf[s--] = field[i][j];

        for (int j = 0; j < 4; j++)
            field[i][j] = buf[j];
    }
}

void CheckIfWin()
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (field[i][j] == EndGameValue)
                gamewin = true;
            
}

void CheckIfLose()
{
    
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (field[i][j] == -1)
                return;

    // Jeśli wszystkie pola są zapewnione to sprawdzamy czy możemy jeszcze coś skonkatenować
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i - 1 >= 0 && field[i - 1][j] == field[i][j] || // góra
                i + 1 < 4  && field[i + 1][j] == field[i][j] || // dół
                j - 1 >= 0 && field[i][j - 1] == field[i][j] || // lewo
                j + 1 < 4  && field[i][j + 1] == field[i][j] ) // prawo
                return;
        }
    }

    gamelose = true;
}

void WriteToFile(string filename)
{
    ofstream myfile;
    myfile.open(filename, ios::out);

    // field
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            myfile << field[i][j] << "\n";
        }
    }
    
    myfile << counter << "\n";
    myfile << gamestarted << "\n";
    myfile << gamewin << "\n";
    myfile << gamelose << "\n";

    myfile.close();
}

void ReadFromFile(string filename)
{
    ifstream myfile;

    myfile.open(filename);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            myfile >> field[i][j];
        }
    }

    myfile >> counter;
    myfile >> gamestarted;
    myfile >> gamewin;
    myfile >> gamelose;

    myfile.close();
}


void AnimateUpShift()
{
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            if (WhatAnimate[i][j] != -1)
            {           
                int s = 0;
                for (int k = i - 1; k >= 0; k--)
                {
                    if (field[k][j] != -1)
                        s++;
                }

                WhatAnimate[s][j] = WhatAnimate[i][j];
                if (s != i) WhatAnimate[i][j] = -1;
            }
        }
    }

    AnimateLeft = HowManyTimeRefresh;
}

void AnimateDownShift()
{
    for (int j = 0; j < 4; j++)
    {
        for (int i = 3; i >= 0; i--)
        {
            if (WhatAnimate[i][j] != -1)
            {
                int s = 0;
                for (int k = i + 1; k < 4; k++)
                    if (field[k][j] != -1)
                        s++;

                WhatAnimate[3 - s][j] = WhatAnimate[i][j];
                if (3 - s != i) WhatAnimate[i][j] = -1;
            }
        }
    }

    AnimateLeft = HowManyTimeRefresh;
}
void AnimateLeftShift()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (WhatAnimate[i][j] != -1)
            {
                int s = 0;
                for (int k = j - 1; k >= 0; k--)
                    if (field[i][k] != -1)
                        s++;

                WhatAnimate[i][s] = WhatAnimate[i][j];
                if (s != j) WhatAnimate[i][j] = -1;
            }
        }
    }

    AnimateLeft = HowManyTimeRefresh;
}
void AnimateRightShift()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 3; j >= 0; j--)
        {
            if (WhatAnimate[i][j] != -1)
            {
                int s = 0;
                for (int k = j + 1; k < 4; k++)
                    if (field[k][j] != -1)
                        s++;

                WhatAnimate[i][3 - s] = WhatAnimate[i][j];
                if (3 - s != j) WhatAnimate[i][j] = -1;
            }
        }
    }

    AnimateLeft = HowManyTimeRefresh;
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