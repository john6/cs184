//
//  Ray.h
//  
//
//  Created by Shannon Axelrod on 3/4/14.
//
//

#ifndef ____Ray__
#define ____Ray__

#include <iostream>
#include "Point.h"

#endif /* defined(____Ray__) */

class Ray{
public:
    Ray(Point, Eigen::Vector3f, float, float);
    Eigen::Vector3f dir;
    float t_min, t_max;
    Point pos;
    
};

