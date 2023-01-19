/**
*
* Solution to course project # 6
* Introduction to programming course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2022/2023
*
* @author Raya Grigorova
* @idnumber 5MI0600167* @compiler Visual Studio
*
* 
*
*/

#include <iostream>
#include <fstream>

const int START_SIZE = 100;
const int MAX_USER_INPUT_LENGTH = 250;

const int UNSIGNED_INT_BITS = 32;

const int HASH_BITS = 256;
const int HASH_CHARACTERS = 64; 

int messagesSize = START_SIZE;
int messagesCount = 0;

int hashesSize = START_SIZE;
int hashesCount = 0;

char** messages = new char* [START_SIZE];
char** hashMessages = new char* [START_SIZE];

unsigned int h0;
unsigned int h1;
unsigned int h2;
unsigned int h3;
unsigned int h4;
unsigned int h5;
unsigned int h6;
unsigned int h7;

unsigned const int k[64] = {
0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

int inputValidation(const char buff[], const char validInputs[]);
bool stringsAreEqual(const char* str1, const char* str2);

void readMessageFromFile();
char* createMessageFromFile(const char* fileName, const unsigned long long length);
void freeSpace(char** arr, const int size);
void addStringToArray(char* str, int& stringsCount, int& arraySize, char** arr);
void increaseArraySize(char** arr, int& arraySize);
unsigned long long stringLength(const char* str);

bool arrayContainsString(char* str, char** array, const int numberOfElements);

void hashMessage();
char* createHash(char* message);
void initVariables();
void saveHashToFile(char* hash, char* fileName);
bool fileExists(char* fileName);
unsigned int* preProcessing(char* message, int& paddedMessageSize);
void chunkLoop(unsigned int* paddedMessage);
unsigned int rightRotate(const unsigned int n, const unsigned int d);
void initHash(unsigned int* finalHash);
void toHex(unsigned int* finalHash, char result[]);

void readHashedMessage();

void createNewMessageFromUserInput();

int main() {
    char buff[MAX_USER_INPUT_LENGTH];
    char validInputs[] = {'0', '1', '2', '3', '4'};
    bool getUserInput = true;

    while (getUserInput) {
        std::cout << "\nTo exit the program, enter 0\n";
        std::cout << "To read a message from file, enter 1\n";
        std::cout << "To hash a message, enter 2\n";
        std::cout << "To read a hashed message, enter 3\n";
        std::cout << "To type a new message, enter 4\n";
        std::cin.getline(buff, MAX_USER_INPUT_LENGTH);
        int operation = inputValidation(buff, validInputs);
        switch (operation) {
        case 0: getUserInput = false; break;
        case 1: readMessageFromFile(); break;
        case 2: hashMessage(); break;
        case 3: readHashedMessage(); break;
        case 4: createNewMessageFromUserInput(); break;
        default: std::cout << "Invalid user input";
        }
    }
    freeSpace(messages, messagesCount);
    freeSpace(hashMessages, hashesCount);
}

//This function will return the number of the operation to be performed or -1
int inputValidation(const char buff[], const char validInputs[]) {
    if (stringLength(buff) > 1) {
        return -1;
    }
    //Compare each of the valid inputs to buff
    for (int i = 0; validInputs[i] != '\0'; i++) {
        if (buff[0] == validInputs[i]) {
            return validInputs[i] - '0';
        }
    }
    return -1;
}

bool stringsAreEqual(const char* str1, const char* str2) {
    if (str1 == nullptr || str2 == nullptr) {
        return false;
    }
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return 0;
        }
        str1++;
        str2++;
    }
    //If one of the strings is longer than the other, they can't be equal
    //This will only return true if *str1 == *str2 == '\0'
    return (*str1 == *str2);
}

void readMessageFromFile() {
    char buff[MAX_USER_INPUT_LENGTH];
    char fileName[MAX_USER_INPUT_LENGTH];

    std::cout << "Enter a valid file name:\n";
    std::cin.getline(fileName, MAX_USER_INPUT_LENGTH);

    std::ifstream file(fileName);
    if (!file) {
        std::cout << "Error reading from file\n";
        return;
    }

    std::cout << "Data from " << fileName << ":\n";
    char ch;
    unsigned long long ctr = 0;
    while (file >> std::noskipws >> ch) {
        std::cout << ch;
        ctr++;
    }
    std::cout << "\n";

    char validInput[] = { '0', '1' };
    std::cout << "Do you want to save this message? Enter 1 if yes and 0 if no\n";
    std::cin.getline(buff, MAX_USER_INPUT_LENGTH);
    switch (inputValidation(buff, validInput)) {
    case 0: 
        file.close();
        return;
    case 1: createMessageFromFile(fileName, ctr); break;
    defualt: std::cout << "Invalid user input\n";
    }
    file.close();
}

//This function will create a char array and return a pointer to it
char* createMessageFromFile(const char* fileName, const unsigned long long length) {
    char* message = new char[length + 1];

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
    message[length] = '\0';

    file.close();

    addStringToArray(message, messagesCount, messagesSize, messages);
    return message;
}

void freeSpace(char** arr, const int size) {
    for (int i = 0; i < size; i++) {
        delete[] arr[i];
        arr[i] = nullptr;
    }
    delete[] arr;
    arr = nullptr;
}

void addStringToArray(char* str, int& stringsCount, int& arraySize, char** arr) {
    if (arrayContainsString(str, arr, stringsCount)) {
        std::cout << "The data is already saved\n";
        return;
    }
    //Before adding a new message, first make sure that there is enough space in the messages array.
    //If not, then increase the size of the array by START_SIZE_MESSAGES
    if (stringsCount >= arraySize) {
        increaseArraySize(arr, arraySize);
    }
    //Now, add the new message to the array
    arr[stringsCount++] = str;
    std::cout << "Added to the array under index " << stringsCount - 1 << "\n";
}

void increaseArraySize(char** arr, int& arraySize) {
    //Instead of deleting the already existing strings,
    //create a new, bigger array of pointers to them
    char** newArray = new char* [arraySize + START_SIZE];

    //Point to the already existing strings 
    for (int i = 0; i < messagesCount; i++) {
        newArray[i] = arr[i];
    }

    //Delete the old array of pointers pointing to the strings
    delete[] arr;

    //Point arr to the new array
    arr = newArray;

    //Update size 
    arraySize += START_SIZE;
}

unsigned long long stringLength(const char* str){
    if (str == nullptr) {
        return -1;
    }
    unsigned long long i = 0;
    while (*str) {
        str++;
        i++;
    }
    return i;
}

bool arrayContainsString(char* str, char** array, const int numberOfElements) {
    for (int i = 0; i < numberOfElements; i++) {
        if (stringsAreEqual(str, array[i])) {
            return true;
        }
    }
    return false;
}

void hashMessage() {
    char* hash = nullptr;

    std::cout << "Enter 0 if you want to hash an already existing message\n";
    std::cout << "Enter 1 if you want to type the message\n";

    char buff[MAX_USER_INPUT_LENGTH];
    char validInputs[] = { '0', '1'};
    std::cin.getline(buff, MAX_USER_INPUT_LENGTH);

    switch (inputValidation(buff, validInputs)) {
    case 0: 
        int ind;
        std::cout << "Enter the index of the message you want to hash.\n";
        std::cin >> ind;
        std::cin.ignore();
        if (ind < 0 || ind >= messagesCount) {
            std::cout << "Invalid input\n";
            return;
        }
        hash = createHash(messages[ind]);
        break;
    case 1: 
        std::cout << "Enter a message:\n";
        std::cin.getline(buff, MAX_USER_INPUT_LENGTH);
        hash = createHash(buff);
        break;
    default: 
        std::cout << "Invalid input\n";
        return;
    }

    std::cout << "Hash: " << hash << "\n";
    std::cout << "Do you want to save this hash ? Enter 1 if yes and 0 if no\n";
    std::cin.getline(buff, MAX_USER_INPUT_LENGTH);

    switch (inputValidation(buff, validInputs)) {
    case 0:
        delete[] hash;
        return;
    case 1:
        addStringToArray(hash, hashesCount, hashesSize, hashMessages);
        break;
    default: 
        std::cout << "Invalid input\n";
        return;
    }

    std::cout << "Do you want to save this hash in a file? Enter 1 if yes and 0 if no\n";
    std::cin.getline(buff, MAX_USER_INPUT_LENGTH);

    switch (inputValidation(buff, validInputs)) {
    case 0:
        return;
    case 1: 
        std::cout << "Enter file name:";
        std::cin.getline(buff, MAX_USER_INPUT_LENGTH);
        saveHashToFile(hash, buff);
        break;
    default:
        std::cout << "Invalid input\n";
    }
}

char* createHash(char* message) {
    initVariables();

    char* result = new char[HASH_CHARACTERS + 1];
    int paddedMessageSize = 0;
    unsigned int* paddedMessage = preProcessing(message, paddedMessageSize);
    //512 (bits in each digest) / 32 (unsigned int bits) = 16 (ints in each digest)
    const int UNSIGNED_INTS_IN_CHUNK = 16;

    //Now, process the message in 512-bit chunks 
    int chunksCount = paddedMessageSize / UNSIGNED_INTS_IN_CHUNK;

    for (int i = 0; i < chunksCount; i++) {
        //Perform the chunk loop for the next 16 integers 
        chunkLoop(paddedMessage + i * UNSIGNED_INTS_IN_CHUNK);
    }

    //Now, create the final hash by appending h0,...,h7
    const int HASH_VALUES_COUNT = 8;
    unsigned int* finalHash = new unsigned int[HASH_VALUES_COUNT];
    initHash(finalHash);
    toHex(finalHash, result);
    result[HASH_CHARACTERS] = '\0';

    delete[] finalHash;
    delete[] paddedMessage;

    return result;
}

void initVariables() {
    h0 = 0x6a09e667;
    h1 = 0xbb67ae85;
    h2 = 0x3c6ef372;
    h3 = 0xa54ff53a;
    h4 = 0x510e527f;
    h5 = 0x9b05688c;
    h6 = 0x1f83d9ab;
    h7 = 0x5be0cd19;
}

void saveHashToFile(char* hash, char* fileName) {
    if (fileExists(fileName)) {
        char validInputs[] = { '0', '1', '2' };
        char buff[MAX_USER_INPUT_LENGTH];
        std::cout << "This file already exists\n";
        std::cout << "(0) cancel\n";
        std::cout << "(1) add to file\n";
        std::cout << "(2) overwrite file\n";
        std::cin.getline(buff, MAX_USER_INPUT_LENGTH);

        switch (inputValidation(buff, validInputs)) {
        case 0:
            return;
        case 1: {
            std::ofstream file;
            file.open(fileName, std::ios::app);
            if (!file) {
                std::cout << "Error reading from file\n";
                return;
            }
            file << "\n";
            file << hash;
            file.close();
            break;
        }
        case 2: {
            std::ofstream file;
            file.open(fileName, std::ios::trunc);
            if (!file) {
                std::cout << "Error reading from file\n";
                return;
            }
            file << hash;
            file.close();
            break;
        }      
        }
    }

    else {
        std::ofstream file(fileName);
        if (!file) {
            std::cout << "Error reading from file\n";
            return;
        }
        file << hash;
    }
    std::cout << "Hash was added to file\n";
}

bool fileExists(char* fileName) {
    std::ifstream f(fileName);
    bool isGood = f.good();
    f.close();
    return isGood;
}

unsigned int* preProcessing(char* message, int& paddedMessageSize) {
    const int DIGEST_SIZE = 512;
    const int BIG_ENDIAN_INT_BITS = 64;

    unsigned long long messageLength = stringLength(message);
    unsigned long long messageLengthInBits = messageLength * 8;

    //The input must be evenly divisible by 512
    //The number of zeroes to be added will be calculated using the length of 
    //the message in bits + the size of the big endian integer + 8 (10000000)
    int rem = (messageLengthInBits + BIG_ENDIAN_INT_BITS + 8) % DIGEST_SIZE;
    int zeroesToAddBits = DIGEST_SIZE - rem;

    //Final size of the array 
    unsigned long long arraySizeBits = messageLengthInBits + 8 + zeroesToAddBits + BIG_ENDIAN_INT_BITS;
    unsigned long long arraySize = arraySizeBits / UNSIGNED_INT_BITS;

    //Save the size of the array
    paddedMessageSize = arraySize;

    unsigned int* arr = new unsigned int[arraySize];

    int arrayInd = 0;
    int messageInd = 0;
    unsigned int integerToAdd = 0;

    //The number of unsigned integers that will be used to store the message
    int messageIntegersCount = messageLengthInBits / UNSIGNED_INT_BITS;
    //If the message length isn't evenly divisible by 32, add zeroes as needed 
    if (messageLengthInBits % UNSIGNED_INT_BITS != 0) {
        messageIntegersCount++;
        //Decrease the zeroes by the number needed to pad the message to an unsigned integer 
        // -8, because I am looking at the 8-bit word '10000000' seperately
        //Hence, if 8 bits are needed to complete the integer, the zeroesToAddBits will not be
        //decreased because 10000000 will be enough to complete the integer 
        zeroesToAddBits -= (UNSIGNED_INT_BITS - messageLengthInBits % UNSIGNED_INT_BITS - 8);
    }

    integerToAdd = 0;
    int i;
    //i starts from 1 because the steps are counted 1-2-3-4...
    for (i = 1; i < messageLength + 1; i++) {
        //Make place for the next character 
        integerToAdd = (integerToAdd << 8);
        //Extract the character into the unsigned integer
        integerToAdd = (integerToAdd | message[messageInd++]);
        //Each 4 steps, add the unsigned integer to the array and initialize it back to 0
        if (i % 4 == 0) {
            arr[arrayInd++] = integerToAdd;
            integerToAdd = 0;
        }
    }

    //Decrease i since its final value will be messageLength + 1
    i--;
    i = i % 4;

    //Adding zeroes
    //If i == 0, simply add the '1' + 31 zeroes after it to the array  
    if (!i) {
        integerToAdd = 1;
        integerToAdd = rightRotate(integerToAdd, 1);
        arr[arrayInd++] = integerToAdd;
        //Decrease the number of zeroes
        zeroesToAddBits -= 24; // 32 - 8 
    }
    else {
        unsigned char mask = 0b10000000;
        //The number of steps left to perform 
        i = 4 - i;
        while (i) {
            integerToAdd = integerToAdd << 8;
            integerToAdd = integerToAdd | mask;
            mask = mask << 8;
            i--;
            //When i reaches 0, add the new integer
            if (!i) {
                arr[arrayInd++] = integerToAdd;
            }
        }
    }

    int zeroesCount = zeroesToAddBits / UNSIGNED_INT_BITS;
    for (int i = 0; i < zeroesCount; i++) {
        arr[arrayInd++] = 0;
    }

    //Now, add the 64-bit big endian integer representing the length of the string
    for (int j = 0; j < 2; j++) {
        integerToAdd = 0;
        //Copy the last 32 bits of the message length 
        integerToAdd = integerToAdd | messageLengthInBits;
        //Shift right so that the next 32 bits can be read
        messageLengthInBits = (messageLengthInBits >> UNSIGNED_INT_BITS);
        //The integer is big-endian, so the big end will be stored first
        arr[arraySize - 1 - j] = integerToAdd;
    }
    return arr;
}

void chunkLoop(unsigned int* paddedMessage) {
    const int MESSAGE_SCHEDULE_SIZE = 64; //the number of unsigned ints used for w0,..w63
    const int INPUT_DATA_SIZE = 16; //the number of unsigned ints used for w0,..w15

    unsigned int* w = new unsigned int [MESSAGE_SCHEDULE_SIZE]();

    //Copy all data from padded message to the new array
    for (int i = 0; i < INPUT_DATA_SIZE; i++) {
        w[i] = paddedMessage[i];
    }

    //Modify the zeroed indices at the end of the array 
    for (int i = INPUT_DATA_SIZE; i < MESSAGE_SCHEDULE_SIZE; i++) {
        unsigned int s0 = rightRotate(w[i - 15], 7) ^ rightRotate(w[i - 15], 18) ^ (w[i - 15] >> 3);
        unsigned int s1 = rightRotate(w[i - 2], 17) ^ rightRotate(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    //Initialize working variables to current hash value:
    unsigned int a = h0;
    unsigned int b = h1;
    unsigned int c = h2;
    unsigned int d = h3;
    unsigned int e = h4;
    unsigned int f = h5;
    unsigned int g = h6;
    unsigned int h = h7;

    //Compression loop 
    for (int i = 0; i < 64; i++) {
        unsigned int s1 = rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);
        unsigned int ch = (e & f) ^ ((~e) & g);
        unsigned temp1 = h + s1 + ch + k[i] + w[i];
        unsigned int s0 = rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);
        unsigned int maj = (a & b) ^ (a & c) ^ (b & c);
        unsigned int temp2 = s0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    //All addition is modulo 2^32 
    h0 = h0 + a;
    h1 = h1 + b;
    h2 = h2 + c;
    h3 = h3 + d;
    h4 = h4 + e;
    h5 = h5 + f;
    h6 = h6 + g;
    h7 = h7 + h;

    delete[] w;
}

unsigned int rightRotate(const unsigned int n, const unsigned int d){
    //By shifting right, we are making place for the d digits that will be put
    //in the beginning of the number. The bitwise or copies those d digits.
    //(n << (UNSIGNED_INT_BITS - d) shifts left until the last d digits of the number are placed first 
    return (n >> d) | (n << (UNSIGNED_INT_BITS - d));
}

void initHash(unsigned int* finalHash) {
    finalHash[0] = h0;
    finalHash[1] = h1;
    finalHash[2] = h2;
    finalHash[3] = h3;
    finalHash[4] = h4;
    finalHash[5] = h5;
    finalHash[6] = h6;
    finalHash[7] = h7;
}

void toHex(unsigned int* finalHash, char result[])
{
    const int HEX_DIGIT_BITS = 4;
    int numberOfIntsInFinalHash = HASH_BITS / UNSIGNED_INT_BITS; // 8
    int hexValuesInUnsignedInt = UNSIGNED_INT_BITS / HEX_DIGIT_BITS; //8
    int resultInd = -1;

    //For each int h0,...,h7
    for (int i = 0; i < numberOfIntsInFinalHash; i++) {
        //After converting each integer, the hex digits should be reversed 
        resultInd = i * hexValuesInUnsignedInt + hexValuesInUnsignedInt - 1;
        for (int j = 0; j < hexValuesInUnsignedInt; j++) {
            int temp = finalHash[i] % 16;
            char ch;
            if (temp < 10) {
                ch = '0' + temp;
            }
            else {
                ch = 'A' + temp - 10;
            }
            finalHash[i] /= 16;
            result[resultInd--] = ch;
        }
    }
}

void readHashedMessage() {
    //SHA256 is irreversible. So the only way to find the original message is to 
    //hash all existing messages and compare them to the hash 
    std::cout << "Enter 0 if you want to read an already existing hash\n";
    std::cout << "Enter 1 if you want to type the hash\n";

    char* hash;
    char buff[MAX_USER_INPUT_LENGTH];
    char validInputs[] = { '0', '1' };
    std::cin.getline(buff, MAX_USER_INPUT_LENGTH);

    switch (inputValidation(buff, validInputs)) {
    case 0:
        int ind;
        std::cout << "Enter the index of the hash you want to read.\n";
        std::cin >> ind;
        std::cin.ignore();
        if (ind < 0 || ind >= hashesCount) {
            std::cout << "Invalid input\n";
            return;
        }
        hash = hashMessages[ind];
        break;
    case 1:
        std::cout << "Enter a hash:\n";
        std::cin.getline(buff, MAX_USER_INPUT_LENGTH);
        hash = buff;
        break;
    default:
        std::cout << "Invalid input\n";
        return;
    }
    
    //Now, compare the hash to the hashes of all existing messages 
    for (int i = 0; i < messagesCount; i++) {
        char* temp = createHash(messages[i]);
        if (stringsAreEqual(hash, temp)) {
            std::cout << "Original message: " << messages[i] << "\n";
            return;
        }
    }
    std::cout << "No matches found\n";
}

void createNewMessageFromUserInput() {
    std::cout << "Enter a message:\n";
    char* message = new char[MAX_USER_INPUT_LENGTH];
    std::cin.getline(message, MAX_USER_INPUT_LENGTH);
    addStringToArray(message, messagesCount, messagesSize, messages);
}

