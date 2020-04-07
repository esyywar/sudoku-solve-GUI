#pragma once

#ifndef SUDOKU_MACRO_HEADER
#define SUDOKU_MACRO_HEADER

#define MAX_LOADSTRING				100

#define SUDOKU_CTRL_BASE_VALUE		1
#define STATIC_GRID_FRAME			89
#define SOLVE_BTN_CLICK				90
#define RESTART_BTN_CLICK			91
#define CLOSE_BTN_CLICK				92

#define MSG_FROM_SDKU_SOLVE			93
#define SDKU_NUMBER_PUT				94
#define SDKU_NUMBER_RM				95

typedef struct {
	NMHDR nmh;
	UINT row;
	UINT column;
	UINT action;
	UINT number;
} Sudoku_Append_t;


#endif // !SUDOKU_MACRO_HEADER
