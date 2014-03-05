//
//  Scene.cpp
//  
//
//  Created by Shannon Axelrod on 2/23/14.
//
//

#include "Main.h"
using namespace std;
#include "Color.h"
#include <Eigen/Dense>
#include <iostream>
#include <string> 
#include <fstream>
#include <sstream>
#include "Ray.h"
#include "Point.h"



//****************************************************
// Camera
//****************************************************
typedef struct Camera{
    Eigen::Vector3f look_from;
    Eigen::Vector3f look_at;
    Eigen::Vector3f up;
    float FOVY;
    
}Camera;


//****************************************************
// GenerateRay
//****************************************************
Ray GenerateRay(Camera cam, int pix_i, int pix_j, int height, int width){
    //THIS IS WHERE WE SHOULD DO THOSE DANK EQUATIONS
    Eigen::Vector3f w = cam.look_at - cam.look_from;
    w.normalize();
    Eigen::Vector3f u = cam.up.cross(w);
    w.normalize();
    Eigen::Vector3f v = w.cross(u);
    Point hi;
    return Ray(hi, u, 1, 1);
}

//****************************************************
// Global Variabels
//****************************************************
Camera myCamera;
int height;
int width;
Color buffer[240][240];


//****************************************************
// Parse the Input File
//****************************************************

bool readvals(stringstream &s, const int numvals, float *values) {
	for (int i = 0 ; i < numvals ; i++) {
		s >> values[i] ;
		if (s.fail()) {
			cout << "Failed reading value " << i << " will skip\n" ;
			return false ;
		}
	}
	return true ;
}

void readFile(const string &filename){
    //parse the input file and get all needed info
    ifstream inFile;
    
    inFile.open(filename.c_str());
    string line , kind;
    bool valid;
    
    getline(inFile, line);
    while(inFile){
        if(!((line.find_first_not_of("\t\r\n") != string::npos) && (line[0] != '#'))){
            getline(inFile, line);
            continue;
        }
        stringstream s(kind);
        s >> kind;
        float values[10];
        if (kind == "size") {
            
        }
        if (kind == "maxdepth") {
            
        }
        if (kind == "output") {
            
        }
        
        if (kind == "directional") {
            
        }
        if (kind == "point") {
            
        }
        if (kind == "ambient") {
            
        }
        if (kind == "attenuation") {
            
        }
        if (kind == "ambient") {
            
        }
        if (kind == "specular") {
            
        }
        if (kind == "emission") {
            
        }
        if (kind == "shininess") {
            
        }
        if (kind == "size") {
            
        }
        if (kind == "camera") {
            valid = readvals(s, 10, values);
            myCamera.look_from = Eigen::Vector3f(values[0], values[1], values[2]);
            myCamera.look_at = Eigen::Vector3f(values[3], values[4], values[5]);
            myCamera.up = Eigen::Vector3f(values[6], values[7], values[8]);
            myCamera.FOVY = values[9];
            
        }
        if (kind == "maxdepth") {
            
        }
        if (kind == "vertex") {
            
        }
        if (kind == "vertexnormal") {
            
        }
        if (kind == "tri") {
            
        }
        if (kind == "trinormal") {
            
        }
        if (kind == "sphere") {
            
        }
        if (kind == "maxverts") {
            
        }
        if (kind == "maxvertnorms") {
            
        }
        if (kind == "translate") {
            
        }
        if (kind == "scale") {
            
        }
        if (kind == "rotate") {
            
        }
        if (kind == "pushTransform") {
            
        }
        if (kind == "popTransform") {
            
        }
        if (kind == "rotate") {
            
        }
        
        
    }
    
}



//****************************************************
// The Rendering Loop
//****************************************************
void render(Camera camera, int height, int width) {
    //insert values into ColorBuffer array
    for (int i= 0; i<width; i++) {
        for (int j = 0; j<height; j++) {
            Ray ray = GenerateRay(camera, i, j, height, width);
           // Color color = RayTracer.Trace(ray, height, width, 0, i, j);
            Color color(0, 0, 0);
            buffer[i][j] = color;
            
        }
    }
           }



//****************************************************
// Output the Image
//****************************************************
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

//****************************************************
// Main
//****************************************************

int main(int argc, char* argv[]){
    
    readFile(argv[1]);
  //  render(/* on shit*/)
    outputFrame();
    
    return 0;
}






