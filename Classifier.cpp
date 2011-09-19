/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#include "Classifier.h"


Classifier::Classifier(int fernNum, int nodeNum, float minScale, float maxScale) {
    // Initialise the ferns
    ferns = new Fern*[fernNum];
    fernCount = fernNum;

    for (int i = 0; i < fernCount; i++) {
        ferns[i] = new Fern(nodeNum, minScale, maxScale);
    }
}


void Classifier::train(IntegralImage *image, int patchX, int patchY, int patchW, int patchH, int patchClass) {
    // Train all the ferns
    for (int i = 0; i < fernCount; i++) {
        ferns[i]->train(image, patchX, patchY, patchW, patchH, patchClass);
    }
}


float Classifier::classify(IntegralImage *image, int patchX, int patchY, int patchW, int patchH) {
    // Calcualte the average fern posterior likelihood
    float sum = 0.0f;
    
    for (int i = 0; i < fernCount; i++) {
        sum += ferns[i]->classify(image, patchX, patchY, patchW, patchH);
    }
    
    return sum / fernCount;
}


Classifier::~Classifier() {
    for (int i = 0; i < fernCount; i++) {
        delete ferns[i];
    }
    
    delete [] ferns;
}
