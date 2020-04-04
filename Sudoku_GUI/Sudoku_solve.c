#include "framework.h"

/*
*   Print out the sudoku grid.
*   This function is called only after puzzle is solved.
*/
void printSoln(uint8_t sudoku[9][9])
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
bool rowCheck(uint8_t sudoku[9][9], uint8_t row, uint8_t number)
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
bool columnCheck(uint8_t sudoku[9][9], uint8_t column, uint8_t number)
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
bool boxCheck(uint8_t sudoku[9][9], uint8_t row, uint8_t column, uint8_t number)
{
    uint8_t cellRow = (row / 3) * 3;
    uint8_t cellColumn = (column / 3) * 3;

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
bool isValid(uint8_t sudoku[9][9], uint8_t row, uint8_t column, uint8_t number)
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
bool findBlank(uint8_t sudoku[9][9], uint8_t* pRow, uint8_t* pColumn)
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
bool solveSudoku(uint8_t sudoku[9][9])
{
    // printSoln(sudoku);
    // printf("\n\n");

    uint8_t blankRow, blankColumn;

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

            if (solveSudoku(sudoku))
            {
                return true;
            }
            else
            {
                // backtrack
                sudoku[blankRow][blankColumn] = 0;
            }
        }
    }

    return false;
}

/*
*   Calls driver 'solveSudoku' function
*
*   Solution is printed or else a message shows telling there is no solution
*/
int main(void)
{
    uint8_t sudoku[9][9] = {
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

void test()
{
    printf("I am working!");
}