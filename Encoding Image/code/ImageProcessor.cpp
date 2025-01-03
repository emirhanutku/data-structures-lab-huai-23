#include <iostream>
#include "ImageProcessor.h"

ImageProcessor::ImageProcessor() {

}

ImageProcessor::~ImageProcessor() {

}


std::string ImageProcessor::decodeHiddenMessage(const ImageMatrix &img) {
    std::string hiddenMessage;


    ImageSharpening imageSharpening;
    ImageMatrix sharpenImg = imageSharpening.sharpen(img, 2);


    EdgeDetector edgeDetector;
    edgePixels = edgeDetector.detectEdges(sharpenImg);

    DecodeMessage decodeMessage;
    hiddenMessage = decodeMessage.decodeFromImage(sharpenImg, edgePixels);


    return hiddenMessage;

}

ImageMatrix ImageProcessor::encodeHiddenMessage(const ImageMatrix &img, const std::string &message) {
    ImageSharpening imageSharpening;
    ImageMatrix sharpenImg = imageSharpening.sharpen(img, 2);


    EdgeDetector edgeDetector;
    edgePixels = edgeDetector.detectEdges(sharpenImg);

    EncodeMessage encodeMessage;
    ImageMatrix embeddedImage = encodeMessage.encodeMessageToImage(img, message, edgePixels);
    return embeddedImage;


}
