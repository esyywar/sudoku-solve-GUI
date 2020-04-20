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

// Control window handles
HWND title, subTitle, speedBar, solveBtn, restartBtn, closeBtn, gridFrame, cellFrame[9], *grid;

// Functions to render GUI
void addControls();
void addFonts();

// Handle for solver thread
HANDLE sudokuSolveThread;


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
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 6);
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
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 750, NULL, NULL, hInstance, NULL);

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
            sudokuSolveThread = CreateThread(NULL, NULL, sudokuSolveDriver, sudokuData, 0, &solverThreadID);

            if (sudokuSolveThread == NULL)
            {
                MessageBox(hWnd, L"Error! Failed so solve", L"Thread Failure", MB_ICONERROR);
            }

        } break;
        case RESTART_BTN_CLICK:
        {        
            if (sudokuSolveThread)
            {
                LPDWORD exitCode;
                GetExitCodeThread(sudokuSolveThread, &exitCode);
                TerminateThread(sudokuSolveThread, exitCode);
                EnableWindow(solveBtn, true);
            }
            

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
        switch (wParam)
        {
        case MSG_FROM_SDKU_SOLVE:
        {
            // Disable solve button from sending again
            EnableWindow(solveBtn, false);

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
                SetDlgItemText(hWnd, boxAddr, NULL);
            }

            UpdateWindow(hWnd);
            RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW);
        } break;
        case MSG_FROM_SDKU_DRIVER:
        {
            if (((LPNMHDR)lParam)->code == SOLVE_THREAD_COMPLETE)
            {
                EnableWindow(solveBtn, true);
            }
        }
        }
    } break;
    case WM_CREATE:
    {
        addControls(hWnd);
        addFonts(hWnd);
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
    // Title of window as static control
    title = CreateWindowEx(0, L"STATIC", L"SUDOKU SOLVER", WS_CHILD | WS_VISIBLE | ES_CENTER, 85, 20, 300, 40, hWnd, (HMENU)STATIC_TITLE_HEADER, NULL, NULL);
    ShowWindow(title, 1);
    UpdateWindow(title);

    // Subtitle of window
    subTitle = CreateWindowEx(0, L"STATIC", L"Backtracking algorithm visualizer", WS_CHILD | WS_VISIBLE | ES_CENTER, 85, 60, 300, 20, hWnd, (HMENU)STATIC_SUBTITLE, NULL, NULL);
    ShowWindow(title, 1);
    UpdateWindow(title);

    // Creating a static control to act as frame for the sudoku
    gridFrame = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_THICKFRAME, 35, 95, 415, 415, hWnd, (HMENU)STATIC_GRID_FRAME, NULL, NULL);
    ShowWindow(gridFrame, 1);
    UpdateWindow(gridFrame);

    // Create 3x3 grid cells
    int xFrame, yFrame, framePitch = 135;
    for (int i = 0; i < 9; i++)
    {
        xFrame = 40 + ((i % 3) * framePitch);
        yFrame = 100 + ((i / 3) * framePitch);

        cellFrame[i] = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, xFrame, yFrame, framePitch, framePitch, hWnd, (HMENU)STATIC_CELL_FRAME, NULL, NULL);
        ShowWindow(cellFrame[i], 1);
        UpdateWindow(cellFrame[i]);
    }

    // Creating 9x9 edit controls to act as sudoku boxes
    grid = (HWND*)calloc(81, sizeof(HWND));
    int xCell, yCell, xOffset = 45, yOffset = 105, boxPitch = 42, spacing = boxPitch + 1, offset = 2;
    for (int i = 0; i < 81; i++)
    {
        // space each 3x3 cell apart in x-dimension
        switch (i % 9)
        {
        case 0:
            xOffset = 44;
            break;
        case 3:
            xOffset = 50;
            break;
        case 6:
            xOffset = 54;
            break;
        }

        // space each 3x3 cell in y-dimension
        switch (i / 27)
        {
        case 0:
            yOffset = 104;
            break;
        case 1:
            yOffset = 110;
            break;
        case 2:
            yOffset = 114;
            break;
        }

        xCell = xOffset + ((i % 9) * spacing);
        yCell = yOffset + ((i / 9) * spacing);

        grid[i] = CreateWindowExW(0, L"EDIT", 0, WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER | WS_BORDER, xCell, yCell, boxPitch, boxPitch, hWnd, (HMENU)(SUDOKU_CTRL_BASE_VALUE + i), NULL, NULL);
        ShowWindow(grid[i], 1);
        UpdateWindow(grid[i]);
    }

    // Create speed set slider (window handle created global)
    speedBar = CreateWindowEx(0, TRACKBAR_CLASS, L"SPEED", WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS, 85, 535, 300, 40, hWnd, (HMENU)SPEED_TRACKBAR, NULL, NULL);
    SendMessage(speedBar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 10));
    ShowWindow(speedBar, 1);
    UpdateWindow(speedBar);

    // Solve button control
    solveBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"SOLVE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 40, 605, 110, 40, hWnd, (HMENU)SOLVE_BTN_CLICK, NULL, NULL);
    ShowWindow(solveBtn, 1);
    UpdateWindow(solveBtn);

    // Restart button control
    restartBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"RESTART", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 190, 605, 110, 40, hWnd, (HMENU)RESTART_BTN_CLICK, NULL, NULL);
    ShowWindow(restartBtn, 1);
    UpdateWindow(restartBtn);

    // Close button control
    closeBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"CLOSE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 340, 605, 110, 40, hWnd, (HMENU)CLOSE_BTN_CLICK, NULL, NULL);
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
    lfHeight = -MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72);
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

/*
* Driver function to solve sudoku - processed in new thread when 'Solve' clicked 
*/
DWORD WINAPI sudokuSolveDriver(LPVOID lpParam)
{
    pSudokuData inputSudoku = (pSudokuData)(lpParam);

    // Allocate memory for 2D sudoku array
    int sudoku[9][9] = { 0 };
 
    for (int i = 0; i < 81; i++)
    {
        uint8_t row = i / 9;
        uint8_t column = i % 9;
        sudoku[row][column] = inputSudoku->sudoku[i];
    }

    // Prepare notify message to send upon thread completion
    NMHDR nmh;
    nmh.hwndFrom = inputSudoku->hWnd;
    nmh.idFrom = MSG_FROM_SDKU_DRIVER;
    nmh.code = SOLVE_THREAD_COMPLETE;

    // Validate sudoku and set all squares 0 for blank, or 1 - 9 (TODO "MEMORY CORRUPTION IS HERE!")
    valuesCheck(inputSudoku->hWnd, sudoku);

    // Check rows, columns, cells for validity
    bool validPuzzle = isValidColumns(sudoku) && isValidRows(sudoku) && isValidCells(sudoku);

    if (!validPuzzle)
    {
        // Send message that solve function has completed
        SendMessage(nmh.hwndFrom, WM_NOTIFY, nmh.idFrom, &nmh);

        MessageBox(inputSudoku->hWnd, L"This puzzle does not meet sudku rules!", L"Puzzle Invalid", MB_OK | MB_ICONERROR);

        // Exit this thread
        LPDWORD ExitCode;
        GetExitCodeThread(sudokuSolveThread, &ExitCode);
        ExitThread(ExitCode);
    }

    // Calling to recursive backtracking solver
    bool isSolved = solveSudoku(inputSudoku->hWnd, sudoku, inputSudoku->speed);

    // Send message that solve function has completed (TODO send message of completion status and let main window display messagebox so that thread can be exited here)
    SendMessage(nmh.hwndFrom, WM_NOTIFY, nmh.idFrom, &nmh);

    if (isSolved)
    {
        MessageBox(inputSudoku->hWnd, L"Puzzle has been solved!", L"Solved", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(inputSudoku->hWnd, L"This puzzle has no solution!", L"No Solution", MB_OK | MB_ICONERROR);
    }
}