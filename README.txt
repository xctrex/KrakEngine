Tommy Walton
CS 560 Project 2

Algorithms
 - The spline interpolation algorithm I used for MAT500 is used to interpolate between 8 control points with first order continuity.
 - The arc length table is built using the adaptive approach. It is then normalized, and a binary search is used for the inverse arc length function.
 - Smoothstep is used to ease-in/out of the path.
 - The center of interest approach is used to control orientation.
 
Running the project
 - Once built, the executable should be moved to the KrakEngine\KrakEngine folder (the one that contains d3dcompiler_46.dll and lua5.1.dll).
    Double click on KrakEngine\KrakEngine.exe to launch the project.

Controls
 - WASD to move the camera position
 - Move the mouse to look around
 - Hit e to edit the path. Click and drag to move control points.
 - Hit e again to go back to moving the camera.
 - Hit Esc. to bring up the main menu and choose quit game to exit the application
 
File Locations
 - The algorithms for this project are in GraphicsSystem.h and GraphicsSystem.cpp. The bottom of GraphicsSystem.h has
 the functions and variables for this project grouped together.
 - Most of the work is done in the UpdateAnimation function in GraphicsSystem.cpp except for the path generation which is
 in the UpdateLinearSystem function in GraphicsSystem.cpp
 
Known Issues
 - To render the skeleton and path, I use a world to screen coordinate conversion function. It uses 
 XMVector3Project to determine the screen coordinate of both endpoints of a bone,
 then uses Direct2D to render a line between those two points. This process works well, 
 however there is an issue with the conversion if the point being converted is actually
 located behind the camera. If you keep the entire path in view of the camera, this will not be an issue.
 I want to point out that it's not an issue with the path calculation, but rather the 2D projection.
 - Sometimes the project wigs out and either throws an unhandled exception or render's the world upside down or scrambled
 the first time it is run. I believed this is a cache issue, because it usually goes away as soon as I re-run the project a second time.
