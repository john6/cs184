//
//  Scene.cpp
//  
//
//  Created by Shannon Axelrod on 2/23/14.
//
//

#include "Main.h"
#include <Eigen/Dense>
#include <iostream>
#include <string> 
#include <fstream>
#include <sstream>
#include <list>
using namespace std;



//****************************************************
// Color
//****************************************************
class Color{
public:
    float r, g, b;
    Color(float _r, float _g, float _b){
        r = _r;
        g = _g;
        b = _b; 
    };
    Color(){};
};

//****************************************************
// Light
//****************************************************
class Light{
public:
    int type; 
    Color LightRGB; 
    Eigen::Vector3f LightVector; 
    Light(int _type, Eigen::Vector3f _LightVector, Color _LightRGB){
        LightVector = _LightVector;
        LightRGB = _LightRGB;
    };
    Light(){}
};

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
// Point
//****************************************************
class Point{
public:
    float x, y, z;
    Point(int _x, int _y, int _z){
        x = _x;
        y = _y;
        z = _z; 
    };
    Point(){}

};

//****************************************************
// Ray
//****************************************************
class Ray{
public:
    Eigen::Vector3f dir;
    float t_min, t_max;
    Point pos;
    Ray(Point _pos, Eigen::Vector3f _dir, float _t_min, float _t_max){
        dir = _dir;
        t_min = _t_min;
        t_max = _t_max;
        pos = _pos;
    };
 Ray(){}   
};


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
    return Ray::Ray(hi, u, 1, 1);
}

//****************************************************
// Global Variabels
//****************************************************
Camera myCamera;
int height, width, depth;
Color ambientColor, specularColor, emissionColor, diffuseColor; 
string out_file; 
int maxvertsnorms;
const int DIR = 1;
const int POINT = 0; 
list<Light> AllLights; 

//****************************************************
// Shading Terms
//****************************************************




//****************************************************
// Parse the Input File
//****************************************************


void readvals(stringstream &s, const int numvals, float *values) {
    for (int i = 0 ; i < numvals ; i++) {
        s >> values[i] ;
    }
}

bool parseLine(string line){
    float values[10];       //maximum 10 opperands after camera 
    bool valid; 
    string operand; 
    if(line.empty())
        return true;
    stringstream ss(stringstream::in | stringstream::out);
    ss.str(line);
    ss >> operand;
    if(operand[0] == '#'){
        return true;

    //GENERAL
    }else if(operand.compare("size") == 0){
        readvals(ss, 2, values);
        width = values[0];  
        height = values[1];
    }else if(operand.compare("maxdepth") == 0){
        ss>>depth; 
    }else if(operand.compare("output") == 0){
        ss>>out_file; 

    //CAMERA
    }else if (operand.compare("camera") == 0){
         readvals(ss, 10, values);
         myCamera.look_from = Eigen::Vector3f(values[0], values[1], values[2]);
         myCamera.look_at = Eigen::Vector3f(values[3], values[4], values[5]);
         myCamera.up = Eigen::Vector3f(values[6], values[7], values[8]);
         myCamera.FOVY = values[9];

    //GEOMETRY
    }else if (operand.compare("sphere") == 0){
        readvals(ss, 4, values);

    }else if (operand.compare("maxverts") == 0){
        ss>>maxvertsnorms;
    }else if (operand.compare("vertex") == 0){
        readvals(ss, 3, values);

    }else if (operand.compare("vertexnormal") == 0){
        readvals(ss, 6, values);

    }else if (operand.compare("tri") == 0){
        readvals(ss, 3, values);
    }else if (operand.compare("trinormal") == 0){
        readvals(ss, 3, values);

     //TRANSFORMATIONS 
    }else if (operand.compare("translate") == 0){
        readvals(ss, 3, values);
    }else if (operand.compare("rotate") == 0){
        readvals(ss, 3, values);
    }else if (operand.compare("scale") == 0){
        readvals(ss, 3, values);
    }else if (operand.compare("pushTransform") == 0){
        //DO SOME KIND OF TRANSFORMATION
    }else if (operand.compare("popTransform") == 0){
        //DO SOMETHING

    //LIGHTS
    }else if (operand.compare("directional") == 0){
        readvals(ss, 6, values);
        Light new_DirLight(DIR, Eigen::Vector3f(values[0], values[1], values[2]), 
                                Color(values[3], values[4], values[5]));
        AllLights.push_front(new_DirLight);

    }else if (operand.compare("point") == 0){
        readvals(ss, 6, values);
        Light new_pointLight(POINT, Eigen::Vector3f(values[0], values[1], values[2]), 
                                    Color(values[3], values[4], values[5]));
        AllLights.push_front(new_pointLight);

    }else if (operand.compare("attenuation") == 0){
        readvals(ss, 3, values);
    }else if (operand.compare("ambient") == 0){
        readvals(ss, 3, values);
        Color temp(values[0], values[1], values[2]);
        ambientColor = temp; 
    }

    //MATERIALS
    else if (operand.compare("diffuse") == 0){
        readvals(ss, 3, values);
        Color temp1(values[0], values[1], values[2]);
        diffuseColor = temp1; 
    }else if (operand.compare("specular") == 0){
        readvals(ss, 3, values);
        Color temp2(values[0], values[1], values[2]);
        specularColor= temp2;
    }else if (operand.compare("shininess") == 0){
        readvals(ss, 1, values);
    }else if (operand.compare("emmission") == 0){
        readvals(ss, 3, values);
        Color temp3(values[0], values[1], values[2]);
        emissionColor = temp3; 
    }

    if (ss.fail())
        return false;
    return true; 
}
void parseScene(string filename){
    char line[1024];
    ifstream inFile(filename.c_str());
    if (!inFile){
        cout<<"could not open file"<<filename;
    }
    while(inFile.good()){
        inFile.getline(line, 1023);
        if(!parseLine(string(line)))
            exit(1);
    }
    inFile.close(); 
}


//****************************************************
// The Rendering Loop
//****************************************************
Color** render(Camera camera, int height, int width) {
    //insert values into ColorBuffer array
    Color** buffer = new Color*[width];
    for(int i=0; i<width; ++i)
        buffer[i] = new Color[height];

    for (int i= 0; i<width; i++) {
        for (int j = 0; j<height; j++) {
            Ray ray = GenerateRay(camera, i, j, height, width);
           // Color color = RayTracer.Trace(ray, height, width, 0, i, j);
           // buffer[i][j] = color; 
            buffer[i][j] = Color(i/(float)height, j/(float)width, 0);
        }
    }
    return buffer; 
           }



//****************************************************
// Output the Image
//****************************************************
int outputFrame(Color** buffer){
    FILE *frameFile;
    char filename[80];
    int i,j;
    int     errno;
    
    //glReadPixels(0,0,frameWidth,frameHeight,GL_RGBA,GL_UNSIGNED_BYTE,frameBuffer);
    out_file += ".ppm";
    sprintf(filename, "%s",out_file.c_str());
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

    parseScene(argv[1]);
    
    Color** testbuffer = render(myCamera, height, width);
    outputFrame(testbuffer);
    
    return 0;
}






