Purpose:
	This program is used to create a ray cast based on a scene input file. The program outputs a file in PPM format. The file supports lights, spheres, planes, and a single camera object. Shading and shadows are applied to elements to make a 3D effect.

Usage:
	The usage of this program is to input the following on the command line:
	raycast width height input.csv output.ppm

Known Issues:
    Planes are not drawing due to the intersection function not returning the correct "t" value. Spheres have inverted y-coordinates. Angular lighting calculations have not been added so lighting on objects is limited.

Author:
	Benjamin Hoffman
	Bth55@nau.edu