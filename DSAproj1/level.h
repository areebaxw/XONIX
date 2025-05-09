#pragma once
#include <SFML/Graphics.hpp>

class Level {
private:
    int** grid;  

  
    void allocateGrid(int rows, int cols);
    void deallocateGrid();
    void initializeGrid();

public:
    Level();
    ~Level(); 

    int rows;
    int cols;
    int enemyCount;
    float gameSpeed;

   
    void setLevel(int level);
    void resetLevel();
    void addObstacles();

    
    int getGridAt(int row, int col);
    void setGridAt(int row, int col, int value);
};