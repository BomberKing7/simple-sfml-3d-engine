# simple-sfml-3d-engine
this sfml engine is created on android  cxxdroid 

You can prees buttons to move areound 
Wich is simple on windows and android 

I have added 4 buttons on the left 
Left,Up,Right,Down buttons
And 6 buttons on the right 
To look up and down and right and left 
Changing the cameras vector 
And 2 buttons to go up and down

The code is super simple
using only two files
main.cpp
space.h

the main2.cpp is just used when writing anothee code that i may need to write txt

The loadFile()
function makes it easier to load structures from txt files

the language used in txt files ( diamond.txt , cube.txt and pyramid.txt )
Is ass following
p (float x) (float y) (float z)
add a point that has these coordinates 
t (int point1) (int point2) (int point3) (int red) (int blue) (int green) (int alpha)
add a triangle using the 3 points
and rbga color
Note that the point integer in the triangle is the index of the point in the point array
When ever you create a point you add it to the array 
Each file has its own array
(starting with 0)
 
r (int point1) (int point2) (int point3) (int point4) (int red) (int blue) (int green) (int alpha)

add a rectangle 
In general add two triangles
(point1,point2,point3)
(point1,point3,point4)

Note that the counter clockwise order lets us see the triangle
We see a triangle ONLY if edges are ordered counter clockwise according to the View 

and finally
c (int n) (int point1) ... (int pointn) (int red) (int blue) (int green) (int alpha)

To make it easier to create polygones and so on 

when you start the project move the camera up a little bit using the right up button 

Good luck and thanks for reading :)



