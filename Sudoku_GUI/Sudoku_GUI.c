// Sudoku_GUI.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Resource.h"
#include "Sudoku_GUI.h"

#define MAX_LOADSTRING      100
#define NUMBER_ENTERED      1

// Global Variables:
HINSTANCE hInst;                            // current instance
WCHAR szTitle[MAX_LOADSTRING];              // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];        // the main window class name

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void AddControls();


// Main windows function
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SUDOKUGUI, szWindowClass, MAX_LOADSTRING);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SUDOKUGUI));

    HWND hWnd = { 0 };
    WNDCLASSEX wcex = { 0 };
    MSG msg;

    // Create window class
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SUDOKUGUI));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SUDOKUGUI);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    // Register window class
    if (!RegisterClassExW(&wcex))
    {
        MessageBoxExW(hWnd, L"Window class registration failed!", L"Registration Error", MB_OK | MB_ICONERROR, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
        return 0;
    }

    // Create window
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 650, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
       MessageBoxExW(hWnd, L"Create window failed!", L"Window Error", MB_OK | MB_ICONERROR, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
       return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
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
            // Parse the menu selections:
            switch (wmId)
            {
            case NUMBER_ENTERED:
            {

            } break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE:
    {
        AddControls(hWnd);
    } break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
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

//Adding sudoku grid controls
void AddControls(HWND hWnd)
{
    HWND title, gridFrame, grid[9][9], solveBtn, restartBtn, closeBtn;

    title = CreateWindowEx(0, L"STATIC", L"SUDOKU SOLVER", WS_CHILD | WS_VISIBLE | ES_CENTER, 85, 30, 300, 40, hWnd, NULL, NULL, NULL);
    ShowWindow(title, 1);
    UpdateWindow(title);

    gridFrame = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_THICKFRAME, 50, 95, 370, 370, hWnd, NULL, NULL, NULL);
    ShowWindow(gridFrame, 1);
    UpdateWindow(gridFrame);

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            grid[i][j] = CreateWindowExW(0, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER | WS_BORDER, 55 + (i * 40), 100 + (j * 40), 40, 40, hWnd, (HMENU)NUMBER_ENTERED, NULL, NULL);
            ShowWindow(grid[i][j], 1);
            UpdateWindow(grid[i][j]);
        } 
    }

    solveBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"SOLVE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 40, 500, 100, 40, hWnd, NULL, NULL, NULL);
    ShowWindow(solveBtn, 1);
    UpdateWindow(solveBtn);

    restartBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"RESTART", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 190, 500, 100, 40, hWnd, NULL, NULL, NULL);
    ShowWindow(restartBtn, 1);
    UpdateWindow(restartBtn);

    closeBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"CLOSE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 340, 500, 100, 40, hWnd, NULL, NULL, NULL);
    ShowWindow(closeBtn, 1);
    UpdateWindow(closeBtn);
}
