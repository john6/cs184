//
//  Film.cpp
//  
//
//  Created by Shannon Axelrod on 2/23/14.
//
//

#include "Film.h"
#include "Color.h"

using namespace std;

#define width 240
#define height 240

Color buffer[width][height];


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