/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#pragma once
#include "Fern.h"
#include "IntegralImage.h"


/*  A random forst classifier. */
class Classifier {
    // Private ===============================================================
    private:
    // Array of ferns
    Fern **ferns;
    
    // Number of ferns
    int fernCount;
    
    
    // Public ================================================================
    public:
    /*  Constructor.
        fernNum: the number of ferns to create
        nodeNum: number of nodes to create in each fern
        minScale: minimum percentage of patch width and height a feature can
            take
        maxScale: maximum percentage of patch width and height a feature can
            take*/
    Classifier(int fernNum, int nodeNum, float minScale, float maxScale);
    
    /*  Trains all ferns in the forest with a single training patch.
        image: image to take the training patch from
        patchX: patch top-left x-position
        patchX: patch top-left y-position
        patchW: patch width
        patchH: patch height
        patchClass: 0 if the patch is negative, 1 if the patch is positive */
    void train(IntegralImage *image, int patchX, int patchY, int patchW, int patchH, int patchClass);
    
    /*  Classifies a given patch.
        Returns the posterior likelihood that the patch is positive. If the
        result is greater than 0.5 the patch is positive, otherwise negative.
        image: image to take the test patch from
        patchX: patch top-left x-position
        patchX: patch top-left y-position
        patchW: patch width
        patchH: patch height */
    float classify(IntegralImage *image, int patchX, int patchY, int patchW, int patchH);
    
    /*  Destructor. */
    ~Classifier(void);
};
