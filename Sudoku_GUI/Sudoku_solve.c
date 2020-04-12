#include "framework.h"
#include "sudoku_macros.h"

void delay(int milli_seconds)
{
    // Storing start time 
    clock_t start_time = clock();

    // looping till required time is not achieved 
    while (clock() < start_time + milli_seconds);
}

/*
*   Print out the sudoku grid.
*   This function is called only after puzzle is solved.
*/
void printSoln(int sudoku[9][9])
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            printf("%i ", sudoku[i][j]);
        }
        printf("\n");
    }
}

/*
*   Check if placing the value will break the row
*/
bool rowCheck(int sudoku[9][9], int row, int number)
{
    for (int i = 0; i < 9; i++)
    {
        if (sudoku[row][i] == number)
        {
            return false;
        }
    }

    return true;
}

/*
*   Check if placing the value will break the column
*/
bool columnCheck(int sudoku[9][9], int column, int number)
{
    for (int i = 0; i < 9; i++)
    {
        if (sudoku[i][column] == number)
        {
            return false;
        }
    }

    return true;
}

/*
*   Check if placing the value will break the cell
*/
bool boxCheck(int sudoku[9][9], int row, int column, int number)
{
    int cellRow = (row / 3) * 3;
    int cellColumn = (column / 3) * 3;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (sudoku[cellRow + i][cellColumn + j] == number)
            {
                return false;
            }
        }
    }

    return true;
}

/*
*   Checks row, column and cell to determine if number can be placed in a position
*/
bool isValid(int sudoku[9][9], int row, int column, int number)
{
    if (rowCheck(sudoku, row, number) && columnCheck(sudoku, column, number) && boxCheck(sudoku, row, column, number))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
*   Find blank point in sudoku
*/
bool findBlank(int sudoku[9][9], int* pRow, int* pColumn)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (sudoku[i][j] == 0)
            {
                *pRow = i;
                *pColumn = j;
                return false;
            }
        }
    }
    return true;
}

/*
*   Main function for solving sudoku
*
*   Makes recursive calls using backtracking algorithm to solve
*/
bool solveSudoku(HWND hWnd, int sudoku[9][9])
{
    // Variables to send application data
    Sudoku_Append_t sudokuData;

    int blankRow, blankColumn;

    // Search for unsolved box in sudoku
    bool isSolved = findBlank(sudoku, &blankRow, &blankColumn);

    // Return if no unsolved boxes found
    if (isSolved)
    {
        return true;
    }

    // Decision space for each box is 1 to 9 - try these values in the unsolved box
    for (int i = 1; i <= 9; i++)
    {
        if (isValid(sudoku, blankRow, blankColumn, i))
        {
            sudoku[blankRow][blankColumn] = i;
            delay(5);

            //Send application data to place number
            sudokuData.row = blankRow;
            sudokuData.column = blankColumn;
            sudokuData.number = i;
            sudokuData.nmh.idFrom = MSG_FROM_SDKU_SOLVE;
            sudokuData.nmh.hwndFrom = hWnd;
            sudokuData.action = SDKU_NUMBER_PUT;
            SendMessage(hWnd, WM_NOTIFY, NULL, (Sudoku_Append_t*)&sudokuData);


            //SetDlgItemInt(hWnd, SUDOKU_CTRL_BASE_VALUE + (blankRow * 9) + blankColumn, i, FALSE);

            // Recursive backtracking
            if (solveSudoku(hWnd, sudoku))
            {
                return true;
            }
            else
            {
                // backtrack (might need this to be NULL)
                sudoku[blankRow][blankColumn] = 0;

                // Send application data to remove number
                sudokuData.action = SDKU_NUMBER_RM;
                SendMessage(hWnd, WM_NOTIFY, NULL, (Sudoku_Append_t*)&sudokuData);
                //SetDlgItemInt(hWnd, SUDOKU_CTRL_BASE_VALUE + (blankRow * 9) + blankColumn, NULL, FALSE);
            }
        }
    }

    return false;
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

    //TODO Validate sudoku and set all squares !(1 - 9) as 0

    if (solveSudoku(inputSudoku->hWnd, sudoku))
    {
        MessageBox(inputSudoku->hWnd, L"Puzzle has been solved!", L"Solved", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(inputSudoku->hWnd, L"This puzzle has no solution!", L"No Solution", MB_OK | MB_ICONERROR);
    }

    int test1 = GetDlgItemInt(inputSudoku->hWnd, (SUDOKU_CTRL_BASE_VALUE + 78), NULL, FALSE);
    int test2 = GetDlgItemInt(inputSudoku->hWnd, (SUDOKU_CTRL_BASE_VALUE + 78), NULL, FALSE);

    printf("%i %i\n", test1, test2);
}