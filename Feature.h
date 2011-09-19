/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#pragma once
#include "IntegralImage.h"


// Constants -----------------------------------------------------------------
// In order to be able to concatenate numbers to indicate a leaf node in
// the classifier, each feature must return a range of values that is a
// power of 2. This is that power (i.e. 2 ^ POWER is the number of different
// possible return values). MUST be set correctly for the feature used.
#define POWER 2


/*  A superclass for the following feature classes:
        HaarTest: set POWER to 1 if using this
        TwoBitBPTest: set POWER to 2 if using this
    
    The single chosen subclass is used in the Fern class. */
class Feature {
    // Protected =============================================================
    protected:
    // Percentage position (x, y) on the patch of the top-left of our test
    float xp, yp;
    
    // Percentage scale (width, height) of our test on the patch
    float wp, hp;
    
    
    // Public ================================================================
    public:
    /*  Constructor.
    minScale: minimum percentage of the patch width and height the feature
        can take
    maxScale: maximum percentage of the patch width and height the feature
        can take */
    Feature(float minScale, float maxScale);
    
    /*  Tests the input patch using the feature. MUST be implemented.
        Returns the result of the feature.
        image: image to take patch from
        patchX: patch top-left x-position
        patchX: patch top-left y-position
        patchW: patch width
        patchH: patch height */
    int test(IntegralImage *image, int patchX, int patchY, int patchW, int patchH);
    
    /*  Destructor. */
    ~Feature();
};
