#pragma once

#include "framework.h"
#include "resource.h"

#ifndef SUDOKU_HEADER
#define SUDOKU_HEADER

void test();

/*
* Print the full sudoku as it is
*/
void printSoln(uint8_t sudoku[9][9]);


/*
* Checks if a specified number can be placed in the specified row
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*           uint8_t row - row number to check
*           uint8_t number - number propsed to be placed
*
* Return:   bool valid - tell if valid
*/
bool rowCheck(uint8_t sudoku[9][9], uint8_t row, uint8_t number);


/*
* Checks if a specified number can be placed in the specified column
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*           uint8_t column - column number to check
*           uint8_t number - number propsed to be placed
*
* Return:   bool valid - tell if valid
*/
bool columnCheck(uint8_t sudoku[9][9], uint8_t column, uint8_t number);


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
bool boxCheck(uint8_t sudoku[9][9], uint8_t row, uint8_t column, uint8_t number);


/*
* Calls rowCheck, columnCheck and cellCheck to see if all are valid
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*           uint8_t row - row where number is to be placed
*           uint8_t column - column where number is to be placed
*           uint8_t number - number propsed to be placed
*
* Return:   bool valid - tell if valid
*/
bool isValid(uint8_t sudoku[9][9], uint8_t row, uint8_t column, uint8_t number);


/*
* Searches sudoku for a blank value. Returns a boolean value to tell if puzzle is complete or if blanks remain
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*           uint8_t* row - pointer to variable to store row of next blank
*           uint8_t* column - pointer to variable to store column of next blank
*
* Return:   bool valid - tell if blank found or not (meaning puzzle is solved)
*/
bool findBlank(uint8_t sudoku[9][9], uint8_t* pRow, uint8_t* pColumn);


/*
* Driver function to solve sudoku. Makes calls directly to isValid, findBlank and printSoln
*
* Inputs:   uint8_t sudoku[9][9] - sudoku grid
*/
bool solveSudoku(uint8_t sudoku[9][9]);

#endif

