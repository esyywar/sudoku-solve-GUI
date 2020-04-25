#pragma once

#include "framework.h"
#include "resource.h"

#ifndef SUDOKU_HEADER
#define SUDOKU_HEADER





/****************************** SUDOKU SOLVE FUNCTIONS ******************************************/

// Solver thread function
DWORD WINAPI sudokuSolveDriver(LPVOID lpParam);

/*
* Void function that takes sudoku input and validates values to all be 0 (for blank) or 1-9
*/
void valuesCheck(HWND hWnd, int sudoku[9][9]);

/*
* Print the full sudoku as it is
*/
void printSoln(int sudoku[9][9]);

/*
* Clears all values on active sudoku board
*/
void clearSudokuBoard(HWND hWnd);


/*
* Checks if a specified number can be placed in the specified row
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*           uint8_t row - row number to check
*           uint8_t number - number propsed to be placed
*
* Return:   bool valid - tell if valid
*/
bool rowCheck(int sudoku[9][9], int row, int number);


/*
* Checks if a specified number can be placed in the specified column
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*           uint8_t column - column number to check
*           uint8_t number - number propsed to be placed
*
* Return:   bool valid - tell if valid
*/
bool columnCheck(int sudoku[9][9], int column, int number);


/*
* Checks if a specified number can be placed in the cell
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*           uint8_t row - row where number is to be placed
*           uint8_t column - column where number is to be placed
*           uint8_t number - number propsed to be placed
*
* Return:   bool valid - tell if valid
*/
bool boxCheck(int sudoku[9][9], int row, int column, int number);


/*
* Calls rowCheck, columnCheck and cellCheck to see if all are valid -> number can be placed here
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*           uint8_t row - row where number is to be placed
*           uint8_t column - column where number is to be placed
*           uint8_t number - number propsed to be placed
*
* Return:   bool isValid - tell if valid
*/
bool isValid(int sudoku[9][9], int row, int column, int number);


/*
* Searches sudoku for a blank value. Returns a boolean value to tell if puzzle is complete or if blanks remain
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*           uint8_t* row - pointer to variable to store row of next blank
*           uint8_t* column - pointer to variable to store column of next blank
*
* Return:   bool isValid - tell if blank found or not (meaning puzzle is solved)
*/
bool findBlank(int sudoku[9][9], int* pRow, int* pColumn);


/*
* Checks all values in sudoku column for duplicates (must already be validated for numbers 0 - 9)
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*
* Return:   bool isValid - tell if all columns are valid
*/
bool isValidColumns(int sudoku[9][9]);


/*
* Checks all values in sudoku row for duplicates (must already be validated for numbers 0 - 9)
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*
* Return:   bool isValid - tell if all rows are valid
*/
bool isValidRows(int sudoku[9][9]);


/*
* Checks all values in each sudoku cell for duplicates (must already be validated for numbers 0 - 9)
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*
* Return:   bool isValid - tell if all cells are valid
*/
bool isValidCells(int sudoku[9][9]);

/*
* Driver function to solve sudoku. Makes calls directly to isValid, findBlank and printSoln
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*/
bool solveSudoku(HWND hWnd, int sudoku[9][9], int delayMultiplier);


#endif

