/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#include "Fern.h"


Fern::Fern(int nodeNum, float minScale, float maxScale) {
    nodeCount = nodeNum;
    int leafNodes = (int)pow(2.0f * (float)POWER, nodeCount);
    p = new int[leafNodes];
    n = new int[leafNodes];
    posteriors = new float[leafNodes];
    nodes = new TwoBitBPTest*[nodeCount];
    
    // Initialise the features
    for (int i = 0; i < nodeCount; i++) {
        nodes[i] = new TwoBitBPTest(minScale, maxScale);
    }
    
    // Initialise p, n, and posteriors
    for (int i = 0; i < leafNodes; i++) {
        p[i] = n[i] = 0;
        posteriors[i] = 0.0f;
    }
}


int Fern::getLeafIndex(IntegralImage *image, int patchX, int patchY, int patchW, int patchH) {
    // Limit patch to image area
    int width = image->getWidth();
    int height = image->getHeight();
    
    // Clamp x and y values between 0 and width and height respectively
    patchX = std::max(std::min(patchX, width - 2), 0);
    patchY = std::max(std::min(patchY, height - 2), 0);
    
    // Limit width and height values to (width - patchX) and (height - patchY)
    // respectively
    patchW = std::min(patchW, width - patchX);
    patchH = std::min(patchH, height - patchY);
    
    // Apply all tests to find the leaf index this patch falls into
    int leaf = 0;
    
    for (int i = 0; i < nodeCount; i++) {
        leaf = leaf | (nodes[i]->test(image, patchX, patchY, patchW, patchH) << i * (int)POWER);
    }
    
    return leaf;
}


void Fern::train(IntegralImage *image, int patchX, int patchY, int patchW, int patchH, int patchClass) {
    // Apply all tests to find the leaf index this patch falls into
    int leaf = getLeafIndex(image, patchX, patchY, patchW, patchH);
    
    // Increment the number of positive or negative patches that fell into
    // this leaf
    if (patchClass == 0) {
        n[leaf]++;
    }
    else {
        p[leaf]++;
    }
    
    // Compute the posterior likelihood of a positive class for this leaf
    if (p[leaf] > 0) {
        posteriors[leaf] = (float)p[leaf] / (float)(p[leaf] + n[leaf]);
    }
}


float Fern::classify(IntegralImage *image, int patchX, int patchY, int patchW, int patchH) {
    // Return the precomputed posterior likelihood of a positive class for
    // this leaf
    return posteriors[getLeafIndex(image, patchX, patchY, patchW, patchH)];
}


Fern::~Fern() {
    for (int i = 0; i < nodeCount; i++) {
        delete nodes[i];
    }

    delete [] nodes;
    delete [] p;
    delete [] n;
    delete [] posteriors;
}
