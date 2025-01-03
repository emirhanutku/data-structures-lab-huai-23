#include "EncodeMessage.h"
#include <cmath>
#include <bitset>
#include <iostream>


// Default Constructor
EncodeMessage::EncodeMessage() {

}

// Destructor
EncodeMessage::~EncodeMessage() {

}

// Function to encode a message into an image matrix
ImageMatrix EncodeMessage::encodeMessageToImage(const ImageMatrix &img, const std::string &message,
                                                const std::vector <std::pair<int, int>> &positions) {
    std::string newMessage;

    //First Step:Fibonacci
    for (int i = 0; i < message.size(); ++i) {
        if (checkPrime(i)) {
            int fibonacciNum{findFibonacci(i)};
            int oldAsciiNum{static_cast<int>(message[i])};
            int newAsciiNum{fibonacciNum + oldAsciiNum};
            if (newAsciiNum >= 127) {
                newAsciiNum = 126;
            } else if (newAsciiNum <= 32) {
                newAsciiNum += 33;
            }
            newMessage.insert(i, 1, static_cast<char>(newAsciiNum));
        } else {
            newMessage.insert(i, 1, message[i]);
        }
    }

    //Second Step:Right Circular Shifting
    std::string::size_type len = newMessage.length();
    int shift = static_cast<int>(len / 2);
    std::string shiftedMessage = newMessage.substr(len - shift) + newMessage.substr(0, len - shift);

    //Third Step:character to Binary
    std::string binaryString;
    for (int i = 0; i < shiftedMessage.length(); ++i) {
        int asciiNum = static_cast<unsigned char>(shiftedMessage[i]);
        std::string binaryOfNum = std::bitset<7>(asciiNum).to_string();

        binaryString.append(binaryOfNum);


    }

    //Fourth step embedded image
    ImageMatrix embeddedImage = img;


    for (int i = 0; i < positions.size(); i++) {
        if (i < binaryString.length()) {
            int row = positions[i].first;
            int column = positions[i].second;
            int pixelToChange = static_cast<int>(embeddedImage.get_data(row, column));
            char binaryValue = binaryString[i];
            if (pixelToChange % 2 == 0 && binaryValue == '1') {
                embeddedImage.get_data()[row][column]++;
            } else if (pixelToChange % 2 != 0 && binaryValue == '0') {
                embeddedImage.get_data()[row][column]--;
            }
        }
    }





    //

    return embeddedImage;


}

bool EncodeMessage::checkPrime(int number) {
    if (number <= 1) {
        return false;
    }
    if (number <= 3) {
        return true;
    }
    if (number % 2 == 0 || number % 3 == 0) {
        return false;
    }

    for (int i = 5; i * i <= number; i += 6) {
        if (number % i == 0 || number % (i + 2) == 0) {
            return false;
        }
    }

    return true;
}

int EncodeMessage::findFibonacci(int n) {
    int fib[n + 2];
    fib[0] = 0;
    fib[1] = 1;

    for (int i = 2; i <= n; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }

    return fib[n];
}

