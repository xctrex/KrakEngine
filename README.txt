Tommy Walton
CS 560 Project 1

Interpolation Algorithms
 - The Interpolate function for two VQSs uses Lerp for translation, Slerp (using the Quaternion class) for rotation, and Elerp for scale. 

Running the project
 - Once built, the executable should be moved to the AetherEngine\AetherEngine folder (the one that contains d3dcompiler_46.dll and lua5.1.dll).
    Double click on AetherEngine\AtherEngine.exe to launch the project.

Controls
 - WASD to move the camera position
 - Z to move down along the y axis
 - X to move up along the y axis
 - Move the mouse to look around
 - G to toggle the rendering modes (animated skeleton, skeleton in bind pose, mesh in bind pose, and mesh in bind pose with skeleton) 
 - F to toggle model 
 - Hit Esc. to bring up the main menu and choose quit game to exit the application
 
File Locations
 - The binary files for the models being rendered are in AetherEngine\AetherEngine\Assets\Models
 - The VQS and Quaternion classes are in VQS.h/cpp
 - The code for loading/rendering the bones and mesh is in Model.h/cpp and Mesh.h/cpp
    There is some irrelevant legacy code in the Model.h/cpp files. The relevant functions
    for this project are Update, Draw, DrawBones, and LoadBinaryModel.
 - The code that controls the animation is in Animation.h/cpp
 
Known Issues
 - To render the skeleton, I use a world to screen coordinate conversion function. It uses 
 XMVector3Project to determine the screen coordinate of both endpoints of a bone,
 then uses Direct2D to render a line between those two points. This process works well, 
 however there is an issue with the conversion if the point being converted is actually
 located behind the camera. This can most easily be visualized by doing a 180 degree turn 
 from the start and face away from the model, at which point you will see the model
 projected upside down in front of you. This generally does not cause any issues with the 
 rendering, however if you move to a point where some of the bones are behind the camera
 and some are in front, the bone drawing will be rendered improperly as some of those points 
 will be projected away. I want to point out that this is not an issue with the interpolation
 or calculation of the bone position in world space, only with the conversion function I am 
 using to get the screen space coordinates needed for line drawing with Direct2D.
 - WASD movement behaves strangely if you move to the other side of the model