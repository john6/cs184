//
//  Ray.cpp
//  
//
//  Created by Shannon Axelrod on 3/4/14.
//
//

#include "Ray.h"

Ray::Ray(Point pos, Eigen::Vector3f dir, float t_min, float t_max){
    this->dir = dir;
    this->t_min = t_min;
    this->t_max = t_max;
    this->pos = pos;
}