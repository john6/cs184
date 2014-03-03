//
//  camera.cpp
//  
//
//  Created by Shannon Axelrod on 2/23/14.
//
//

#include "camera.h"


Camera::Camera(float lookfromX, float lookfromY, float lookfromZ, float lookatX, float lookatY, float lookatZ, float upX, float upY, float upZ, float fovY, float near, int width, int height){
    
    this.lookfromVec = Eigen::Vector3f lookfromCoord(lookfromX, lookfromY, lookfromZ);
    this.lookatVec = Eigen::Vector3f lookatCoord(lookatX, lookatY, lookatZ);
    this.upVec = Eigen::Vector3f upVecCoord(upX, upY, upZ);
    this.fovY = fovY;
    this.near = near;
    this.width = width;
    this.height = height; 
    this.pixelLength = fovY/height;
    this.fovX = width * pixelLength;
    this.topRightCorner = Eigen::Vector3f screenCornerCoord((this.lookatVec[1] - (fovX/2)), (this.lookatVec[2] + (fovY/2)), upZ);

    
}
Camera::pixelCoord(int i, int j){
	pixelLocation = Eigen::Vector3f pixelCoord(this.topRightCorner[0] + (i * pixelLength) (this.pixelLength/2), this.topRightCorner[1] - (i * pixelLength) - (this.pixelLength/2), this.topRightCorner[2])
}

void Camera::generateRay(Sample &sample, Ray *ray){
    //ray(t) = lookFromVec + t(lookAtVec - lookFromVec)
}
