// hello i am humna
// hi humna i am areeba

#include <SFML/Graphics.hpp>
#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include "level.h"
#include "menu.h"
#include "auth.h"
#include "score.h"  
#include "theme.h"
#include "player.h"
#include "leaderboard.h"
#include <ctime>
#include <iomanip>
#include "fstream"
#include "LinkedList.h"

using namespace sf;
using namespace std;



const int M = 35;
const int N = 55;
int grid[M][N] = { 0 };
int ts = 18;
bool Game = true;


bool pause = false;

Leaderboard leaderboard;


//---------------MAIN GAME LOGIC---------------------------//
struct Enemy
{
    int x, y, dx, dy;
    bool isPaused;
    int originalDx, originalDy;

    Enemy()
    {
        x = y = 300;
        dx = 4 - rand() % 8;
        dy = 4 - rand() % 8;
        originalDx = dx;
        originalDy = dy;
        isPaused = false;
    }

    void move()
    {
     
        if (!isPaused) {
            x += dx;

			//-- Check for collision with left wall-----//
            if (x < 0) 
            { 
                x = 0; dx = -dx; }
         
			//-- Check for collision with right wall-----//

            if (x > (N * ts) - 1) 
            { x = (N * ts) - 1; dx = -dx; }
            if (grid[y / ts][x / ts] == 1) { dx = -dx; x += dx; }



			//-- Check for collision with top wall-----//
            y += dy;
            if (y < 0)
            { y = 0; dy = -dy; }


			//-- Check for collision with bottom wall-----//
            if (y > (M * ts) - 1) { y = (M * ts) - 1; dy = -dy; }
            if (grid[y / ts][x / ts] == 1) { dy = -dy; y += dy; }
        }
    }


    void pause() {
        originalDx = dx;
        originalDy = dy;
        dx = 0;
        dy = 0;
        isPaused = true;
    }

    void resume() {
        dx = originalDx;
        dy = originalDy;
        isPaused = false;
    }
};



//----------------------- Function to check for player collision---------------------//
bool checkPlayerCollision(int x1, int y1, int x2, int y2, int grid[M][N]) {
    
    if (x1 == x2 && y1 == y2) {
      

        return (grid[y1][x1] == 2 || grid[y1][x1] == 1);
    }
    return false;
}

//----------------------- Function to check for tile construction collision---------------------//

bool checkConstructionCollision(int x1, int y1, int x2, int y2, int grid[M][N],
    bool isPlayer1Constructing, bool isPlayer2Constructing) {
   
    if (x1 == x2 && y1 == y2) {
       
        if (isPlayer1Constructing != isPlayer2Constructing) {
            return true;
        }
    }
    return false;

}

//----------------------- Function to check for construction state collision---------------------//
bool checkConstructionStateCollision(int x1, int y1, int x2, int y2, bool isPlayer1Constructing, bool isPlayer2Constructing) 
{
    

	//--------Check if both players are at the same tile----------------//
    if (x1 == x2 && y1 == y2) {
   

		//-----------------Check if both players are constructing or not-------//
        if (isPlayer1Constructing && isPlayer2Constructing) {
            return true;
        }

		//-----------------Check if only one player is constructing----------------//
    
        if ((isPlayer1Constructing && !isPlayer2Constructing) ||
            (!isPlayer1Constructing && isPlayer2Constructing)) {
            return true;
        }
    }

    return false;
}

//--------------------- Additional helper function to make movement more flexible----------------------------//
void adjustPlayerMovement(int& x, int& y, int dx, int dy, int gridWidth, int gridHeight) {
    
    x += dx;
    y += dy;

   
    x = max(0, min(x, gridWidth - 1));
    y = max(0, min(y, gridHeight - 1));
}


//--------------------- Function to drop enemies----------------------------//
void drop(int y, int x)
{
    if (y < 0 || y >= M || x < 0 || x >= N)
        return;
    if (grid[y][x] != 0) 
        return;
    grid[y][x] = -1;
    drop(y - 1, x);
    drop(y + 1, x);
    drop(y, x - 1);
    drop(y, x + 1);
}



//--------------------- Function to reset the game----------------------------//
void resetGame(bool resetMenu, Menu& menu, ScoreManager& scoreManager,
    int& x1, int& y1, int& dx1, int& dy1,
    int& x2, int& y2, int& dx2, int& dy2)
{
    
    for (int i = 1; i < M - 1; i++)
        for (int j = 1; j < N - 1; j++)
            grid[i][j] = 0;

   
    x1 = 0; y1 = 0; dx1 = dy1 = 0;
    x2 = N - 1; y2 = M - 1; dx2 = dy2 = 0;

    
    Game = true;

    
    scoreManager.resetScores();

    
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    if (resetMenu) {
        menu.gameStarted = false;
        menu.showingMainMenu = true;
    }
}

string saveGame(const string& filename, int grid[M][N], int x1, int y1, int dx1, int dy1,
     Enemy enemies[], int enemyCount, ScoreManager& scoreManager) 
{
    ofstream outFile(filename);

    if (!outFile) {
        cout << "Failed to open save file!" << endl;
        return "";
    }

    time_t now = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime); 
    string timeStamp = string(buffer);


    outFile << timeStamp << "\n";
    // Save grid

	LinkedList linkedList;
	linkedList.convert2D(grid, M, N);
	Node* current = linkedList.getHead();
    for (int i = 0; i < M; ++i) 
    {
        for (int j = 0; j < N; ++j) 
        {
            if(current)
            {
                outFile << current->data << " ";
                current = current->next;
            }
        }
        
        outFile << "\n";
    }

    outFile << x1 << " " << y1 << " " << dx1 << " " << dy1 << "\n";

    outFile << enemyCount << "\n";
    for (int i = 0; i < enemyCount; ++i) {
        outFile << enemies[i].x << " " << enemies[i].y << " " << enemies[i].dx << " "
            << enemies[i].dy << " " << enemies[i].isPaused << "\n";
    }

    outFile << scoreManager.getPlayer1Score() << " " << scoreManager.getPlayer2Score() << "\n";

    outFile.close();
    cout << "Game saved successfully." << endl;

	return timeStamp;
}

string loadGame(const string& filename, int grid[M][N], int& x1, int& y1, int& dx1, int& dy1,
	Enemy enemies[], int& enemyCount, ScoreManager& scoreManager) {
	
    ifstream inFile(filename);
	
	if (!inFile) {
		cout << "Failed to open save file!" << endl;
		return "";
	}

    string timesaved;
	getline(inFile, timesaved);

	LinkedList linkedList;

    for (int i = 0; i < M; ++i)
    {
        int temp;
        for (int j = 0; j < N; ++j)
        {
            inFile >> temp;
			linkedList.insert(temp);
        }
    }

	linkedList.convertTo2D(grid);



	inFile >> x1 >> y1 >> dx1 >> dy1;
	inFile >> enemyCount;
	for (int i = 0; i < enemyCount; ++i) {
		inFile >> enemies[i].x >> enemies[i].y >> enemies[i].dx >> enemies[i].dy >> enemies[i].isPaused;
	}
	inFile >> scoreManager.player1Score >> scoreManager.player2Score;
	inFile.close();
	cout << "Game loaded successfully." << endl;

    return timesaved;
}

int main() {
   
    RenderWindow authWindow(VideoMode(600, 500), "Xonix - Login");

	Font font;
	font.loadFromFile("Courier Prime Bold.ttf");

	Text saveText;
	saveText.setFont(font);
	saveText.setCharacterSize(25);
	saveText.setFillColor(Color::Black);
	saveText.setPosition(40, 50);

	Text loadText;
	loadText.setFont(font);
	loadText.setCharacterSize(25);
	loadText.setFillColor(Color::Black);
	loadText.setPosition(40, 70);

	Clock saveClock;
	Clock loadClock;

    //--------------------  music--------------------------------//
    Music music;
    if (!music.openFromFile("Glass Animals - Heat Waves-[AudioTrimmer.com].wav")) {
        return -1; 
    }

    music.setLoop(true);     
    music.setVolume(50);      
    music.play();

    string time1, time2;
	bool save = false;
	bool load = false;


    Authentication auth;


    Clock powerUpClock;
    bool isPowerUpActive = false;

    
 //-----------------------------------------AUTHENTICATION----------------------------//
    string player1Username = auth.getCurrentUsername(); 
    if (!auth.showAuthScreen(authWindow)) {
        return 0;
    }

    
    string username = auth.getCurrentUsername();

    srand(time(0));



    
    //------------------------ GAME WINDOW -------------------------------//
    string windowTitle = "Xonix Game  (User: " + username + ")";
    RenderWindow window(VideoMode(N * ts, M * ts), windowTitle);
    window.setFramerateLimit(60);

   


    ScoreManager scoreManager;
  
    if (!scoreManager.initializeFont("arial.ttf")) {
        cout << "Failed to load font!" << endl;
        return -1;
    }


    Menu menu(window, username, auth);

    
   
    Texture t1, t2, t3;
    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/gameover.png");
    t3.loadFromFile("images/enemy.png");

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(100, 100);
    sEnemy.setOrigin(20, 20);


    Texture texture;
    if (!texture.loadFromFile("images/game.jpg")) {  
        cout << "Error loading texture" << endl;
        return -1; 
    }


    Sprite sprite(texture);
    
	//--------------------- Initialize game settings ----------------------------//
    int enemyCount = 4;
    Enemy a[10];

    int x1 = 0, y1 = 0, dx1 = 0, dy1 = 0;
    int x2 = N - 1, y2 = M - 1, dx2 = 0, dy2 = 0;
    float timer = 0, delay = 0.1;
    Clock clock;



    Level level;
    int currentLevel = 0;
    level.setLevel(currentLevel);

    enemyCount = level.enemyCount;
    delay = level.gameSpeed;



	//--------------------- Initialize grid ----------------------------//
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;






	//--------------------- Initialize game state ----------------------------//
    while (window.isOpen()) {

        
        
        if (menu.showingMainMenu) 
        {
            
            menu.displayMainMenu();
            menu.handleMainMenu();
            
			//--------------------- Handle Level Selection ----------------------------//
            if (menu.getCurrentLevel() != currentLevel) {
                currentLevel = menu.getCurrentLevel();
                level.setLevel(currentLevel);
                enemyCount = level.enemyCount;
                delay = level.gameSpeed;
                cout << "Updated settings for level " << currentLevel + 1 << endl;
            }
        }
        //--------------------- Handle Mode----------------------------------//
        else if (menu.showingModeSelection) {
            menu.selectMode();
            menu.handleModeSelection();
        }

        //--------------------- Handle Matchmaking----------------------------//
        else if (menu.showingMatchmakingQueue) {  
            menu.showMatchmakingQueue();
            menu.handleMatchmakingQueue();
        }

        
		//--------------------- Handle matchmaking and level ----------------------------//
        else if (menu.showingLevelSelection) {
            menu.displayLevelSelection();
            menu.handleLevelSelection();
        }
        else if (menu.gameStarted) {

            
           
            menu.syncPlayerNames();

            
            scoreManager.setGameMode(menu.isSinglePlayerSelected);

            

			//--------------------- Multiplayer Mode ----------------------------//
            if (!menu.isSinglePlayerSelected) {
            
                string player1Name = menu.getValidPlayerName(menu.matchedPlayer1, username);
                string player2Name = menu.getValidPlayerName(menu.matchedPlayer2, "Player2");

               
                scoreManager.setPlayerUsernames(player1Name, player2Name);

              
                string windowTitle = "Xonix Game - Multiplayer! (Players: " +
                    player1Name + " vs " + player2Name + ")";
                window.setTitle(windowTitle);
            }
            else 
            {
                
                scoreManager.setPlayerUsernames(username, "");
            }

            float time = clock.getElapsedTime().asSeconds();
            clock.restart();
            timer += time;


            Event e;
            while (window.pollEvent(e)) {
                if (e.type == Event::Closed)
                    window.close();


                if (e.type == Event::KeyPressed)
                {
					//------------------- Pause Game ------------------//
                    if (e.key.code == Keyboard::Space) {
                        if (pause)
                        {
                            pause = false;
                        }
                        else {
                            pause = true;
                        }
                    }

					if (e.key.code == Keyboard::S) 
                    {
						string filename = username + "_save.txt";
					    time1 = saveGame(filename, grid, x1, y1, dx1, dy1, a, enemyCount, scoreManager);
                        save = 1;
						saveText.setString("Game saved successfully at: " + time1 + " by "+username);
						saveClock.restart();
					}

                    if (e.key.code == Keyboard::L) 
                    {
						string filename = username + "_save.txt";
                        time2 = loadGame(filename, grid, x1, y1, dx1, dy1, a, enemyCount, scoreManager);
						load = 1;
						loadText.setString("Game loaded successfully at: " + time2 + " by " + username);
						loadClock.restart();
                    }
                }

				//------------------- Reset Game ------------------//
                if (e.type == Event::KeyPressed)
                {
                    if (e.key.code == Keyboard::Escape) {
                        resetGame(true, menu, scoreManager, x1, y1, dx1, dy1, x2, y2, dx2, dy2);
                    }

                  
                    if (e.key.code == Keyboard::R) {
                        resetGame(false, menu, scoreManager, x1, y1, dx1, dy1, x2, y2, dx2, dy2);
                    }

					//------------------- Power-Up Activation ------------------//
                    if (e.key.code == Keyboard::P) {
                        if (scoreManager.tryActivatePowerUp(true)) {
                           
                            for (int i = 0; i < enemyCount; i++) {
                                a[i].pause();
                            }
                            isPowerUpActive = true;
                            powerUpClock.restart();
                        }
                    }

                }
            }
			//------------------- Handle Player Movement ------------------//
            if (!pause) {
				//------------------- Single Player Movement ------------------//
                if (menu.isSinglePlayerSelected) {

                    
                    if (Keyboard::isKeyPressed(Keyboard::Left))
                    {
                        dx1 = -1; dy1 = 0;
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Right))
                    {
                        dx1 = 1; dy1 = 0;
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Up))
                    {
                        dx1 = 0; dy1 = -1;
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Down))
                    {
                        dx1 = 0; dy1 = 1;
                    }
                }
				//------------------- Multiplayer Movement ------------------//
                else {
                   
                    if (Keyboard::isKeyPressed(Keyboard::Left))
                    {
                        dx1 = -1; dy1 = 0;
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Right))
                    {
                        dx1 = 1; dy1 = 0;
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Up))
                    {
                        dx1 = 0; dy1 = -1;
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Down))
                    {
                        dx1 = 0; dy1 = 1;
                    }

                  
                    if (Keyboard::isKeyPressed(Keyboard::A))
                    {
                        dx2 = -1; dy2 = 0;
                    }
                    if (Keyboard::isKeyPressed(Keyboard::D))
                    {
                        dx2 = 1; dy2 = 0;
                    }
                    if (Keyboard::isKeyPressed(Keyboard::W))
                    {
                        dx2 = 0; dy2 = -1;
                    }
                    if (Keyboard::isKeyPressed(Keyboard::S))
                    {
                        dx2 = 0; dy2 = 1;
                    }
                }

				//------------------- Check for Game Over ------------------//
                if (!Game) 
                    continue;


                
                if (timer > delay) {
                       
                    int prevFilledTiles1 = ScoreManager::countFilledTiles(grid);

                    x1 += dx1;
                    y1 += dy1;
                    if (x1 < 0) 
                        x1 = 0;
                    if (x1 > N - 1) 
                        x1 = N - 1;
                    if (y1 < 0) 
                        y1 = 0;
                    if (y1 > M - 1) 
                        y1 = M - 1;
                    if (grid[y1][x1] == 2)
                        Game = false;
                    if (grid[y1][x1] == 0) 
                        grid[y1][x1] = 2;

                  
                    int currentFilledTiles1 = ScoreManager::countFilledTiles(grid);

					//-------------------- Multiplayer Movement ------------------//
                    if (!menu.isSinglePlayerSelected) 
                    {
                        int prevFilledTiles2 = currentFilledTiles1;

						//----------------------CALCULATIONS-----------------------//
                        x2 += dx2;
                        y2 += dy2;
                        if (x2 < 0) 
                            x2 = 0;
                        if (x2 > N - 1) 
                            x2 = N - 1;
                        if (y2 < 0)
                            y2 = 0; 
                        if (y2 > M - 1) 
                            y2 = M - 1;
                        if (grid[y2][x2] == 2)
                            Game = false;
                        if (grid[y2][x2] == 0) 
                            grid[y2][x2] = 2;

                        
                        if (x1 == x2 && y1 == y2) {
                            
							//----------------- Check if both players are constructing or not-------//
                            if (grid[y1][x1] == 2 && grid[y2][x2] != 2) {
                                Game = false; 
                            }
                          

                            else if (grid[y2][x2] == 2 && grid[y1][x1] != 2) {
                                Game = false;  
                            }
                        }

                      
                        bool isPlayer1Constructing = (grid[y1][x1] == 2);
                        bool isPlayer2Constructing = (grid[y2][x2] == 2);

                        
						//------------------ Check for collision with enemies and construction state-----------------------//
                        if (checkPlayerCollision(x1, y1, x2, y2, grid) ||
                            checkConstructionCollision(x1, y1, x2, y2, grid, isPlayer1Constructing, isPlayer2Constructing)) {
                          
                            Game = false;
                        }

                       
                        int currentFilledTiles2 = ScoreManager::countFilledTiles(grid);

                        scoreManager.updateScore(currentFilledTiles1 - prevFilledTiles1,currentFilledTiles2 - prevFilledTiles2 );
                    }
                    else {
                     
                        scoreManager.updateScore( currentFilledTiles1 - prevFilledTiles1, 0);
                    }

                    timer = 0;
                }

				//------------------- Enemy Movement ------------------//
                for (int i = 0; i < enemyCount; i++) {
                    a[i].move();

					//-- Check for collision with player 1-----//
                    if ((a[i].x / ts == x1 && a[i].y / ts == y1) ||
                        (!menu.isSinglePlayerSelected && a[i].x / ts == x2 && a[i].y / ts == y2))
                        Game = false;
                }

				//------------------- Check for collision with players and construction state------------------//
                if (!menu.isSinglePlayerSelected) {
                    bool isPlayer1Constructing = (grid[y1][x1] == 2);
                    bool isPlayer2Constructing = (grid[y2][x2] == 2);

                  
                    if (checkPlayerCollision(x1, y1, x2, y2, grid) ||
                        checkConstructionCollision(x1, y1, x2, y2, grid, isPlayer1Constructing, isPlayer2Constructing) ||
                        checkConstructionStateCollision(x1, y1, x2, y2, isPlayer1Constructing, isPlayer2Constructing)) {
                     
                        Game = false;
                    }
                }
				//------------------- Check for power-up activation ------------------//
                if (isPowerUpActive) {
                    float powerUpTime = powerUpClock.getElapsedTime().asSeconds();
                    if (powerUpTime >= 3.0f) {
                        // Resume enemy movement after 3 seconds
                        for (int i = 0; i < enemyCount; i++) {
                            a[i].resume();
                        }
                        isPowerUpActive = false;
                    }
                }

				//------------------- Check for game over ------------------//
                if (grid[y1][x1] == 1) {
                    dx1 = dy1 = 0;
                    for (int i = 0; i < enemyCount; i++)
                        drop(a[i].y / ts, a[i].x / ts);
                    for (int i = 0; i < M; i++)
                        for (int j = 0; j < N; j++)
                            if (grid[i][j] == -1) grid[i][j] = 0;
                            else grid[i][j] = 1;
                }

                if (!menu.isSinglePlayerSelected && grid[y2][x2] == 1) {
                    dx2 = dy2 = 0;
                    for (int i = 0; i < enemyCount; i++)
                        drop(a[i].y / ts, a[i].x / ts);
                    for (int i = 0; i < M; i++)
                        for (int j = 0; j < N; j++)
                            if (grid[i][j] == -1) grid[i][j] = 0;
                            else grid[i][j] = 1;
                }
                

                //-----------------------------------------IMPORTANT PART OF OUR GAME--------------------------//
                if (!Game) {

                   
                    menu.syncPlayerNames();

                    string player1Name = menu.isSinglePlayerSelected ? username : menu.getValidPlayerName(menu.matchedPlayer1, "Player1");

                    string player2Name = menu.isSinglePlayerSelected ? "" : menu.getValidPlayerName(menu.matchedPlayer2, "Player2");

                 
                    scoreManager.setPlayerUsernames(player1Name, player2Name);

                    menu.displayEndMenu( scoreManager.getPlayer1Score(), menu.isSinglePlayerSelected ? 0 : scoreManager.getPlayer2Score());

                    scoreManager.recordGameResults("score.txt", "powerup.txt", player1Name, player2Name, menu.isSinglePlayerSelected);

                    leaderboard.saveLeaderboardFromScoreFile("score.txt", "leaderboard.txt");
                    continue;
                }

           
            }

			if (saveClock.getElapsedTime().asSeconds() > 5)
				save = false;

			if (loadClock.getElapsedTime().asSeconds() > 5)
				load = false;

            window.clear();


            //Texture texture;
            //if (!texture.loadFromFile("images/game.jpg")) {  
            //    cerr << "Error loading texture" << endl;
            //    return -1; // Exit if image fails to load
            //}


            //Sprite sprite(texture);


            window.draw(sprite);
            
			//---------------------- Draw grid ----------------------------//
            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++) {
                    if (grid[i][j] == 0) 
                        continue;
                    if (grid[i][j] == 1)
                        sTile.setTextureRect(IntRect(18, 0, ts, ts));
                    if (grid[i][j] == 2)
                        sTile.setTextureRect(IntRect(54, 0, ts, ts));
                    sTile.setPosition(j * ts, i * ts);
                    window.draw(sTile);
                }

			//--------------------- Draw players ------------------//
            sTile.setTextureRect(IntRect(36, 0, ts, ts));
            sTile.setPosition(x1 * ts, y1 * ts);
            window.draw(sTile);


			//--------------------- Draw player 2 ------------------//
            if (!menu.isSinglePlayerSelected) {
                sTile.setTextureRect(IntRect(36, 0, ts, ts));
                sTile.setPosition(x2 * ts, y2 * ts);
                window.draw(sTile);
            }

			//--------------------- Draw enemies ------------------//
            sEnemy.rotate(10);
            for (int i = 0; i < enemyCount; i++) {
                sEnemy.setPosition(a[i].x, a[i].y);
                window.draw(sEnemy);
            }

			
            string p1Name = menu.isSinglePlayerSelected ? username: menu.getValidPlayerName(menu.matchedPlayer1, username);
            string p2Name = menu.isSinglePlayerSelected ? "" : menu.getValidPlayerName(menu.matchedPlayer2, "Player2");


   
            scoreManager.setPlayerUsernames(p1Name, p2Name);

            
            scoreManager.drawScores(window, menu.isSinglePlayerSelected);


            if (saveClock.getElapsedTime().asSeconds() < 5 && save)
            {
                window.draw(saveText);
            }

            if (loadClock.getElapsedTime().asSeconds() < 5 && load)
            {
                window.draw(loadText);
            }

            window.display();
        }
    }

    return 0;
}