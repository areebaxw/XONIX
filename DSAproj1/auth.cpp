#include "auth.h"
#include <string>
#include <iostream>

using namespace std;


bool hasSpecialCharacter(const string& password) {
const string specialChars = "!@#$%^&*()_+-=[]{}|;:,.<>?";

for (char c : password) 
{
    if (specialChars.find(c) !=string::npos) {
        return true;
    }
}
return false;
}

Authentication::Authentication() : currentUser(""), isLoginMode(true), userListHead(nullptr) {

if (!font.loadFromFile("arial.ttf"))
{
   cout<< "Error loading font" <<endl;
}


loadUsers();


titleText.setFont(font);
titleText.setString("Xonix Game - Authentication");
titleText.setCharacterSize(24);
titleText.setFillColor(sf::Color::White);
titleText.setPosition(100, 50);

// Setup username text
usernameText.setFont(font);
usernameText.setString("Username:");
usernameText.setCharacterSize(20);
usernameText.setFillColor(sf::Color::White);
usernameText.setPosition(100, 150);

// Setup password text
passwordText.setFont(font);
passwordText.setString("Password:");
passwordText.setCharacterSize(20);
passwordText.setFillColor(sf::Color::White);
passwordText.setPosition(100, 250);

// Setup mode text
modeText.setFont(font);
modeText.setString("Press S to switch between Login and Signup");
modeText.setCharacterSize(16);
modeText.setFillColor(sf::Color::White);
modeText.setPosition(100, 400);

// Setup message text
messageText.setFont(font);
messageText.setCharacterSize(18);
messageText.setFillColor(sf::Color::Red);
messageText.setPosition(100, 350);

// Setup input boxes
usernameBox.setSize(sf::Vector2f(300, 40));
usernameBox.setFillColor(sf::Color(100, 100, 100));
usernameBox.setPosition(250, 140);

passwordBox.setSize(sf::Vector2f(300, 40));
passwordBox.setFillColor(sf::Color(100, 100, 100));
passwordBox.setPosition(250, 240);
}

Authentication::~Authentication() {
// Clear the linked list
clearUserList();
}

void Authentication::loadUsers() {
// Clear existing list first
clearUserList();

// Open file and read users
ifstream file("users.txt");
if (!file.is_open()) return;

string line;
while (getline(file, line)) {
    size_t colonPos = line.find(':');
    if (colonPos !=string::npos) {
       string username = line.substr(0, colonPos);
       string password = line.substr(colonPos + 1);
        addUser(username, password);
    }
}
file.close();
}

void Authentication::saveUsers() {
// Open file for writing
ofstream file("users.txt");
if (!file.is_open()) return;

// Traverse linked list and write users
UserNode* current = userListHead;
while (current != nullptr) {
    file << current->username << ":" << current->password <<endl;
    current = current->next;
}
file.close();
}

void Authentication::addUser(const string& username, const string& password) {
// Create new node
UserNode* newNode = new UserNode(username, password);

// If list is empty, set as head
if (userListHead == nullptr) {
    userListHead = newNode;
    return;
}

// Traverse to end of list
UserNode* current = userListHead;
while (current->next != nullptr) {
    current = current->next;
}

// Add new node at end
current->next = newNode;
}

bool Authentication::findUser(const string& username) {
UserNode* current = userListHead;
while (current != nullptr) {
    if (current->username == username) {
        return true;
    }
    current = current->next;
}
return false;
}

bool Authentication::checkPassword(const string& username, const string& password) {
UserNode* current = userListHead;
while (current != nullptr) {
    if (current->username == username && current->password == password) {
        return true;
    }
    current = current->next;
}
return false;
}

void Authentication::clearUserList() {
while (userListHead != nullptr) {
    UserNode* temp = userListHead;
    userListHead = userListHead->next;
    delete temp;
}
}

// ... (rest of the implementation remains the same as in the previous version)
bool Authentication::showAuthScreen(sf::RenderWindow& window) {
string username, password;
sf::Text inputUsername(username, font, 20);
sf::Text inputPassword(password, font, 20);
inputUsername.setPosition(260, 150);
inputPassword.setPosition(260, 250);
inputPassword.setFillColor(sf::Color::White);

bool isUsernameSelected = true;
messageText.setString("");

while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return false;
        }

        // Handle key presses for input
        if (event.type == sf::Event::KeyPressed) {
            // Backspace handling
            if (event.key.code == sf::Keyboard::BackSpace) {
                if (isUsernameSelected) {
                    if (!username.empty()) {
                        username.pop_back();
                        inputUsername.setString(username);
                    }
                }
                else {
                    if (!password.empty()) {
                        password.pop_back();
                        inputPassword.setString(string(password.length(), '*'));
                    }
                }
            }

            // Switch between username and password input with Enter
            if (event.key.code == sf::Keyboard::Return) {
                if (isUsernameSelected) {
                    // If on username, move to password
                    if (!username.empty()) {
                        isUsernameSelected = false;
                    }
                }
                else {
                    // If on password, submit credentials
                    if (!username.empty() && !password.empty()) {
                        if (isLoginMode) {
                            // Login mode
                            if (!findUser(username)) {
                                messageText.setString("User does not exist. Switch to Signup.");
                            }
                            else if (checkPassword(username, password)) {
                                currentUser = username;
                                return true;
                            }
                            else {
                                messageText.setString("Incorrect password");
                            }
                        }
                        else {
                            // Signup mode
                            // Check for special character in password
                            if (!hasSpecialCharacter(password)) {
                                messageText.setString("Password must contain a special character");
                            }
                            else if (findUser(username)) {
                                messageText.setString("Username already exists");
                            }
                            else {
                                addUser(username, password);
                                saveUsers(); // Save to file
                                currentUser = username;
                                return true;
                            }
                        }
                    }
                    else {
                        messageText.setString("Username and password cannot be empty");
                    }
                }
            }

            // Toggle between Login and Signup modes
            if (event.key.code == sf::Keyboard::S) {
                isLoginMode = !isLoginMode;
                messageText.setString(isLoginMode ? "Login Mode" : "Signup Mode");
            }
        }

        // Handle text input for printable characters
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode < 128) {
                char inputChar = static_cast<char>(event.text.unicode);
                if (inputChar >= 32 && inputChar < 127) {  // Printable characters
                    if (isUsernameSelected) {
                        if (username.length() < 20) {
                            username += inputChar;
                            inputUsername.setString(username);
                        }
                    }
                    else {
                        if (password.length() < 20) {
                            password += inputChar;
                            inputPassword.setString(string(password.length(), '*'));
                        }
                    }
                }
            }
        }

        // Handle mouse clicks for input selection
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (usernameBox.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                isUsernameSelected = true;
            }
            if (passwordBox.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                isUsernameSelected = false;
            }
        }
    }

    // Clear the window
    window.clear(sf::Color(50, 50, 50));

    // Draw all elements
    window.draw(titleText);
    window.draw(usernameText);
    window.draw(passwordText);
    window.draw(usernameBox);
    window.draw(passwordBox);
    window.draw(inputUsername);
    window.draw(inputPassword);
    window.draw(messageText);
    window.draw(modeText);

    // Display the window
    window.display();
}

return false;
}

string Authentication::getCurrentUsername() const {
    return currentUser;

}


//FRIENDLIST 
// Add this method to the Authentication class implementation
void Authentication::getValidUsers(std::string* usernames, int& count, const std::string& currentUsername) {
    count = 0;
    UserNode* current = userListHead;
    while (current != nullptr) {
        if (current->username != currentUsername) {
            usernames[count] = current->username;
            count++;
        }
        current = current->next;
    }
}