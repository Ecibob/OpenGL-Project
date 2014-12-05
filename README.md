OpenGL Demo
==============
This demo was made for an upper division Computer Graphics class. You can run the .cpp file directly, or build using make. This software was written on a linux machine, so if you aren't running a linux build, you will have to change your glu.h and glut.h paths.

Instructions:
==============
* Left and Right arrow keys will rotate the teapot on the ground
* Up and Down arrow keys (and 'w' and 's' keys) will zoom in and out on the center of the platform
* 'q' and 'e' will rotate the entire scene
* 'd' will drop the ball, where it will bounce a few time before coming to a rest
* 'p' will push the ball, where it will roll until coming to rest. NOTE: this only works if the ball has been dropped
* Clicking the scene will move the ball and teapot towards the sector that was clicked. NOTE: This only works properly on an unrotated scene. Rotating the scene 180 degrees will cause the movements to be mirrored.
* 'r' will reset the scene and camera.
