/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#pragma once
#include "Feature.h"
#include "IntegralImage.h"
#include <cstdlib>


/*  A test on a patch using a Haar-like feature. This means we compare the sum
    of pixel intensities in two halves of a rectangle placed on a patch,
    returning 0 if the left patch intensity is greatest, otherwise 1.
    The area to test is randomly chosen as a percentage of patch size to allow
    for variable sized test patches.
    Tests are performed on integral images (using the IntegralImage class) for
    efficiency.
    
    Note: srand(time(0)) should be called before initialising any instances
    of HaarTest.
    
    Note: typically a value of TOTAL_NODES = 13 is chosen in TLD.cpp when
    using this feature in Fern. */
class HaarTest : public Feature {
    // Public ================================================================
    public:
    /*  Constructor.
        minScale: minimum percentage of the patch width and height the feature
            can take
        maxScale: maximum percentage of the patch width and height the feature
            can take */
    HaarTest(float minScale, float maxScale);
    
    /*  Tests the input patch.
        Returns 0 if the left area intensity is greatest, otherwise 1.
        image: image to take patch from
        patchX: patch top-left x-position
        patchX: patch top-left y-position
        patchW: patch width
        patchH: patch height */
    int test(IntegralImage *image, int patchX, int patchY, int patchW, int patchH);
    
    /*  Destructor. */
    ~HaarTest();
};
