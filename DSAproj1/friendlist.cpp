#include "friendlist.h"
using namespace std;

FriendList::FriendNode::FriendNode(const string& username) : friendUsername(username), next(NULL) {}

FriendList::FriendRequestNode::FriendRequestNode(const string& username) : requestUsername(username), next(NULL) {}

FriendList::FriendList() : friendsHead(NULL), friendRequestsHead(NULL) {}

FriendList::~FriendList()
{
    while (friendsHead != NULL)
    {
        FriendNode* temp = friendsHead;
        friendsHead = friendsHead->next;
        delete temp;
    }
    while (friendRequestsHead != NULL)
    {
        FriendRequestNode* temp = friendRequestsHead;
        friendRequestsHead = friendRequestsHead->next;
        delete temp;
    }
}

void FriendList::addFriend(const string& username)
{
    if (hasFriend(username))
    {
        return;
    }
    FriendNode* newNode = new FriendNode(username);

    if (friendsHead == NULL)
    {
        friendsHead = newNode;
        return;
    }

    FriendNode* current = friendsHead;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = newNode;
}

void FriendList::removeFriend(const string& username)
{
    if (friendsHead == NULL)
    {
        return;
    }

    if (friendsHead->friendUsername == username)
    {
        FriendNode* temp = friendsHead;
        friendsHead = friendsHead->next;
        delete temp;
        return;
    }

    FriendNode* current = friendsHead;
    while (current->next != NULL)
    {
        if (current->next->friendUsername == username)
        {
            FriendNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
            return;
        }
        current = current->next;
    }
}


bool FriendList::hasFriend(const string& username) const
{
    FriendNode* current = friendsHead;
    while (current != NULL) {
        if (current->friendUsername == username)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

void FriendList::sendFriendRequest(const string& username)
{
    if (friendRequestsHead == NULL)       // duplicates prvention
    {
        friendRequestsHead = new FriendRequestNode(username);
        return;
    }

    FriendRequestNode* current = friendRequestsHead;
    while (current != NULL)
    {
        if (current->requestUsername == username)
        {
            return;
        }
        current = current->next;
    }

    current = friendRequestsHead;     //new requests
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = new FriendRequestNode(username);
}

bool FriendList::acceptFriendRequest(const string& username)
{
    if (friendRequestsHead == NULL) return false;

    if (friendRequestsHead->requestUsername == username)
    {
        addFriend(username);

        FriendRequestNode* temp = friendRequestsHead; // removing reqs
        friendRequestsHead = friendRequestsHead->next;
        delete temp;
        return true;
    }

    FriendRequestNode* current = friendRequestsHead;
    while (current->next != NULL)     // traversing to find reqs
    {
        if (current->next->requestUsername == username)
        {
            addFriend(username);
            FriendRequestNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
            return true;
        }
        current = current->next;
    }
    return false;
}


bool FriendList::acceptFriendRequestWithFile(const string& currentUsername, const string& username)
{
    bool accepted = acceptFriendRequest(username); /// accepting

    if (accepted)
    {
        string friendsFilename = currentUsername + "_friends.txt";
        ofstream friendsFile(friendsFilename, ios_base::app);

        if (friendsFile.is_open())
        {
            friendsFile << username <<endl;
            friendsFile.close();
        }

        string requestsFilename = currentUsername + "_friend_requests.txt";
        ifstream inputFile(requestsFilename);
        ofstream tempFile("temp_requests.txt");
        string line;

        while (getline(inputFile, line))
        {
            if (line != username)
            {
                tempFile << line << endl;
            }
        }

        inputFile.close();
        tempFile.close();
        remove(requestsFilename.c_str());
        rename("temp_requests.txt", requestsFilename.c_str());
    }

    return accepted;
}

void FriendList::rejectFriendRequest(const string& username)
{
    if (friendRequestsHead == NULL) return;

    if (friendRequestsHead->requestUsername == username)
    {
        FriendRequestNode* temp = friendRequestsHead;
        friendRequestsHead = friendRequestsHead->next;
        delete temp;
        return;
    }

    FriendRequestNode* current = friendRequestsHead;
    while (current->next != NULL)
    {
        if (current->next->requestUsername == username)
        {
            FriendRequestNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
            return;
        }
        current = current->next;
    }
}

int FriendList::getFriendCount() const
{
    int count = 0;
    FriendNode* current = friendsHead;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}

int FriendList::getFriendRequestCount() const
{
    int count = 0;
    FriendRequestNode* current = friendRequestsHead;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}

void FriendList::getFriendsList(string friendsList[], int& count) const
{
    count = 0;
    FriendNode* current = friendsHead;
    while (current != NULL && count < 100)      /// setting friends max to 100
    {
        friendsList[count] = current->friendUsername;
        count++;
        current = current->next;
    }
}

void FriendList::getFriendRequestsList(string requestsList[], int& count) const
{
    count = 0;
    FriendRequestNode* current = friendRequestsHead;
    while (current != NULL && count < 100)           /// setting friends requests max to 100
    {
        requestsList[count] = current->requestUsername;
        count++;
        current = current->next;
    }
}


void FriendList::saveFriendsToFile(const string& username)
{
    string filename = username + "_friends.txt";
    ofstream file(filename);

    if (!file.is_open())
    {
       cout << "Unable to open friends file for " << username << endl;
        return;
    }

    FriendNode* current = friendsHead;
    while (current != NULL)
    {
        file << current->friendUsername << endl;
        current = current->next;
    }

    file.close();
}

void FriendList::saveFriendRequestsToFile(const string& username)
{
    string filename = username + "_friend_requests.txt";
    ofstream file(filename);

    if (!file.is_open())
    {
       cout << "Unable to open friend requests file for " << username << endl;
        return;
    }

    FriendRequestNode* current = friendRequestsHead;
    while (current != NULL)
    {
        file << current->requestUsername << endl;
        current = current->next;
    }

    file.close();
}

void FriendList::loadFriendsFromFile(const string& username)
{
    string filename = username + "_friends.txt";
    ifstream file(filename);

    if (!file.is_open())
    {
       cout << "No friends file found for " << username << endl;
        return;
    }
    while (friendsHead != NULL)
    {
        FriendNode* temp = friendsHead;
        friendsHead = friendsHead->next;
        delete temp;
    }

   string friendUsername;
    while (getline(file, friendUsername))
    {
        addFriend(friendUsername);
    }

    file.close();
}

void FriendList::loadFriendRequestsFromFile(const string& username)
{
    string filename = username + "_friend_requests.txt";
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "No friend requests file found for " << username << endl;
        return;
    }
    while (friendRequestsHead != NULL)
    {
        FriendRequestNode* temp = friendRequestsHead;
        friendRequestsHead = friendRequestsHead->next;
        delete temp;
    }

    string requestUsername;
    while (getline(file, requestUsername))
    {
        sendFriendRequest(requestUsername);
    }

    file.close();
}