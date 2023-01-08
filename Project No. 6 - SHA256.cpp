#include <iostream>
#include <fstream>

const int START_SIZE_MESSAGES = 100;

char* createMessageFromFile(const char* fileName);
int countCharactersInFile(const char* fileName);
void freeSpace(char** messages, const int numberOfMessages);
void addNewMessage(char** messages, int& size, int& messagesCount, const char* fileName);
void increaseArraySize(char** messages, int& size, const int messagesCount);

int main()
{
    int size = START_SIZE_MESSAGES;
    int messagesCount = 0;
    char** messages = new char* [START_SIZE_MESSAGES];
    addNewMessage(messages, size, messagesCount, "message1.txt");
    std::cout << messages[0];

    freeSpace(messages, messagesCount);
}

//This function will create a char array and return a pointer to it
char* createMessageFromFile(const char* fileName) {
    int arrayLength = countCharactersInFile(fileName);
    char* message = new char[arrayLength + 1];

    std::ifstream file(fileName);
    if (!file) {
        std::cout << "Error reading from file";
        return nullptr;
    }
    char ch;
    int i = 0;
    while (file >> std::noskipws >> ch) {
        message[i] = ch;
        i++;
    }
    message[arrayLength] = '\0';
    file.close();
    return message;
}

int countCharactersInFile(const char* fileName) {
    std::ifstream file(fileName);
    if (!file) {
        std::cout << "Error reading from file";
        return -1;
    }

    char ch;
    int ctr = 0;
    while (file >> std::noskipws >> ch) {
        ctr++;
    }
    file.close();
    //std::cout << "Number of characters: " << ctr << "\n";
    return ctr;
}

void freeSpace(char** messages, const int numberOfMessages) {
    for (int i = 0; i < numberOfMessages; i++) {
        delete[] messages[i];
        messages[i] = nullptr;
    }
    delete[] messages;
    messages = nullptr;
}

void addNewMessage(char** messages, int& size, int& messagesCount, const char* fileName){
    //Before adding a new message, we should first make sure that there is enough space in the messages array.
    //If not, then we must increase the size of the array by START_SIZE_MESSAGES
    if (messagesCount >= size) {
        increaseArraySize(messages, size, messagesCount);
    }
    //Now, we can freely add the new message to the array
    messages[messagesCount++] = createMessageFromFile(fileName);
}

void increaseArraySize(char** messages, int& size, const int messagesCount) {
    //There is no use of deleting the already existing messages. 
    //We can simply create a new, bigger array of pointers to the messages
    char** newMessagesArray = new char* [size + START_SIZE_MESSAGES];

    //Point to the already existing strings 
    for (int i = 0; i < messagesCount; i++) {
        newMessagesArray[i] = messages[i];
    }

    //Delete the old array of pointers pointing to the strings
    delete[] messages;

    //When deleting the contents of messages, we aren't actually deleting the pointer called 'messages'
    //We want to point it to the new array
    messages = newMessagesArray;

    //Update the size 
    size += START_SIZE_MESSAGES;
}
