#include "auth.h"
#include <string>
#include <iostream>

using namespace std;

bool hasSpecialCharacter(const string& password)
{
    const string specialChars = "!@#$%^&*()_+-=[]{}|;:,.<>?";
    for (char c : password)
    {
        for (char specialChar : specialChars)
        {
            if (c == specialChar)
            {
                return true;
            }
        }
    }
    return false;
}

Authentication::Authentication() : currentUser(""), isLoginMode(true), userListHead(NULL) {

if (!font.loadFromFile("Courier Prime Bold.ttf"))
{
   cout<< "Error loading font" <<endl;
}


loadUsers();


titleText.setFont(font);
titleText.setString("Xonix");
titleText.setCharacterSize(35);
titleText.setFillColor(Color(189, 23, 205));
titleText.setPosition(245, 65);


usernameText.setFont(font);
usernameText.setString("Username:");
usernameText.setCharacterSize(23);
usernameText.setFillColor(Color(189, 23, 205)); 
usernameText.setPosition(105, 140);


passwordText.setFont(font);
passwordText.setString("Password:");
passwordText.setCharacterSize(23);
passwordText.setFillColor(Color(189, 23, 205));//(a shade of purple)
passwordText.setPosition(105, 245);


modeText.setFont(font);
modeText.setString("Press S to switch between Login and Signup");
modeText.setCharacterSize(18);
modeText.setFillColor(Color(15, 133, 155));
modeText.setPosition(80, 375);


ourtext.setFont(font);
ourtext.setString("By Humna & Areeba");
ourtext.setCharacterSize(15);
ourtext.setFillColor(Color(189, 23, 205));
ourtext.setPosition(240, 400);



messageText.setFont(font);
messageText.setCharacterSize(22);
messageText.setFillColor(Color(229, 122, 75)); // Lighter orange-red shade



messageText.setPosition(100, 350);



usernameBox.setSize(Vector2f(200, 40));
usernameBox.setFillColor(Color::White);
usernameBox.setPosition(245, 140);

passwordBox.setSize(Vector2f(200, 40));
passwordBox.setFillColor(Color::White);
passwordBox.setPosition(245, 240);
}

Authentication::~Authentication()
{

clearUserList();
}

void Authentication::loadUsers() {

    clearUserList();


    ifstream file("users.txt");


    if (!file.is_open())
        return;

    string line;


    while (getline(file, line)) 
    {
        int colonPos = -1;
        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == ':') 
            {
                colonPos = i;
                break;
            }
        }

        if (colonPos != -1)
        {
            string username = line.substr(0, colonPos);
            string password = line.substr(colonPos + 1);
            addUser(username, password);
        }
    }
    file.close();
}

void Authentication::saveUsers() 
{

ofstream file("users.txt");


if (!file.is_open()) 
return;

UserNode* current = userListHead;



while (current != NULL)

{
    file << current->username << ":" << current->password <<endl;
    current = current->next;
}
file.close();
}

void Authentication::addUser(const string& username, const string& password)

{

UserNode* newNode = new UserNode(username, password);


if (userListHead == NULL) {
    userListHead = newNode;
    return;
}

UserNode* current = userListHead;
while (current->next != NULL) {
    current = current->next;
}

// Adding new node at end
current->next = newNode;
}




bool Authentication::findUser(const string& username)
{
UserNode* current = userListHead;
while (current != NULL)
{
    if (current->username == username) 
    {
        return true;
    }
    current = current->next;
}
return false;
}


bool Authentication::checkPassword(const string& username, const string& password) 
{

UserNode* current = userListHead;


while (current != NULL)
{
    if (current->username == username && current->password == password) 
    {
        return true;
    }
    current = current->next;
}
return false;
}

void Authentication::clearUserList()
{
while (userListHead != NULL) 
{
    UserNode* temp = userListHead;

    userListHead = userListHead->next;

    delete temp;
}
}
string dsubstr(const string& str, unsigned int start, unsigned int length) {

    if (start >= str.length())
    {
        return "";  
    }

    unsigned int end = start + length;
    if (end > str.length())
    {
        end = str.length();  
    }

    string result = "";
    for (unsigned int i = start; i < end; ++i) {
        result += str[i];
    }

    return result;
}


bool Authentication::showAuthScreen(RenderWindow& window)
{
    //variables
    string username, password;
    Text inputUsername(username, font, 20);
    Text inputPassword(password, font, 20);
    inputUsername.setPosition(260, 150);
    inputUsername.setFillColor(Color::Black);
    inputPassword.setPosition(260, 250);
    inputPassword.setFillColor(Color::Black);

    bool isUsernameSelected = true;
    //variables




    messageText.setString("");



    while (window.isOpen()) 
    
    {
        Event event;
        while (window.pollEvent(event)) 
        
        {
            if (event.type == Event::Closed)
            {
                window.close();
                return false;
            }

           
            if (event.type == Event::KeyPressed) {
                // Backspace handling
                if (event.key.code == Keyboard::BackSpace) {
                    if (isUsernameSelected) 
                    {
                        if (!username.empty()) 
                        {
                            username = dsubstr(username, 0, username.length() - 1);
                            inputUsername.setString(username);
                        }
                    }
                    else {
                        if (!password.empty())
                        {
                            password = dsubstr(password, 0, password.length() - 1);

                            string maskedPass = "";
                            for (unsigned int i = 0; i < password.length(); ++i) {
                                maskedPass += '*';
                            }
                            inputPassword.setString(maskedPass);

                        }
                    }
                }
                if (event.key.code == Keyboard::Return)
                {
                    if (isUsernameSelected)
                    {

                        if (!username.empty())
                        {
                            isUsernameSelected = false;
                        }
                    }
                    else {

                        if (!username.empty() && !password.empty())

                        {
                            if (isLoginMode)

                            {
                                // Login mode
                                if (!findUser(username))
                                {
                                    messageText.setString("User does not exist. Switch to Signup.");
                                }
                                else if (checkPassword(username, password))
                                {
                                    currentUser = username;
                                    return true;
                                }
                                else
                                {
                                    messageText.setString("Incorrect password");
                                }
                            }
                            else {
                                //----------------------------------- Signup mode------------------------------------------------//
                                if (!hasSpecialCharacter(password))
                                {
                                    messageText.setString("Password must contain a special character");
                                }
                                else if (findUser(username))
                                {
                                    messageText.setString("Username already exists");
                                }
                                else
                                {
                                    addUser(username, password);
                                    saveUsers();
                                    currentUser = username;
                                    return true;
                                }
                            }
                        }
                        else
                        {
                            messageText.setString("Username and password cannot be empty");
                        }
                    }
                }

                // Toggling b/w Login and Signup modes
                if (event.key.code == Keyboard::S)
                {
                    isLoginMode = !isLoginMode;
                    messageText.setString(isLoginMode ? "Login Mode" : "Signup Mode");
                }
            }

            //text input for printable characters
            if (event.type == Event::TextEntered)
            {

                if (event.text.unicode < 128)
                {
                    char inputChar = static_cast<char>(event.text.unicode);

                    if (inputChar >= 32 && inputChar < 127)
                    {
                        if (isUsernameSelected)
                        {
                            if (username.length() < 20)
                            {
                                username += inputChar;
                                inputUsername.setString(username);
                            }
                        }
                        else {
                            if (password.length() < 20)
                            {
                                password += inputChar;
                                inputPassword.setString(string(password.length(), '*'));
                            }
                        }
                    }
                }
            }

            //mouse clicks for input selection
            if (event.type == Event::MouseButtonPressed)
            {
                Vector2i mousePos = Mouse::getPosition(window);

                if (usernameBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    isUsernameSelected = true;
                }

                if (passwordBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    isUsernameSelected = false;
                }
            }
        }

        window.clear();


        Texture texture;

        if (!texture.loadFromFile("images/auth.png"))
        {
            cout << "Error loading texture" << endl;
            return -1;
        }


        Sprite sprite(texture);


        window.draw(sprite);











        window.draw(titleText);
        window.draw(usernameText);
        window.draw(passwordText);
        window.draw(usernameBox);
        window.draw(passwordBox);
        window.draw(inputUsername);
        window.draw(inputPassword);
        window.draw(messageText);
        window.draw(modeText);
        window.draw(ourtext);

        window.display();
    }


return false;
}

string Authentication::getCurrentUsername() const 
{
    return currentUser;

}


//FRIENDLIST functions
void Authentication::getValidUsers(string* usernames, int& count, const string& currentUsername)
{
    count = 0;
    UserNode* current = userListHead;
    while (current != NULL) 
    {
        if (current->username != currentUsername) {
            usernames[count] = current->username;
            count++;
        }
        current = current->next;
    }
}