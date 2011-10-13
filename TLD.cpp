/*  Copyright 2011 Ben Pryke.
    This file is part of Ben Pryke's TLD Implementation available under the
    terms of the GNU General Public License as published by the Free Software
    Foundation. This software is provided without warranty of ANY kind. */

#include "cv.h"
#include "highgui.h"
#include "Detector.h"
#include "Classifier.h"
#include "Tracker.h"
#include <math.h>
#include <vector>


/// Globals ==================================================================
// Constants -----------------------------------------------------------------
// Number of ferns in the classifier
#define TOTAL_FERNS 10

// Number of nodes per fern
#define TOTAL_NODES 5

// Minimum percentage of patch width and height a feature can take
#define MIN_FEATURE_SCALE 0.1f

// Maximum percentage of patch width and height a feature can take
#define MAX_FEATURE_SCALE 0.5f

// Minimum confidence of the previous frame trajectory patch for us to learn
// this frame
#define MIN_LEARNING_CONF 0.8

// When a detected patch has higher confidence than the tracked patch, it must
// still have higher confidence than this for us to reinitialise
// Note: Should be <= MIN_LEARNING_CONF
#define MIN_REINIT_CONF 0.8

// Minimum confidence of the tracked patch in the previous frame for us to
// track in the next frame
#define MIN_TRACKING_CONF 0.1


// Variables -----------------------------------------------------------------
// Our classifier, tracker and detector
static Classifier *classifier;
static Tracker *tracker;
static Detector *detector;

// Lets us know whether TLD has been initialised or not
static bool initialised = false;

// Initial size of the bounding-box
static float initBBWidth;
static float initBBHeight;

// Size of each frame
int frameWidth;
int frameHeight;
CvSize *frameSize;

// Confidence of the previous frame's trajectory patch
double confidence;



/// Methods ==================================================================
/*  Converts an image received from Matlab to an IplImage.
    Returns: the converted image.
    mxImage: the image straight from Matlab */
IplImage *imageFromMatlab(const mxArray *mxImage) {
    // Get pointer
    unsigned char *values = (unsigned char *)mxGetPr(mxImage);
    
    // Create our return image
    IplImage *image = cvCreateImage(*frameSize, IPL_DEPTH_8U, 1);
    
    // Loop through the new image getting values from the old one
    // Note: values are effectively rotated 90 degrees
    for (int i = 0; i < frameWidth; i++) {
        for (int j = 0; j < frameHeight; j++) {
            image->imageData[j * frameWidth + i] = values[i * frameHeight + j];
        }
    }
    
    return image;
}


/*  Trains the classifier on warps of a bounding-box patch.
    frame: frame to take warps from
    bb: first-frame bounding-box [x, y, width, height] */
void bbWarpPatch(IntegralImage *frame, double *bb) {
    // Transformation matrix
    float *m = new float[4];
    
    // Loop through various rotations and skews
    for (float r = -0.1f; r < 0.1f; r += 0.005f) {
        float sine = sin(r);
        float cosine = cos(r);
        
        for (float sx = -0.1f; sx < 0.1f; sx += 0.05f) {
            for (float sy = -0.1f; sy < 0.1f; sy += 0.05f) {
                // Set transformation
                /*  Rotation matrix * skew matrix =
                    
                    | cos r   sin r | * | 1   sx | = 
                    | -sin r  cos r |   | sy   1 |
                    
                    | cos r + sy * sin r   sx * cos r + sin r |
                    | sy * cos r - sin r   cos r - sx * sin r | */
                m[0] = cosine + sy * sine;
                m[1] = sx * cosine + sine;
                m[2] = sy * cosine - sine;
                m[3] = cosine - sx * sine;
                
                // Create warp and train classifier
                IntegralImage *warp = new IntegralImage();
                warp->createWarp(frame, bb, m);
                classifier->train(warp, 0, 0, (int)bb[2], (int)bb[3], 1);
                delete warp;
            }
        }
    }
    
    delete m;
}


/*  Trains the classifier on negative training patches, i.e. patches from the
    first frame that don't overlap the bounding-box patch.
    frame: frame to take warps from
    tbb: first-frame bounding-box [x, y, width, height] */
void trainNegative(IntegralImage *frame, double *tbb) {
    // Minimum and maximum scales for the bounding-box, the number of scale
    // iterations to make, and the amount to increment scale by each iteration
    float minScale = 0.5f;
    float maxScale = 1.5f;
    int iterationsScale = 5;
    float scaleInc = (maxScale - minScale) / (iterationsScale - 1);
    
    // Loop through a range of bounding-box scales
    for (float scale = minScale; scale <= maxScale; scale += scaleInc) {
        int minX = 0;
        int currentWidth = (int)(scale * initBBWidth);
        int maxX = frameWidth - currentWidth;
        int iterationsX = 20;
        int incX = (maxX - minX) / (iterationsX - 1);
        
        // If bounding-box width >= frame width, make only 1 iteration of the
        // following for loop
        if (incX <= 0) {
            maxX = 0;
            incX = 1;
        }
        
        // Loop through all bounding-box top-left x-positions
        for (int x = minX; x <= maxX; x += incX) {
            // Same for y
            int minY = 0;
            int currentHeight = (int)(scale * initBBHeight);
            int maxY = frameHeight - currentHeight;
            int iterationsY = 20;
            int incY = (maxY - minY) / (iterationsY - 1);
            
            // If bounding-box height >= frame height, make only 1 iteration
            // of the following for loop
            if (incY <= 0) {
                maxY = 0;
                incY = 1;
            }
            
            // Loop through all bounding-box top-left x-positions
            for (int y = minY; y <= maxY; y += incY) {
                // Define the patch and test whether it's overlap with the
                // first-frame patch is less than MIN_LEARNING_OVERLAP, if
                // so, train as negative
                double *bb = new double[4];
                bb[0] = (double)x;
                bb[1] = (double)y;
                bb[2] = (double)currentWidth;
                bb[3] = (double)currentHeight;
                
                if (Detector::bbOverlap(tbb, bb) < MIN_LEARNING_OVERLAP) {
                    classifier->train(frame, x, y, currentWidth, currentHeight, 0);
                } else {
                    //classifier->train(frame, x, y, currentWidth, currentHeight, 1);
                }
                
                delete [] bb;
            }
        }
    }
}


/*  Entry point for mex.
    Call form: [left, hand, side, outs] = Detector(right, hand, side, args)
    Either use:
    To initialise:
        TLD(frame width, frame height, first frame, selected bounding-box)
    To process a frame:
        new trajectory bounding-box = TLD(current frame, trajectory bounding-box)
    
    nlhs: number of left-hand side outputs
    plhs: the left-hand side outputs
    nrhs: number of right-hand side arguments
    prhs: the right-hand side arguments */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    // Initialisation --------------------------------------------------------
    if (nlhs == 0 && nrhs == 4) {
        // Get input
        frameWidth = (int)*mxGetPr(prhs[0]);
        frameHeight = (int)*mxGetPr(prhs[1]);
        frameSize = (CvSize *)malloc(sizeof(CvSize));
        *frameSize = cvSize(frameWidth, frameHeight);
        IntegralImage *firstFrame = new IntegralImage();
        firstFrame->createFromMatlab(prhs[2]);
        IplImage *firstFrameIplImage = imageFromMatlab(prhs[2]);
        double *bb = mxGetPr(prhs[3]);
        initBBWidth = (float)bb[2];
        initBBHeight = (float)bb[3];
        confidence = 1.0f;
        
        // Initialise classifier, tracker and detector
        srand((unsigned int)time(0));
        classifier = new Classifier(TOTAL_FERNS, TOTAL_NODES, MIN_FEATURE_SCALE, MAX_FEATURE_SCALE);
        tracker = new Tracker(frameWidth, frameHeight, frameSize, firstFrameIplImage, classifier);
        detector = new Detector(frameWidth, frameHeight, bb, classifier);
        
        // Train the classifier on the bounding-box patch and warps of it
        classifier->train(firstFrame, (int)bb[0], (int)bb[1], (int)initBBWidth, (int)initBBHeight, 1);
        bbWarpPatch(firstFrame, bb);
        trainNegative(firstFrame, bb);
        
        // Free memory and set initialised
        delete firstFrame;
        initialised = true;
        
        return;
    }
    
    
    // Validate --------------------------------------------------------------
    // The remainder of this function handles the frame processing call
    // Ensure we get the correct call form Matlab and are initialised
    if (!initialised || nlhs != 1 || nrhs != 2) {
        // Error
        return;
    }
    
    
    // Get Input -------------------------------------------------------------
    // Current frame
    IplImage *nextFrame = imageFromMatlab(prhs[0]);
    IntegralImage *nextFrameIntImg = new IntegralImage();
    nextFrameIntImg->createFromMatlab(prhs[0]);
    
    // Trajectory bounding-box [x, y, width, height]
    double *bb = mxGetPr(prhs[1]);
    
    
    // Track and Detect ------------------------------------------------------
    // Only track if we were confident enough in the previous iteration
    // The tracker handles the memory freeing of nextFrame from here on
    double *tbb;
    vector<double *> *dbbs;
    
    if (confidence > MIN_TRACKING_CONF) {
        tbb = tracker->track(nextFrame, nextFrameIntImg, bb);
        dbbs = detector->detect(nextFrameIntImg, tbb);
    } else {
        dbbs = detector->detect(nextFrameIntImg, NULL);
        tracker->setPrevFrame(nextFrame);
        tbb = new double[5];
        tbb[0] = 0;
        tbb[1] = 0;
        tbb[2] = 0;
        tbb[3] = 0;
        tbb[4] = MIN_TRACKING_CONF;
    }
    
    
    // Learn -----------------------------------------------------------------
    // Get greatest detected patch confidence
    double dbbMaxConf = 0.0f;
    int dbbMaxConfIndex = -1;
    
    for (int i = 0; i < dbbs->size(); i++) {
        double dbbConf = dbbs->at(i)[4];
        
        if (dbbConf > dbbMaxConf) {
            dbbMaxConf = dbbConf;
            dbbMaxConfIndex = i;
        }
    }
    
    // Reset the tracker bounding-box if a detected patch had highest
    // confidence and is more confident than MIN_REINIT_CONF
    if (dbbMaxConf > tbb[4] && dbbMaxConf > MIN_REINIT_CONF) {
        delete tbb;
        tbb = new double[5];
        double *dbb = dbbs->at(dbbMaxConfIndex);
        tbb[0] = dbb[0];
        tbb[1] = dbb[1];
        tbb[2] = dbb[2];
        tbb[3] = dbb[3];
        tbb[4] = dbb[4];
    }
    
    // Apply constraints if the tracked patch had the greatest confidence and
    // we were confident enough last frame
    else if (tbb[4] > dbbMaxConf && confidence > MIN_LEARNING_CONF) {
        for (int i = 0; i < dbbs->size(); i++) {
            // Train the classifier on positive (overlapping with tracked
            // patch) and negative (classed as positive but non-overlapping)
            // patches
            double *dbb = dbbs->at(i);
            
            if (dbb[5] == 1) {
                classifier->train(nextFrameIntImg, (int)dbb[0], (int)dbb[1], (int)dbb[2], (int)dbb[3], 1);
            }
            else if (dbb[5] == 0) {
                classifier->train(nextFrameIntImg, (int)dbb[0], (int)dbb[1], (int)dbb[2], (int)dbb[3], 0);
            }
        }
    }
    
    // Set confidence for next iteration
    confidence = tbb[4];
    
    
    // Set output ------------------------------------------------------------
    // We output a list of bounding-boxes; the first bounding-box defines the
    // tracked patch, the rest are detected positive match patches.
    // Rows correspond to individual bounding boxes
    // Columns correspond to [x, y, width, height, confidence, overlapping]
    int bbCount = (int)dbbs->size() + 1;
    plhs[0] = mxCreateDoubleMatrix(bbCount, 6, mxREAL);
    double *outputBBs = mxGetPr(plhs[0]);
    
    // Set the tracked bounding-box
    outputBBs[0 * bbCount] = tbb[0];
    outputBBs[1 * bbCount] = tbb[1];
    outputBBs[2 * bbCount] = tbb[2];
    outputBBs[3 * bbCount] = tbb[3];
    outputBBs[4 * bbCount] = tbb[4];
    outputBBs[5 * bbCount] = 0;
    
    // Set detected bounding-boxes
    for (int i = 1; i < bbCount; i++) {
        double *bb = dbbs->at(i - 1);
        outputBBs[0 * bbCount + i] = bb[0];
        outputBBs[1 * bbCount + i] = bb[1];
        outputBBs[2 * bbCount + i] = bb[2];
        outputBBs[3 * bbCount + i] = bb[3];
        outputBBs[4 * bbCount + i] = bb[4];
        outputBBs[5 * bbCount + i] = bb[5];
        delete bb;
    }
    
    // Free memory
    free(tbb);
    dbbs->clear();
    delete nextFrameIntImg;
}


// Entry point to keep Visual Studio happy
int main() {}
