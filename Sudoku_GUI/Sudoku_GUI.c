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
void addControls(HWND hWnd);
void addFonts(HWND hWnd);

// Helper function to clear sudoku board
void clearSudokuBoard(HWND hWnd);

// Three locally stored sudokus for user to load
uint8_t localSDKU1[81] = {0, 0, 8, 4, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 9, 4, 0, 0, 4, 5, 0, 0, 0, 0, 2, 3, 0, 1, 0, 0, 6, 4, 0, 0, 0, 0, 0, 7, 2, 0, 8, 4, 0, 0, 0, 0, 0, 9, 7, 0, 0, 6, 0, 9, 6, 0, 0, 0, 0, 1, 5, 0, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 7, 0, 0};
uint8_t localSDKU2[81] = {0, 0, 0, 0, 2, 0, 6, 0, 9, 2, 6, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 4, 0, 0, 5, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 2, 9, 0, 8, 3, 0, 1, 7, 0, 5, 4, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 2, 0, 0, 4, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 1, 3, 6, 0, 9, 0, 7, 0, 0, 0, 0};
uint8_t localSDKU3[81] = {0, 0, 6, 0, 0, 0, 0, 0, 3, 8, 2, 0, 0, 9, 6, 0, 0, 0, 9, 3, 0, 4, 0, 0, 0, 1, 0, 7, 8, 0, 0, 1, 0, 0, 0, 0, 0, 9, 0, 0, 4, 0, 0, 3, 0, 0, 0, 0, 0, 7, 0, 0, 6, 2, 0, 1, 0, 0, 0, 2, 0, 4, 6, 0, 0, 0, 7, 3, 0, 0, 9, 1, 2, 0, 0, 0, 0, 0, 3, 0, 0};

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
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 540, 750, NULL, NULL, hInstance, NULL);

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
            int *userSudoku = (int*)malloc(81 * sizeof(int));

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
            sudokuData->speedWnd = speedBar;
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
            
            clearSudokuBoard(hWnd);
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
        {
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
        } break;
        case IDM_EXIT:
        {
            DestroyWindow(hWnd);
        } break;
        case ID_FILE_LOADSUDOKU:
        {
            /*
            * Load one of 3 puzzles from memory onto board
            */

            if (sudokuSolveThread)
            {
                LPDWORD exitCode;
                GetExitCodeThread(sudokuSolveThread, &exitCode);
                TerminateThread(sudokuSolveThread, exitCode);
                EnableWindow(solveBtn, true);
                EnableWindow(speedBar, true);
            }

            clearSudokuBoard(hWnd);

            uint8_t* pLoadedPuzzle;
            static uint8_t puzzleSelect = 0;

            puzzleSelect = puzzleSelect % 3;
            switch (puzzleSelect)
            {
            case 0: pLoadedPuzzle = localSDKU1; break;
            case 1: pLoadedPuzzle = localSDKU2; break;
            case 2: pLoadedPuzzle = localSDKU3; break;
            default: pLoadedPuzzle = localSDKU1;
            }

            for (int i = 0; i < 81; i++)
            {
                if (pLoadedPuzzle[i] != 0) SetDlgItemInt(hWnd, SUDOKU_CTRL_BASE_VALUE + i, pLoadedPuzzle[i], FALSE);
            }

            puzzleSelect++;
        } break;
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
            /*
            *   Brief: Message from sudoku driver means puzzle has been processed
            *
            *   Possible solve results: invalid user data, puzzle solved, puzzle has no solution
            */

            // Enable frozen controls
            EnableWindow(solveBtn, true);

            switch (((LPNMHDR)lParam)->code)
            {
            case SOLVE_INPUT_INVALID:
            {
                MessageBox(hWnd, L"This puzzle does not meet sudku rules!", L"Puzzle Invalid", MB_OK | MB_ICONERROR);
            } break;
            case SOLVE_SUDOKU_SOLVED:
            {
                MessageBox(hWnd, L"Puzzle has been solved!", L"Solved", MB_OK | MB_ICONINFORMATION);
            } break;
            case SOLVE_SUDOKU_NO_SOLN:
            {
                MessageBox(hWnd, L"This puzzle has no solution!", L"No Solution", MB_OK | MB_ICONERROR);
            }
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
            HDC hdc_x = CreateCompatibleDC(NULL);
            // Add any drawing code that uses hdc here...

            // Background colour for window
            HBRUSH myBrush = CreateSolidBrush(RGB(145, 171, 199));
            FillRect(hdc, &ps.rcPaint, myBrush);

            // Writing text to window
            RECT rectWnd;
            GetWindowRect(hWnd, &rectWnd);
            BitBlt(hdc, 0, 0, rectWnd.right - rectWnd.left, rectWnd.bottom, hdc_x, 0, 0, SRCCOPY);

            long lfHeight;
            HFONT hFont;

            // Write Title at top of window
            lfHeight = -MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72);
            hFont = CreateFont(lfHeight, 0, 0, 0, FW_NORMAL, TRUE, 0, 0, 0, OUT_TT_ONLY_PRECIS, 0, ANTIALIASED_QUALITY, 0, L"Showcard Gothic");
            SelectObject(hdc, hFont);
            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, 130, 20, L"SUDOKU SOLVER", strlen("SUDOKU SOLVER"));

            // Write subtitle under main title
            lfHeight = -MulDiv(14, GetDeviceCaps(hdc, LOGPIXELSY), 72);
            hFont = CreateFont(lfHeight, 0, 0, 0, FW_NORMAL, TRUE, 0, 0, 0, OUT_TT_ONLY_PRECIS, 0, PROOF_QUALITY, 0, L"Calibri");
            SelectObject(hdc, hFont);
            TextOut(hdc, 130, 60, L"Backtracking algorithm visualizer", strlen("Backtracking algorithm visualizer"));

            // Green colour "FAST" and red colour "SLOW"
            lfHeight = -MulDiv(20, GetDeviceCaps(hdc, LOGPIXELSY), 72);
            hFont = CreateFont(lfHeight, 0, 0, 0, FW_SEMIBOLD, TRUE, 0, 0, 0, OUT_TT_ONLY_PRECIS, 0, ANTIALIASED_QUALITY, 0, L"Bahnschrift Condensed");
            SelectObject(hdc, hFont);
            SetTextColor(hdc, RGB(18, 105, 41));
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, 35, 535, L"FAST!", strlen("FAST!"));

            SetTextColor(hdc, RGB(160, 0, 0));
            TextOut(hdc, 412, 535, L"SLOW!", strlen("SLOW!"));

            ReleaseDC(hWnd, hdc);
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
    // Creating a static control to act as frame for the sudoku
    gridFrame = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_THICKFRAME, 55, 95, 415, 415, hWnd, (HMENU)STATIC_GRID_FRAME, NULL, NULL);
    ShowWindow(gridFrame, 1);
    UpdateWindow(gridFrame);

    // Create 3x3 grid cells
    int xFrame, yFrame, framePitch = 135;
    for (int i = 0; i < 9; i++)
    {
        xFrame = 60 + ((i % 3) * framePitch);
        yFrame = 100 + ((i / 3) * framePitch);

        cellFrame[i] = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, xFrame, yFrame, framePitch, framePitch, hWnd, (HMENU)STATIC_CELL_FRAME, NULL, NULL);
        ShowWindow(cellFrame[i], 1);
        UpdateWindow(cellFrame[i]);
    }

    // Creating 9x9 edit controls to act as sudoku boxes
    grid = (HWND*)calloc(81, sizeof(HWND));
    int xCell, yCell, xOffset = 65, yOffset = 105, boxPitch = 42, spacing = boxPitch + 1, offset = 2;
    for (int i = 0; i < 81; i++)
    {
        // space each 3x3 cell apart in x-dimension
        switch (i % 9)
        {
        case 0:
            xOffset = 64;
            break;
        case 3:
            xOffset = 70;
            break;
        case 6:
            xOffset = 74;
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
    speedBar = CreateWindowEx(0, TRACKBAR_CLASS, L"SPEED", WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS, 105, 535, 300, 40, hWnd, (HMENU)SPEED_TRACKBAR, NULL, NULL);
    SendMessage(speedBar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 10));
    ShowWindow(speedBar, 1);
    UpdateWindow(speedBar);

    // Solve button control
    solveBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"SOLVE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 60, 605, 110, 40, hWnd, (HMENU)SOLVE_BTN_CLICK, NULL, NULL);
    ShowWindow(solveBtn, 1);
    UpdateWindow(solveBtn);

    // Restart button control
    restartBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"RESTART", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 210, 605, 110, 40, hWnd, (HMENU)RESTART_BTN_CLICK, NULL, NULL);
    ShowWindow(restartBtn, 1);
    UpdateWindow(restartBtn);

    // Close button control
    closeBtn = CreateWindowExW(WS_EX_CLIENTEDGE, L"BUTTON", L"CLOSE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, 360, 605, 110, 40, hWnd, (HMENU)CLOSE_BTN_CLICK, NULL, NULL);
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

    // Create font for the number cells
    lfHeight = -MulDiv(20, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    controlFont = CreateFont(lfHeight, 0, 0, 0, FW_NORMAL, TRUE, 0, 0, 0, OUT_TT_ONLY_PRECIS, 0, ANTIALIASED_QUALITY, 0, L"Calibri");

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
* Clears all values on active sudoku board
*/
void clearSudokuBoard(HWND hWnd)
{
    for (int i = 0; i < 81; i++)
    {
        int row = i / 9;
        int column = i % 9;
        SetDlgItemText(hWnd, (SUDOKU_CTRL_BASE_VALUE + i), NULL);
    }
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

    // Free memory allocated to input structure
    free(inputSudoku->sudoku);

    // Prepare notify message to send upon thread completion
    NMHDR callback;
    callback.hwndFrom = inputSudoku->hWnd;
    callback.idFrom = MSG_FROM_SDKU_DRIVER;

    // Validate sudoku and set all squares 0 for blank, or 1 - 9
    valuesCheck(inputSudoku->hWnd, sudoku);

    // Check rows, columns, cells for validity
    bool validPuzzle = isValidColumns(sudoku) && isValidRows(sudoku) && isValidCells(sudoku);

    if (!validPuzzle)
    {
        // Send message that solve function has completed
        callback.code = SOLVE_INPUT_INVALID;
        SendMessage(callback.hwndFrom, WM_NOTIFY, callback.idFrom, &callback);

        // Exit this thread
        LPDWORD ExitCode;
        GetExitCodeThread(sudokuSolveThread, &ExitCode);
        ExitThread(ExitCode);
    }

    // Calling to recursive backtracking solver
    bool isSolved = solveSudoku(inputSudoku->hWnd, inputSudoku->speedWnd, sudoku);

    // Send message that indicates if puzzle has been solved
    callback.code = (isSolved) ? SOLVE_SUDOKU_SOLVED : SOLVE_SUDOKU_NO_SOLN;
    SendMessage(callback.hwndFrom, WM_NOTIFY, callback.idFrom, &callback);
}