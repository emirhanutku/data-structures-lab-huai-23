#include <iostream>

#include "Convolution.h"

// Default constructor 
Convolution::Convolution() {
}


// Parametrized constructor for custom kernel and other parameters
Convolution::Convolution(double **customKernel, int kh, int kw, int stride_val, bool pad) {

    this->customKernel = new double *[kh];
    for (int i = 0; i < kh; ++i) {
        this->customKernel[i] = new double[kw];
    }


    for (int i = 0; i < kh; ++i) {
        for (int j = 0; j < kw; j++) {
            this->customKernel[i][j] = customKernel[i][j];
        }
    }

    this->kh = kh;
    this->kw = kw;
    this->stride_val = stride_val;
    this->pad = pad;


}

// Destructor
Convolution::~Convolution() {
    if (customKernel != nullptr) {
        for (int i = 0; i < kh; ++i) {
            delete[] customKernel[i];
        }
        delete[] customKernel;
    }

}

// Copy constructor
Convolution::Convolution(const Convolution &other) {
    this->kh = other.kh;
    this->kw = other.kw;

    this->customKernel = new double *[kh];
    for (int i = 0; i < kh; ++i) {
        this->customKernel[i] = new double[kw];
    }


    for (int i = 0; i < kh; ++i) {
        for (int j = 0; j < kw; j++) {
            this->customKernel[i][j] = other.customKernel[i][j];
        }
    }


}

// Copy assignment operator
Convolution &Convolution::operator=(const Convolution &other) {
    if (this == &other) {
        return *this;
    }


    for (int i = 0; i < kh; ++i) {
        delete[] customKernel[i];
    }
    delete[] customKernel;

    // Allocate new resources
    kh = other.kh;
    kw = other.kw;
    customKernel = new double *[kh];
    for (int i = 0; i < kh; ++i) {
        customKernel[i] = new double[kw];
        for (int j = 0; j < kw; j++) {
            customKernel[i][j] = other.customKernel[i][j];
        }
    }


    return *this;

}


ImageMatrix Convolution::convolutionEvent(const ImageMatrix &matrixToConvolve, const ImageMatrix &inputMatrix) const {
    int padValue{0};
    if (pad) { padValue = 1; }


    ImageMatrix result(((inputMatrix.get_height() - kh + 2 * padValue) / stride_val) + 1,
                       ((inputMatrix.get_width() - kw + 2 * padValue) / stride_val) + 1);

    int valueXAxis{0};
    for (int i = 0; i < matrixToConvolve.get_height() - kh + 1; i += stride_val) {

        int valueYAxis{0};
        for (int j = 0; j < matrixToConvolve.get_width() - kw + 1; j += stride_val) {


            double sum = 0;
            for (int x = 0; x < kh; x++) {
                for (int y = 0; y < kw; y++) {

                    sum += matrixToConvolve.get_data()[i + x][j + y] * customKernel[x][y];
                }
            }
            result.get_data()[valueXAxis][valueYAxis] = sum;
            valueYAxis += 1;

        }
        valueXAxis += 1;
    }
    return result;
}


// Convolve Function: Responsible for convolving the input image with a kernel and return the convolved image.
ImageMatrix Convolution::convolve(const ImageMatrix &input_image) const {
    if (pad) {
        int newHeight = input_image.get_height() + 2;

        int newWidth = input_image.get_width() + 2;
        ImageMatrix padded_Matrix(newHeight, newWidth);

        for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                padded_Matrix.get_data()[i][j] = 0.0;
            }
        }

        for (int i = 1; i < 1 + input_image.get_height(); i++) {
            for (int j = 1; j < 1 + input_image.get_width(); j++) {
                padded_Matrix.get_data()[i][j] = input_image.get_data()[i - 1][j - 1];
            }
        }


        ImageMatrix result = convolutionEvent(padded_Matrix, input_image);

        return result;

    } else {

        ImageMatrix result = convolutionEvent(input_image, input_image);
        return result;
    }


}


