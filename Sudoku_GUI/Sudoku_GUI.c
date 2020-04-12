// Sudoku_GUI.cpp : Defines the entry point for the application.
//

#include "sudoku_GUI.h"
#include "sudoku_macros.h"


// Global Variables:
HINSTANCE hInst;                            // current instance
WCHAR szTitle[MAX_LOADSTRING];              // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];        // the main window class name

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void AddControls();
void paintWindow();


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
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW - 1);
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
        int wp = LOWORD(wParam);
        int btnResp;

        // Parse the menu selections:
        switch (wp)
        {
        case 1:
        {

        } break;
        case SOLVE_BTN_CLICK:
        {
            int sudoku[9][9];
            BOOL didGet = FALSE;

            // Get all values in grid which will be passed to solving function
            for (int i = 0; i < 81; i++)
            {
                int row = i / 9;
                int column = i % 9;
                sudoku[row][column] = GetDlgItemInt(hWnd, (SUDOKU_CTRL_BASE_VALUE + i), &didGet, FALSE);
            }

            // Call function to solve in Sudoku_solve.c
            sudokuSolveDriver(hWnd, sudoku);
        } break;
        case RESTART_BTN_CLICK:
        {
            
            PMYDATA myTest;

            myTest = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MYDATA));

            myTest->val1 = 4;
            myTest->val2 = 10;
            myTest->hWnd = hWnd;

            DWORD solverThreadID;
            HANDLE solveThread = CreateThread(NULL, 0, test, myTest, 0, &solverThreadID);

            if (solveThread == NULL)
            {
                MessageBox(hWnd, L"FAILED THREAD", L"ERROR", MB_ICONERROR);
            }
            

            /*
            for (int i = 0; i < 81; i++)
            {
                int row = i / 9;
                int column = i % 9;
                SetDlgItemText(hWnd, (SUDOKU_CTRL_BASE_VALUE + i), NULL);
            }
            */

        } break;
        case CLOSE_BTN_CLICK:
        {
            btnResp = MessageBox(hWnd, L"Are you sure you want to exit?", L"Exit", MB_YESNO | MB_ICONEXCLAMATION);

            if (btnResp == IDYES)
            {
                DestroyWindow(hWnd);
            }
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
    } break;
    case WM_NOTIFY:
    {
        Sudoku_Append_t solveData = *((Sudoku_Append_t*)lParam);
        int row = solveData.row;
        int column = solveData.column;
        int boxAddr = SUDOKU_CTRL_BASE_VALUE + (row * 9) + column;

        if (solveData.action == SDKU_NUMBER_PUT)
        {
            int numPlace = solveData.number;
            SetDlgItemInt(hWnd, boxAddr, numPlace, FALSE);
        }
        else if (solveData.action == SDKU_NUMBER_RM)
        {
            SetDlgItemInt(hWnd, boxAddr, NULL, FALSE);
        }

        UpdateWindow(hWnd);
        RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW);
    } break;
    case WM_CREATE:
    {
        AddControls(hWnd);
        paintWindow();
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
    HWND title, solveBtn, restartBtn, closeBtn, gridFrame, grid[81];

    // Title of window as static control
    title = CreateWindowEx(0, L"STATIC", L"SUDOKU SOLVER", WS_CHILD | WS_VISIBLE | ES_CENTER, 85, 30, 300, 40, hWnd, NULL, NULL, NULL);
    ShowWindow(title, 1);
    UpdateWindow(title);

    // Creating a static control to act as frame for the sudoku
    gridFrame = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_THICKFRAME, 44, 89, 382, 382, hWnd, (HMENU)STATIC_GRID_FRAME, NULL, NULL);
    ShowWindow(gridFrame, 1);
    UpdateWindow(gridFrame);

    // Creating 9x9 edit controls to act as sudoku boxes
    for (int i = 0; i < 81; i++)
    {
        int xStart = 55 + ((i % 9) * 40);
        int yStart = 100 + ((i / 9) * 40);

        grid[i] = CreateWindowExW(0, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER | WS_BORDER, xStart, yStart, 40, 40, hWnd, (HMENU)(i + SUDOKU_CTRL_BASE_VALUE), NULL, NULL);
        ShowWindow(grid[i], 1);
        UpdateWindow(grid[i]);
    }

    // Solve button control
    solveBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"SOLVE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 40, 500, 100, 40, hWnd, (HMENU)SOLVE_BTN_CLICK, NULL, NULL);
    ShowWindow(solveBtn, 1);
    UpdateWindow(solveBtn);

    // Restart button control
    restartBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"RESTART", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 190, 500, 100, 40, hWnd, (HMENU)RESTART_BTN_CLICK, NULL, NULL);
    ShowWindow(restartBtn, 1);
    UpdateWindow(restartBtn);

    // Close button control
    closeBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"CLOSE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 340, 500, 100, 40, hWnd, (HMENU)CLOSE_BTN_CLICK, NULL, NULL);
    ShowWindow(closeBtn, 1);
    UpdateWindow(closeBtn);
}

void paintWindow()
{
    DWORD oldColour;

    oldColour = GetSysColor((HMENU)STATIC_GRID_FRAME);

    int purple = RGB(0x80, 0x00, 0x80);  // dark purple
    int element = STATIC_GRID_FRAME;

    bool try = SetSysColors(1, &element, &purple);
    printf(try ? "true" : "false");
}
