Tommy Walton
CS 560 Project 2

Algorithms
 - The spline interpolation algorithm I used for project 3 is used to interpolate between two control points to path to the object
   when it is out of range. I restricted the target point to the x and y planes, so this ends up just being a straight line along the x axis
 - Cyclic Coordinate Descent is used for the IK algorithm. It operates on a manipulator with 6 links that have joints that can rotate
   in a 2D plane (along the x & y axis). The algorithm enforces a priority such that the furthest link from the root has the highest priority
   and each successive link decreases in priority down to the root. It also imposes constraints on the rotation at each joint. The outermost
   joint has 360 degrees of freedom, and each successive joint beneath it has less, all the way down to the root join which cannot rotate at all.
 
Running the project
 - Once built, the executable should be moved to the KrakEngine\KrakEngine folder (the one that contains d3dcompiler_46.dll and lua5.1.dll).
    Double click on KrakEngine\KrakEngine.exe to launch the project.

Controls
 - WASD to move the camera position
 - x and z to move up and down the y axis
 - Move the mouse to look around.
 - Hit e to edit the target (the small red circle). Click anywhere to move the target to that point on the z = 0 plane.
 - Hit e again to go back to moving the camera.
 - Hit Esc. to bring up the main menu and choose quit game to exit the application.
 
Function/File Locations
 - The algorithms for this project are in GraphicsSystem.cpp. And Animation.cpp
 - The UpdateAnimation function in GraphicsSystem.cpp manages the overall flow of the program. It switches between interpolating
 the root along a path to move closer to the target, processing the IK to look for a solution, and interpolating/animating the manipulator
 once an IK solution has been found that will move the end effector to the target.
 - Inside Animation.cpp, ProcessIK2D() runs one iteration of the Cyclic Coordinate Descent algorithm, and returns true if either the end
effector has reached its target or the difference between this iteration and the previous one is very small. It is up to the caller (in this
case the UpdateAnimation() function in GraphicsSystem.cpp) to call this function multiple times until it reaches a solution.
CalculatePosition2D() iterates through the rotations for each joint from the root to the end effector and sets the positions based on each rotation and the length of each link.
RenderSkeleton2D() actually renders the manipulator.
 
Known Issues
 - Currently, if there is no solution to the IK that enables the end effector to reach the target, the manipulator paths towards the target
 to try and move within reach. This works, however if after the first path the end effector still cannot reach the target due to its constraints,
 it will not path to a new position, so it will just flail where it is until a new position is selected for the target.
 - Sometimes the project wigs out and either throws an unhandled exception or render's the world upside down or scrambled
 the first time it is run. I believed this is a cache issue, because it usually goes away as soon as I re-run the project a second time.
