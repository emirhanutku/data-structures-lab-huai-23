// EdgeDetector.cpp

#include "EdgeDetector.h"
#include <cmath>

#include "EdgeDetector.h"
#include <cmath>
#include <iostream>

// Default constructor
EdgeDetector::EdgeDetector() {
    const double *sobelOperatorGx[3] = {
            new double[3]{-1.0, 0.0, 1.0},
            new double[3]{-2.0, 0.0, 2.0},
            new double[3]{-1.0, 0.0, 1.0}
    };
    ImageMatrix sobelOperatorGxMatrix(sobelOperatorGx, 3, 3);

    const double *sobelOperatorGy[3] = {
            new double[3]{-1.0, -2.0, -1.0},
            new double[3]{0.0, 0.0, 0.0},
            new double[3]{1.0, 2.0, 1.0}
    };
    ImageMatrix sobelOperatorGyMatrix(sobelOperatorGy, 3, 3);

    this->gX = new double *[3];
    for (int i = 0; i < 3; ++i) {
        this->gX[i] = new double[3];
    }


    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; j++) {
            this->gX[i][j] = sobelOperatorGxMatrix.get_data()[i][j];
        }
    }

    this->gY = new double *[3];
    for (int i = 0; i < 3; ++i) {
        this->gY[i] = new double[3];
    }


    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; j++) {
            this->gY[i][j] = sobelOperatorGyMatrix.get_data()[i][j];
        }
    }


    for (int i = 0; i < 3; i++) {
        delete[] sobelOperatorGx[i];
    }

    for (int i = 0; i < 3; i++) {
        delete[] sobelOperatorGy[i];
    }

}

// Destructor
EdgeDetector::~EdgeDetector() {

    if (gX != nullptr) {
        for (int i = 0; i < 3; ++i) {
            delete[] gX[i];
        }
        delete[] gX;
    }
    if (gY != nullptr) {
        for (int i = 0; i < 3; ++i) {
            delete[] gY[i];
        }
        delete[] gY;
    }

}

// Detect Edges using the given algorithm
std::vector <std::pair<int, int>> EdgeDetector::detectEdges(const ImageMatrix &input_image) {

    Convolution convolutionX(gX, 3, 3, 1, true);
    ImageMatrix horizontalChanges{convolutionX.convolve(input_image)};

    Convolution convolutionY(gY, 3, 3, 1, true);
    ImageMatrix verticalChanges(convolutionY.convolve(input_image));


    ImageMatrix gradientMagnitude(horizontalChanges.get_height(), horizontalChanges.get_width());
    double totalGradiant{0};
    for (int i = 0; i < horizontalChanges.get_height(); i++) {
        for (int j = 0; j < horizontalChanges.get_width(); j++) {
            double horizontalValue{std::pow(horizontalChanges.get_data()[i][j], 2)};
            double verticalValue{std::pow(verticalChanges.get_data()[i][j], 2)};
            gradientMagnitude.get_data()[i][j] = sqrt(horizontalValue + verticalValue);
            totalGradiant += std::sqrt(horizontalValue + verticalValue);
        }
    }
    double threshHold{totalGradiant / (gradientMagnitude.get_height() * gradientMagnitude.get_width())};

    std::vector <std::pair<int, int>> edgePixels;

    for (int i = 0; i < gradientMagnitude.get_height(); i++) {
        for (int j = 0; j < gradientMagnitude.get_width(); j++) {
            if (gradientMagnitude.get_data()[i][j] > threshHold) {
                edgePixels.push_back({i, j});
            }
        }
    }


    return edgePixels;


}

