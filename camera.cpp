//
//  camera.cpp
//  
//
//  Created by Shannon Axelrod on 2/23/14.
//
//

#include "camera.h"
#include <iostream>
#include <Eigen/Dense>

//Global Variables

Eigen::Vector3f lookfromCoord

//Camera(float lookfromX, float lookfromY, float lookfromZ, float lookatX, float lookatY, float lookatZ,float upX, float upY, float upZ, float fovY, float near, int width, int height){
//    
//    this.lookfromVec = Eigen::Vector3f lookfromCoord(lookfromX, lookfromY, lookfromZ);
//    this.lookatVec = Eigen::Vector3f lookatCoord(lookatX, lookatY, lookatZ);
//    this.upVec = Eigen::Vector3f upVecCoord(upX, upY, upZ);
//    this.fovY = fovY;
//    this.near = near;
//    this.width = width;
//    this.height = height; 
//    this.pixelLength = fovY/height;
//    this.fovX = width * pixelLength;
//    this.topRightCorner = Eigen::Vector3f screenCornerCoord((this.lookatVec[1] - (fovX/2)), (this.lookatVec[2] + (fovY/2)), upZ);
//
//    
//}
//void pixelCoord(int i, int j){
//	Eigen::Vector3f pixelLocation(this.topRightCorner[0] + (i * pixelLength) (this.pixelLength/2), this.topRightCorner[1] - (i * pixelLength) - (this.pixelLength/2), this.topRightCorner[2])
//}

//void generateRay(Sample &sample, Ray *ray){
    //ray(t) = lookFromVec + t(lookAtVec - lookFromVec)
//}
