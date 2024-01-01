#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "solver.h"

using namespace std;

int main(int argc, char* argv[]) {
	
    string inputFile = argv[1];

	//check if file is empty
	ifstream fileSize(inputFile);

	string firstLine;
	getline(fileSize, firstLine);
	istringstream iss(firstLine);
    int number;
	int count = 0;
    while (iss >> number) {
        count++;
    }
	const int SIZE = count;
	fileSize.close();
	
	ifstream file(inputFile);
	if (!file.is_open()) {
		cerr << "Error opening file: " << inputFile << endl;
		return 1;
	}

	// check whether the input grid is 9x9 or 16x16
	if (SIZE == 9) {
        //read input file and store in 2D array
		int Puzzle_1[9][9] = { {0} };
		for (int i = 0; i < 9; ++i) {
			for (int j = 0; j < 9; ++j) {
				file >> Puzzle_1[i][j];
			}
		}
		//print input grid
		cout<<endl;
        cout << "Input File: " << inputFile << endl<<endl;
		cout << "Input Puzzle:" << endl;
        printGrid_1(Puzzle_1);

		//create output file name
        string outputFile = inputFile.substr(0, inputFile.find_last_of('.')) + "_output.txt";
		// Solve the Sudoku and write the solution to the output file
        solveSudoku_1(Puzzle_1, outputFile);

    } else if (SIZE == 16) {
		//read input file and store in 2D array
		int Puzzle[16][16] = { {0} };
		for (int i = 0; i < 16; ++i) {
			for (int j = 0; j < 16; ++j) {
				file >> Puzzle[i][j];
			}
		}
		//print input grid
		cout<<endl;
        cout << "Input File: " << inputFile << endl<<endl;
		cout << "Input Puzzle:" << endl;	
        printGrid_2(Puzzle);

        // create output file name
        string outputFile = inputFile.substr(0, inputFile.find_last_of('.')) + "_output.txt";
        // Solve the Sudoku and write the solution to the output file
        solveSudoku_2(Puzzle, outputFile);
		
    } else
        cout << "Invalid puzzle size" << endl;
	
	file.close();
    return 0;
}