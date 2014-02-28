//
//  Scene.cpp
//  
//
//  Created by Shannon Axelrod on 2/23/14.
//
//

#include "Scene.h"

// This is the main rendering loop
void render() {
    while (!sampler.generateSample(&sample) {
        camera.generateRay(sample, &ray);
        raytracer.trace(ray, &color);
        film.commit(sample, color);
    }
           film.writeImage();
           }

