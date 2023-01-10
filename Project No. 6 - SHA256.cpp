#include <iostream>
#include <fstream>
#include <climits>

const int START_SIZE_MESSAGES = 100;
const int DIGEST_SIZE = 512;
const int BITS_IN_A_BYTE = 8;
const int UNSIGNED_INT_BITS = 32;
const int UNSIGNED_CHAR_BITS = 8;
const int BIG_ENDIAN_INT_BITS = 64;
const int BIG_ENDIAN_INT_BYTES = 8;
const int FINAL_HASH_BITS = 256;
const int FINAL_HASH_LENGTH = 64;

int size = START_SIZE_MESSAGES;
int messagesCount = 0;
unsigned char** messages = new unsigned char* [START_SIZE_MESSAGES];
unsigned char** hashMessages = new unsigned char* [START_SIZE_MESSAGES];

unsigned int h0 = 0x6a09e667;
unsigned int h1 = 0xbb67ae85;
unsigned int h2 = 0x3c6ef372;
unsigned int h3 = 0xa54ff53a;
unsigned int h4 = 0x510e527f;
unsigned int h5 = 0x9b05688c;
unsigned int h6 = 0x1f83d9ab;
unsigned int h7 = 0x5be0cd19;

unsigned int k[64] = {
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
unsigned char* createMessageFromFile(const char* fileName);
int countCharactersInFile(const char* fileName);
void freeSpace(unsigned char** arr, const int size);
void addNewMessage(const char* fileName);
void increaseArraySize();
int messageLength(unsigned char* message);
bool messageAlreadyExists(unsigned char* message);

void createHash(unsigned char* message, unsigned char result[]);
unsigned char* preProcessing(unsigned char* message);
void chunkLoop(unsigned char* paddedMessage, const int numberOf8BitWordsInDigest);
unsigned int rightRotate(const unsigned int n, const unsigned int d);
unsigned int leftRotate(const unsigned int n, const unsigned int d);
unsigned int reverseBits(unsigned int num);
void initHash(unsigned int* finalHash);
void toHex(unsigned int* finalHash, unsigned char result[]);

void printBinaryNumber(unsigned int a);


int main()
{
    char buff[100];
    char validInputs[] = {'0', '1', '2', '3'};
    bool getUserInput = true;
    while (getUserInput) {
        std::cout << "To exit the program, enter 0\n";
        std::cout << "To read a message from file, enter 1\n";
        std::cout << "To hash a message, enter 2\n";
        std::cout << "To read a hashed message, enter 3\n";
        std::cin.getline(buff, 100);
        int operation = inputValidation(buff, validInputs);
        switch (operation) {
        case 0: getUserInput = false; break;
        case 1: readMessageFromFile(); break;
        case 2: hashMessage(); break;
        default: std::cout << "Invalid user input";
        }
    }

    //unsigned char** messages = new unsigned char* [START_SIZE_MESSAGES];
    //addNewMessage(messages, size, messagesCount, "message1.txt");
    //std::cout << messages[0] << "\n";

    //unsigned char** hashMessages = new unsigned char* [START_SIZE_MESSAGES];
    //createHash(messages[0], hashMessages[messagesCount - 1]);
    //std::cout << hashMessages[0];

    freeSpace(messages, messagesCount);
    freeSpace(hashMessages, messagesCount);
}

//This function will return the number of the operation to be performed or -1
int inputValidation(const char buff[], const char validInputs[]) {
    //Compare each of the valid inputs to buff
    for (int i = 0; validInputs[i] != '\0'; i++) {
        if (stringsAreEqual(buff, &validInputs[i])) {
            return validInputs[i];
        }
    }
    return -1;
}

bool stringsAreEqual(const char* str1, const char* str2)
{
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return 0;
        }
    }
    //If one of the strings is longer than the other, they can't be equal
    //This will only return true if *str1 == *str2 == '\0'
    return (*str1 == *str2);
}

void readMessageFromFile() {
    char buff[100];
    char fileName[100];
    std::cout << "Enter a valid file name:\n";
    std::cin.getline(fileName, 100);

    std::ifstream file(fileName);
    if (!file) {
        std::cout << "Error reading from file\n";
        return;
    }
    std::cout << "Data from " << fileName << ":\n";
    while (file >> buff) {
        std::cout << buff;
    }
    char validInput[] = { '0', '1' };
    std::cout << "Do you want to save this message? Enter 1 if yes and 0 if no";
    std::cin.getline(buff, 100);
    switch (inputValidation(buff, validInput)) {
    case 0: return;
    case 1: createMessageFromFile(fileName); break;
    defualt: std::cout << "Invalid user input\n";
    }

    file.close();
}

//This function will create a char array and return a pointer to it
unsigned char* createMessageFromFile(const char* fileName) {
    int arrayLength = countCharactersInFile(fileName);
    unsigned char* message = new unsigned char[arrayLength + 1];

    std::ifstream file(fileName);
    if (!file) {
        std::cout << "Error reading from file";
        return nullptr;
    }
    unsigned char ch;
    int i = 0;
    while (file >> std::noskipws >> ch) {
        message[i] = ch;
        i++;
    }
    message[arrayLength] = '\0';
    std::cout << "Message was created.\n";
    file.close();

    addNewMessage(fileName);
    return message;
}

int countCharactersInFile(const char* fileName) {
    std::ifstream file(fileName);
    if (!file) {
        std::cout << "Error reading from file";
        return -1;
    }

    unsigned char ch;
    int ctr = 0;
    while (file >> std::noskipws >> ch) {
        ctr++;
    }
    file.close();
    return ctr;
}

void freeSpace(unsigned char** arr, const int size) {
    for (int i = 0; i < size; i++) {
        delete[] arr[i];
        arr[i] = nullptr;
    }
    delete[] arr;
    arr = nullptr;
}

void addNewMessage(const char* fileName) {
    //Before adding a new message, we should first make sure that there is enough space in the messages array.
    //If not, then we must increase the size of the array by START_SIZE_MESSAGES
    if (messagesCount >= size) {
        increaseArraySize();
    }
    //Now, we can freely add the new message to the array
    messages[messagesCount++] = createMessageFromFile(fileName);
    std::cout << "Message was added to the array of messages under number " << messagesCount << "\n";
}

void increaseArraySize() {
    //There is no use of deleting the already existing messages. 
    //We can simply create a new, bigger array of pointers to the messages
    unsigned char** newMessagesArray = new unsigned char* [size + START_SIZE_MESSAGES];

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

int messageLength(unsigned char* message) {
    int i = 0;
    while (*message) {
        message++;
        i++;
    }
    return i;
}

bool messageAlreadyExists(unsigned char* message)
{
    for (int i = 0; i < messagesCount; i++) {
        if (stringsAreEqual(message, messages[i])) {
            return true;
        }
    }
    return false;
}

void createHash(unsigned char* message, unsigned char result[]) {
    unsigned char* paddedMessage = preProcessing(message);

    //Now, process the message in 512-bit chunks 
    //So, in each step, process 64 characters from the paddedMessage array
    int numberOf8BitWordsInDigest = DIGEST_SIZE / BITS_IN_A_BYTE; //64
    for (int i = 0; paddedMessage[i] != UCHAR_MAX; i += numberOf8BitWordsInDigest) {
        chunkLoop(paddedMessage + i * numberOf8BitWordsInDigest, numberOf8BitWordsInDigest);
    }
    //Now, create the final hash by appending h0,...,h7
    unsigned int* finalHash = new unsigned int[FINAL_HASH_BITS / UNSIGNED_INT_BITS];
    initHash(finalHash);
    toHex(finalHash, result);
    result[FINAL_HASH_LENGTH] = '\0';

    delete[] finalHash;
    delete[] paddedMessage;
}

unsigned char* preProcessing(unsigned char* message) {
    //begin with the original message of length L bits
    //append a single '1' bit
    //append K '0' bits, where K is the minimum number >= 0 such that L + 1 + K + 64 is a multiple of 512
    //append L as a 64 - bit big - endian integer, making the total post - processed length a multiple of 512 bits
    const int SINGLE_BIT_SIZE = 1;

    int length = messageLength(message);
    int L = BITS_IN_A_BYTE * length;
    int rem = (L + BITS_IN_A_BYTE + BIG_ENDIAN_INT_BITS) % DIGEST_SIZE;
    //This is the number of 0 bits, but it makes more sense to think of them as bytes.
    int K = DIGEST_SIZE - rem;

    //The size of the padded message will be equal to the original length of the message + 1 byte for the appended
    //'1' and the zeroes after it + K / BITS_IN_A_BYTE + 8 bytes for the big-endian int + 1 for the null-terminator 
    int paddedMessageLength = length + 1 + K / BITS_IN_A_BYTE + BIG_ENDIAN_INT_BYTES + 1;
    unsigned char* paddedMessage = new unsigned char[paddedMessageLength];
    
    //Copy the message to the paddedMessage array 
    for (int i = 0; i < length; i++) {
        paddedMessage[i] = message[i];
    }

    //The next byte should be equal to 10000000
    paddedMessage[length] = 0b10000000;

    //Pad with the necessary zeroes 
    int endInd = paddedMessageLength - BIG_ENDIAN_INT_BYTES - 1;
    for (int i = length + 1; i < endInd; i++) {
        paddedMessage[i] = 0b00000000;
    }

    unsigned char mask = 0xFF; //11111111
    for (int i = paddedMessageLength - 2; i >= endInd; i--) {
        //Read the last 8 bits of originalInputLengthInBinary
        unsigned char ch = L & mask;
        paddedMessage[i] = ch;
        //Rightshift originalInputLengthInBinary by 8 bits 
        L = L >> BITS_IN_A_BYTE;
    }
    //Since the padding has been done using a char with value zero, I will use the biggest possible unsigned char 
    //instead of a null terminator 
    paddedMessage[paddedMessageLength - 1] = UCHAR_MAX;

    return paddedMessage;
}

void chunkLoop(unsigned char* paddedMessage, const int numberOf8BitWordsInDigest) {
    int _8BitWordsIn32BitWord = UNSIGNED_INT_BITS / UNSIGNED_CHAR_BITS;
    const int MESSAGE_SCHEDULE_SIZE = 64;
    const int INPUT_DATA_SIZE = 16; 

    unsigned int* w = new unsigned int [MESSAGE_SCHEDULE_SIZE]();
    int wInd = 0;

    //Copy all data from padded message to the new array
    for (int i = 0; i < 64; i += _8BitWordsIn32BitWord) {
        //For this step, we need to merge 4 8-bit characters in a 32-bit unsigned int 
        //To do this, I am going to do the following: 


        //Now, extract the bits of each character into w[wInd]
        for (int j = 0; j < _8BitWordsIn32BitWord; j++) {
            unsigned int _8BitWordUnsignedInt = paddedMessage[i + j];
            _8BitWordUnsignedInt <<= ((_8BitWordsIn32BitWord - j - 1) * UNSIGNED_CHAR_BITS);
            w[wInd] = w[wInd] | _8BitWordUnsignedInt;

        }

        wInd++;
    }

    //Modify the zeroes indices at the end of the array 
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
    return (n >> d) | (n << (UNSIGNED_INT_BITS - d));
}

unsigned int leftRotate(const unsigned int n, const unsigned int d) {
    return (n << d) | (n >> (UNSIGNED_INT_BITS - d));
}

void printBinaryNumber(unsigned int a) {
    int binaryNum[32];

    for (int i = 0; i < 32; i++) {
        binaryNum[i] = a % 2;
        a /= 2;
    }

    for (int j = 31; j >= 0; j--)
        std::cout << binaryNum[j];
}

unsigned int reverseBits(unsigned int num)
{
    unsigned int reversed = 0;
    while (num > 0){
        //Making place for the next digit
        reversed <<= 1;

        // If the last bit of num is 1, save it in the back of the reversed number 
        if ((num & 1) == 1) {
            reversed ^= 1;
        }

        //This way, we are deleting the last bit of num 
        num >>= 1;
    }
    return reversed;
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

void toHex(unsigned int* finalHash, unsigned char result[])
{
    const int HEX_DIGIT_BITS = 4;
    int numberOfIntsInFinalHash = FINAL_HASH_BITS / UNSIGNED_INT_BITS; // 8
    int hexValuesInUnsignedInt = UNSIGNED_INT_BITS / HEX_DIGIT_BITS; //8
    int resultInd = -1;

    //For each int
    for (int i = 0; i < numberOfIntsInFinalHash; i++) {
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

