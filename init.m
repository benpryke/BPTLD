% Copyright 2011 Ben Pryke.
% This file is part of Ben Pryke's TLD Implementation available under the
% terms of the GNU General Public License as published by the Free Software
% Foundation. This software is provided without warranty of ANY kind.

% Opens a figure displaying a raw camera stream and allows the user to
% define a bounding-box around the object.
% Returns [stream:      source video stream,
%          bb:          bounding box around object [x, y, width, height]
%          resolution:	dimensions of the video stream]

function [stream, bb, resolution] = init()
    
    % Data source initialisation ==========================================
    % Define the source as camera output
    stream = videoinput('winvideo', 1, 'YUY2_320x240');
    
    % Set some properties
    set(stream, 'ReturnedColorSpace', 'grayscale');
    resolution = get(stream, 'VideoResolution');
    bands = get(stream, 'NumberOfBands');
    sourceHandle = image(zeros(resolution(2), resolution(1), bands));
    
    % Display the raw camera stream in a figure
    preview(stream, sourceHandle);
    hold on;
    
    
    % Bounding-box initialisation =========================================
    % Display instructions and allow the user to draw a rectangle over the
    % object in the figure
    msg = 'Draw a box around the object and double-click inside';
    instructionsHandle = text(10, 10, msg, 'color', 'y', 'FontWeight', 'bold');
    
    % Wait for user to double-click inside a large enough rectangle
    errorMsg = 'Box not large enough!';
    bb = zeros(1, 4);
    
    while (bb(3) < 40 || bb(4) < 40)
        rectHandle = imrect;
        bb = wait(rectHandle);
        delete(rectHandle);
        delete(instructionsHandle);
        instructionsHandle = text(10, 10, errorMsg, 'color', 'y', 'FontWeight', 'bold');
    end
    
    % Remove the instructions from the figure
    delete(instructionsHandle);
    
end
