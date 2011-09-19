/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#pragma once
#include "Feature.h"
#include "HaarTest.h"
#include "IntegralImage.h"
#include "TwoBitBPTest.h"
#include <algorithm>
#include <math.h>


/*  Implementation of a random fern. */
class Fern {
    // Private ===============================================================
    private:
    // Array of nodes
    TwoBitBPTest **nodes;
    
    // Number of nodes
    int nodeCount;
    
    // Array containing the number of positive patches that fell into each
    // leaf node
    int *p;
    
    // Array containing the number of negative patches that fell into each
    // leaf node
    int *n;
    
    // Array containing the precomputed posterior likelihoods that each leaf
    // node is positive. Values are computed during training so that this
    // isn't required during classification
    float *posteriors;
    
    /*  Computes the index of the leaf node a patch falls into.
        Returns the index.
        image: image to take patch from
        patchX: patch top-left x-position
        patchX: patch top-left y-position
        patchW: patch width
        patchH: patch height */
    int getLeafIndex(IntegralImage *image, int patchX, int patchY, int patchW, int patchH);
    
    
    // Public ================================================================
    public:
    /*  Constructor.
        nodeNum: number of nodes to create
        minScale: minimum percentage of patch width and height a feature can
            take
        maxScale: maximum percentage of patch width and height a feature can
            take */
    Fern(int nodeNum, float minScale, float maxScale);
    
    /*  Trains this fern with a single training patch.
        image: image to take the training patch from
        patchX: patch top-left x-position
        patchX: patch top-left y-position
        patchW: patch width
        patchH: patch height
        patchClass: 0 if the patch is negative, 1 if the patch is positive */
    void train(IntegralImage *image, int patchX, int patchY, int patchW, int patchH, int patchClass);
    
    /*  Classifies a given patch.
        Returns the posterior liklihood that the patch is positive.
        image: image to take the test patch from
        patchX: patch top-left x-position
        patchX: patch top-left y-position
        patchW: patch width
        patchH: patch height */
    float classify(IntegralImage *image, int patchX, int patchY, int patchW, int patchH);
    
    /*  Destructor. */
    ~Fern(void);
};
