#include "player.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <iostream>


using namespace std;
using namespace sf;



PlayerProfile::PlayerProfile(const string& playerUsername)
{
username = playerUsername;
friendCount = 0;
matchHistoryCount = 0;
highestScore = 0;
loadProfileData();
}

void PlayerProfile::saveProfileData() 
{
string filename = username + "_profile.txt";
ofstream outFile(filename);
if (!outFile)
{
    cout << "Could not open file to save profile: " << filename << endl;
    return;
}


outFile << "Username: " << username << endl;
outFile << "Highest Score: " << highestScore << endl;
outFile << "Friends Count: " << friendCount << endl;



//friends list
for (int i = 0; i < friendCount; i++)
{
    outFile << "Friend " << (i + 1) << ": " << friends[i] << endl;
}

// match history count and details


outFile << "Match History Count: " << matchHistoryCount << endl;
for (int i = 0; i < matchHistoryCount; i++) {
    outFile << "Match " << (i + 1) << ":" << endl;
    outFile << "  Date: " << matchHistory[i].date << endl;
    outFile << "  Game Mode: " << matchHistory[i].gameMode << endl;
    outFile << "  Score: " << matchHistory[i].score << endl;
    outFile << "  Result: " << (matchHistory[i].isWin ? "Win" : "Loss") << endl;
}

ifstream inFile("score.txt");
if (!inFile) 
{
    cout << "Could not open score.txt file" << endl;
    outFile.close();
    return;
}

   
int recentPowerUps = 0;
string recentPowerUpStatus = "None";
string mostRecentGameMode = "Unknown";
int mostRecentBonusLevel = 0;

string line;
string userScorePrefix = "Game Score for " + username;

 
bool found = false;
while (getline(inFile, line)) 
{
       

    if (line.substr(0, userScorePrefix.length()) == userScorePrefix) {
        found = true;
          
        while (getline(inFile, line)) {
              
            if (line.substr(0, 14) == "Game Score for ") {
                break;
            }

               
            if (line.substr(0, 10) == "Game Mode:") 
            {
                mostRecentGameMode = line.substr(11);  
            }
            if (line.substr(0, 12) == "Bonus Level:") 
            {
                //bonus level number 
                mostRecentBonusLevel = extractNumber(line, 13);
            }
            if (line.substr(0, 20) == "Power-Ups Collected:")
            {
                //number of power-ups
                recentPowerUps = extractNumber(line, 21);
            }
            if (line.substr(0, 17) == "Power-Ups Used:") 
            {
                if (line.substr(18) == "Yes") 
                {
                    recentPowerUpStatus = "Used";
                }
                else if (line.substr(18) == "No") 
                {
                    recentPowerUpStatus = "Not Used";
                }
                break;
            }
        }
        break; 
    }
}

 
if (found) {
    outFile << "Most Recent Game Mode: " << mostRecentGameMode << endl;
    outFile << "Most Recent Bonus Level: " << mostRecentBonusLevel << endl;
    outFile << "Most Recent Power-Ups Collected: " << recentPowerUps << endl;
    outFile << "Most Recent Power-Up Status: " << recentPowerUpStatus << endl;
}
else {
    outFile << "No recent match data found." << endl;
}


inFile.close();
outFile.close();
}




//Helper function
int PlayerProfile::extractNumber(const string& str, int startPos) {
int number = 0;
for (int i = startPos; i < str.length(); i++) {
    if (str[i] >= '0' && str[i] <= '9') {
        number = number * 10 + (str[i] - '0');
    }
    else {
        break;
    }
}
return number;
}




void PlayerProfile::loadProfileData()
{
string filename = "score.txt";
ifstream inFile(filename);

// Check if the file can be opened
if (!inFile)
{
    cout << "No existing profile found for " << username << endl;
    return;
}

string line;
int extractedHighestScore = 0;   
int recentPowerUps = 0;           
string recentPowerUpStatus = "None";  
bool foundMostRecentMatch = false;  

while (getline(inFile, line)) {
      

    if (line.find("Game Score for " + username) != string::npos)
    {
        int currentScore = 0;

       
        while (getline(inFile, line)) 
        {
            int scorePos = line.find("Final Score:");


            if (scorePos != string::npos)
            {
                //score taking everything after "Final Score:"
                string scoreStr = line.substr(scorePos + 12);

                currentScore = 0;
                for (int i = 0; i < scoreStr.length(); ++i) 
                {
                    if (scoreStr[i] >= '0' && scoreStr[i] <= '9') 
                    {
                        currentScore = currentScore * 10 + (scoreStr[i] - '0');
                    }
                }

                extractedHighestScore = (currentScore > extractedHighestScore) ? currentScore : extractedHighestScore;
                break;
            }
        }

        //most recent match details
        if (!foundMostRecentMatch) {
            while (getline(inFile, line)) 
            {
                   
                if (line.find("Game Score for ") != string::npos) 
                {
                    break;
                }

                //Power-Ups Collected
                int powerUpPos = line.find("Power-Ups Collected:");
                if (powerUpPos != string::npos) 
                {
                    //power-ups count
                    recentPowerUps = 0;
                    for (int i = powerUpPos + 20; i < line.length(); ++i)
                    {
                        if (line[i] >= '0' && line[i] <= '9') 
                        {
                            recentPowerUps = recentPowerUps * 10 + (line[i] - '0');
                        }
                    }
                }

                //Power-Ups Used
                int powerUpUsedPos = line.find("Power-Ups Used:");
                if (powerUpUsedPos != string::npos)
                {
                    if (line.find("Power-Ups Used: Yes") != string::npos)
                    {
                        recentPowerUpStatus = "Used";
                    }
                    else if (line.find("Power-Ups Used: No") != string::npos) {
                        recentPowerUpStatus = "Not Used";
                    }
                    break;
                }
            }

            foundMostRecentMatch = true;
        }
    }
}


// Updating profile with highest score 
if (extractedHighestScore > highestScore) {
    highestScore = extractedHighestScore;
    saveProfileData();
}

  
cout << "Loaded Profile Data for " << username << endl;
cout << "Highest Score: " << highestScore << endl;
cout << "Most Recent Power-Ups: " << recentPowerUps << endl;
cout << "Recent Power-Up Status: " << recentPowerUpStatus << endl;

inFile.close();
}


string PlayerProfile::getUsername() const {
return username;
}

int PlayerProfile::getHighestScore() const {
return highestScore;
}

int PlayerProfile::getFriendCount() const {
return friendCount;
}

int PlayerProfile::getMatchHistoryCount() const
{
return matchHistoryCount;
}

void PlayerProfile::getFriendsList(string friendsList[]) const 
{
// Copying friends list to the provided array
for (int i = 0; i < friendCount; i++) 
{
    friendsList[i] = friends[i];
}
}

void PlayerProfile::getMatchHistory(MatchRecord matchList[]) const
{
// Copying match history to the provided array
for (int i = 0; i < matchHistoryCount; i++) 
{
    matchList[i] = matchHistory[i];
}
}

bool PlayerProfile::addFriend(const string& friendUsername) 
{
// Checking if friends list is full
if (friendCount >= MAX_FRIENDS) {
    return false;
}

//duplicates check
for (int i = 0; i < friendCount; i++) {
    if (friends[i] == friendUsername) {
        return false; 
    }
}

// Adding new friend
friends[friendCount] = friendUsername;
friendCount++;

 
saveProfileData();
return true;
}

bool PlayerProfile::removeFriend(const string& friendUsername)
{
for (int i = 0; i < friendCount; i++) 
{
    if (friends[i] == friendUsername)
    {
    //shifting
        for (int j = i; j < friendCount - 1; j++) 
        {
            friends[j] = friends[j + 1];
        }
        friendCount--;

        
        saveProfileData();
        return true;
    }
}
return false;
}

void PlayerProfile::recordMatch(const MatchRecord& match) 
{
// Shifting match history if full
if (matchHistoryCount >= MAX_MATCH_HISTORY)
{ // Remove oldest match
       
    for (int i = 0; i < MAX_MATCH_HISTORY - 1; i++) 
    {
        matchHistory[i] = matchHistory[i + 1];
    }
    matchHistoryCount--;
}

// Adding new match
matchHistory[matchHistoryCount] = match;
matchHistoryCount++;

saveProfileData();
}

void PlayerProfile::updateHighestScore(int newScore) 
{
if (newScore > highestScore) {
    highestScore = newScore;

    saveProfileData();
}
}

void PlayerProfile::displayProfile(RenderWindow& window, MySprite& bgPlayer)
{



if (!font.loadFromFile("arial.ttf")) 

{
    return;
}

//variables
int extractedHighestScore = 0;
int mostRecentPowerUps = 0;
string mostRecentPowerUpStatus = "None";
string mostRecentGameMode = "Unknown";
int mostRecentBonusLevel = 0;

string extractedFriends[MAX_FRIENDS];
int extractedFriendCount = 0;

string friendsFilename = username + "_friends.txt";
ifstream friendsFile(friendsFilename);

//variables


if (friendsFile.is_open()) 
{
    string friendName;
    while (getline(friendsFile, friendName) && extractedFriendCount < MAX_FRIENDS) {
        extractedFriends[extractedFriendCount] = friendName;
        extractedFriendCount++;
    }
    friendsFile.close();

    // Updating friendCount
    friendCount = extractedFriendCount;
}

ifstream scoreFile("score.txt");

if (scoreFile.is_open())
{
    string line;
    string gameScoreLinePrefix = "Game Score for " + username;

    scoreFile.seekg(0, ios::beg);

       
    while (getline(scoreFile, line))
    {
        if (line.find(gameScoreLinePrefix) != string::npos) {
            string currentLine;
            int currentMatchScore = 0;

           
            while (getline(scoreFile, currentLine) &&
                currentLine.find("--- Game Score for ") == string::npos)
            {
              
                if (currentLine.find("Final Score:") != string::npos) 
{
                    int scorePos = currentLine.find("Final Score:");

                    string scoreStr = currentLine.substr(scorePos + 12);

                    currentMatchScore = stoi(scoreStr);
                        
                    //highest score need
                    extractedHighestScore = max(extractedHighestScore, currentMatchScore);
                }

                //(only for the last match)
                if (currentLine.find("Game Mode:") != string::npos)
                {
                    int modePos = currentLine.find("Game Mode:");
                    mostRecentGameMode = currentLine.substr(modePos + 11);
                }

                //Bonus Level
                if (currentLine.find("Bonus Level:") != string::npos)
                {
                    int bonusPos = currentLine.find("Bonus Level:");

                    mostRecentBonusLevel = stoi(currentLine.substr(bonusPos + 12));
                }

                //Power-Ups Collected
                if (currentLine.find("Power-Ups Collected:") != string::npos)
                {
                    int powerUpPos = currentLine.find("Power-Ups Collected:");

                    mostRecentPowerUps = stoi(currentLine.substr(powerUpPos + 20));
                }

                //Power-Ups Used Status
                if (currentLine.find("Power-Ups Used:") != string::npos)
                    
                {
                    if (currentLine.find("Power-Ups Used: Yes") != string::npos) 
                    {
                        mostRecentPowerUpStatus = "Used";
                    }
                    else if (currentLine.find("Power-Ups Used: No") != string::npos) 
                    {
                        mostRecentPowerUpStatus = "Not Used";
                    }
                    break;
                }
            }
        }
    }
    scoreFile.close();
}
else {
    return;
}



// Updating highest score if the extracted score is higher
if (extractedHighestScore > highestScore) {
    highestScore = extractedHighestScore;
    saveProfileData();
}



window.clear(Color::Black);




Texture texture;
if (!texture.loadFromFile("images/plyr.jpg")) { 
    cout << "Error loading texture" << endl;
    
}


Sprite sprite(texture);


window.draw(sprite);







//for profile information
Text titleText;
titleText.setFont(font);
titleText.setString("Player Profile");
titleText.setCharacterSize(40);
titleText.setFillColor(Color::White);
titleText.setPosition(350, 50);

// Username
Text usernameText;
usernameText.setFont(font);
usernameText.setString("Username: " + username);
usernameText.setCharacterSize(30);
usernameText.setFillColor(Color::White);
usernameText.setPosition(350, 150);

// Highest Score
Text scoreText;
scoreText.setFont(font);
scoreText.setString("Highest Score: " + to_string(highestScore));
scoreText.setCharacterSize(30);
scoreText.setFillColor(Color::White);
scoreText.setPosition(350, 200);

  
Text matchDetailsText;
/* matchDetailsText.setFont(font);
matchDetailsText.setString("Most Recent Match Details:\n" +
    string("Game Mode: ") + mostRecentGameMode + "\n" +
    "Bonus Level: " + to_string(mostRecentBonusLevel)
);
matchDetailsText.setCharacterSize(30);
matchDetailsText.setFillColor(Color::White);
matchDetailsText.setPosition(50, 250);*/

// Power-Up Information

Text powerUpText;
powerUpText.setFont(font);
powerUpText.setString("Power-Ups: " + to_string(mostRecentPowerUps));

powerUpText.setCharacterSize(30);
powerUpText.setFillColor(Color::White);
powerUpText.setPosition(350, 250);

// Friends List
Text friendsText;


string friendsStr = "Friends (" + to_string(extractedFriendCount) + "): ";

for (int i = 0; i < extractedFriendCount; i++)
{
    friendsStr += extractedFriends[i];
    if (i < extractedFriendCount - 1) friendsStr += ", ";
}

//when no friends exist
if (extractedFriendCount == 0) 
{
    friendsStr += "No friends yet";
}

friendsText.setFont(font);
friendsText.setString(friendsStr);
friendsText.setCharacterSize(30);
friendsText.setFillColor(Color::White);
friendsText.setPosition(280, 300);

//// Match History
//Text matchHistoryText;
//string matchHistoryStr = "Recent Matches (" + to_string(matchHistoryCount) + "):\n";
//for (int i = 0; i < matchHistoryCount; i++) {
//    matchHistoryStr += matchHistory[i].date +
//        " - Score: " + (matchHistory[i].score == 0 ? "0" : to_string(matchHistory[i].score)) +
//        " - Mode: " + matchHistory[i].gameMode +
//        " - " + (matchHistory[i].isWin ? "Win" : "Loss") + "\n";
//}
//matchHistoryText.setFont(font);
//matchHistoryText.setString(matchHistoryStr);
//matchHistoryText.setCharacterSize(25);
//matchHistoryText.setFillColor(Color::White);
//matchHistoryText.setPosition(50, 400);





// Drawing all texts

bgPlayer.Draw(window); 
window.draw(titleText);
window.draw(usernameText);
window.draw(scoreText);
window.draw(matchDetailsText);
window.draw(powerUpText);
window.draw(friendsText);
/* window.draw(matchHistoryText);*/

window.display();
}

bool PlayerProfile::handleProfileInput(Event& event)
{
 
if (event.type == Event::KeyPressed)
{
    if (event.key.code == Keyboard::Escape) 
    {
      
        return false;
    }
}
return true;
}