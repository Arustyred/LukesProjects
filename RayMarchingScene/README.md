Ray Marching Scene is a personal project using glew, glfw, and other graphics libraries to create a simple scene rendered using a technique called ray marching.
Similar to raycasting, ray marching uses vectors and collision detection for rendering objects rather than rasterization.

The difference between the two techniques is that raycasting will shoot a single infinite ray per pixel while ray marching will shoot a ray that only detects 
collisions withing a certain distance. This distance is determined by a "distance function" which given a point, will return the maximum distance a ray in
any direction can move without colliding with any objects. The ray is shot again in the same direction from the new position using the distance function again
and the process is repeated until the distance function returns a value less than a defined epsilon value. At this point, the ray has essentially collided with
an object and the color for that pixel can then be determined.

This program produces a scene that looks like the following image.
![Scene](/Images/scene.png)
