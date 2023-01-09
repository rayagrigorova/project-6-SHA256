#include <iostream>
#include <fstream>
#include <climits>

const int START_SIZE_MESSAGES = 100;
const int DIGEST_SIZE = 512;
const int BITS_IN_A_BYTE = 8;
const int UNSIGNED_INT_BITS = 32;
const int UNSIGNED_CHAR_BITS = 8;

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

unsigned char* createMessageFromFile(const char* fileName);
int countCharactersInFile(const char* fileName);
void freeSpace(unsigned char** messages, const int numberOfMessages);
void addNewMessage(unsigned char** messages, int& size, int& messagesCount, const char* fileName);
void increaseArraySize(unsigned char** messages, int& size, const int messagesCount);
int messageLength(unsigned char* message);

unsigned char* createHash(unsigned char* message);
unsigned char* preProcessing(unsigned char* message);
void chunkLoop(unsigned char* paddedMessage, const int i, const int numberOf8BitWordsInDigest);
unsigned int rightRotate(const unsigned int n, const unsigned int d);
unsigned int leftRotate(const unsigned int n, const unsigned int d);
unsigned int reverseBits(unsigned int num);

void printBinaryNumber(unsigned int a);


int main()
{
    int size = START_SIZE_MESSAGES;
    int messagesCount = 0;
    unsigned char** messages = new unsigned char* [START_SIZE_MESSAGES];
    addNewMessage(messages, size, messagesCount, "message1.txt");
    std::cout << messages[0] << "\n";

    unsigned char* hashMessage = createHash(messages[0]);

    freeSpace(messages, messagesCount);
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
    file.close();
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
    //std::cout << "Number of characters: " << ctr << "\n";
    return ctr;
}

void freeSpace(unsigned char** messages, const int numberOfMessages) {
    for (int i = 0; i < numberOfMessages; i++) {
        delete[] messages[i];
        messages[i] = nullptr;
    }
    delete[] messages;
    messages = nullptr;
}

void addNewMessage(unsigned char** messages, int& size, int& messagesCount, const char* fileName) {
    //Before adding a new message, we should first make sure that there is enough space in the messages array.
    //If not, then we must increase the size of the array by START_SIZE_MESSAGES
    if (messagesCount >= size) {
        increaseArraySize(messages, size, messagesCount);
    }
    //Now, we can freely add the new message to the array
    messages[messagesCount++] = createMessageFromFile(fileName);
}

void increaseArraySize(unsigned char** messages, int& size, const int messagesCount) {
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

unsigned char* createHash(unsigned char* message) {
    unsigned char* paddedMessage = preProcessing(message);

    //Now, process the message in 512-bit chunks 
    //So, in each step, process 64 characters from the paddedMessage array
    int numberOf8BitWordsInDigest = DIGEST_SIZE / BITS_IN_A_BYTE; //64
    for (int i = 0; paddedMessage[i] != UCHAR_MAX; i += numberOf8BitWordsInDigest) {
        chunkLoop(paddedMessage, i, numberOf8BitWordsInDigest);
    }

    delete[] paddedMessage;
    return nullptr;
}

unsigned char* preProcessing(unsigned char* message) {
    //begin with the original message of length L bits
    //append a single '1' bit
    //append K '0' bits, where K is the minimum number >= 0 such that L + 1 + K + 64 is a multiple of 512
    //append L as a 64 - bit big - endian integer, making the total post - processed length a multiple of 512 bits
    const int SINGLE_BIT_SIZE = 1;
    const int BIG_ENDIAN_INT_SIZE = 64;

    int length = messageLength(message);
    int L = BITS_IN_A_BYTE * length;
    int rem = (L + SINGLE_BIT_SIZE + BIG_ENDIAN_INT_SIZE) % DIGEST_SIZE;
    //This is the number of 0 bits, but it makes more sense to think of them as bytes.
    int K = DIGEST_SIZE - rem;
    //We have to add a single 1 to the message, but we should add 7 0s after it
    K -= (BITS_IN_A_BYTE - SINGLE_BIT_SIZE);

    int bitsForBigEndian = BIG_ENDIAN_INT_SIZE / BITS_IN_A_BYTE;

    //The size of the padded message will be equal to the original length of the message + 1 byte for the appended
    //'1' and the zeroes after it + K / BITS_IN_A_BYTE + 8 bytes for the big-endian int + 1 for the null-terminator 
    int paddedMessageLength = length + SINGLE_BIT_SIZE + K / BITS_IN_A_BYTE + bitsForBigEndian + 1;
    unsigned char* paddedMessage = new unsigned char[paddedMessageLength];
    
    //Copy the message to the paddedMessage array 
    for (int i = 0; i < length; i++) {
        paddedMessage[i] = message[i];
    }

    //The next byte should be equal to 10000000
    paddedMessage[length] = 0b10000000;

    //Pad with the necessary zeroes 
    int endInd = paddedMessageLength - bitsForBigEndian - 1;
    for (int i = length + 1; i < endInd; i++) {
        paddedMessage[i] = 0b00000000;
    }

    unsigned long long originalInputLengthInBinary = length * BITS_IN_A_BYTE;
    unsigned char mask = 0xFF; //11111111
    for (int i = endInd; i < paddedMessageLength - 1; i++) {
        //Read the last 8 bits of originalInputLengthInBinary
        unsigned char ch = originalInputLengthInBinary & mask;
        paddedMessage[i] = ch;
        //Rightshift originalInputLengthInBinary by 8 bits 
        originalInputLengthInBinary = originalInputLengthInBinary >> BITS_IN_A_BYTE;
    }
    //Since the padding has been done using a char with value zero, I will use the biggest possible unsigned char 
    //instead of a null terminator 
    paddedMessage[paddedMessageLength - 1] = UCHAR_MAX;

    //for (int i = 0; i < paddedMessageLength; i++) {
    //    std::cout << paddedMessage[i] << '\n';
    //}

    return paddedMessage;
}

void chunkLoop(unsigned char* paddedMessage, const int i, const int numberOf8BitWordsInDigest) {
    const int startInd = i * numberOf8BitWordsInDigest;
    const int endInd = startInd + numberOf8BitWordsInDigest;
    int _8BitWordsIn32BitWord = UNSIGNED_INT_BITS / UNSIGNED_CHAR_BITS;

    unsigned int* w = new unsigned int [numberOf8BitWordsInDigest]();
    int wInd = 0;

    //Copy all data from padded message to the new array
    for (int i = startInd; i < endInd; i += _8BitWordsIn32BitWord) {
        //For this step, we need to merge 4 8-bit characters in a 32-bit unsigned int 
        //To do this, I am going to do the following: 


        //Now, extract the bits of each character into w[wInd]
        for (int j = 0; j < _8BitWordsIn32BitWord; j++) {
            ////Copy the current 8-bit char to the unsigned int 
            //w[wInd] = w[wInd] | paddedMessage[i + j];
            ////Rotate the integer so the 8 bits come in place
            //w[wInd] = rightRotate(w[wInd], UNSIGNED_CHAR_BITS);

            //Turn the 8-bit word into a 32-bit unsigned int and 
            //leffshift it as much as needed 
            unsigned int _8BitWordUnsignedInt = paddedMessage[i + j];
            _8BitWordUnsignedInt <<= ((_8BitWordsIn32BitWord - j - 1) * UNSIGNED_CHAR_BITS);
            w[wInd] = w[wInd] | _8BitWordUnsignedInt;

        }

        std::cout << "w[wind] = " << w[wInd] << "\n";
        std::cout << "\n";

        wInd++;
    }
    //for (int i = 0; i < 64; i++) {
    //    std::cout << w[i] << " ";
    //}

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

    // counter for binary array
    for (int i = 0; i < 32; i++) {

        // storing remainder in binary array
        binaryNum[i] = a % 2;
        a /= 2;
    }

    // printing binary array in reverse order
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
