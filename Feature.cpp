/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#include "Feature.h"


Feature::Feature(float minScale, float maxScale) {
    // Generate random scales between minScale and maxScale
    wp = (maxScale - minScale) * ((float)rand() / (float)RAND_MAX) + minScale;
    hp = (maxScale - minScale) * ((float)rand() / (float)RAND_MAX) + minScale;
    
    // Generate random position between 0 and width/height
    xp = (1.0f - wp) * ((float)rand() / (float)RAND_MAX);
    yp = (1.0f - hp) * ((float)rand() / (float)RAND_MAX);
}


int Feature::test(IntegralImage *image, int patchX, int patchY, int patchW, int patchH) {
    printf("ERROR: FEATURE SUPERCLASS NOT USED CORRECTLY!\n");
    return 0;
}


Feature::~Feature() {
}
