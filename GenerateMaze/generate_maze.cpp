#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <tuple>
#include <set>
#include <vector>
#include <time.h>
#include <queue>
#include <map>

using namespace std;

namespace Maze { //Namespace created to have enumerated types for Cell and Point
	struct Cell {
		int coordX;
		int coordY;
		int g;
		int h;
		int previousX;
		int previousY;

		//no parameters makes an empty cell object
		Cell() {
			coordX = 0;
			coordY = 0;
			g = 0;
			h = 0;
			previousX = 0;
			previousY = 0;
		}

		//overload constructor with two parameters
		Cell(int x, int y) {
			coordX = x;
			coordY = y;
			g = 0;
			h = 0;
			previousX = 0;
			previousY = 0;
		}

		bool operator== (const Cell& rhs) {
			return coordX == rhs.coordX && coordY == rhs.coordY;
		}

		bool operator< (const Cell& rhs) {
			return g + h < rhs.g + rhs.h;
		}

	};

	/*typedef pair<Maze::Cell, Maze::Cell>;
	pair<Maze::Cell, Maze::Cell> operator== (Cell& lhs, Cell& rhs)
	{
		return make_pair(lhs, rhs);
	};*/
}

using namespace Maze;

//input validation for the start menu of the program
//returns true of the input is valid
int validateMenu() {
	int choice;
	cin >> choice;

	while (1) {
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Select a number." << endl;
			cin >> choice;
		}
		else if (choice <= 0 || choice >= 3) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Please select from the menu." << endl;
			cin >> choice;
		}
		else {
			break;
		}
	}

	return choice;
}

//input validation for number of rows
int validateRows() {
	int choice;
	cin >> choice;

	while (1) {
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Select a number." << endl;
			cin >> choice;
		}
		else if (choice <= 4) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Must be greater than 4." << endl;
			cin >> choice;
		}
		else {
			break;
		}
	}

	return choice;
}

//input validation for number of columns
int validateColumns() {
	int choice;
	cin >> choice;

	while (1) {
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Select a number." << endl;
			cin >> choice;
		}
		else if (choice <= 4) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Must be greater than 4." << endl;
			cin >> choice;
		}
		else {
			break;
		}
	}

	return choice;
}

//input validation for number of exits
int validateExits() {
	int choice;
	cin >> choice;

	while (1) {
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Select a number." << endl;
			cin >> choice;
		}
		else if (choice <= 0) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. Must have at least one exit." << endl;
			cin >> choice;
		}
		else if (choice > 4) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input incorrect. No more than 4 exits." << endl;
			cin >> choice;
		}
		else {
			break;
		}
	}

	return choice;
}

//initialise grid
char** initGrid(int row, int col) {
	char** mazeGrid = 0;
	mazeGrid = new char* [row]; //initialise the array with number of rows

	//for all rows, add columns
	for (int i = 0; i < row; i++) {

		//add the second array to the current array
		mazeGrid[i] = new char[col];

		//for all columns and rows, fill in values
		for (int j = 0; j < col; j++) {
			//fill the array with X chars
			mazeGrid[i][j] = 'X';
		}
	}

	return mazeGrid;
}

bool isValid(int row, int col, int maxR, int maxC) {
	return (row >= 0) && (row < maxR) &&
		(col >= 0) && (col < maxC);
}

bool** makeOpenList(int row, int col) {
	bool** openList = 0;
	openList = new bool* [row]; //initialise the array with number of rows

	//for all rows, add columns
	for (int i = 0; i < row; i++) {

		//add the second array to the current array
		openList[i] = new bool[col];

		//for all columns and rows, fill in values
		for (int j = 0; j < col; j++) {
			//fill the array with 'false' bool values
			openList[i][j] = false;
		}
	}

	return openList;
}

//find a random start point
pair<int, int> findStartPoint(int r, int c) {
	//find the center of the maze coordinates
	int centerCoords[2] = { floor(r / 2), floor(c / 2) }; //half the input values of the maze then round down to find the index in the array

	bool isValid = false;
	pair<int, int> startPoint;

	//while starting point is invalid
	while (!isValid) {
		//pick random start point on the maze
		//don't pick anywhere in the center square
		startPoint.first = { rand() % r };
		startPoint.second = { rand() % c };

		//if the current position is within the center square then it isn't valid
		isValid = (startPoint.first != centerCoords[0] && startPoint.second != centerCoords[1] ? true : false);
	}
	return startPoint;
}

//find cell neighbours and return one at random
vector<pair<Cell, Cell>> GetNeighbours(Cell& cell, int rMax, int cMax, bool** openList) {
	const auto x = cell.coordX;
	const auto y = cell.coordY;

	vector<pair<Cell, Cell>> neighbours;

	//north neighbour is x-1, y
	Cell* parentNorth = new Cell(x - 2, y);
	//if not already visited
	Cell* childNorth = new Cell(x - 1, y);
	if ((parentNorth->coordX > 0 && parentNorth->coordX < rMax) && (parentNorth->coordY > 0 && parentNorth->coordY < cMax)) {
		if (openList[parentNorth->coordX][parentNorth->coordY] == false)
			neighbours.push_back(make_pair(*parentNorth, *childNorth));
	}


	//western neightbour is x, y-1
	Cell* parentWest = new Cell(x, y - 2);
	Cell* childWest = new Cell(x, y - 1);
	if (isValid(parentWest->coordX, parentWest->coordY, rMax, cMax)) {
		if (openList[parentWest->coordX][parentWest->coordY] == false)
			neighbours.push_back(make_pair(*parentWest, *childWest));
	}

	//eastern neighbour is x, y+2, child is y+1
	Cell* parentEast = new Cell(x, y + 2);
	Cell* childEast = new Cell(x, y + 1);
	if ((parentEast->coordX > 0 && parentEast->coordX < rMax) && (parentEast->coordY > 0 && parentEast->coordY < cMax)) {
		if (openList[parentEast->coordX][parentEast->coordY] == false)
			neighbours.push_back(make_pair(*parentEast, *childEast));
	}

	//southern neighbour is x+1, y
	Cell* parentSouth = new Cell(x + 2, y);
	Cell* childSouth = new Cell(x + 1, y);
	if ((parentSouth->coordX > 0 && parentSouth->coordX < rMax) && (parentSouth->coordY > 0 && parentSouth->coordY < cMax)) {
		if (openList[parentSouth->coordX][parentSouth->coordY] == false)
			neighbours.push_back(make_pair(*parentSouth, *childSouth));
	}

	return neighbours;
}

//find cell neighbours and return one at random
vector<Cell> GetPathNeighbours(Cell& cell, int rMax, int cMax, bool** openList) {
	const auto x = cell.coordX;
	const auto y = cell.coordY;

	vector<Cell> neighbours;

	//north neighbour is x-1, y
	Cell* north = new Cell(x - 1, y);
	if ((north->coordX > 0 && north->coordX < rMax) && (north->coordY > 0 && north->coordY < cMax)) {
		if (openList[north->coordX][north->coordY] == false)
			neighbours.push_back(*north);
	}


	//western neightbour is x, y-1
	Cell* west = new Cell(x, y - 1);
	if (isValid(west->coordX, west->coordY, rMax, cMax)) {
		if (openList[west->coordX][west->coordY] == false)
			neighbours.push_back(*west);
	}

	//eastern neighbour is x, y+2, child is y+1
	Cell* east = new Cell(x, y + 1);
	if ((east->coordX > 0 && east->coordX < rMax) && (east->coordY > 0 && east->coordY < cMax)) {
		if (openList[east->coordX][east->coordY] == false)
			neighbours.push_back(*east);
	}

	//southern neighbour is x+1, y
	Cell* south = new Cell(x + 1, y);
	if ((south->coordX > 0 && south->coordX < rMax) && (south->coordY > 0 && south->coordY < cMax)) {
		if (openList[south->coordX][south->coordY] == false)
			neighbours.push_back(*south);
	}

	return neighbours;
}

//display maze
void displayMaze(char** maze, int rows, int cols) {
	size_t rowCount = rows;
	size_t colCount = cols;

	for (int i = 0; i < rowCount; i++) {
		for (int j = 0; j < colCount; j++) {
			cout << maze[i][j];
		}
		cout << endl;
	}
}

void createCenter(pair<int, int> c, char** m) {

	//<<Left column>>
	m[c.first - 1][c.second - 1] = ' '; //top
	m[c.first][c.second - 1] = ' '; //middle
	m[c.first + 1][c.second - 1] = ' '; //bottom

	//<<Middle column>>
	m[c.first - 1][c.second] = ' '; //top
	m[c.first][c.second] = 'S'; //center
	m[c.first + 1][c.second] = ' '; //bottom

	//<<Right column>>
	m[c.first - 1][c.second + 1] = ' '; //top
	m[c.first][c.second + 1] = ' '; //middle
	m[c.first + 1][c.second + 1] = ' '; //bottom
}

void changeChar(char** m, int r, int c) {
	m[r][c] = ' ';
}

//place random exits at any side in the maze
vector<Cell> placeExits(char** m, int r, int c, int exits) {
	vector<Cell> exitCells;
	Cell* exit;
	for (int i = 0; i < exits; i++) {
		int randSide = rand() % 4;
		if (randSide == 0 || randSide == 2) {
			int x = rand() % r - 1;
			int y = randSide == 1 ? 0 : c - 1;
			m[x][y] = 'E';
			exit = new Cell(x, y);
		}
		else {
			int y = rand() % c - 1;
			int x = randSide == 1 ? 0 : r - 1;
			m[x][y] = 'E';
			exit = new Cell(x, y);
		}
		exitCells.push_back(*exit);
	}

	return exitCells;
}

char** genMaze(int rows, int cols) {
	char** maze = initGrid(rows, cols);
	bool** openList = makeOpenList(rows, cols);

	pair<int, int> startPoint = findStartPoint(rows, cols);
	Cell* startCell = new Cell(startPoint.first, startPoint.second);

	vector<Cell> pathSet;
	pathSet.push_back(*startCell);

	Cell currentCell = pathSet[0];

	//while there are still tiles to check
	while (!pathSet.empty()) {

		openList[currentCell.coordX][currentCell.coordY] = true; //mark as visited
		changeChar(maze, currentCell.coordX, currentCell.coordY); //mark on maze

		auto neighbourCells = GetNeighbours(currentCell, rows, cols, openList); //find the neighbours, returns a vector of pairs

		//if the set of neighbours is not empty
		if (!neighbourCells.empty()) {

			//for all pairs of neighbours
			for (auto p : neighbourCells) {
				pathSet.push_back(p.first); //add parent neighbour

				//mark child as visited
				openList[p.second.coordX][p.second.coordY] = true;
				changeChar(maze, p.second.coordX, p.second.coordY);
			}

			auto pos = find(pathSet.begin(), pathSet.end(), currentCell);
			pathSet.erase(pos); //remove current cell from set
			//only cell remaining should be the parent neighbour

			neighbourCells.clear();
		}
		else {
			pathSet.erase(find(pathSet.begin(), pathSet.end(), currentCell));
		}

		if (pathSet.empty())
			break;

		int randIndex = pathSet.size() != 0 ? rand() % pathSet.size() : 0;
		currentCell = pathSet.at(randIndex);
	}

	return maze;
}

int calcHeuristic(Cell src, Cell dest) {
	int h = (abs(src.coordX - dest.coordX) + abs(src.coordY - dest.coordY));
	return h;
}

Cell** createCellMap(int rows, int cols) {
	Cell** cellGrid;
	cellGrid = new Cell* [rows]; //initialise the array with number of rows

	//for all rows, add columns
	for (int i = 0; i < rows; i++) {

		//add the second array to the current array
		cellGrid[i] = new Cell[cols];

		//for all columns and rows, fill in values
		for (int j = 0; j < cols; j++) {
			//fill the array with X chars
			cellGrid[i][j].coordX = i;
			cellGrid[i][j].coordY = j;
			cellGrid[i][j].g = FLT_MAX;
			cellGrid[i][j].h = FLT_MAX;
			cellGrid[i][j].previousX = -1;
			cellGrid[i][j].previousY = -1;
		}
	}

	return cellGrid;
}

//TO-DO//
bool aStarSearch(char** maze, pair<int,int> start, pair<int,int> goal, int rows, int cols, vector<pair<int, int>> &path) {
	
	bool** closedList = makeOpenList(rows, cols); //make closedlist
	Cell** cellMap = createCellMap(rows, cols);

	Cell* startCell = &cellMap[start.first][start.second];
	Cell* destCell = &cellMap[goal.first][goal.second];

	calcHeuristic(*startCell, *destCell); //set start cell h value

	vector<Cell*> openSet; //set which will hold all cells visited on path
	openSet.push_back(startCell);

	while (!openSet.empty()) { //while not empty
		Cell* current = openSet[0];
		int bestF = (openSet[0]->g + openSet[0]->h);

		for (auto& c : openSet) {
			int cF = c->g + c->h;
			if (cF < bestF) {
				bestF = cF;
				current = c;
			}
		}

		if (*current == *destCell) {
			cout << "Reached goal." << endl;
			break;
		}

		for (int i = 0; i < 4; i++) {
			int cellX = current->coordX + (i % 2 == 0 ? i - 1 : 0);
			int cellY = current->coordY + (i % 2 == 0 ? 0 : i - 2);

			if (cellX < 1 || cellY < 1 || cellX > rows || cellY > cols)
				continue;

			Cell* cellChecking = &cellMap[cellX][cellY];

			if (closedList[cellChecking->coordX][cellChecking->coordY] == true || maze[cellChecking->coordX][cellChecking->coordY] == 'X')
				continue;

			int tempG = current->g + 1;
			int tempH = calcHeuristic(*(cellChecking), *destCell);
			int tempF = tempG + tempH;

			if ((find(openSet.begin(), openSet.end(), cellChecking) != end(openSet)) && tempF > (cellChecking->g + cellChecking->h)) {
				continue;
			}
			else {
				cellChecking->g = tempG;
				cellChecking->h = tempH;
				cellChecking->previousX = current->coordX;
				cellChecking->previousY = current->coordY;

				if (find(openSet.begin(), openSet.end(), cellChecking) == end(openSet))
					openSet.push_back(cellChecking);
			}
		}
		if (find(begin(openSet), end(openSet), current) != end(openSet)) {
			openSet.erase(find(openSet.begin(), openSet.end(), current)); //remove current from openset
			closedList[current->coordX][current->coordY] = true;
		}
	}

	if (destCell->previousX && destCell->previousY) {
		Cell* pathCell = destCell;
		while (pathCell != startCell) {
			pair<int,int> p = make_pair(pathCell->coordX, pathCell->coordY);
			path.push_back(p);
			pathCell = &cellMap[pathCell->previousX][pathCell->previousY];
		}
	}

	return true;
}

	//Opens and saves a new maze file
	void createMazeFile(char** m, int rows, int cols) {
		string fileName;
		cout << "Name of file: " << endl;
		cin >> fileName;

		ofstream outFile;
		outFile.open((fileName + ".txt"), ios::out); //open file using input name

		size_t rowCount = rows;
		size_t colCount = cols;

		//for each row and column, output the maze to the file
		for (int i = 0; i < rowCount; i++) {
			for (int j = 0; j < colCount; j++) {
				outFile << m[i][j];
			}
			outFile << endl;
		}

		outFile.close();
	}

	//Save a maze generated into file format
	void saveMaze(char** m, int rows, int cols) {
		char choice;
		cout << "Would you like to save this maze? (Y/N)" << endl; //input prompt, need to validate input
		cin >> choice;

		if (choice == 'Y' || choice == 'y') {
			createMazeFile(m, rows, cols);
		}
	}

	void findPath(char** maze, pair<int, int> center, pair<int,int> exit, int rows, int cols) {
		vector<pair<int, int>> path;
		bool queueList = aStarSearch(maze, center, exit, rows, cols, path);
		for (auto p : path) {
			if(maze[p.first][p.second] != 'E')
				maze[p.first][p.second] = 'o';
		}
	}

	//Create a new maze using user input
	void newMaze() {
		int rows, cols, exits;

		cout << "Select number of rows: ";
		rows = validateRows();
		cout << "Select number of columns: ";
		cols = validateColumns();
		cout << "Select number of exits: ";
		exits = validateExits();

		char** maze = genMaze(rows, cols);
		auto centerCoords = make_pair(rows / 2, cols / 2);
		createCenter(centerCoords, maze);

		auto exitCells = placeExits(maze, rows, cols, exits);
		int index = (rand() % exitCells.size());
		pair<int, int> exitCell = make_pair(exitCells[index].coordX, exitCells[index].coordY);

		findPath(maze, centerCoords, exitCell, rows, cols);
		displayMaze(maze, rows, cols);

		saveMaze(maze, rows, cols);
	}

	//load a maze file from user input
	void loadMaze() {
		//load file
	}

	//Initial menu interface
	void startMenu() {
		cout << "Would you like to: " << endl;

		cout << "\t 1. Start a new maze" << endl;
		cout << "\t 2. Load an existing maze" << endl;

		int validInput = validateMenu();

		if (validInput == 1) {
			newMaze();
		}
		else {
			loadMaze();
		}
	}

	bool executeAgain() {
		char choice;
		cout << "Would you like to execute again? (Y/N)" << endl;
		cin >> choice;

		if (choice == 'Y' || choice == 'y') {
			return true;
		}
		else {
			return false;
		}
	}

	//main
	int main() {

		//srand(time(NULL));

		do {
			startMenu();
		} while (executeAgain());

	}
