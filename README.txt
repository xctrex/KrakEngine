Tommy Walton
CS 560 Project 4

Algorithms
 - The numerical analysis algorithm I used for this project is the Euller method.
 - I chose option B, a 27+ mass point cube with two anchor points and vertical, horizontal, and diagonal springs
 - Standard physics laws are used to compute the forces.
 
Running the project
 - Once built, the executable should be moved to the KrakEngine\KrakEngine folder (the one that contains d3dcompiler_46.dll and lua5.1.dll).
    Double click on KrakEngine\KrakEngine.exe to launch the project.

Controls
 - WASD to move the camera position
 - x and z to move up and down the y axis
 - Move the mouse to look around.
 - Hit e to edit the anchor points. Left click to make the nearest point an anchor point.
   Right click to make the nearest anchor point no longer an anchor point.
 - Hit e again to go back to moving the camera.
 - Hit Esc. to bring up the main menu and choose quit game to exit the application.
 
 Mass points are rendered green, springs are orange, and anchor points are red
 
Function/File Locations
 - The algorithms for this project are in PhysicsSystem.cpp.
 - The rigid body struct used for this project is rBody, located in RBody.h
 Not to be confused with the RigidBody class, which is leftover code from an old game project and not used here.
 - The CubeForce functions add up the forces on each mass point
 - The ode runs the numerical analysis
 
Known Issues
 - The dampening does not work completely, so the non-anchored points do not settle on a stable solution.
 - Sometimes the project wigs out and either throws an unhandled exception or render's the world upside down or scrambled
 the first time it is run. I believed this is a cache issue, because it usually goes away as soon as I re-run the project a second time.
