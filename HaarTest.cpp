/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#include "HaarTest.h"


HaarTest::HaarTest(float minScale, float maxScale)
: Feature(minScale, maxScale) {
}


int HaarTest::test(IntegralImage *image, int patchX, int patchY, int patchW, int patchH) {
    // Compute the properties of the test rectangles relative to the size and
    // position of the patch
    int x = (int)(xp * (float)patchW) + patchX;
    int y = (int)(yp * (float)patchH) + patchY;
    int w = (int)(wp * (float)patchW * 0.5f);
    int h = (int)(hp * (float)patchH);
    
    // Compare the left and right halfs of the feature on the patch
    int left = image->sumRect(x, y, w, h);
    int right = image->sumRect(x + w, y, w, h);
    
    return (left > right ? 0 : 1);
}


HaarTest::~HaarTest() {
}
