#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <ctime>
#include "solver.h"

using namespace std;

#define MAX_SOLUTIONS 1000
#define GRID_SIZE 9


string outputFileName;
const int GRID_SIZE_SQUARED = GRID_SIZE * GRID_SIZE;
const int GRID_SIZE_SQRT = sqrt(static_cast<double>(GRID_SIZE));
const int ROW_COUNT = GRID_SIZE * GRID_SIZE * GRID_SIZE;
const int COLUMN_COUNT = 4 * GRID_SIZE * GRID_SIZE;

struct Node Head;
struct Node *HeadNode = &Head;
struct Node *solutions[MAX_SOLUTIONS];
struct Node *originalValues[MAX_SOLUTIONS];
bool exactCoverMatrix[ROW_COUNT][COLUMN_COUNT] = {{false}};
bool isSolved = false;
clock_t solverTimer, solutionTimer;
void printGrid_1(int SudokuGrid[][GRID_SIZE]);
void MapSolutionToGrid(int Sudoku[][GRID_SIZE]);

// Print only the numbers in the Sudoku grid
void printGrid_1(int SudokuGrid[][GRID_SIZE])
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

void writeSolutionToFile(int SudokuGrid[][GRID_SIZE])
{
    // Open output file
    ofstream outputFile(outputFileName);
    if (!outputFile.is_open())
    {
        cerr << "Error opening file: " << outputFileName << endl;
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
    cout << "File created and saved to : " << outputFileName  << endl
         << endl;
}

//DLX functions
void coverColumn(Node* col) {
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

void uncoverColumn(Node* col) {
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

void search(int k) {

	if (HeadNode->right == HeadNode) {
		solutionTimer = clock() - solverTimer;
		int Grid[GRID_SIZE][GRID_SIZE] = { {0} };
		MapSolutionToGrid(Grid);
		cout << "Solved Puzzle:" << endl;
		printGrid_1(Grid);
		cout << "Time Elapsed: " << (float)solutionTimer / CLOCKS_PER_SEC << " seconds.\n\n";
		solverTimer = clock();
        writeSolutionToFile(Grid);
        isSolved = true;
		return;
	}

	//Choose the column with the least number of nodes
	Node* Col = HeadNode->right;
	for (Node* temp = Col->right; temp != HeadNode; temp = temp->right)
		if (temp->size < Col->size)
			Col = temp;

	coverColumn(Col);
	
	//Try all possible rows for the chosen column
	for (Node* temp = Col->down; temp != Col; temp = temp->down) {
		solutions[k] = temp;
		for (Node* node = temp->right; node != temp; node = node->right) {
			coverColumn(node->head);
		}

		search(k + 1);

		temp = solutions[k];
		solutions[k] = NULL;
		Col = temp->head;
		for (Node* node = temp->left; node != temp; node = node->left) {
			uncoverColumn(node->head);
		}
	}

	uncoverColumn(Col);
}

//MAP THE SOLUTION TO THE GRID
void MapSolutionToGrid(int Sudoku[][GRID_SIZE]) {
	for (int i = 0; solutions[i] != NULL; i++) {
			Sudoku[solutions[i]->rowID[1]-1][solutions[i]->rowID[2]-1] = solutions[i]->rowID[0];
	}
	for (int i = 0; originalValues[i] != NULL; i++) {
		Sudoku[originalValues[i]->rowID[1] - 1][originalValues[i]->rowID[2] - 1] = originalValues[i]->rowID[0];
	}
}

//BUILD THE EXACT COVER MATRIX
void buildExactCoverMatrix(bool matrix_9[ROW_COUNT][COLUMN_COUNT]) {

	//Constraint 1: There can only be one instance of a number in any given cell
	int j = 0, counter = 0;
	for (int i = 0; i < ROW_COUNT; i++) { 
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
void buildLinkedList(bool matrix_9[ROW_COUNT][COLUMN_COUNT]) {

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

		//Create Nodes for each row
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
	HeadNode = header;
}

//TRANSFORM THE GRID INTO THE CURRENT EXACT COVER PROBLEM
void transformListToCurrentGrid(int Puzzle[][GRID_SIZE]) {
	int index = 0;
	//Find the original values in the grid and cover the corresponding columns
	for(int i = 0 ; i<GRID_SIZE; i++ )
		for(int j = 0 ; j<GRID_SIZE; j++)
			if (Puzzle[i][j] > 0) {
				Node* Col = NULL;
				Node* temp = NULL;
				for (Col = HeadNode->right; Col != HeadNode; Col = Col->right) {
					for (temp = Col->down; temp != Col; temp = temp->down)
						if (temp->rowID[0] == Puzzle[i][j] && (temp->rowID[1] - 1) == i && (temp->rowID[2] - 1) == j)
							goto ExitLoops; //Break out of both loops
				}
ExitLoops:		coverColumn(Col);
				originalValues[index] = temp;
				index++;
				for (Node* node = temp->right; node != temp; node = node->right) {
					coverColumn(node->head);
				}
			}
}

//SOLVE THE SUDOKU
void solveSudoku_1(int Sudoku[][GRID_SIZE], string outputFile) {
	solverTimer = clock();
	//Build the exact cover matrix
	buildExactCoverMatrix(exactCoverMatrix);
	//Build the linked list
	buildLinkedList(exactCoverMatrix);
	//Transform the grid into the current exact cover problem
	transformListToCurrentGrid(Sudoku);
	//Solve the exact cover problem
	outputFileName = outputFile;
	search(0);
	if (!isSolved)
		cout << "No Solution!" << endl;
	isSolved = false;
}

