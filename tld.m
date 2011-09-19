% Copyright 2011 Ben Pryke.
% This file is part of Ben Pryke's TLD Implementation available under the
% terms of the GNU General Public License as published by the Free Software
% Foundation. This software is provided without warranty of ANY kind.

% Runs TLD using your webcam

% Initialisation ==========================================================
% Get video stream and object bounding box
% globals.stream: video stream
% globals.tbb: trajectory bounding-box of previous frame
% globals.resolution: video stream resolution (width, height)
[globals.stream, globals.tbb, globals.resolution] = init();

% Get the 1st frame
% globals.frame: current frame from the video stream
globals.frame = getsnapshot(globals.stream);

% Initialise the tracker, detector, and classifier
TLD(globals.resolution(1), globals.resolution(2), globals.frame, globals.tbb);


% Run-time Loop ===========================================================
while 1
    % Start frame timer
    tic;
    
    % Get current frame
    globals.frame = getsnapshot(globals.stream);
    
    % Run tracker and detector
	% globals.dbbs: detected positive match bounding-boxes in rows
    % [x, y, width, height, confidence, overlapping; ...]
    bbs = TLD(globals.frame, globals.tbb);
    globals.tbb = bbs(1, :);
    globals.dbbs = bbs(2:length(bbs(:, 1)), :);
    
    % Stop timer, update fps, draw trajectory bounding-box
    updateDisplay(globals, toc);
end
