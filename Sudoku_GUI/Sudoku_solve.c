#include "framework.h"
#include "sudoku_macros.h"

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
    NMHDR nmh;

    int blankRow, blankColumn;

    bool isSolved = findBlank(sudoku, &blankRow, &blankColumn);

    if (isSolved)
    {
        return true;
    }

    for (int i = 1; i <= 9; i++)
    {
        if (isValid(sudoku, blankRow, blankColumn, i))
        {
            sudoku[blankRow][blankColumn] = i;

            // Send application data to place number
            nmh.idFrom = MSG_FROM_SDKU_SOLVE;
            nmh.code = SDKU_NUMBER_PUT;
            nmh.hwndFrom = hWnd;
            SendMessage(hWnd, WM_NOTIFY, i, (LPARAM)&nmh);

            // Recursive backtracking
            if (solveSudoku(hWnd, sudoku))
            {
                return true;
            }
            else
            {
                // backtrack
                sudoku[blankRow][blankColumn] = 0;

                // Send application data to remove number
                nmh.idFrom = MSG_FROM_SDKU_SOLVE;
                nmh.code = SDKU_NUMBER_RM;
                nmh.hwndFrom = hWnd;
                SendMessage(hWnd, WM_NOTIFY, i, (LPARAM)&nmh);
            }
        }
    }

    return false;
}

/*
*   Calls driver 'solveSudoku' function
*
*   Solution is printed or else a message shows telling there is no solution

int main(void)
{
    int sudoku[9][9] = {
        {0, 0, 8, 2, 0, 0, 0, 0, 0},
        {0, 1, 2, 0, 0, 5, 8, 0, 0},
        {4, 0, 0, 3, 0, 0, 0, 1, 0},
        {7, 0, 0, 1, 0, 0, 0, 5, 0},
        {0, 6, 0, 0, 7, 0, 1, 0, 3},
        {0, 0, 0, 5, 0, 0, 0, 0, 6},
        {3, 0, 0, 0, 0, 0, 0, 4, 0},
        {0, 9, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 8, 0, 7, 0, 0}
    };

    if (solveSudoku(sudoku))
    {
        printf("Here is the solution: \n");
        printSoln(sudoku);
    }
    else
    {
        printf("This puzzle cannot be solved!\n");
    }

    return 0;
}
*/