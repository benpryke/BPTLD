/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#pragma once
#include "Classifier.h"
#include <vector>

using namespace std;


// Constants -----------------------------------------------------------------
// The minimum percentage overlap between a tracked and detected patch that
// counts as an overlap
#define MIN_LEARNING_OVERLAP 0.6


/*  Object detector implemented using a sliding-window approach. */
class Detector {
    // Private ===============================================================
    private:
    // Pointer to the classifier for the entire program
    Classifier *classifier;
    
    // Size of each frame
    int width;
    int height;
    
    // Initial size of the bounding-box
    float initBBWidth;
    float initBBHeight;
    
    
    // Public ================================================================
    public:
    /*  Constructor.
        frameWidth: width of the video stream frames
        frameHeight: height of the video stream frames
        bb: array containing the trajectory bounding-box
            [x, y, width, height]
        classifier: pointer to the classifier for the program */
    Detector(int frameWidth, int frameHeight, double *bb, Classifier *classifier);
    
    /*  Detects the object in the given frame.
        Returns a pointer to a vector of bounding-box arrays each
            containing [x, y, width, height, confidence, overlapping] that
            are either positive, or negative and overlapping with the
            trajectory bounding-box.
        frame: current frame as an IntegralImage; this is NOT freed
        tbb: tracked bounding-box this frame [x, y, width, height] */
    vector<double *> *detect(IntegralImage *frame, double *tbb);
    
    /*  Returns the intersection between two bounding boxes as a percentage of
        their total area.
        bb1: first bounding-box [x, y, width, height]
        bb2: second bounding-box [x, y, width, height] */
    static double bbOverlap(double *bb1, double *bb2);
    
    /*  Destructor. */
    ~Detector();
};
