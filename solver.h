#ifndef SOLVER_H
#define SOLVER_H

struct Node {
	Node *left;
	Node *right;
	Node *up;
	Node *down;
	Node *head;
	
	int size;		//used for Column header
	int rowID[3];	//used to identify row in order to map solutions to a sudoku grid
};

void printGrid_1(int Sudoku[][9]);
void solveSudoku_1(int Sudoku[][9], std::string outputFile);

void printGrid_2(int Sudoku[][16]);
void solveSudoku_2(int Sudoku[][16], std::string outputFile);

#endif 
