/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#include "TwoBitBPTest.h"


TwoBitBPTest::TwoBitBPTest(float minScale, float maxScale)
: Feature(minScale, maxScale) {
}


int TwoBitBPTest::test(IntegralImage *image, int patchX, int patchY, int patchW, int patchH) {
    // Compute the properties of the test rectangles relative to the size and
    // position of the patch
    int x = (int)(xp * (float)patchW) + patchX;
    int y = (int)(yp * (float)patchH) + patchY;
    int w = (int)(wp * (float)patchW * 0.5f);
    int h = (int)(hp * (float)patchH * 0.5f);
    
    // Compare the various halfs of the feature on the patch
    int left = image->sumRect(x, y, w, h * 2);
    int right = image->sumRect(x + w, y, w, h * 2);
    int top = image->sumRect(x, y, w * 2, h);
    int bottom = image->sumRect(x, y + h, w * 2, h);
    
    if (left > right) {
        if (top > bottom) {
            return 0;
        }
        else {
            return 1;
        }
    }
    else {
        if (top > bottom) {
            return 2;
        }
        else {
            return 3;
        }
    }
    
    // This also does it: return (left > right ? 0 : 2) + (top > bottom ? 0 : 1);
}


TwoBitBPTest::~TwoBitBPTest() {
}
