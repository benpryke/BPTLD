/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#pragma once
#include "cv.h"
#include "highgui.h"
#include "IntegralImage.h"
#include "Classifier.h"
#include <math.h>

using namespace cv;


// Constants -----------------------------------------------------------------
// The number of points in a single dimension on the bounding-box
#define DIM_POINTS 10

// Total number of points on the bounding-box
#define TOTAL_POINTS (DIM_POINTS * DIM_POINTS)

// Defines the size of the search window in cvCalcOpticalFlowPyrLK
#define WINDOW_SIZE 4

// Maximal pyramid level number
// If 0, pyramids are not used (single level); if 1, two levels are used etc.
#define LEVEL 5



/*  The Median Flow Tracker.
    
    Algorithm:
    1) Initialise uniformly distributed set of points over the previous
       frame's trajectory patch
    2) Track these points using the Lucas-Kanade algorithm
    3) Estimate the tracking error of each point
    4) Filter out the tracked points with greatest error (typically 50% of
       points)
    5) Set the bounding-box scale and position for the current frame based on
       the median scale and position changes of the remaining points
    
    This implementation does not implement steps 3 and 4. */
class Tracker {
    // Private ===============================================================
    private:
    // Size of each frame
    int width;
    int height;
    
    // Previous video stream frame
    IplImage *prevFrame;
    
    // Buffers for the pyramids used by cvCalcOpticalFlowPyrLK
    IplImage *prevPyramid;
    IplImage *nextPyramid;
    
    // The coordinates of the points placed in the bounding-box
    // These points are those that are tracked
    // predPoints = predicted 1st frame points from tracking backwards
    CvPoint2D32f *prevPoints;
    CvPoint2D32f *nextPoints;
    //CvPoint2D32f *predPoints;
    
    // Size of the search window of each pyramid level in cvCalcOpticalFlowPyrLK
    CvSize *windowSize;
    
    // Status output array of cvCalcOpticalFlowPyrLK. Elements are set to 1 if
    // the flow for the corresponding feature has been found, otherwise 0
    // status is for forward tracking, predStatus is for backward tracking
    char *status;
    //char *predStatus;
    
    // Specifies the termination criteria of the iterative search algorithm in
    // cvCalcOpticalFlowPyrLK
    TermCriteria *termCriteria;
    
    // Pointer to the classifier for the entire program
    Classifier *classifier;
    
    /*  Returns the median of an array of float values.
        Note: has side-effect of sorting array A.
        A: the array
        length: length of array A */
    float median(float *A, int length);
    
    
    // Public ================================================================
    public:
    /*  Constructor. Initialises the tracker.
        frameWidth: width of the video stream frames
        frameHeight: height of the video stream frames
        frameSize: size of the video frame as a CvSize object
        firstFrame: the first video stream frame
        classifier: pointer to the classifier for the program */
    Tracker(int frameWidth, int frameHeight, CvSize *frameSize, IplImage *firstFrame, Classifier *classifier);
    
    /*  Tracks the region of the input frame indicated by the given
        bounding-box.
        Returns the estimated new bounding box
            [x, y, width, height, confidence].
        nextFrame: next video stream frame
        bb: array containing the trajectory bounding-box
            [x, y, width, height] */
    double *track(IplImage *nextFrameIpl, IntegralImage *nextFrame, double *bb);
    
    /*  Setter for prevFrame (also frees the current value of prevFrame). */
    void setPrevFrame(IplImage *frame);
    
    /*  Destructor. */
    ~Tracker();
};
