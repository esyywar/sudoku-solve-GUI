#pragma once

#ifndef SUDOKU_MACRO_HEADER
#define SUDOKU_MACRO_HEADER

#define MAX_LOADSTRING				100

#define SUDOKU_CTRL_BASE_VALUE		1
#define STATIC_TITLE_HEADER			87
#define STATIC_SUBTITLE				88
#define STATIC_GRID_FRAME			89
#define STATIC_CELL_FRAME			90
#define SPEED_TRACKBAR				91
#define SOLVE_BTN_CLICK				92
#define RESTART_BTN_CLICK			93
#define CLOSE_BTN_CLICK				94

#define MSG_FROM_SDKU_SOLVE			95
#define SDKU_NUMBER_PUT				96
#define SDKU_NUMBER_RM				97

#define MSG_FROM_SDKU_DRIVER		98
#define SOLVE_THREAD_COMPLETE		99


/**************** STRUCTS *********************/

/*
* For sending initial data from main window to solver
*/
typedef struct {
	HWND hWnd;
	int speed;
	int *sudoku;
} SudokuData, *pSudokuData;

/*
* For message sending from solver thread to main window procedure
*/
typedef struct {
	NMHDR nmh;
	UINT row;
	UINT column;
	UINT action;
	UINT number;
} Sudoku_Append_t;

#endif // !SUDOKU_MACRO_HEADER
