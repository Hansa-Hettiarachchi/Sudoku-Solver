#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <ctime>
#include <sstream>
#include "solver.h"

using namespace std;

#define MAX_SOLUTIONS 1000
#define GRID_SIZE 16

string outputFileName_2;
const int GRID_SIZE_SQUARED = GRID_SIZE * GRID_SIZE;
const int GRID_SIZE_SQRT = sqrt(static_cast<double>(GRID_SIZE));
const int ROW_COUNT = GRID_SIZE * GRID_SIZE * GRID_SIZE;
const int COLUMN_COUNT = 4 * GRID_SIZE * GRID_SIZE;

struct Node Head_2;
struct Node *HeadNode_2 = &Head_2;
struct Node *solutions_2[MAX_SOLUTIONS];
struct Node *originalValues_2[MAX_SOLUTIONS];
bool exactCoverMatrix_2[ROW_COUNT][COLUMN_COUNT] = {{false}};
bool isSolved_2 = false;
clock_t solverTimer_2, solutionTimer_2;
void printGrid_2(int SudokuGrid[][GRID_SIZE]);
void MapSolutionToGrid_2(int Sudoku[][GRID_SIZE]);

// Print only the numbers in the Sudoku grid
void printGrid_2(int SudokuGrid[][GRID_SIZE])
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            cout << SudokuGrid[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}


// Write the solution to the output file
void writeSolutionToFile_2(int SudokuGrid[][GRID_SIZE])
{
    ofstream outputFile(outputFileName_2);
    if (!outputFile.is_open())
    {
        cerr << "Error opening file: " << outputFileName_2 << endl;
        return;
    }

    for (int i = 0; i < GRID_SIZE; ++i)
    {
        for (int j = 0; j < GRID_SIZE; ++j)
        {
            outputFile << SudokuGrid[i][j] << " ";
        }
        outputFile << endl;
    }

    outputFile.close();
    cout << "File created and saved to : " << outputFileName_2  << endl
         << endl;
}

//DLX functions
void coverColumn_2(Node* col) {
	col->left->right = col->right;
	col->right->left = col->left;
	for (Node* node = col->down; node != col; node = node->down) {
		for (Node* temp = node->right; temp != node; temp = temp->right) {
			temp->down->up = temp->up;
			temp->up->down = temp->down;
			temp->head->size--;
		}
	}
}

void uncoverColumn_2(Node* col) {
	for (Node* node = col->up; node != col; node = node->up) {
		for (Node* temp = node->left; temp != node; temp = temp->left) {
			temp->head->size++;
			temp->down->up = temp;
			temp->up->down = temp;
		}
	}
	col->left->right = col;
	col->right->left = col;
}

void search_2(int k) {

	if (HeadNode_2->right == HeadNode_2) {
		solutionTimer_2 = clock() - solverTimer_2;
		int Grid[GRID_SIZE][GRID_SIZE] = { {0} };
		MapSolutionToGrid_2(Grid);
		cout << "Solved Puzzle:" << endl;
		printGrid_2(Grid);
		cout << "Time Elapsed: " << (float)solutionTimer_2 / CLOCKS_PER_SEC << " seconds.\n\n";
		solverTimer_2 = clock();
        writeSolutionToFile_2(Grid);
        isSolved_2 = true;
		return;
	}

	//Choose the column with the least number of nodes
	Node* Col = HeadNode_2->right;
	for (Node* temp = Col->right; temp != HeadNode_2; temp = temp->right)
		if (temp->size < Col->size)
			Col = temp;

	coverColumn_2(Col);
	
	//Try each row in the column
	for (Node* temp = Col->down; temp != Col; temp = temp->down) {
		solutions_2[k] = temp;
		for (Node* node = temp->right; node != temp; node = node->right) {
			coverColumn_2(node->head);
		}

		search_2(k + 1);

		temp = solutions_2[k];
		solutions_2[k] = NULL;
		Col = temp->head;
		for (Node* node = temp->left; node != temp; node = node->left) {
			uncoverColumn_2(node->head);
		}
	}

	uncoverColumn_2(Col);
}

//MAP THE SOLUTION TO THE GRID
void MapSolutionToGrid_2(int Sudoku[][GRID_SIZE]) {
	for (int i = 0; solutions_2[i] != NULL; i++) {
			Sudoku[solutions_2[i]->rowID[1]-1][solutions_2[i]->rowID[2]-1] = solutions_2[i]->rowID[0];
	}
	for (int i = 0; originalValues_2[i] != NULL; i++) {
		Sudoku[originalValues_2[i]->rowID[1] - 1][originalValues_2[i]->rowID[2] - 1] = originalValues_2[i]->rowID[0];
	}
}

//BUILD THE EXACT COVER MATRIX
void buildExactCoverMatrix_2(bool matrix_9[ROW_COUNT][COLUMN_COUNT]) {

	//Constraint 1: There can only be one instance of a number in any given cell
	int j = 0, counter = 0;
	for (int i = 0; i < ROW_COUNT; i++) { //iterate over all rows
		matrix_9[i][j] = 1;
		counter++;
		if (counter >= GRID_SIZE) {
			j++;
			counter = 0;
		}
	}

	//Constraint 2: There can only be one instance of a number in any given row
	int x = 0;
	counter = 1;
	for (j = GRID_SIZE_SQUARED; j < 2 * GRID_SIZE_SQUARED; j++) {
		for (int i = x; i < counter*GRID_SIZE_SQUARED; i += GRID_SIZE)
			matrix_9[i][j] = 1;

		if ((j + 1) % GRID_SIZE == 0) {
			x = counter*GRID_SIZE_SQUARED;
			counter++;
		}
		else
			x++;
	}

	//Constraint 3: There can only be one instance of a number in any given column
	j = 2 * GRID_SIZE_SQUARED;
	for (int i = 0; i < ROW_COUNT; i++)
	{
		matrix_9[i][j] = 1;
		j++;
		if (j >= 3 * GRID_SIZE_SQUARED)
			j = 2 * GRID_SIZE_SQUARED;
	}

	//Constraint 4: There can only be one instance of a number in any given subgrid
	x = 0;
	for (j = 3 * GRID_SIZE_SQUARED; j < COLUMN_COUNT; j++) {

		for (int l = 0; l < GRID_SIZE_SQRT; l++) {
			for (int k = 0; k<GRID_SIZE_SQRT; k++)
				matrix_9[x + l*GRID_SIZE + k*GRID_SIZE_SQUARED][j] = 1;
		}

		int temp = j + 1 - 3 * GRID_SIZE_SQUARED;

		if (temp % (int)(GRID_SIZE_SQRT * GRID_SIZE) == 0)
			x += (GRID_SIZE_SQRT - 1)*GRID_SIZE_SQUARED + (GRID_SIZE_SQRT - 1)*GRID_SIZE + 1;
		else if (temp % GRID_SIZE == 0)
			x += GRID_SIZE*(GRID_SIZE_SQRT - 1) + 1;
		else
			x++;
	}
}

//BUILD THE LINKED LIST
void buildLinkedList_2(bool matrix_9[ROW_COUNT][COLUMN_COUNT]) {

	Node* header = new Node;
	header->left = header;
	header->right = header;
	header->down = header;
	header->up = header;
	header->size = -1;
	header->head = header;
	Node* temp = header;

	//Create Column Nodes
	for (int i = 0; i < COLUMN_COUNT; i++) {
		Node* newNode = new Node;
		newNode->size = 0;
		newNode->up = newNode;
		newNode->down = newNode;
		newNode->head = newNode;
		newNode->right = header;
		newNode->left = temp;
		temp->right = newNode;
		temp = newNode;
	}

	int ID[3] = { 0,1,1 };
	//Create Row Nodes
	for (int i = 0; i < ROW_COUNT; i++) {
		Node* top = header->right;
		Node* prev = NULL;

		if (i != 0 && i%GRID_SIZE_SQUARED == 0) {
			ID[0] -= GRID_SIZE - 1;
			ID[1]++;
			ID[2] -= GRID_SIZE - 1;
		}
		else if (i!= 0 && i%GRID_SIZE == 0) {
			ID[0] -= GRID_SIZE - 1;
			ID[2]++;
		}
		else {
			ID[0]++;
		}

		for (int j = 0; j < COLUMN_COUNT; j++, top = top->right) {
			if (matrix_9[i][j]) {
				Node* newNode = new Node;
				newNode->rowID[0] = ID[0];
				newNode->rowID[1] = ID[1];
				newNode->rowID[2] = ID[2];
				if (prev == NULL) {
					prev = newNode;
					prev->right = newNode;
				}
				newNode->left = prev;
				newNode->right = prev->right;
				newNode->right->left = newNode;
				prev->right = newNode;
				newNode->head = top;
				newNode->down = top;
				newNode->up = top->up;
				top->up->down = newNode;
				top->size++;
				top->up = newNode;
				if (top->down == top)
					top->down = newNode;
				prev = newNode;
			}
		}
	}
	HeadNode_2 = header;
}

//TRANSFORM THE LIST TO THE CURRENT GRID
void transformListToCurrentGrid_2(int Puzzle[][GRID_SIZE]) {
	int index = 0;
	for(int i = 0 ; i<GRID_SIZE; i++ )
		for(int j = 0 ; j<GRID_SIZE; j++)
			if (Puzzle[i][j] > 0) {
				Node* Col = NULL;
				Node* temp = NULL;
				//Find the node with the same rowID as the original value
				for (Col = HeadNode_2->right; Col != HeadNode_2; Col = Col->right) {
					for (temp = Col->down; temp != Col; temp = temp->down)
						if (temp->rowID[0] == Puzzle[i][j] && (temp->rowID[1] - 1) == i && (temp->rowID[2] - 1) == j)
							goto ExitLoops; //break out of both loops
				}
ExitLoops:		coverColumn_2(Col);
				originalValues_2[index] = temp;
				index++;
				for (Node* node = temp->right; node != temp; node = node->right) {
					coverColumn_2(node->head);
				}
			}
}

//SOLVE THE SUDOKU
void solveSudoku_2(int Sudoku[][GRID_SIZE], string outputFile) {
	solverTimer_2 = clock();
	//Build the exact cover matrix and the linked list
	buildExactCoverMatrix_2(exactCoverMatrix_2);
	buildLinkedList_2(exactCoverMatrix_2);
	//Transform the list to the current grid
	transformListToCurrentGrid_2(Sudoku);
	outputFileName_2 = outputFile;
	//Solve the exact cover problem
	search_2(0);
	if (!isSolved_2)
		cout << "No Solution!" << endl;
	isSolved_2 = false;
}
