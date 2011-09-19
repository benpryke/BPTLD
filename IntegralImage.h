/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#pragma once
#include "mex.h"
#include <algorithm>


/*  An integral image, or summed area table, allows fast computation of 
    rectangular areas of pixel intensities in an image.
    
    Computed by:
        I(x, y) = i(x, y) + I(x - 1, y) + I(x, y - 1) - I(x - 1, y - 1)
        where I is the integral image and i is the original image.
    
    The intensity sum of a rectangle can then be computed by:
        top-left + bottom-right - top-right - bottom-left
    
    Integral images are stored in a 2D array as such:
        0 0 0 0 0 0
        0 - - - - -
        0 - - - - -
        0 - - - - -
        0 - - - - -
    
    where width = image width + 1, height = image height + 1, dashes represent
    elements containing sums from the original image, and zeros represent the
    top row and left column, which hold only zeros. */
class IntegralImage {
    // Private ===============================================================
    private:
    // 2-dimensional array containing values of the integral image
    int **data;
    
    // Dimensions of the image
    int width, height;
    
    // If this image was created from Matlab or as a warp, its data was copied
    // by value, otherwise it was copied by reference and cannot be freed.
    // This information is important as we only free the data upon destruction
    // if this is the case
    bool containsFreeableData;
    
    
    // Public ================================================================
    public:
    /*  Constructor. */
    IntegralImage(void);
    
    /*  Creates an integral image from Matlab.
        mxImage: the image straight from Matlab */
    void createFromMatlab(const mxArray *mxImage);
    
    /*  Instantiates this instance with a patch of another IntegralImage
        specified by the given parameters.
        image: image to take patch from
        x: top-left x-dimension point of the patch
        y: top-left y-dimension point of the patch
        w: width to take
        h: height to take */
    void createFromIntegralImage(IntegralImage *image, int x, int y, int w, int h);
    
    /*  Instantiates this instance containing the contents of the bounding-box
        in the given image instance warped by the given matrix.
        image: image to create warp from
        bb: bounding-box [x, y, width, height]
        m: 2x2 transformation matrix (translation is added inside) */
    void createWarp(IntegralImage *image, double *bb, float *m);
    
    /*  Returns the sum of pixel intensities in the rectangular area
        designated by the given parameters.
        x: top-left x-position of rectangle
        y: top-left y-position of rectangle
        w: width of rectangle
        h: height of rectangle */
    int sumRect(int x, int y, int w, int h);
    
    /*  Getter for width. */
    int getWidth(void);
    
    /*  Getter for height. */
    int getHeight(void);
    
    /*  Destructor. */
    ~IntegralImage(void);
    
    
    // Protected =============================================================
    protected:
    /*  Sets the data field by reference, i.e. the array is not copied.
        d: data to set */
    void setData(int **d);
    
    /*  Returns the data field of this instance. */
    int **getData(void);
};
