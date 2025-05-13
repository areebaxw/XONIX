#include "level.h"
#include <iostream>



using namespace std;

Level::Level() {
    rows = 25;
    cols = 40;
    enemyCount = 4;
    gameSpeed = 0.07f;
    grid = NULL;
    allocateGrid(rows, cols);
    initializeGrid();
}

Level::~Level() {
    deallocateGrid();
}

void Level::setLevel(int level) {
    deallocateGrid();
    switch (level) {
    case 0:  // Easy
        rows = 25;
        cols = 40;
        enemyCount = 5;
        gameSpeed = 0.07f;
        break;
    case 1:  // Medium
        rows = 30;
        cols = 50;
        enemyCount = 8;
        gameSpeed = 0.06f;
        break;
    case 2:  // Hard
        rows = 35;
        cols = 60;
        enemyCount = 10;
        gameSpeed = 0.05f;
        break;
    default:
        setLevel(0);  // Default
        return;
    }
    allocateGrid(rows, cols);

    initializeGrid();

    cout << "Level " << level + 1 << " set with " << enemyCount << " enemies." << endl;
}



void Level::allocateGrid(int rows, int cols) 
{
    grid = new int* [rows];
    for (int i = 0; i < rows; i++) 
    {
        grid[i] = new int[cols];
    }
}

void Level::deallocateGrid() 
{
    if (grid)
    {
        for (int i = 0; i < rows; i++)
        {
            delete[] grid[i];
        }
        delete[] grid;
        grid = NULL;
    }
}

void Level::initializeGrid() 
{
    for (int i = 0; i < rows; i++) 
    {
        for (int j = 0; j < cols; j++) 
        {
            grid[i][j] = 0;
        }
    }

   
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++) {
            if (i == 0 || j == 0 || i == rows - 1 || j == cols - 1) 
            {
                grid[i][j] = 1;  // 1 represents wall
            }
        }
    }
}

void Level::resetLevel() 
{
    initializeGrid();
    cout << "Level reset!" << endl;
}

//
//void Level::addObstacles() {
//    
//    if (rows >= 30) {  
//       
//        for (int i = 0; i < 5; i++) {
//            int randomRow = 5 + rand() % (rows - 10);
//            int randomCol = 5 + rand() % (cols - 10);
//            grid[randomRow][randomCol] = 1; 
//        }
//    }
//
//    if (rows >= 35) {  
//     
//        for (int i = 10; i < 15; i++) {
//            grid[i][cols / 2] = 1;  
//        }
//
//        for (int j = cols / 4; j < 3 * cols / 4; j++) {
//            grid[rows / 2][j] = 1;  
//        }
//    }
//}



//--------------------------------------To get grid value at a specific position----------------------//
int Level::getGridAt(int row, int col) 
{
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        return -1; 
    }
    return grid[row][col];
}

//----------------------------To set grid value at a specific position----------------------------------//
void Level::setGridAt(int row, int col, int value) 
{
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        grid[row][col] = value;
    }
}
