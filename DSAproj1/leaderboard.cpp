

#include "leaderboard.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <string>

using namespace std;

//note that we used substr and find with 
// proper knowledge that 
// it helps in extracting specific parts of a string


const int MAX_PLAYERS = 10;  

//global
string playerNames[MAX_PLAYERS];
int playerScores[MAX_PLAYERS];
int playerCount = 0;  
// global


//functions start

void Leaderboard::saveLeaderboardFromScoreFile(const string& scoreFilePath, const string& leaderboardFilePath) 
{
    ifstream scoreFile(scoreFilePath);
    ofstream leaderboardFile(leaderboardFilePath);

    if (!scoreFile.is_open())
    {
        cout << "Error: Could not open score file for reading." << endl;
        return;
    }
    if (!leaderboardFile.is_open()) 
    {
        cout << "Error: Could not open leaderboard file for writing." << endl;
        return;
    }

    //variables
    string line;
    string playerName;
    int playerScore = 0;
    bool isSinglePlayer = false;
    //variables




    
    while (getline(scoreFile, line)) 
    {
        
        if (line.find("Game Mode: Single Player") != -1)
        {
            isSinglePlayer = true;
        }

        if (line.find("Game Score for ") != -1)
        {
            playerName = line.substr(15);
        }


  
        if (line.find("Final Score:") != -1 && isSinglePlayer) 
        {
            string temp = line.substr(13);

            playerScore = 0;
            for (char c : temp) 
            {
                if (isdigit(c)) 
                {
                    playerScore = playerScore * 10 + (c - '0'); 
                }
            }

           
            bool playerFound = false;
            for (int i = 0; i < playerCount; i++) 
            {
                if (playerNames[i] == playerName) 
                {
                   
                    if (playerScore > playerScores[i]) 
                    {
                        playerScores[i] = playerScore; 
                    }
                    playerFound = true;
                    break;
                }
            }

            
            if (!playerFound && playerCount < MAX_PLAYERS)
            {
                playerNames[playerCount] = playerName;
                playerScores[playerCount] = playerScore;
                playerCount++;
            }

           
            isSinglePlayer = false;
        }
    }

  
    for (int i = 0; i < playerCount; i++)
    {
        leaderboardFile << playerNames[i] << "," << playerScores[i] << endl;
    }

    scoreFile.close();
    leaderboardFile.close();

    cout << "Leaderboard saved to " << leaderboardFilePath << " successfully." << endl;
}


Leaderboard::Leaderboard() : heapSize(0) 
{
    for (int i = 0; i < HEAP_SIZE; i++) 
    {
        heap[i].username = "";
        heap[i].score = 0;
    }
}

void Leaderboard::setDisplayMode(int mode)
{
    currentDisplayMode = mode;
}

int Leaderboard::getDisplayMode() const {
    return currentDisplayMode;
}


void Leaderboard::insertPlayer(const string& username, int score) 
{
  
    for (int i = 0; i < heapSize; i++) 
    {
        if (heap[i].username == username)
        {
           
            if (score > heap[i].score)
            {
                heap[i].score = score;

                int current = i;


                while (current > 0 && heap[(current - 1) / 2].score > heap[current].score)
                {
                    swapPlayers(heap[current], heap[(current - 1) / 2]);
                    current = (current - 1) / 2;
                }
                minHeapify(i);
            }
            return;
        }
    }

    // If player not in heap so inserting new player
    if (heapSize < HEAP_SIZE) 
    {
        heap[heapSize].username = username;
        heap[heapSize].score = score;

        //up the new entry
        int current = heapSize;


        while (current > 0 && heap[(current - 1) / 2].score > heap[current].score) 
        {
            swapPlayers(heap[current], heap[(current - 1) / 2]);
            current = (current - 1) / 2;
        }

        heapSize++;
    }

    
    else if (score > heap[0].score)  // If heap is full and new score is higher than root
    
    {
        // Replacing root with new player
        heap[0].username = username;
        heap[0].score = score;

        // Restoring min heap property
        minHeapify(0);
    }
}

void Leaderboard::updatePlayerScore(const LeaderboardEntry& entry) 
{
   
    for (int i = 0; i < heapSize; i++)
    {
        
        if (entry.isMultiplayer) // For multiplayer, checking individual players
        {  
            //for player 1
            if (heap[i].player1Name == entry.player1Name) 
            {
             
                if (entry.player1Score > heap[i].player1Score)     // Updating if new score is higher
                {
                    heap[i].player1Score = entry.player1Score;
                    
                    heap[i].score = heap[i].player1Score + heap[i].player2Score;  // Updating total score if needed
                }
            }

            //for Player 2
            if (heap[i].player2Name == entry.player2Name) 
            {
                
                if (entry.player2Score > heap[i].player2Score) 
                {
                    heap[i].player2Score = entry.player2Score;
                
                    heap[i].score = heap[i].player1Score + heap[i].player2Score;
                }
            }
        }
        // For single-player
        else 
        {
            if (heap[i].username == entry.username) 
            {
                
                if (entry.score > heap[i].score) 
                {
                    heap[i] = entry;
                    minHeapify(i);
                }
                return;
            }
        }
    }

   
    if (heapSize < HEAP_SIZE)    // If not found, insert new entry
    {
        heap[heapSize] = entry;

       
        int current = heapSize;
        while (current > 0 && heap[(current - 1) / 2].score > heap[current].score) 

        {
            swapPlayers(heap[current], heap[(current - 1) / 2]);
            current = (current - 1) / 2;
        }

        heapSize++;
    }

    // If heap is full and new score is higher than root
    else if (entry.score > heap[0].score) 
    {
   
        heap[0] = entry;        // Replacing root with new entry
        minHeapify(0);
    }
}

void Leaderboard::recordGameResult(const string& player1, int score1, const string& player2, int score2,
    bool isMultiplayer, int level) {

    string temp1 = player1.empty() ? "Player1" : player1;
    string temp2 = player2.empty() ? "Player2" : player2;

  //variables
    LeaderboardEntry entry;
    entry.player1Name = temp1;
    entry.player1Score = score1;
    entry.player2Name = temp2;  
    entry.player2Score = score2;
    entry.isMultiplayer = isMultiplayer;
    entry.gameLevel = level;

    // variables

    if (isMultiplayer) 
    {
        entry.username = temp1 + " vs " + temp2;
        entry.score = score1 + score2;  //combined score
    }
    else 
    {
        entry.username = temp1;
        entry.score = score1;
    }

 
    updatePlayerScore(entry);

 
    saveDetailedLeaderboard(entry);
}

//not useful as such...
void Leaderboard::saveDetailedLeaderboard(const LeaderboardEntry& entry) 
{
    ofstream detailedFile("detailed_leaderboard.txt", ios::app);

    if (detailedFile.is_open()) {
        detailedFile << entry.username << ","
            << entry.score << ","
            << entry.player1Name << ","
            << entry.player1Score << ","
            << entry.player2Name << ","
            << entry.player2Score << ","
            << (entry.isMultiplayer ? "Multiplayer" : "Single Player") << ","
            << entry.gameLevel
            << endl;
        detailedFile.close();
    }
}










int stringtoint(const string& str) {
    int result = 0;
    bool negative = false;
    int i = 0;

    if (str[i] == '-') {
        negative = true;
        i++;
    }

    for (; i < str.size(); ++i) {
        result = result * 10 + (str[i] - '0');
    }

    if (negative) {
        result = -result;
    }

    return result;
}





int convertStringToInt(const string& str) {
    int result = 0;
    bool isNegative = false;
    int startIndex = 0;

   
    if (str[0] == '-') {
        isNegative = true;
        startIndex = 1;
    }

  
    for (int i = startIndex; i < str.length(); ++i) {
        if (str[i] < '0' || str[i] > '9') {
            break; 
        }
        result = result * 10 + (str[i] - '0');
    }

    if (isNegative) {
        result = -result;
    }

    return result;
}



void Leaderboard::loadLeaderboard() {
   
    heapSize = 0;

 
    ifstream scoreFile("score.txt");

    if (!scoreFile.is_open()) 
    {
        cout << "No score file found." << endl;
        insertPlayer("Player", 100);  
        return;
    }

    string line;
    string currentGameMode = "";
    string playerName = "";
    int playerScore = 0;

    while (getline(scoreFile, line)) {
        
        if (line.empty() || line.find("---") != -1) 
            continue;

    
        if (line.find("Game Mode:") != -1) 
        {
            currentGameMode = line.substr(line.find(":") + 2);

            cout << "Current Game Mode: " << currentGameMode << endl;
            playerName = "";
            playerScore = 0;
            continue;
        }

        
        int namePos = line.find("Game Score for ");

        if (namePos != -1) {


            playerName = line.substr(namePos + 15);
            int spacePos = playerName.find(" ");


            if (spacePos != -1) {
                playerName = playerName.substr(0, spacePos);  

            }
            cout << "Player Name: " << playerName << endl;
        }

       //----------------------------SINGLE PLAYER MODE-----------------------------------//


        if (currentGameMode == "Single Player") 
        {
          int scorePos = line.find("Final Score:");
            if (scorePos != -1) 
            {
                playerScore = stoi(line.substr(scorePos + 12));

                LeaderboardEntry entry;
                entry.username = playerName;
                entry.score = playerScore;
                entry.player1Name = playerName;
                entry.player1Score = playerScore;
                entry.isMultiplayer = false;

                updatePlayerScore(entry); 

               /* cout << "Single Player Entry - Name: " << playerName
                    << ", Score: " << playerScore << endl;*/
            }
        }
        
        //-------------------------------MULTIPLAYER MODE---------------------//


        else if (currentGameMode == "Multiplayer")
        {
           
           int player1ScorePos = line.find("Player 1 Score:");

		   //----------------------------player1 name-----------------------------------//

            if (player1ScorePos != -1) 
            {
               int openParen = line.find("(");

               int player1Score = 0;
               int scoreStartPos = player1ScorePos + 15;
               
               int scoreEndPos;
               if (openParen != -1) {
                   scoreEndPos = openParen;
               }
               else {
                   scoreEndPos = line.length();
               }

               player1Score = stoi(line.substr(scoreStartPos, scoreEndPos - scoreStartPos));

              

          //-----------------------------player2-----------------------------------//


                string player2Line;
                getline(scoreFile, player2Line);

               
                string player2Name = "";
                int player2Score = 0;


				//----------------------------player2 name-----------------------------------//

               int player2NamePos = player2Line.find("Game Score for ");
                if (player2NamePos != -1) 
                {
                    player2Name = player2Line.substr(player2NamePos + 15);
                    int spacePos = player2Name.find(" ");

                    if (spacePos != -1) 
                    {
                        player2Name = player2Name.substr(0, spacePos);  
                    }
                }

                
				//----------------------------player2 score-----------------------------------//

               int player2ScorePos = player2Line.find("Player 2 Score:");
                if (player2ScorePos != -1) 
                {
                   int openParen = player2Line.find("(");

                   int player2Score = 0;
                   int scoreStartPos = player2ScorePos + 15;
                   int scoreEndPos;

                   if (openParen != -1) {
                       scoreEndPos = openParen;
                   }
                   else {
                       scoreEndPos = player2Line.length();
                   }

                   player2Score = stoi(player2Line.substr(scoreStartPos, scoreEndPos - scoreStartPos));

                }

             

				//----------------------------MULTIPLAYER ENTRY-----------------------------------//
                LeaderboardEntry entry;
                entry.player1Name = playerName;
                entry.player1Score = player1Score;
                entry.player2Name = player2Name;
                entry.player2Score = player2Score;
                entry.isMultiplayer = true;
                entry.username = playerName + " vs " + player2Name;
                entry.score = player1Score + player2Score;

                updatePlayerScore(entry);  

                cout << "Multiplayer Entry - Player 1: " << playerName
                    << ", Score: " << player1Score
                    << " vs Player 2: " << player2Name
                    << ", Score: " << player2Score << endl;
            }
        }
    }

    scoreFile.close();

   
    if (heapSize == 0) {
        insertPlayer("Player", 100);  
    }

    cout << "Total Leaderboard Entries: " << heapSize << endl;
}

void Leaderboard::sortForDisplay(LeaderboardEntry* sortedArray) 
{

  // variables
    LeaderboardEntry singlePlayerEntries[HEAP_SIZE];
    LeaderboardEntry multiplayerEntries[HEAP_SIZE];
    int singlePlayerCount = 0;
    int multiplayerCount = 0;
	// variables


    for (int i = 0; i < heapSize; i++) 
    {
        if (heap[i].isMultiplayer) 
        {
            multiplayerEntries[multiplayerCount++] = heap[i];
        }
        else 
        {
            singlePlayerEntries[singlePlayerCount++] = heap[i];
        }
    }

    // sorting for single-player entries (descending order by player1Score)
    for (int i = 0; i < singlePlayerCount - 1; i++) 
    {
        for (int j = 0; j < singlePlayerCount - i - 1; j++) {
            if (singlePlayerEntries[j].player1Score < singlePlayerEntries[j + 1].player1Score) {
                LeaderboardEntry temp = singlePlayerEntries[j];
                singlePlayerEntries[j] = singlePlayerEntries[j + 1];
                singlePlayerEntries[j + 1] = temp;
            }
        }
    }

    // sorting logic for multiplayer leaderboard entries (bubble sort)

    for (int i = 0; i < multiplayerCount - 1; i++) 
    {
        for (int j = 0; j < multiplayerCount - i - 1; j++) 
        {
            int currentMatchScore = multiplayerEntries[j].player1Score + multiplayerEntries[j].player2Score;
            int nextMatchScore = multiplayerEntries[j + 1].player1Score + multiplayerEntries[j + 1].player2Score;

            if (currentMatchScore < nextMatchScore) 
            {
                LeaderboardEntry temp = multiplayerEntries[j];
                multiplayerEntries[j] = multiplayerEntries[j + 1];
                multiplayerEntries[j + 1] = temp;
            }
        }
    }

    
    LeaderboardEntry* selectedEntries;
    int selectedCount;

    if (currentDisplayMode == 0)  // Single Player Mode
    { 
        selectedEntries = singlePlayerEntries;
        selectedCount = singlePlayerCount;
    }
    else  // Multiplayer Mode
    { 
        selectedEntries = multiplayerEntries;
        selectedCount = multiplayerCount;
    }

    // Copying
    for (int i = 0; i < selectedCount; i++)
    
    {
        sortedArray[i] = selectedEntries[i];
    }

   
    for (int i = selectedCount; i < HEAP_SIZE; i++) 
    {
        sortedArray[i] = LeaderboardEntry(); 
    }
}



void Leaderboard::swapPlayers(LeaderboardEntry& a, LeaderboardEntry& b) 
{
    LeaderboardEntry temp = a;
    a = b;
    b = temp;
}


void Leaderboard::minHeapify(int index) 
{
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heapSize && heap[left].score < heap[smallest].score) 
    {
        smallest = left;
    }

    if (right < heapSize && heap[right].score < heap[smallest].score) 
    {
        smallest = right;
    }

    if (smallest != index) 
    {
        swapPlayers(heap[index], heap[smallest]);
        minHeapify(smallest);
    }
}


// for match making logic
string Leaderboard::getTopScorer()
{
    if (heapSize > 0) 
    {
        
        return heap[0].username;  
    }
    return "";  
}

