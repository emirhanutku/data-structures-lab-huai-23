#include "ImageMatrix.h"
#include <iostream>


// Default constructor
ImageMatrix::ImageMatrix() {

}


// Parameterized constructor for creating a blank image of given size
ImageMatrix::ImageMatrix(int imgHeight, int imgWidth) : height(imgHeight), width(imgWidth) {
    data = new double *[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new double[width];
    }

}

// Parameterized constructor for loading image from file. PROVIDED FOR YOUR CONVENIENCE
ImageMatrix::ImageMatrix(const std::string &filepath) {
    // Create an ImageLoader object and load the image
    ImageLoader imageLoader(filepath);

    // Get the dimensions of the loaded image
    height = imageLoader.getHeight();
    width = imageLoader.getWidth();


    // Allocate memory for the matrix
    data = new double *[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new double[width];
    }

    // Copy data from imageLoader to data
    double **imageData = imageLoader.getImageData();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; j++) {
            data[i][j] = imageData[i][j];
        }
    }
}


// Destructor
ImageMatrix::~ImageMatrix() {
    if (data != nullptr) {
        for (int i = 0; i < height; ++i) {
            delete[] data[i];
        }
        delete[] data;
    }
}

// Parameterized constructor - direct initialization with 2D matrix
ImageMatrix::ImageMatrix(const double **inputMatrix, int imgHeight, int imgWidth) {
    height = imgHeight;
    width = imgWidth;


    data = new double *[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new double[width];
    }


    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; j++) {
            data[i][j] = inputMatrix[i][j];
        }
    }


}

// Copy constructor
ImageMatrix::ImageMatrix(const ImageMatrix &other) {
    this->height = other.height;
    this->width = other.width;

    data = new double *[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new double[width];
    }


    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            this->get_data()[i][j] = other.get_data()[i][j];
        }
    }

}

// Copy assignment operator
ImageMatrix &ImageMatrix::operator=(const ImageMatrix &other) {


    return *this;

}



// Overloaded operators

// Overloaded operator + to add two matrices
ImageMatrix ImageMatrix::operator+(const ImageMatrix &other) const {
    ImageMatrix result(this->height, this->width);
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            result.get_data()[i][j] = this->get_data()[i][j] + other.get_data()[i][j];
        }
    }
    return result;
}

// Overloaded operator - to subtract two matrices
ImageMatrix ImageMatrix::operator-(const ImageMatrix &other) const {
    ImageMatrix result(this->height, this->width);
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            result.get_data()[i][j] = this->get_data()[i][j] - other.get_data()[i][j];
        }
    }
    return result;
}

// Overloaded operator * to multiply a matrix with a scalar
ImageMatrix ImageMatrix::operator*(const double &scalar) const {
    ImageMatrix result(this->height, this->width);
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            result.get_data()[i][j] = this->data[i][j];
            result.get_data()[i][j] *= scalar;
        }
    }
    return result;
}


// Getter function to access the data in the matrix
double **ImageMatrix::get_data() const {
    return data;

}

// Getter function to access the data at the index (i, j)
double ImageMatrix::get_data(int i, int j) const {
    return data[i][j];
}


int ImageMatrix::get_height() const {
    return height;
}

int ImageMatrix::get_width() const {
    return width;
}

