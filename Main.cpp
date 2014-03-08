//
//  Main.cpp
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
#include <iterator>
using namespace std;



//****************************************************
// Color
//****************************************************
class Color{
public:
    Eigen::Vector3f rgb_vec; 
    float r, g, b;
    Color(float _r, float _g, float _b){
        r = _r;
        g = _g;
        b = _b; 
        Eigen::Vector3f temp(r, g, b);
        rgb_vec = temp;  
      
    };
    Color(){};
    void reset(){
            r = rgb_vec[0];
            g = rgb_vec[1];
            b = rgb_vec[2];
        }

};

//****************************************************
// Point
//****************************************************
class Point{
public:
    float x, y, z;
    Eigen::Vector3f pt_vec; 
    Point(float _x, float _y, float _z){
        x = _x;
        y = _y;
        z = _z; 
        Eigen::Vector3f temp(x, y, z);
        pt_vec = temp; 
    };
    Point(){}
    //Computes the distance between two points
    float dist(Point other){ 
        return sqrt(pow((x - other.x),2) + pow((y - other.y),2) + pow((z-other.z),2));
    }

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
// Sphere
//****************************************************
class Sphere{
public:
    Point pos;
    float radius; 
    Sphere(Point _pos, float _radius){
        pos = _pos;
        radius = _radius;
    }
    Sphere(){}

};

//****************************************************
// Tri
//****************************************************
class Tri{
public:
    Point v1, v2, v3;
    Tri(Point _v1, Point _v2, Point _v3){
        v1 = _v1;
        v2 = _v2;
        v3 = _v3; 
    }
    Tri(){}
};

//****************************************************
// Global Variabels
//****************************************************
//Camera myCamera;
int height, width, depth = 5, threshhold;
Color ambientColor, specularColor, emissionColor, diffuseColor; 
Color black_pix(0.0, 0.0, 0.0);
float sp_v;
string out_file; 
int maxvertsnorms;
const int DIR = 1;
const int POINT = 0; 
Light AllLights[10]; int numLights; 
Sphere AllSpheres[100]; int numSpheres;
Tri AllTri[100]; int numTri;
Point vertices[100]; int numVerts;

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
       top_right_c = center_screen_coord - (verticle_dist_from_center * up) + (horizontal_dist_from_center * right);
       vert_length = verticle_dist_from_center * 2;
       hor_length = horizontal_dist_from_center * 2;
       pixel_length = vert_length/height;
       half_pixel_offset = pixel_length/2;

   }
   




    
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
   float dist_from_left_side = cam.half_pixel_offset + cam.pixel_length * pix_i; 
   float dist_from_top = cam.half_pixel_offset + cam.pixel_length * pix_j; 


    Eigen::Vector3f u(0,0,0);
    Point hi;
    return Ray::Ray(hi, u, 1, 1);
}



//****************************************************
// Shading Terms
//****************************************************
Eigen::Vector3f ambientTerm(Color color, Light myLight){
    Eigen::Vector3f ambient = Eigen::Vector3f(color.r, color.g, color.b);
    ambient = ambient.cross(myLight.LightRGB.rgb_vec);
    return ambient;
}

Eigen::Vector3f diffuseTerm(Color color, Light myLight, Eigen::Vector3f surf_norm){
    float d;
    d = myLight.LightVector.dot(surf_norm);
    Eigen::Vector3f d_color(color.r, color.g, color.b);
    //take the max of kd an0
    d = fmax(d, 0);
    d_color = d * d_color;
    //multiply Kd by the intesity
    d_color = d_color.cross(myLight.LightRGB.rgb_vec);

     return d_color; 
}

Eigen::Vector3f specularTerm(Color color, Eigen::Vector3f surf_normal, Light myLight, Eigen::Vector3f viewVector){
    Eigen::Vector3f r_vec = (2*(myLight.LightVector.dot(surf_normal))*surf_normal) - myLight.LightVector;
    r_vec.normalize();
    Eigen::Vector3f s_color(color.r, color.g, color.b);
    float r_dotV = r_vec.dot(viewVector);
    s_color = s_color* pow(fmax(r_dotV, 0), sp_v);
    s_color = s_color.cross(myLight.LightRGB.rgb_vec);
   return s_color; 
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
        printf("%d\n", depth );
    }else if(operand.compare("output") == 0){
        ss>>out_file; 

    //CAMERA
    }else if (operand.compare("camera") == 0){
         readvals(ss, 10, values);
         Camera temp(Eigen::Vector3f(values[0], values[1], values[2]), 
                     Eigen::Vector3f(values[3], values[4], values[5]), 
                     Eigen::Vector3f(values[6], values[7], values[8]),
                     values[9]);

    //GEOMETRY
    }else if (operand.compare("sphere") == 0){
        readvals(ss, 4, values);
        Sphere newSphere(Point(values[0], values[1], values[2]), values[4]);
        numSpheres++;

    }else if (operand.compare("maxverts") == 0){
        ss>>maxvertsnorms;
    }else if (operand.compare("vertex") == 0){
        readvals(ss, 3, values);
        Point newVertex(values[0], values[1], values[2]);
        vertices[numVerts] = newVertex;
        numVerts++; 

    }else if (operand.compare("vertexnormal") == 0){
        readvals(ss, 6, values);

    }else if (operand.compare("tri") == 0){
        readvals(ss, 3 ,values);
        Tri newTri(vertices[(int)values[0]], vertices[(int)values[1]], vertices[(int)values[2]]);
        numTri++;

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
        AllLights[numLights] = new_DirLight;
        numLights++;

    }else if (operand.compare("point") == 0){
        readvals(ss, 6, values);
        Light new_pointLight(POINT, Eigen::Vector3f(values[0], values[1], values[2]), 
                                    Color(values[3], values[4], values[5]));
        AllLights[numLights] = new_pointLight;
        numLights++;

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
        ss>>sp_v;
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
// Intersections
//****************************************************
bool sphereIntersection(Sphere sphere, Ray ray){
    Eigen::Vector3f p0 = ray.pos.pt_vec; 
    Eigen::Vector3f p1 = ray.dir; 
    Eigen::Vector3f d_vec = p1 - p0; 
    Eigen::Vector3f center = sphere.pos.pt_vec; 
    float discriminant; 

    float a = d_vec.dot(d_vec);
    float b = ((p0 - center)*2).dot(d_vec);
    float c = center.dot(center) + p0.dot(p0) + (-2*(center.dot(p0))) - pow(sphere.radius, 2); 
    discriminant = pow(b,2) - (4*a*c);
    if(discriminant >= 0){
        return true; 
    }
    return false; 
}



//****************************************************
// The Trace Function 
//****************************************************

Color Trace(Ray ray, int depth) {
   
   bool intersection = false; 
   Color returnColor;
 //   if (depth > threshhold) {
   //     return black_pix;
  // }
        for(int i; i<numSpheres; i++){
            if((sphereIntersection(AllSpheres[i], ray))){
                intersection = true;
                break;
            }
            /*
            if(!intersection){  //if there was a sphere intersection already no need to check this
                for(int j; j<numTri; j++){
                    if(INTERSECTION WITH TR ){
                        intersection = true;
                        break; 
                    }
                }
                */
            }
    if (!intersection) {
        // No intersection
        return black_pix;
    }
    /*
        // Obtain the brdf at intersection point
        in.primitive->getBRDF(in.local, &brdf);
        
        // There is an intersection, loop through all light source
        */
        for (int i=0; i<numLights; i++) {
           // lights[i].generateLightRay(in.local, &lray, &lcolor);
            Light MyLight = AllLights[i]; 
            // Check if the light is blocked or not
           // if (!primitive->intersectP(lray))
                Eigen::Vector3f temp;
                Eigen::Vector3f surface_Normal; 
                Eigen::Vector3f viewVector(0, 0, 1.0);
                Eigen::Vector3f ambient = ambientTerm(ambientColor, MyLight);
                Eigen::Vector3f diffuse = diffuseTerm(diffuseColor, MyLight, surface_Normal);
                Eigen::Vector3f specular = specularTerm(specularColor, surface_Normal, MyLight, viewVector);
                temp = ambient + diffuse + specular;
                returnColor.rgb_vec = returnColor.rgb_vec.cross(temp); 
        }
        /*
        
        // Handle mirror reflection
        if (brdf.kr > 0) {
            reflectRay = createReflectRay(in.local, ray);
            
            // Make a recursive call to trace the reflected ray
            trace(reflectRay, depth+1, &tempColor);
            *color += brdf.kr * tempColor;
        }
        
        }
        */
        returnColor.reset(); 
        return returnColor; 
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
            // Ray ray = GenerateRay(camera, i, j, height, width);
            Ray ray;
            Color new_color = Trace(ray, depth);
            buffer[i][j] = new_color; 
           // buffer[i][j] = Color(i/(float)height, j/(float)width, 0);
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

   
    Camera myCamera;
    Color** testbuffer = render(myCamera, height, width);
    outputFrame(testbuffer);
    
    return 0;
}






