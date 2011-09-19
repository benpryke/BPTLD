% Copyright 2011 Ben Pryke.
% This file is part of Ben Pryke's TLD Implementation available under the
% terms of the GNU General Public License as published by the Free Software
% Foundation. This software is provided without warranty of ANY kind.

% Set typical include and library paths depending on the operating system
% Update the paths if yours differ
if ispc
    include = ' -IC:\OpenCV2.2\include\opencv\ -IC:\OpenCV2.2\include\';
    libpath = 'C:\OpenCV2.2\lib\';
    files = dir([libpath '*.lib']);
elseif ismac
    include = ' -I/opt/local/include/opencv/ -I/opt/local/include/';
    libpath = '/opt/local/lib/';
    files = dir([libpath 'libopencv*.dylib']);
elseif isunix
    include = ' -I/usr/local/include/opencv/ -I/usr/local/include/';
    libpath = '/usr/local/lib/';
    files = dir([libpath 'libopencv*.so*']);
end

% Make a list of all library files
libs = [];
for i = 1:length(files)
    libs = [libs ' ' libpath files(i).name];
end

% Compiles the program
eval(['mex -O TLD.cpp Classifier.cpp Tracker.cpp Detector.cpp ' ... 
    'IntegralImage.cpp Feature.cpp HaarTest.cpp TwoBitBPTest.cpp ' ... 
    'Fern.cpp' include libs]);
