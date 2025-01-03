// DecodeMessage.cpp

#include "DecodeMessage.h"
#include <iostream>
#include <bitset>


// Default constructor
DecodeMessage::DecodeMessage() {

}

// Destructor
DecodeMessage::~DecodeMessage() {

}


std::string
DecodeMessage::decodeFromImage(const ImageMatrix &image, const std::vector <std::pair<int, int>> &edgePixels) {
    std::string binaryString;
    for (const std::pair<int, int> &pixel: edgePixels) {
        int row = pixel.first;
        int col = pixel.second;
        binaryString.append(std::to_string(static_cast<int>(image.get_data()[row][col]) & 1));
    }
    binaryString = compatibleBinary(binaryString);


    std::string hiddenMessage{binaryStringToAscII(binaryString)};


    return hiddenMessage;
}

std::string DecodeMessage::binaryStringToAscII(const std::string &binaryString) {
    std::string asciiResult;


    for (size_t i = 0; i < binaryString.length(); i += 7) {
        std::string chunk = binaryString.substr(i, 7);


        int decimalValue = static_cast<int>(std::bitset<7>(chunk).to_ulong());


        if (decimalValue <= 32) {
            decimalValue += 33;
        } else if (decimalValue >= 127) {
            decimalValue = 126;
        }

        char asciiChar = static_cast<char>(decimalValue);
        asciiResult += asciiChar;
    }


    return asciiResult;


}


std::string DecodeMessage::compatibleBinary(std::string binaryString) {
    std::string::size_type binaryLength = binaryString.length();
    std::string::size_type remainder = binaryLength % 7;


    if (remainder != 0) {
        std::string::size_type zerosToAdd = 7 - remainder;
        binaryString = std::string(zerosToAdd, '0') + binaryString;
    }
    return binaryString;

}



