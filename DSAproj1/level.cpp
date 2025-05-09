#include "level.h"
#include <iostream>

Level::Level() {
    rows = 25;
    cols = 40;
    enemyCount = 4;
    gameSpeed = 0.07f;
    grid = nullptr;
    allocateGrid(rows, cols);
    initializeGrid();
}

Level::~Level() {
    deallocateGrid();
}

void Level::setLevel(int level) {
    deallocateGrid();
    switch (level) {
    case 0:  // Easy level
        rows = 25;
        cols = 40;
        enemyCount = 5;
        gameSpeed = 0.07f;
        break;
    case 1:  // Medium level
        rows = 30;
        cols = 50;
        enemyCount = 8;
        gameSpeed = 0.06f;
        break;
    case 2:  // Hard level
        rows = 35;
        cols = 60;
        enemyCount = 10;
        gameSpeed = 0.05f;
        break;
    default:
        setLevel(0);  // Default to easy level
        return;
    }
    allocateGrid(rows, cols);
    initializeGrid();
    std::cout << "Level " << level + 1 << " set with " << enemyCount << " enemies." << std::endl;
}

void Level::allocateGrid(int rows, int cols) {
    grid = new int* [rows];
    for (int i = 0; i < rows; i++) {
        grid[i] = new int[cols];
    }
}

void Level::deallocateGrid() {
    if (grid) {
        for (int i = 0; i < rows; i++) {
            delete[] grid[i];
        }
        delete[] grid;
        grid = nullptr;
    }
}

void Level::initializeGrid() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = 0;
        }
    }

    // Set boundaries as walls
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (i == 0 || j == 0 || i == rows - 1 || j == cols - 1) {
                grid[i][j] = 1;  // 1 represents wall
            }
        }
    }
}

void Level::resetLevel() {
    initializeGrid();
    std::cout << "Level reset!" << std::endl;
}

// Add obstacles for different levels
void Level::addObstacles() {
    // This can be expanded based on level requirements
    if (rows >= 30) {  // Medium and Hard levels
        // Add some random obstacles in the middle of the field
        for (int i = 0; i < 5; i++) {
            int randomRow = 5 + rand() % (rows - 10);
            int randomCol = 5 + rand() % (cols - 10);
            grid[randomRow][randomCol] = 1;  // Add a wall obstacle
        }
    }

    if (rows >= 35) {  // Hard level only
        // Add more complex patterns for hard level
        for (int i = 10; i < 15; i++) {
            grid[i][cols / 2] = 1;  // Vertical wall in the middle
        }

        for (int j = cols / 4; j < 3 * cols / 4; j++) {
            grid[rows / 2][j] = 1;  // Horizontal wall in the middle
        }
    }
}

// Helper method to get grid value at a specific position
int Level::getGridAt(int row, int col) {
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        return -1;  // Out of bounds
    }
    return grid[row][col];
}

// Helper method to set grid value at a specific position
void Level::setGridAt(int row, int col, int value) {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        grid[row][col] = value;
    }
}
