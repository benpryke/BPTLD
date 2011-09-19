% Copyright 2011 Ben Pryke.
% This file is part of Ben Pryke's TLD Implementation available under the
% terms of the GNU General Public License as published by the Free Software
% Foundation. This software is provided without warranty of ANY kind.

% Updates the information displayed in the figure.
% Input
% globals: globals variable
% t: time (double scalar) taken to update; result of 'toc' function
% Returns nothing

function updateDisplay(globals, t)
    
    % Delete all content on the current figure
    h = get(gca, 'Children');
    delete(h(1:end - 1));
    
    % Display the fps
    text(10, 10, [num2str(round(1 / t)) ' fps'], 'color', 'y');
    
    % Display the detector bounding-boxes
%     for i = 1:length(globals.dbbs(:, 1))
%         % We draw in green if the bounding-box was a positive training
%         % example, and in black for negative examples
%         if (globals.dbbs(i, 6) == 0)
%             colour = 'k';
%         else
%             colour = 'g';
%         end
%         
%         rectangle('Position', globals.dbbs(i, 1:4), 'EdgeColor', colour);
%     end
    
    % If valid, display the trajectory bounding-box (drawn after detected
    % bounding-boxes so this rectangle is always visible)
    if globals.tbb(3) > 0 && globals.tbb(4) > 0
        rectangle('Position', globals.tbb(1:4), 'EdgeColor', 'y', 'LineWidth', 2);
    end
    
    % Force Matlab to draw now
    drawnow;
    
end
