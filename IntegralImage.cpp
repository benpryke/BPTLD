/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#include "IntegralImage.h"


IntegralImage::IntegralImage() {}


void IntegralImage::createFromMatlab(const mxArray *mxImage) {
    // Get pointer
    unsigned char *values = (unsigned char *)mxGetPr(mxImage);
    
    // Get width and height
    width = (int)mxGetN(mxImage);
    height = (int)mxGetM(mxImage);
    
    // Create our image
    data = new int *[width + 1];
    
    // Zero first column
    data[0] = new int[height + 1];
    
    for (int j = 0; j <= height; j++) {
        data[0][j] = 0;
    }
    
    // Loop through the data taking values from the matlab image
    for (int i = 1; i <= width; i++) {
        data[i] = new int[height + 1];
        data[i][0] = 0;
        
        for (int j = 1; j <= height; j++) {
            data[i][j] = values[(i - 1) * height + (j - 1)] + data[i - 1][j] + data[i][j - 1] - data[i - 1][j - 1];
        }
    }
    
    containsFreeableData = true;
}


void IntegralImage::createFromIntegralImage(IntegralImage *image, int x, int y, int w, int h) {
    // Check we don't exceed image dimensions
    // Note: assumes all parameters are positive
    if (x + w <= image->getWidth() && y + h <= image->getHeight()) {
        width = w;
        height = h;
        containsFreeableData = false;
        int **imageData = image->getData();
        data = new int *[w + 1];
        
        // Assign each value in our data array to a pointer to the correct
        // element in our original array
        for (int i = 0; i <= w; i++) {
            data[i] = &(imageData[x + i][y]);
        }
    } else {
        printf("ERROR: IMAGE SIZE OUT OF BOUNDS! (%d, %d, %d, %d)\n", x, y, w, h);
    }
}


void IntegralImage::createWarp(IntegralImage *image, double *bb, float *m) {
    // Initialise variables
    // Remember an IntegralImage has dimensions (width + 1)x(height + 1)
    width = (int)bb[2];
    height = (int)bb[3];
    containsFreeableData = true;
    data = new int *[width + 1];
    int **imageData = image->getData();
    
    // Get centre of bounding-box (cx, cy) and the offset relative to this of
    // the top-left of the bounding-box (ox, oy)
    int ox = -(int)(width * 0.5);
    int oy = -(int)(height * 0.5);
    int cx = (int)(bb[0] - ox);
    int cy = (int)(bb[1] - oy);
    
    // Loop through pixels of this image, width then height, calculating the
    // position of corresponding pixels in the source image
    for (int x = ox; x <= ox + width; x++) {
        data[x - ox] = new int[height + 1];
        
        for (int y = oy; y <= oy + height; y++) {
            int xp = (int)(m[0] * (float)x + m[1] * (float)y + cx);
            int yp = (int)(m[2] * (float)x + m[3] * (float)y + cy);
            
            // Limit pixels to those in the given bounding-box
            xp = std::max(std::min(xp, (int)bb[0] + width), (int)bb[0]);
            yp = std::max(std::min(yp, (int)bb[1] + height), (int)bb[1]);
            
            data[x - ox][y - oy] = imageData[xp][yp];
        }
    }
}


int IntegralImage::sumRect(int x, int y, int w, int h) {
    // Check all parameters are positive and within the image bounds
    if (x >= 0 && w > 0 && x + w <= width && y >= 0 && h > 0 && y + h <= height) {
        return data[x][y] + data[x + w][y + h] - data[x + w][y] - data[x][y + h];
    } else {
        printf("ERROR: SUM RECT OUT OF BOUNDS! (%d, %d, %d, %d)\n", x, y, w, h);
        return 0;
    }
}


int IntegralImage::getWidth() {
    return width;
}


int IntegralImage::getHeight() {
    return height;
}


int **IntegralImage::getData() {
    return data;
}


void IntegralImage::setData(int **d) {
    data = d;
}


IntegralImage::~IntegralImage() {
    // Only delete all the data if it was created from Matlab
    if (containsFreeableData) {
        for (int i = 0; i <= width; i++) {
            delete [] data[i];
        }
    }
    
    delete [] data;
}
