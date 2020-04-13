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

// Window for speed bar
HWND speedBar;

// Functions to render GUI
void addControls();
void addFonts();
void paintWindow();

// Handle for solver thread
HANDLE sudokuSolveThread;

// Solver thread function
DWORD WINAPI sudokuSolveDriver(LPVOID lpParam);


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
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
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
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 695, NULL, NULL, hInstance, NULL);

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

/*
*   FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
*
*   WM_COMMAND  - process the application menu
*   WM_PAINT    - Paint the main window
*   WM_DESTROY  - post a quit message and return
*
*   PURPOSE: Processes messages for the main window.
*
*/
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
            int userSudoku[81];

            pSudokuData sudokuData;
            DWORD solverThreadID;
            sudokuData = (pSudokuData)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SudokuData));

            BOOL didGet = FALSE;
            
            // Get all values in grid which will be passed to solving function
            for (int i = 0; i < 81; i++)
            {
                userSudoku[i] = GetDlgItemInt(hWnd, (SUDOKU_CTRL_BASE_VALUE + i), &didGet, FALSE);
            }

            // Preparing data to send in new thread
            sudokuData->hWnd = hWnd;
            sudokuData->speed = SendMessage(speedBar, TBM_GETPOS, NULL, NULL);
            sudokuData->sudoku = userSudoku;

            // Run function to solve sudoku in new thread
            sudokuSolveThread = CreateThread(NULL, 0, sudokuSolveDriver, sudokuData, 0, &solverThreadID);

            if (sudokuSolveThread == NULL)
            {
                MessageBox(hWnd, L"Error! Failed so solve", L"Thread Failure", MB_ICONERROR);
            }

        } break;
        case RESTART_BTN_CLICK:
        {        
            for (int i = 0; i < 81; i++)
            {
                int row = i / 9;
                int column = i % 9;
                SetDlgItemText(hWnd, (SUDOKU_CTRL_BASE_VALUE + i), NULL);
            }
        } break;
        case CLOSE_BTN_CLICK:
        {
            SuspendThread(sudokuSolveThread);

            btnResp = MessageBox(hWnd, L"Are you sure you want to exit?", L"Exit", MB_YESNO | MB_ICONEXCLAMATION);

            if (btnResp == IDYES)
            {
                DestroyWindow(hWnd);
            }
            else
            {
                ResumeThread(sudokuSolveThread);
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
        addControls(hWnd);
        addFonts(hWnd);
        paintWindow(hWnd);
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

/*
* Adding sudoku grid controls, static titles and buttons
*/
void addControls(HWND hWnd)
{
    HWND title, subTitle, solveBtn, restartBtn, closeBtn, gridFrame, grid[81];

    // Title of window as static control
    title = CreateWindowEx(0, L"STATIC", L"SUDOKU SOLVER", WS_CHILD | WS_VISIBLE | ES_CENTER, 85, 25, 300, 40, hWnd, (HMENU)STATIC_TITLE_HEADER, NULL, NULL);
    ShowWindow(title, 1);
    UpdateWindow(title);

    // Subtitle of window
    subTitle = CreateWindowEx(0, L"STATIC", L"Backtracking algorithm visualizer", WS_CHILD | WS_VISIBLE | ES_CENTER, 85, 65, 300, 20, hWnd, (HMENU)STATIC_SUBTITLE, NULL, NULL);
    ShowWindow(title, 1);
    UpdateWindow(title);


    // Creating a static control to act as frame for the sudoku
    gridFrame = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_THICKFRAME, 44, 99, 382, 382, hWnd, (HMENU)STATIC_GRID_FRAME, NULL, NULL);
    ShowWindow(gridFrame, 1);
    UpdateWindow(gridFrame);

    // Creating 9x9 edit controls to act as sudoku boxes
    for (int i = 0; i < 81; i++)
    {
        int xStart = 55 + ((i % 9) * 40);
        int yStart = 110 + ((i / 9) * 40);

        grid[i] = CreateWindowExW(0, L"EDIT", 0, WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER | WS_BORDER, xStart, yStart, 40, 40, hWnd, (HMENU)(SUDOKU_CTRL_BASE_VALUE + i), NULL, NULL);
        ShowWindow(grid[i], 1);
        UpdateWindow(grid[i]);
    }

    // Create speed set slider (window handle created global)
    speedBar = CreateWindowEx(0, TRACKBAR_CLASS, L"SPEED", WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS, 85, 500, 300, 40, hWnd, (HMENU)SPEED_TRACKBAR, NULL, NULL);
    SendMessage(speedBar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 10));
    ShowWindow(speedBar, 1);
    UpdateWindow(speedBar);

    // Solve button control
    solveBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"SOLVE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 40, 565, 110, 40, hWnd, (HMENU)SOLVE_BTN_CLICK, NULL, NULL);
    ShowWindow(solveBtn, 1);
    UpdateWindow(solveBtn);

    // Restart button control
    restartBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"RESTART", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 190, 565, 110, 40, hWnd, (HMENU)RESTART_BTN_CLICK, NULL, NULL);
    ShowWindow(restartBtn, 1);
    UpdateWindow(restartBtn);

    // Close button control
    closeBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"CLOSE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 340, 565, 110, 40, hWnd, (HMENU)CLOSE_BTN_CLICK, NULL, NULL);
    ShowWindow(closeBtn, 1);
    UpdateWindow(closeBtn);
}

/*
* Adds the desired fonts to title, sudoku cells and buttons
*/
void addFonts(HWND hWnd)
{
    // Create fonts for controls
    HFONT controlFont;
    HDC hdc;
    long lfHeight;
    hdc = GetDC(NULL);

    // Create font for title
    lfHeight = -MulDiv(20, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    controlFont = CreateFont(lfHeight, 0, 0, 0, FW_NORMAL, TRUE, 0, 0, 0, OUT_TT_ONLY_PRECIS, 0, ANTIALIASED_QUALITY, 0, L"Showcard Gothic");
    SendDlgItemMessage(hWnd, STATIC_TITLE_HEADER, WM_SETFONT, (WPARAM)controlFont, TRUE);

    // Create font for the number cells
    lfHeight = -MulDiv(18, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    controlFont = CreateFont(lfHeight, 0, 0, 0, FW_SEMIBOLD, TRUE, 0, 0, 0, OUT_TT_ONLY_PRECIS, 0, ANTIALIASED_QUALITY, 0, L"Bahnschrift Semibold");

    for (int i = 0; i < 81; i++)
    {
        SendDlgItemMessage(hWnd, (SUDOKU_CTRL_BASE_VALUE + i), WM_SETFONT, (WPARAM)controlFont, TRUE);
    }

    // Create font for buttons
    lfHeight = -MulDiv(14, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    controlFont = CreateFont(lfHeight, 0, 0, 0, FW_NORMAL, TRUE, 0, 0, 0, OUT_TT_ONLY_PRECIS, 0, ANTIALIASED_QUALITY, 0, L"Arial");

    for (int i = SOLVE_BTN_CLICK; i < CLOSE_BTN_CLICK + 1; i++)
    {
        SendDlgItemMessage(hWnd, i, WM_SETFONT, (WPARAM)controlFont, TRUE);
    }
   
    ReleaseDC(NULL, hdc);
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


DWORD WINAPI sudokuSolveDriver(LPVOID lpParam)
{
    pSudokuData inputSudoku = (pSudokuData)(lpParam);

    // Unpack data into 2D array
    int sudoku[9][9];

    for (int i = 0; i < 81; i++)
    {
        uint8_t row = i / 9;
        uint8_t column = i % 9;
        sudoku[row][column] = inputSudoku->sudoku[i];
    }

    // Validate sudoku and set all squares 0 for blank, or 1 - 9
    // This function is also fixing some data corruption... Unsure why it occurs
    sudokuValidate(sudoku);

    if (solveSudoku(inputSudoku->hWnd, sudoku, inputSudoku->speed))
    {
        MessageBox(inputSudoku->hWnd, L"Puzzle has been solved!", L"Solved", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(inputSudoku->hWnd, L"This puzzle has no solution!", L"No Solution", MB_OK | MB_ICONERROR);
    }
}
