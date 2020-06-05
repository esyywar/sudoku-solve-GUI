#include "framework.h"
#include "sudoku_macros.h"


/******************************************************** HELPER FUNCTIONS *************************************************************/

// Static sorting function
static int ascendSort(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}


/************************************************************ GUI MANAGEMENT ************************************************************/

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


/******************************************************* USER INPUT VALIDATION FUNCTIONS ************************************************/

/*
* Checks each column entered by user is valid by sudoku rules
*/
bool isValidColumns(int sudoku[9][9])
{
    int column[9], countNums = 0;

    // For each column
    for (int i = 0; i < 9; i++)
    {
        // For each row
        for (int j = 0; j < 9; j++)
        {
            // Add to array if non-0
            if (sudoku[j][i] > 0)
            {
                column[countNums] = sudoku[j][i];
                countNums++;
            }
        }

        // Check the column array for duplicates
        qsort(column, (size_t)countNums, sizeof(int), ascendSort);
        for (int n = 0; n < countNums - 1; n++)
        {
            if (column[n] == column[n + 1]) return false;
        }

        // Reset before checking next column
        countNums = 0;
    }

    return true;
}

/*
* Checks each row entered by user is valid by sudoku rules
*/
bool isValidRows(int sudoku[9][9])
{
    int row[9], countNums = 0;

    // For each row
    for (int i = 0; i < 9; i++)
    {
        // For each column
        for (int j = 0; j < 9; j++)
        {
            // Add to array if non-0
            if (sudoku[i][j] > 0)
            {
                row[countNums] = sudoku[i][j];
                countNums++;
            }
        }

        // Check the row array for duplicates
        qsort(row, (size_t)countNums, sizeof(int), ascendSort);
        for (int n = 0; n < countNums - 1; n++)
        {
            if (row[n] == row[n + 1]) return false;
        }

        // Reset before checking next row
        countNums = 0;
    }

    return true;
}

/*
* Check each cell given by user for validity
*/
bool isValidCells(int sudoku[9][9])
{
    int cell[9], startColumn, startRow, countNums = 0;

    // For each cell
    for (int i = 0; i < 9; i++)
    {
        // Move to upper left of next cell
        startColumn = (i / 3) * 3;
        startRow = i % 3 * 3;

        // Check each value in cell
        for (int j = 0; j < 9; j++)
        {
            if (sudoku[startRow + j / 3][startColumn + (j % 3)] > 0)
            {
                cell[countNums] = sudoku[startRow + j / 3][startColumn + (j % 3)];
                countNums++;
            }
        }

        // Check the cell for duplicates
        qsort(cell, (size_t)countNums, sizeof(int), ascendSort);
        for (int n = 0; n < countNums - 1; n++)
        {
            if (cell[n] == cell[n + 1]) return false;
        }

        // Reset before checking next row
        countNums = 0;
    }

    return true;
}

/*
* Validates that all numbers are 1 - 9 or else sets to 0
*/
void valuesCheck(HWND hWnd, int sudoku[9][9])
{
    Sudoku_Append_t sudokuData;

    // Check that all numbers are correct
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (sudoku[i][j] > 9 || sudoku[i][j] < 0)
            {
                // Invalid entry - append value in array to 0
                sudoku[i][j] = 0;

                // Send message to main WNDPROC to remove value on GUI
                sudokuData.row = i;
                sudokuData.column = j;
                sudokuData.nmh.idFrom = MSG_FROM_SDKU_SOLVE;
                sudokuData.nmh.hwndFrom = hWnd;
                sudokuData.action = SDKU_NUMBER_RM;
                SendMessage(hWnd, WM_NOTIFY, sudokuData.nmh.idFrom, (Sudoku_Append_t*)&sudokuData);
            }
        }
    }
}


/************************************** SOLVER FUNCTIONS ******************************************************/

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
bool solveSudoku(HWND hWnd, HWND speedBar, int sudoku[9][9])
{
    // Variables to send application data
    Sudoku_Append_t sudokuData;

    int blankRow, blankColumn;

    // Delay according to speed setting
    Sleep(20 * SendMessage(speedBar, TBM_GETPOS, NULL, NULL));

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

            //Send application data to place number
            sudokuData.row = blankRow;
            sudokuData.column = blankColumn;
            sudokuData.number = i;
            sudokuData.nmh.idFrom = MSG_FROM_SDKU_SOLVE;
            sudokuData.nmh.hwndFrom = hWnd;
            sudokuData.action = SDKU_NUMBER_PUT;
            SendMessage(hWnd, WM_NOTIFY, sudokuData.nmh.idFrom, (Sudoku_Append_t*)&sudokuData);

            // Recursive backtracking
            if (solveSudoku(hWnd, speedBar, sudoku))
            {
                return true;
            }
            else
            {
                // backtrack (might need this to be NULL)
                sudoku[blankRow][blankColumn] = 0;

                // Send application data to remove number
                sudokuData.action = SDKU_NUMBER_RM;
                SendMessage(hWnd, WM_NOTIFY, sudokuData.nmh.idFrom, (Sudoku_Append_t*)&sudokuData);
            }
        }
    }

    return false;
}