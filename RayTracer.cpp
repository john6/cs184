//
//  RayTracer.cpp
//  
//
//  Created by Shannon Axelrod on 2/24/14.
//
//

#include "RayTracer.h"
#include "Color.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define width 240
#define height 240

Color buffer[width][height];
/*

void trace(Ray& ray, int depth, Color* color) {
    if (depth exceed some threshold) {
        Make the color black and return
    }
    if (!primitive.intersect(ray, &thit, &in) {
        // No intersection
        Make the color black and return
    }
        // Obtain the brdf at intersection point
        in.primitive->getBRDF(in.local, &brdf);
        
        // There is an intersection, loop through all light source
        for (i = 0; i < #lights; i++) {
            lights[i].generateLightRay(in.local, &lray, &lcolor);
            
            // Check if the light is blocked or not
            if (!primitive->intersectP(lray))
                // If not, do shading calculation for this
                // light source
                *color += shading(in.local, brdf, lray, lcolor);
        }
        
        // Handle mirror reflection
        if (brdf.kr > 0) {
            reflectRay = createReflectRay(in.local, ray);
            
            // Make a recursive call to trace the reflected ray
            trace(reflectRay, depth+1, &tempColor);
            *color += brdf.kr * tempColor;
        }
        
        }
 */



        int outputFrame(){
        FILE *frameFile;
        char filename[80];
        int i,j;
        int     errno;
            
            //glReadPixels(0,0,frameWidth,frameHeight,GL_RGBA,GL_UNSIGNED_BYTE,frameBuffer);
            sprintf(filename,"image1.ppm");
            printf("output to file %s\n",filename);
            
            if ( (frameFile = fopen(filename,"w")) == NULL) {
                printf("** outputFrame error: fopen of frame file failed\n");
                exit(1);
            }
        
                
                if ( (errno = fprintf(frameFile,"P3\n")) < 0) {
                    printf("** outputFrame error: print to file error (%d)\n",errno);
                }
                fprintf(frameFile,"%d %d\n",width,height);
                fprintf(frameFile,"255\n");
                for (j=height-1; j>=0; j--) {
                    for (i=0; i<width; i++) {
                        fprintf(frameFile,"%u %u %u ",
                                (unsigned char)(255*buffer[i][j].r),
                                (unsigned char)(255*buffer[i][j].g),
                                (unsigned char)(255*buffer[i][j].b));
                    }
                    fprintf(frameFile,"\n");
                }
                if ( (errno = fclose(frameFile)) != 0) {
                    printf("** outputFrame error: frame file close error (%d)\n",errno);
                    return 0;
                }
                return 1;
                
                               }

                            
        int main(){
            int i, j;
            for (j=height-1; j>=0; j--) {
                for (i=0; i<width; i++) {
                    buffer[i][j] = Color(i/(float)height, j/(float)width, 0);
                   
                }
            }
            outputFrame();
            
                return 0;
                    }
                        
                               

    