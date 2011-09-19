/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#pragma once
#include "Feature.h"
#include "IntegralImage.h"
#include <cstdlib>


/*  A test on a patch using 2-bit Binary Pattern (2bitBP) features as
    suggested in the paper 'Online learning of robust object detectors during
    unstable tracking'.
    
    Each comparison is between the summed intensity of an area of the patch.
    We test whether the left-half is greater than the right-half. If so, we
    return 0 if the top-half is more intense than the bottom-half, otherwise
    we return 1 for this case. If however the right-half was more intense, we
    return 2 if the top-half is more intense than the bottom-half, otherwise
    we return 3. This is depicted in the diagram below, indicating the
    location of the darkest patch.
    
    ---------
    | 0 | 2 |
    ---------
    | 1 | 3 |
    ---------
    
    Tests are performed on integral images (using the IntegralImage class) for
    efficiency.
    
    Note: Bare in mind the TOTAL_NODES constant in TLD.cpp. Using
    this feature creates 4 ^ TOTAL_NODES leaf-nodes, and too many will cause
    an attempt to allocate too much memory. (4 ^ 10 = 1048576.) Typically, a
    value of TOTAL_NODES = 5-6 is chosen when using this feature in a Fern. */
class TwoBitBPTest : public Feature {
    // Public ================================================================
    public:
    /*  Constructor.
        minScale: minimum percentage of the patch width and height the feature
            can take
        maxScale: maximum percentage of the patch width and height the feature
            can take */
    TwoBitBPTest(float minScale, float maxScale);
    
    /*  Tests the input patch.
        Returns 0-3 depending (see class description).
        image: image to take patch from
        patchX: patch top-left x-position
        patchX: patch top-left y-position
        patchW: patch width
        patchH: patch height */
    int test(IntegralImage *image, int patchX, int patchY, int patchW, int patchH);
    
    /*  Destructor. */
    ~TwoBitBPTest();
};
