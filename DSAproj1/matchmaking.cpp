#include "matchmaking.h"
#include <fstream>
#include <iostream>
#include <string>

// Constructor
Matchmaking::Matchmaking() :
    frontIndex(0),
    rearIndex(0),
    currentSize(0),
    nextEntryOrder(0) {}


//---------------------HELPER FUNCTIONS-----------------------------//



void Matchmaking::copyString(char* destination, const string& source) 
{
    int maxLength = MAX_USERNAME_LENGTH - 1;
    int i = 0;
    while (i < source.length() && i < maxLength) 
    {
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0';
}


int Matchmaking::stringToInt(const string& str) 
{
    int result = 0;
    int sign = 1;
    int i = 0;

   
    if (!str.empty() && str[0] == '-') {
        sign = -1;
        i++;
    }


    while (i < str.length() && str[i] >= '0' && str[i] <= '9') {
       
        if (result > (2147483647 - (str[i] - '0')) / 10) 
        {
            
            return sign > 0 ? 2147483647 : -2147483648;
        }
        result = result * 10 + (str[i] - '0');
        i++;
    }

    return result * sign;
}

//--------------------------------------------------------------//


//---------------------------FUNCTIONS-------------------//

int Matchmaking::readUserHighestScore(const string& username) 

{
    string filename = username + "_profile.txt";
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Could not open profile file for " << username << endl;
        return 0;
    }

    string line;
    int highestScore = 0;

    while (getline(file, line)) 
    {
        int pos = line.find("Highest Score:");


        if (pos != -1) 
        {
            string scorestring = line.substr(pos + 14); 

			//------- Remove leading and trailing spaces----//

            int firstNonSpace = -1;

            for (int i = 0; i < scorestring.length(); ++i) 
            {
                if (!isspace(scorestring[i])) 
                {
                    firstNonSpace = i;
                    break;
                }
            }

			

            int lastNonSpace = -1;


            for (int i = scorestring.length() - 1; i >= 0; --i) 
            {
                if (!isspace(scorestring[i])) 
                {
                    lastNonSpace = i;
                    break;
                }
            }

            //-----------------------------------------//

            string temp = "";
            if (firstNonSpace != -1 && lastNonSpace != -1 && firstNonSpace <= lastNonSpace) 
            {
                for (int i = firstNonSpace; i <= lastNonSpace; ++i) 
                {
                    temp += scorestring[i];
                }
            }

            highestScore = 0;



            for (char c : temp) 
            {
                if (c >= '0' && c <= '9') 
                {
                    highestScore = highestScore * 10 + (c - '0');
                }
                else {
                    highestScore = 0; 
                    break;
                }
            }
            break; 
        }
    }


    file.close();

    


    cout << "Loaded highest score for " << username
        << ": " << highestScore << endl;

    return highestScore;
}

void Matchmaking::sortQueueByPriority() 
{
	//---------------- Sorting players in the queue based on their highest score and entry order---------------//

    for (int i = 0; i < currentSize - 1; i++) 
    {
        for (int j = 0; j < currentSize - i - 1; j++) {
            bool shouldSwap = false;

    

			//-----------------if the current player has a lower score than the next player--------------//
            if (players[j].highestScore < players[j + 1].highestScore) 
            {
                shouldSwap = true;
            }
       
			//-------------------if the current player has the same score as the next player--------------//
			else if (players[j].highestScore == players[j + 1].highestScore &&
				players[j].queueEntryOrder < players[j + 1].queueEntryOrder)
			{
				shouldSwap = true;
			}
          
            if (shouldSwap) 
            {
                MatchPlayer temp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = temp;
            }
        }
    }
}



bool Matchmaking::arePlayersUnique(const MatchPlayer& p1, const MatchPlayer& p2) 
{
   
    for (int i = 0; i < MAX_USERNAME_LENGTH; ++i) 
    {
        if (p1.username[i] != p2.username[i]) 
        {
            return true;
        }

        if (p1.username[i] == '\0') {
            break;
        }
    }
    return false;
}



//--------------------FOR OUR OWN HELP---------------------------//

//void Matchmaking::logMatchmakingEvent(const string& eventDescription) {
//    ofstream logFile("matchmaking_log.txt", ios_base::app);
//    if (logFile.is_open()) {
//        logFile << eventDescription << endl;
//        logFile.close();
//    }
//}


//void Matchmaking::debugPrintQueue() 
//{
//    cout << "\n--- Detailed Queue Debug Information ---" << endl;
//    cout << "Current Queue Size: " << currentSize << endl;
//
//    if (currentSize == 0) {
//        cout << "Queue is empty!" << endl;
//        return;
//    }
//
//    cout << "Queue Contents:" << endl;
//    for (int i = 0; i < currentSize; i++) {
//        cout << "Player " << (i + 1)
//            << ": Username = " << players[i].username
//            << ", Score = " << players[i].highestScore
//            << ", Entry Order = " << players[i].queueEntryOrder
//            << endl;
//    }
//    cout << "--------------------------------------\n" << endl;
//}



//-------------------------------------------------------------//


bool Matchmaking::enterQueue(const string& username, int highestScore) 
{
    if (username.empty() || username.length() >= MAX_USERNAME_LENGTH) {
        cout << "Invalid username: " << username << endl;
        return false;
    }

   
    if (currentSize >= MAX_QUEUE_SIZE) {
        cout << "Matchmaking queue is full" << endl;
        return false;
    }

  
    MatchPlayer newPlayer;
    copyString(newPlayer.username, username);


    newPlayer.highestScore = highestScore;
    newPlayer.queueEntryOrder = nextEntryOrder++;




    // Adding player to queue
    players[rearIndex] = newPlayer;
    rearIndex = (rearIndex + 1) % MAX_QUEUE_SIZE;
    currentSize++;


    sortQueueByPriority();

  
    /*string logMessage = "Player " + username +
        " entered queue with score " + to_string(highestScore);
    logMatchmakingEvent(logMessage);*/

    //// Debug: Print queue state after entry
    //cout << "\nPlayer " << username << " entered the queue:" << endl;
    //debugPrintQueue();

    return true;
}


bool Matchmaking::matchPlayers(string& player1Username, string& player2Username) 
{
    
    if (currentSize < 2) {
        cout << "Not enough players to match. Current queue size: "
            << currentSize << endl;
       // debugPrintQueue();
        return false;
    }

    
    //cout << "Pre-Match Queue State:" << endl;
    //debugPrintQueue();





    MatchPlayer player1 = players[0];


   
    int secondPlayerIndex = -1;

	// ------------------case 1 of equal or higher score--------------------------------//
    for (int i = 1; i < currentSize; i++) 
    {
        if (arePlayersUnique(player1, players[i]) &&
            players[i].highestScore >= player1.highestScore) {
            secondPlayerIndex = i;
            break;
        }
    }

    //----------------------------- case 2 the closest lower score-----------------------------//
    if (secondPlayerIndex == -1) 
    {
        int closestLowerScoreIndex = -1;

        int closestLowerScoreDiff = 1000000;  

        for (int i = 1; i < currentSize; i++) 
        {
            if (arePlayersUnique(player1, players[i])) 
            {
                int scoreDiff = player1.highestScore - players[i].highestScore;

               
                if (scoreDiff > 0 && scoreDiff < closestLowerScoreDiff)
                {
                    closestLowerScoreIndex = i;
                    closestLowerScoreDiff = scoreDiff;
                }
            }
        }

        secondPlayerIndex = closestLowerScoreIndex;
    }

    
    if (secondPlayerIndex == -1) {
        //cout << "No suitable match found for player: "
        //    << player1.username << endl;
        ////debugPrintQueue();
        return false;
    }

   



    player1Username = string(player1.username);
    player2Username = string(players[secondPlayerIndex].username);


    //// Display match details
    //cout << "Matchmaking: Player 1 (" << player1Username
    //    << ", Score: " << player1.highestScore
    //    << ") vs Player 2 (" << player2Username
    //    << ", Score: " << players[secondPlayerIndex].highestScore << ")" << endl;




    // Dequeue the matched players
    // Remove first player
    for (int i = 0; i < currentSize - 1; i++) {
        players[i] = players[i + 1];
    }
    currentSize--;

    //------------------ Remove second player------------------------------//
    for (int i = 0; i < currentSize; i++) 
    {
        if (string(players[i].username) == player2Username) 
        {
            for (int j = i; j < currentSize - 1; j++) 
            {
                players[j] = players[j + 1];
            }
            currentSize--;
            break;
        }
    }

    
    sortQueueByPriority();


    //cout << "Post-Match Queue State:" << endl;
    //debugPrintQueue();

    return true;
}



int Matchmaking::getQueueSize() const 
{
    return currentSize;
}


string Matchmaking::getTopPlayerFromLeaderboard() 
{
    string topPlayer = "";
    int highestScore = -1;

 
    ifstream file("leaderboard.txt");
    if (!file.is_open()) {
        return ""; 
    }

    string line;

    while (getline(file, line)) 
    {
        int commaPos = line.find(',');  


        if (commaPos != -1) 
        {
            string player = line.substr(0, commaPos);  

            string scorestring = line.substr(commaPos + 1);
            
            int score = stringToInt(scorestring);  

           
            if (score > highestScore) 
            {
                highestScore = score;  
                topPlayer = player; 
            }
        }
    }

    file.close(); 
    return topPlayer;  
}

void Matchmaking::populateQueueFromLeaderboard(const string& loggedInUser) 

{
    ifstream file("leaderboard.txt");
    string line;


    
    while (getline(file, line)) 
    {
        int commaPos = line.find(','); 
        if (commaPos != -1) 
        {
            string playerName = line.substr(0, commaPos);
            int playerScore = stringToInt(line.substr(commaPos + 1));

			//----------------------- Skipping the logged-in user-------------------------------//
            if (playerName != loggedInUser) {
                
                enterQueue(playerName, playerScore);
                cout << "Added " << playerName << " with score " << playerScore << " to the queue." << endl;
            }
            else 
{
                cout << "Skipped adding logged-in user " << loggedInUser << " to the queue." << endl;
            }
        }
    }

    file.close();
}

string Matchmaking::getNextTopPlayerFromLeaderboard(const string& topPlayerUsername) {
    ifstream file("leaderboard.txt");

	if (!file.is_open()) {
		cout << "Could not open leaderboard file." << endl;
		return "";
	}


    string line;
    bool foundTopPlayer = false;

    while (getline(file, line)) 
    {
        int commaPos = line.find(',');
        
    
        if (commaPos !=-1) {
            string player = line.substr(0, commaPos);  
            if (foundTopPlayer) 
            {
                return player;  
            }
            if (player == topPlayerUsername) 
            {

                foundTopPlayer = true; 
            }
        }
    }

    file.close(); 
    return "";
}