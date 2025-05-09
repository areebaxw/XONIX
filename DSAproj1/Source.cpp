#include <SFML/Graphics.hpp>
#include <iostream>
#include <SFML/Window.hpp>
#include <time.h>
#include "level.h"
#include "menu.h"
#include "auth.h"
#include "score.h"  
#include "theme.h"
#include "player.h"
#include "leaderboard.h"

using namespace sf;
using namespace std;

const int M = 35;
const int N = 55;
int grid[M][N] = { 0 };
int ts = 18;
bool Game = true;
Leaderboard leaderboard;

// Enemy structure definition
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
        // Only move if not paused
        if (!isPaused) {
            x += dx;
            if (x < 0) { x = 0; dx = -dx; }
            if (x > (N * ts) - 1) { x = (N * ts) - 1; dx = -dx; }
            if (grid[y / ts][x / ts] == 1) { dx = -dx; x += dx; }

            y += dy;
            if (y < 0) { y = 0; dy = -dy; }
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
bool checkPlayerCollision(int x1, int y1, int x2, int y2, int grid[M][N]) {
    // More forgiving collision detection
    if (x1 == x2 && y1 == y2) {
        // Collision only if both players are constructing or on a border tile
        return (grid[y1][x1] == 2 || grid[y1][x1] == 1);
    }
    return false;
}

bool checkConstructionCollision(int x1, int y1, int x2, int y2, int grid[M][N],
    bool isPlayer1Constructing, bool isPlayer2Constructing) {
    // Prevent problematic tile crossings
    if (x1 == x2 && y1 == y2) {
        // Collision if players are on the same tile with different construction states
        if (isPlayer1Constructing != isPlayer2Constructing) {
            return true;
        }
    }
    return false;

}
bool checkConstructionStateCollision(int x1, int y1, int x2, int y2, bool isPlayer1Constructing, bool isPlayer2Constructing) {
    // Scenario 2: If players are on the same tile and in different construction states
    if (x1 == x2 && y1 == y2) {
        // Scenario A: Both players are constructing simultaneously
        if (isPlayer1Constructing && isPlayer2Constructing) {
            return true;
        }

        // Scenario B: One player is constructing, the other is not
        if ((isPlayer1Constructing && !isPlayer2Constructing) ||
            (!isPlayer1Constructing && isPlayer2Constructing)) {
            return true;
        }
    }

    return false;
}

// Additional helper function to make movement more flexible
void adjustPlayerMovement(int& x, int& y, int dx, int dy, int gridWidth, int gridHeight) {
    // Allow movement within grid boundaries
    x += dx;
    y += dy;

    // Boundary checks with more flexibility
    x = std::max(0, std::min(x, gridWidth - 1));
    y = std::max(0, std::min(y, gridHeight - 1));
}


// Drop (flood fill)
void drop(int y, int x)
{
    if (y < 0 || y >= M || x < 0 || x >= N) return;
    if (grid[y][x] != 0) return;
    grid[y][x] = -1;
    drop(y - 1, x);
    drop(y + 1, x);
    drop(y, x - 1);
    drop(y, x + 1);
}

// Modify the resetGame function to incorporate ScoreManager
void resetGame(bool resetMenu, Menu& menu, ScoreManager& scoreManager,
    int& x1, int& y1, int& dx1, int& dy1,
    int& x2, int& y2, int& dx2, int& dy2)
{
    // Reset the grid
    for (int i = 1; i < M - 1; i++)
        for (int j = 1; j < N - 1; j++)
            grid[i][j] = 0;

    // Reset player positions and movement
    x1 = 0; y1 = 0; dx1 = dy1 = 0;
    x2 = N - 1; y2 = M - 1; dx2 = dy2 = 0;

    // Reset game state
    Game = true;

    // Reset scores
    scoreManager.resetScores();

    // Create border
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    if (resetMenu) {
        menu.gameStarted = false;
        menu.showingMainMenu = true;
    }
}

int main() {
    // Create authentication window first
    sf::RenderWindow authWindow(sf::VideoMode(600, 500), "Xonix - Authentication");
    Authentication auth;

    Clock powerUpClock;
    bool isPowerUpActive = false;

    // Assuming the username is already retrieved from authentication
    string player1Username = auth.getCurrentUsername(); // From authentication



    // Show authentication screen
    if (!auth.showAuthScreen(authWindow)) {
        return 0; // Exit if authentication fails
    }

    // Get the authenticated username
    string username = auth.getCurrentUsername();

    srand(time(0));

    // Modify window title to include username
    string windowTitle = "Xonix Game  (User: " + username + ")";
    RenderWindow window(VideoMode(N * ts, M * ts), windowTitle);
    window.setFramerateLimit(60);

    // Score Manager initialization
    ScoreManager scoreManager;
    // Load font (replace with your font path)
    if (!scoreManager.initializeFont("arial.ttf")) {
        cout << "Failed to load font!" << endl;
        return -1;
    }

    // Menu setup
/// friend list
    Menu menu(window, username, auth);

    Texture t1, t2, t3;
    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/gameover.png");
    t3.loadFromFile("images/enemy.png");

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(100, 100);
    sEnemy.setOrigin(20, 20);

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

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    while (window.isOpen()) {
        if (menu.showingMainMenu) {
            menu.displayMainMenu();
            menu.handleMainMenu();
            if (menu.getCurrentLevel() != currentLevel) {
                currentLevel = menu.getCurrentLevel();
                level.setLevel(currentLevel);
                enemyCount = level.enemyCount;
                delay = level.gameSpeed;
                cout << "Updated settings for level " << currentLevel + 1 << endl;
            }
        }
        else if (menu.showingModeSelection) {
            menu.selectMode();
            menu.handleModeSelection();
        }

        else if (menu.showingMatchmakingQueue) {  // New state
            menu.showMatchmakingQueue();
            menu.handleMatchmakingQueue();
        }


        else if (menu.showingLevelSelection) {
            menu.displayLevelSelection();
            menu.handleLevelSelection();
        }
        else if (menu.gameStarted) {

            
            // Synchronize player names before starting game
            menu.syncPlayerNames();

            // Set score manager game mode
            scoreManager.setGameMode(menu.isSinglePlayerSelected);

            // Check if it's a multiplayer game
            if (!menu.isSinglePlayerSelected) {
                // Get valid player names
                string player1Name = menu.getValidPlayerName(menu.matchedPlayer1, username);
                string player2Name = menu.getValidPlayerName(menu.matchedPlayer2, "Player2");

                // Set player usernames in score manager
                scoreManager.setPlayerUsernames(player1Name, player2Name);

                // Update window title with player names
                string windowTitle = "Xonix Game - Multiplayer! (Players: " +
                    player1Name + " vs " + player2Name + ")";
                window.setTitle(windowTitle);
            }
            else {
                // Set username for single player mode
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
                    if (e.key.code == Keyboard::Escape) {
                        resetGame(true, menu, scoreManager, x1, y1, dx1, dy1, x2, y2, dx2, dy2);
                    }

                    if (e.key.code == Keyboard::R) {
                        resetGame(false, menu, scoreManager, x1, y1, dx1, dy1, x2, y2, dx2, dy2);
                    }

                    if (e.key.code == Keyboard::P) {
                        if (scoreManager.tryActivatePowerUp(true)) {
                            // Stop all enemies
                            for (int i = 0; i < enemyCount; i++) {
                                a[i].pause();
                            }
                            isPowerUpActive = true;
                            powerUpClock.restart();
                        }
                    }

                }
            }

            if (menu.isSinglePlayerSelected) {
                if (Keyboard::isKeyPressed(Keyboard::Left)) { dx1 = -1; dy1 = 0; }
                if (Keyboard::isKeyPressed(Keyboard::Right)) { dx1 = 1; dy1 = 0; }
                if (Keyboard::isKeyPressed(Keyboard::Up)) { dx1 = 0; dy1 = -1; }
                if (Keyboard::isKeyPressed(Keyboard::Down)) { dx1 = 0; dy1 = 1; }
            }
            else {
                // Player 1 (Arrow Keys)
                if (Keyboard::isKeyPressed(Keyboard::Left)) { dx1 = -1; dy1 = 0; }
                if (Keyboard::isKeyPressed(Keyboard::Right)) { dx1 = 1; dy1 = 0; }
                if (Keyboard::isKeyPressed(Keyboard::Up)) { dx1 = 0; dy1 = -1; }
                if (Keyboard::isKeyPressed(Keyboard::Down)) { dx1 = 0; dy1 = 1; }

                // Player 2 (WASD Keys)
                if (Keyboard::isKeyPressed(Keyboard::A)) { dx2 = -1; dy2 = 0; }
                if (Keyboard::isKeyPressed(Keyboard::D)) { dx2 = 1; dy2 = 0; }
                if (Keyboard::isKeyPressed(Keyboard::W)) { dx2 = 0; dy2 = -1; }
                if (Keyboard::isKeyPressed(Keyboard::S)) { dx2 = 0; dy2 = 1; }
            }

            if (!Game) continue;

            if (timer > delay) {
                // Store previous filled tile count
                int prevFilledTiles1 = ScoreManager::countFilledTiles(grid);

                x1 += dx1;
                y1 += dy1;
                if (x1 < 0) x1 = 0; if (x1 > N - 1) x1 = N - 1;
                if (y1 < 0) y1 = 0; if (y1 > M - 1) y1 = M - 1;
                if (grid[y1][x1] == 2) Game = false;
                if (grid[y1][x1] == 0) grid[y1][x1] = 2;

                // Calculate new filled tiles for player 1
                int currentFilledTiles1 = ScoreManager::countFilledTiles(grid);

                if (!menu.isSinglePlayerSelected) {
                    int prevFilledTiles2 = currentFilledTiles1;

                    x2 += dx2;
                    y2 += dy2;
                    if (x2 < 0) x2 = 0; if (x2 > N - 1) x2 = N - 1;
                    if (y2 < 0) y2 = 0; if (y2 > M - 1) y2 = M - 1;
                    if (grid[y2][x2] == 2) Game = false;
                    if (grid[y2][x2] == 0) grid[y2][x2] = 2;

                    // Specific collision logic you requested
                    if (x1 == x2 && y1 == y2) {
                        // Player 1 is constructing, Player 2 is not
                        if (grid[y1][x1] == 2 && grid[y2][x2] != 2) {
                            Game = false;  // Player 1 dies
                        }
                        // Player 2 is constructing, Player 1 is not
                        else if (grid[y2][x2] == 2 && grid[y1][x1] != 2) {
                            Game = false;  // Player 2 dies
                        }
                    }

                   // Improved collision detection
                    bool isPlayer1Constructing = (grid[y1][x1] == 2);
                    bool isPlayer2Constructing = (grid[y2][x2] == 2);


                    // Add the collision check HERE
                   // More nuanced collision checks
                    if (checkPlayerCollision(x1, y1, x2, y2, grid) ||
                        checkConstructionCollision(x1, y1, x2, y2, grid, isPlayer1Constructing, isPlayer2Constructing)) {
                        // Only end game for truly problematic collisions
                        Game = false;
                    }

                    // Calculate new filled tiles for player 2
                    int currentFilledTiles2 = ScoreManager::countFilledTiles(grid);





                    // Update scores for both players
                    scoreManager.updateScore(
                        currentFilledTiles1 - prevFilledTiles1,
                        currentFilledTiles2 - prevFilledTiles2
                    );
                }
                else {
                    // Update score for single player
                    scoreManager.updateScore(
                        currentFilledTiles1 - prevFilledTiles1,
                        0
                    );
                }

                timer = 0;
            }

            for (int i = 0; i < enemyCount; i++) {
                a[i].move();

                if ((a[i].x / ts == x1 && a[i].y / ts == y1) ||
                    (!menu.isSinglePlayerSelected && a[i].x / ts == x2 && a[i].y / ts == y2))
                    Game = false;
            }

            if (!menu.isSinglePlayerSelected) {
                bool isPlayer1Constructing = (grid[y1][x1] == 2);
                bool isPlayer2Constructing = (grid[y2][x2] == 2);

                // Comprehensive collision checks
                if (checkPlayerCollision(x1, y1, x2, y2, grid) ||
                    checkConstructionCollision(x1, y1, x2, y2, grid, isPlayer1Constructing, isPlayer2Constructing) ||
                    checkConstructionStateCollision(x1, y1, x2, y2, isPlayer1Constructing, isPlayer2Constructing)) {
                    // Only end game for truly problematic collisions
                    Game = false;
                }
            }

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
            if (!Game) {
                menu.syncPlayerNames();

                string player1Name = menu.isSinglePlayerSelected ?
                    username :
                    menu.getValidPlayerName(menu.matchedPlayer1, "Player1");

                string player2Name = menu.isSinglePlayerSelected ?
                    "" :
                    menu.getValidPlayerName(menu.matchedPlayer2, "Player2");

                // Update the scoreManager with the correct player names
                scoreManager.setPlayerUsernames(player1Name, player2Name);

                menu.displayEndMenu(
                    scoreManager.getPlayer1Score(),
                    menu.isSinglePlayerSelected ? 0 : scoreManager.getPlayer2Score()
                );

                scoreManager.recordGameResults(
                    "score.txt",
                    "powerup.txt",
                    player1Name,
                    player2Name,
                    menu.isSinglePlayerSelected


                );
                leaderboard.saveLeaderboardFromScoreFile("score.txt", "leaderboard.txt");
                continue;
            }

            ///////// draw /////////
            window.clear();

            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++) {
                    if (grid[i][j] == 0) continue;
                    if (grid[i][j] == 1)
                        sTile.setTextureRect(IntRect(0, 0, ts, ts));
                    if (grid[i][j] == 2)
                        sTile.setTextureRect(IntRect(54, 0, ts, ts));
                    sTile.setPosition(j * ts, i * ts);
                    window.draw(sTile);
                }

            // Draw players
            sTile.setTextureRect(IntRect(36, 0, ts, ts));
            sTile.setPosition(x1 * ts, y1 * ts);
            window.draw(sTile);

            if (!menu.isSinglePlayerSelected) {
                sTile.setTextureRect(IntRect(36, 0, ts, ts));
                sTile.setPosition(x2 * ts, y2 * ts);
                window.draw(sTile);
            }

            // Draw enemies
            sEnemy.rotate(10);
            for (int i = 0; i < enemyCount; i++) {
                sEnemy.setPosition(a[i].x, a[i].y);
                window.draw(sEnemy);
            }

            // Make sure player names are properly set before drawing scores
            string p1Name = menu.isSinglePlayerSelected ?
                username :
                menu.getValidPlayerName(menu.matchedPlayer1, username);
            string p2Name = menu.isSinglePlayerSelected ?
                "" :
                menu.getValidPlayerName(menu.matchedPlayer2, "Player2");

            // Update player names in score manager
            scoreManager.setPlayerUsernames(p1Name, p2Name);

            // Draw scores
            scoreManager.drawScores(window, menu.isSinglePlayerSelected);


            window.display();
        }
    }

    return 0;
}