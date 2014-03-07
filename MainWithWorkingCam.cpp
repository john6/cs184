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
// Global Variabels
//****************************************************
//Camera myCamera;
int height, width, depth;
Color ambientColor, specularColor, emissionColor, diffuseColor; 
string out_file; 
int maxvertsnorms;
const int DIR = 1;
const int POINT = 0; 
list<Light> AllLights; 

//****************************************************
// Camera
//****************************************************
class Camera{
public:
    float aspect_ratio, verticle_dist_from_center, horizontal_dist_from_center, 
              vert_length, hor_length, pixel_length, half_pixel_offset, FOVY;

    Eigen::Vector3f look_from, look_at, up, right, center_screen_coord, top_left_c, top_right_c, 
                    bottom_left_c, bottom_right_c;
    Camera(){};
   
  Camera(Eigen::Vector3f eye, Eigen::Vector3f view_dir, Eigen::Vector3f up_dir, float field_of_view){
       look_from = eye;
       look_at = view_dir;
       up = up_dir;
       FOVY = field_of_view;
       look_at.normalize(); //normalize the viewing direction
       up.normalize(); //normalize up vector
       aspect_ratio = (width/height); // give ratio of horizontal screen size to verticle
       right = look_at.cross(up); //create horizontal direction based on hand rule
       center_screen_coord = look_from + look_at; //create pixel at center of "screen"
       verticle_dist_from_center = tan(FOVY/2);
       horizontal_dist_from_center = verticle_dist_from_center * aspect_ratio;
       top_left_c = center_screen_coord + (verticle_dist_from_center * up) - (horizontal_dist_from_center * right);
       top_right_c = center_screen_coord + (verticle_dist_from_center * up) + (horizontal_dist_from_center * right);
       bottom_left_c = center_screen_coord - (verticle_dist_from_center * up) - (horizontal_dist_from_center * right);
       bottom_right_c = center_screen_coord - (verticle_dist_from_center * up) + (horizontal_dist_from_center * right);
       vert_length = verticle_dist_from_center * 2;
       hor_length = horizontal_dist_from_center * 2;
       pixel_length = vert_length/height;
       half_pixel_offset = pixel_length/2;

   }




    
};

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
       //  P = (u)(vLL+ (1!v)UL) + (1!u)(vLR+ (1!v)UR)
   float dist_from_left_side = cam.half_pixel_offset + cam.pixel_length * pix_i; //this could be fucking j for all I know right now
   float dist_from_top = cam.half_pixel_offset + cam.pixel_length * pix_j; //this could be fucking i for all I know right now


    Eigen::Vector3f u(0,0,0);
    Point hi;
    return Ray::Ray(hi, u, 1, 1);
}



//****************************************************
// Shading Terms
//****************************************************
Eigen::Vector3f ambientTerm(Color color, Light myLight){
    Eigen::Vector3f ambient = Eigen::Vector3f(color.r, color.g, color.b);
    ambient[0] = ambientColor.r*myLight.LightRGB.r;
    ambient[1] = ambientColor.g*myLight.LightRGB.g;
    ambient[2] = ambientColor.b*myLight.LightRGB.b;
    return ambient;
}

Eigen::Vector3f diffuseTerm(Color color, Light myLight, Eigen::Vector3f surf_norm){
     Eigen::Vector3f d_color; 

     return d_color; 
}

Eigen::Vector3f specularTerm(Color color, Eigen::Vector3f surf_normal, Light myLight, Eigen::Vector3f viewVector){
    Eigen::Vector3f r_vec = (2*(myLight.LightVector.dot(surf_normal))*surf_normal) - myLight.LightVector;

    return r_vec; 
}



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
     //    myCamera.look_from = Eigen::Vector3f(values[0], values[1], values[2]);
       //  myCamera.look_at = Eigen::Vector3f(values[3], values[4], values[5]);
       //  myCamera.up = Eigen::Vector3f(values[6], values[7], values[8]);
        // myCamera.FOVY = values[9];

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

    //setting params to specific things just to test
    height = 10;
    width = 10;
    Eigen::Vector3f eye = Eigen::Vector3f(0,3,0);
    Eigen::Vector3f view_dir = Eigen::Vector3f(1,0,0);
    Eigen::Vector3f up_dir = Eigen::Vector3f(0,1,0);
    float fovY = 1.57079632679; //  pi/2

     // set up camera
    Camera cam = Camera(eye, view_dir, up_dir, fovY);

    //printf("\n : %f", );
    float testing_tan = tan((1.57079632679/2));
    printf("\ntesting_tan: %f", testing_tan);
    printf("\n aspect_ratio: %f", cam.aspect_ratio);
    printf("\n verticle_dist_from_center: %f", cam.verticle_dist_from_center);
    printf("\n vert_length: %f", cam.vert_length);
    printf("\n horizontal_dist_from_center: %f", cam.horizontal_dist_from_center);
    printf("\n hor_length: %f", cam.hor_length);
    printf("\n pixel_length: %f", cam.pixel_length);
    printf("\n half_pixel_offset: %f", cam.half_pixel_offset);
    printf("\n FOVY: %f", cam.FOVY);
    printf("\n look_from: %f, %f, %f", cam.look_from(0), cam.look_from(1), cam.look_from(2));
    printf("\n look_at: %f, %f, %f", cam.look_at(0), cam.look_at(1), cam.look_at(2));
    printf("\n up: %f, %f, %f", cam.up(0), cam.up(1), cam.up(2));
    printf("\n right: %f, %f, %f", cam.right(0), cam.right(1), cam.right(2));
    printf("\n center_screen_coord: %f, %f, %f", cam.center_screen_coord(0), cam.center_screen_coord(1), cam.center_screen_coord(2));
    //printf("\n : %f", );
    printf("\ntop_left_c: %f, %f, %f", cam.top_left_c(0), cam.top_left_c(1), cam.top_left_c(2));
    printf("\ntop_right_c: %f, %f, %f", cam.top_right_c(0), cam.top_right_c(1), cam.top_right_c(2));
    printf("\nbottom_left_c: %f, %f, %f", cam.bottom_left_c(0), cam.bottom_left_c(1), cam.bottom_left_c(2));
    printf("\nbottom_right_C: %f, %f, %f", cam.bottom_right_c(0), cam.bottom_right_c(1), cam.bottom_right_c(2));
    printf("\n");
    
    // Color** testbuffer = render(myCamera, height, width);
    // outputFrame(testbuffer);
    
    return 0;
}






