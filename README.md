#Object-Property-Extractor

## Description

This is a YARP modules that extract property of a target object. This module was designed to work with the output of the zeroDisparityFilter module. The zeroDisparityFilter provide a segmented image of an object at zero disparity, on this <b><i>template</i></b> the extraction of properties is done. 

The **template** is as subregion of the complete image where the target object is located.

This module deal with numerical values, the aim is to connect it with a fuzzy logic module that converts it to word. 

## Input Ports
- **/imageRGB:i**: RGB image of the targeted object, the image need to represent only the object.

- **/object2DPos** : Position of the left top corner of the object in the complete image.

## Output Ports

- **/features** : string lists of the features
	- color
	- 2D position ( center of the object ie the image )
	- 3D position 
	- size ( diagonal of the object ie the image )
	

## Dependencies
- YARP
- OpenCV


## Author 
>Gonzalez Jonas ( jonas.gonzalez@iit.it )