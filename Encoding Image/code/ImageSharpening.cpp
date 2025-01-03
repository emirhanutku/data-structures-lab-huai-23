#include <iostream>
#include "ImageSharpening.h"

// Default constructor
ImageSharpening::ImageSharpening() {


}

ImageSharpening::~ImageSharpening() {


}

ImageMatrix ImageSharpening::sharpen(const ImageMatrix &input_image, double k) {

    const double *kernel[3] = {
            new double[3]{1.0 / 9, 1.0 / 9, 1.0 / 9},
            new double[3]{1.0 / 9, 1.0 / 9, 1.0 / 9},
            new double[3]{1.0 / 9, 1.0 / 9, 1.0 / 9}
    };
    ImageMatrix kernel_matrix(kernel, 3, 3);


    Convolution convolution(kernel_matrix.get_data(), 3, 3, 1, true);

    ImageMatrix blurredImage{convolution.convolve(input_image)};


    ImageMatrix sharpImg = input_image + ((input_image - blurredImage) * k);

    for (int i = 0; i < sharpImg.get_height(); i++) {
        for (int j = 0; j < sharpImg.get_width(); j++) {
            if (sharpImg.get_data()[i][j] < 0) { sharpImg.get_data()[i][j] = 0; }
            else if (sharpImg.get_data()[i][j] > 255) { sharpImg.get_data()[i][j] = 255; }
        }
    }


    for (int i = 0; i < 3; i++) {
        delete[] kernel[i];
    }
    return sharpImg;
}
