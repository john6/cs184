//
//  Main.cpp
//  
//
//  Created by Shannon Axelrod on 2/23/14.
//
//

#include "Main.h"
#include <Eigen/Dense>
#include "CImg.h"
#include <iostream>
#include <string> 
#include <fstream>
#include <sstream>
#include <cfloat> 
using namespace std;
using namespace Eigen;
using namespace cimg_library;

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
// Light
//****************************************************
class Light{
public:
    int type; 
    Vector3f LightRGB; 
    Vector3f LightRay; 
    Vector3f LightCord;
    Light(int _type, Vector3f _LightCord, Vector3f _LightRGB){
        LightCord = _LightCord;
        LightRGB = _LightRGB;
        type = _type; 
    };
    Light(){}
};

//****************************************************
// Sphere
//****************************************************
class Sphere{
public:
    Vector3f pos;
    float radius;
    Vector3f color; 
    Sphere(Vector3f _pos, float _radius, Vector3f _RGB){
        pos = _pos;
        radius = _radius;
        color = _RGB;
    }
    Sphere(){}

};

//****************************************************
// Tri
//****************************************************
class Tri{
public:
    Vector3f v1, v2, v3;
    Vector3f color;
    Tri(Vector3f _v1, Vector3f _v2, Vector3f _v3, Vector3f _RGB){
        v1 = _v1;
        v2 = _v2;
        v3 = _v3; 
        color = _RGB;
    }
    Tri(){}
};

//****************************************************
// Global Variabels
//****************************************************
int height, width, depth = 0, threshhold = 3;
Color ambientColor, specularColor, diffuseColor; 
Color black_pix(0.0, 0.0, 0.0);
float sp_v;
string out_file; 
int maxvertsnorms;
const int DIRECTIONAL = 1;
const int POINT = 0; 
Light AllLights[10]; int numLights; 
Sphere AllSpheres[100]; int numSpheres;
Tri AllTri[100]; int numTri;
Vector3f vertices[100]; int numVerts;
#define PI 3.14159265  
Vector3f intersect_point = Vector3f(0,0,0);
Vector3f surface_normal = Vector3f(0,0,0);
Vector3f eye = Vector3f(0,0,0);
float glob_t; 
Vector3f currColor; 
float kr = 0.0; //reflective compnenet


Vector3f UL = Vector3f(-.5,.5,-1);
Vector3f UR = Vector3f(.5,.5,-1);
Vector3f LR = Vector3f(.5,-.5,-1);
Vector3f LL = Vector3f(-.5,-.5,-1);


//****************************************************
// Ray
//****************************************************
class Ray{
public:
    Vector3f dir;
    float t_min, t_max;
    Vector3f start;
    Ray(Vector3f _pos, Vector3f _dir, float _t_min, float _t_max){
        dir = _dir;
        t_min = _t_min;
        t_max = _t_max;
        start = _pos;
    };

 Ray(){}   
};

//****************************************************
// Camera
//****************************************************
class Camera{
public:
    float FOVY;

    Eigen::Vector3f look_from, up, look_at; 


    Camera(){};
   
  Camera(Vector3f _eye, Vector3f view_dir, Vector3f up_dir, float field_of_view){
       look_from = _eye;
       look_at = view_dir;
       up = up_dir;
       FOVY = field_of_view * PI / 180.0;
   }

    
};


//****************************************************
// GenerateRay
//****************************************************
Ray GenerateRay(int pix_i, int pix_j){
    float u = (1- ((float)pix_j/ (float)height)) - 1/(width*2);
    float v = ((float) pix_i/ (float) width) + 1/(width*2);
    Vector3f P;
    P = (u*(v*LL + (1-v)*UL)) + ((1-u)*(v*LR + (1-v)*UR));
    return Ray(eye, P-eye, -FLT_MAX, FLT_MAX);

}



//****************************************************
// Shading Terms
//****************************************************
Vector3f ambientTerm(Color color, Light myLight){
    Eigen::Vector3f ambient;
    ambient[0] = color.r * myLight.LightRGB[0];
    ambient[1] = color.g * myLight.LightRGB[1];
    ambient[2] = color.b * myLight.LightRGB[2];
    return ambient;
}

Vector3f diffuseTerm(Color color, Light myLight, Vector3f surf_norm){
    float d;
    myLight.LightRay.normalize();
    d = myLight.LightRay.dot(surf_norm);
    Vector3f d_color = color.rgb_vec;
    d = fmaxf(d, 0);
    d_color = d * d_color; 
    d_color[0] = d_color[0] * myLight.LightRGB[0];
    d_color[1] = d_color[1] * myLight.LightRGB[1];
    d_color[2] = d_color[2] * myLight.LightRGB[2];
    return d_color; 
}

Vector3f specularTerm(Color color, Vector3f surf_normal, Light myLight, Vector3f viewVector){
    myLight.LightRay.normalize();
    Vector3f r_vec = (2*(myLight.LightRay.dot(surf_normal))*surf_normal) - myLight.LightRay;
    r_vec.normalize();
    Vector3f s_color = color.rgb_vec;
    float r_dotV = r_vec.dot(viewVector);
    s_color = s_color* pow(fmaxf(r_dotV, 0), sp_v);
    s_color[0] = s_color[0] * myLight.LightRGB[0];
    s_color[1] = s_color[1] * myLight.LightRGB[1];
    s_color[2] = s_color[2] * myLight.LightRGB[2];
    return s_color; 
   // return Vector3f(0, 1, 1);
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

    //GEOMETRY
    }else if (operand.compare("sphere") == 0){
        readvals(ss, 7, values);
        Vector3f temp = Vector3f(values[0], values[1], values[2]);
        Vector3f RGB = Vector3f(values[4], values[5], values[6]);
        Sphere newSphere(temp, values[3], RGB);
        AllSpheres[numSpheres] = newSphere;
        numSpheres++;

    }else if (operand.compare("vertex") == 0){
        readvals(ss, 3, values);
        Vector3f newVertex(values[0], values[1], values[2]);
        vertices[numVerts] = newVertex;
        numVerts++; 

    }else if (operand.compare("tri") == 0){
        readvals(ss, 6 ,values);
        Vector3f RGB = Vector3f(values[3], values[4], values[5]);
        Tri newTri(vertices[(int)values[0]], vertices[(int)values[1]], vertices[(int)values[2]], RGB);
        AllTri[numTri] = newTri;
        numTri++;
    }
     //TRANSFORMATIONS 
    else if (operand.compare("translate") == 0){
        readvals(ss, 3, values);
    }else if (operand.compare("rotate") == 0){
        readvals(ss, 3, values);
    }else if (operand.compare("scale") == 0){
        readvals(ss, 3, values);
    }

    //LIGHTS
    else if (operand.compare("directional") == 0){
        readvals(ss, 6, values);
        Light new_DirLight(DIRECTIONAL, Vector3f(values[0], values[1], values[2]), 
                                Vector3f(values[3], values[4], values[5]));
        AllLights[numLights] = new_DirLight;
        numLights++;

    }else if (operand.compare("point") == 0){
        readvals(ss, 6, values);
        Light new_pointLight(POINT, Vector3f(values[0], values[1], values[2]), 
                                    Vector3f(values[3], values[4], values[5]));
        AllLights[numLights] = new_pointLight;
        numLights++;


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
    }else if (operand.compare("kr") == 0){  //Mirror reflection
        ss>>kr; 
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
    Vector3f p0 = ray.start; 
    Vector3f dir = ray.dir; 
    float discriminant, s0, s1, t;
    Vector3f center = sphere.pos;
    float a = dir.dot(dir);
    float b = 2*dir.dot(p0-center);
    float c = center.dot(center) + (p0.dot(p0)) + (-2*center.dot(p0)) - (sphere.radius*sphere.radius);

    discriminant = (b*b)-(4*a*c);

    if(discriminant < 0){
        return false; 
    }
    float discSqrt = sqrt(discriminant);
    float q;
    if(b<0)
        q = (-b - discSqrt)/2;
    else
        q = (-b + discSqrt)/2;

    s0 = q/a;
    s1 = c/q;
    if(s0>s1){
        float temp = s0; 
        s0 = s1;
        s1 = temp; 
    }
    if(s1<0){
        return false;
    }
    if(s0<0){
        intersect_point = p0 + (dir-p0)*s1; 
        glob_t = s1; 
        surface_normal = (intersect_point - center)/sphere.radius;
        return true; 
    }
    intersect_point = p0 + (dir-p0)*s0; 
    glob_t = s0;
    surface_normal = (intersect_point - center)/sphere.radius;
    return true;
    
}

bool triIntersection(Tri tri, Ray ray){
    Vector3f p0 = tri.v1; 
    Vector3f p1 = tri.v2;
    Vector3f p2 = tri.v3; 
    Vector3f n = (p1 -p0).cross(p2 - p0);
    Vector3f o = ray.start;
    float NdotRaydir = n.dot(ray.dir);
    if(NdotRaydir == 0){
        return false; 
    }
    float d = n.dot(p0);
    float t = -(n.dot(o) + d) / NdotRaydir;
 
    Vector3f P = o + (t*ray.dir);
    glob_t = t; 

    //INSIDE OUTSIDE TESTS
    Vector3f C;
    //edge 0
    Vector3f edge0 = p1 - p0;
    Vector3f Vp0 = P - p0; 
    C = edge0.cross(Vp0);
    if(n.dot(C) < 0){
        return false; 
    }
    //edge1
    Vector3f edge1 = p2 -p1; 
    Vector3f Vp1 = P -p1; 
    C = edge1.cross(Vp1);
    if(n.dot(C) < 0){
        return false;
    }
    //edge2
    Vector3f edge2 = p0 -p2;
    Vector3f Vp2 = P - p2;
    C = edge2.cross(Vp2);
    if(n.dot(C) < 0){
        return false;
    }
    surface_normal = n;
    surface_normal.normalize(); 
    intersect_point = P; 
    return true; 
}

bool checkIntersection(Ray ray){
    bool intersection = false;
      for(int i=0; i<numSpheres; i++){
            if((sphereIntersection(AllSpheres[i], ray))){
                currColor = AllSpheres[i].color;
                intersection = true;
               break;
           }
       }
    
     if(!intersection){
       for(int i=0; i<numTri; i++){
            if(triIntersection(AllTri[i], ray)){
                currColor = AllTri[i].color; 
                intersection = true;
                break; 
            }
       }
   }

   return intersection;
}

Ray generateShadowRay(Light light, Vector3f intersect_point){
    Ray ShadowRay; 
     if(light.type == DIRECTIONAL){
                ShadowRay.start = intersect_point+ 0.01*light.LightCord;
                ShadowRay.dir = (light.LightCord);
                ShadowRay.t_min = 0.1;
                ShadowRay.t_max = FLT_MAX; 
                
            }
            else{
              ShadowRay.start = intersect_point + 0.01*light.LightCord;
              ShadowRay.dir = light.LightCord - intersect_point;
              ShadowRay.t_min = 0.1;
              ShadowRay.t_max = 1; 
            }
        return ShadowRay;
}


//****************************************************
// The Transformation Function 
//****************************************************

bool transformation(Ray ray, Sphere sphere, float scaleX, float scaleY, float scaleZ){
    Matrix4f m(4,4);
    m(0,0) = scaleX;
    m(0,1) = 0;
    m(0,2) = 0;
    m(0,3) = 0;
    m(1,1) = scaleY;
    m(1,0) = 0;
    m(1,2) = 0;
    m(1,3) = 0;
    m(2,2) = scaleZ;
    m(2,0) = 0;
    m(2,1) = 0;
    m(2,3) = 0;
    m(3,3) = 1;
    m(3,0) = 0;
    m(3,1) = 0;
    m(3,2) = 0;
    Matrix4f invM = m.inverse();

    Matrix4f rayDir(4,4);      //transform into homgenized matrix
    rayDir(0,0) = ray.dir(0);
    rayDir(1,1) = ray.dir(1);
    rayDir(2,2) = ray.dir(2);
    rayDir(3,3) = 0;     //when you homogenize a direction you pad it with zero rather than one

    Matrix4f rayStart(4,4);
    rayStart(0,0) = ray.start(0);
    rayStart(1,1) = ray.start(1);
    rayStart(2,2) = ray.start(2);
    rayStart(3,3) = 1;

    rayDir = invM * rayDir;     // by multiplying the ray direction and start vector into object space
    rayStart = invM * rayStart; //transform the ray from object space to world space

    ray.dir = Vector3f(rayDir(0,0), rayDir(1,1), rayDir(2,2));
    ray.start = Vector3f(rayStart(0,0), rayStart(1,1), rayStart(2,2));

    bool intersection = checkIntersection(ray);

    Vector4f intersectPoint;
    intersectPoint(0) = intersect_point(0);
    intersectPoint(1) = intersect_point(1);
    intersectPoint(2) = intersect_point(2);
    intersectPoint(3) = 1;

    Vector4f surfNormal;
    surfNormal(0) = surface_normal(0);
    surfNormal(1) = surface_normal(1);
    surfNormal(2) = surface_normal(2);
    surfNormal(3) = 0;
                                                                    //first have to turn the intersect point and surf norm to 4 vectors from 3 vectors 
    intersectPoint =  m * intersectPoint;                            //transform postion back to world space using M
    surfNormal =  invM.transpose() * surfNormal;        //transform normal using the Trasnpose of the inverse of M
                                    //need something called localGeo which holds both postion and normal
    intersect_point(0) = intersectPoint(0);
    intersect_point(1) = intersectPoint(1);
    intersect_point(2) = intersectPoint(2);

    surface_normal(0) = surfNormal(0);
    surface_normal(1) = surfNormal(1);
    surface_normal(2) = surfNormal(2);

    return intersection;
}

Ray createReflectRay(Vector3f intersect_point, Ray ray, Vector3f surf_norm){
    Vector3f V = ray.dir; 
    Vector3f N = surf_norm;
    Vector3f R = -V + 2*N*(V.dot(N));
    return Ray((intersect_point + .01*R), (R - intersect_point), 1, 1);
}



//****************************************************
// The Trace Function 
//****************************************************

Color Trace(Ray ray, int depth) {
   
   bool intersection = false; 
   Color returnColor = black_pix;
   intersection = checkIntersection(ray);
 // intersection = transformation(ray, AllSpheres[0], 2, 1, 1);
   if (depth > threshhold) {
        return black_pix;
   }
    
    if (!intersection) {        // No intersection
        return black_pix;
    }    

        ambientColor = Color(currColor[0], currColor[1], currColor[2]);
        returnColor.rgb_vec = ambientColor.rgb_vec; 

    for (int i=0; i<numLights; i++) {   
        Light light = AllLights[i];
        Ray ShadowRay = generateShadowRay(light, intersect_point);
        light.LightRay = ShadowRay.dir; 


        if(checkIntersection(ShadowRay)){
                break;
            }
               Vector3f viewVector = eye - intersect_point;
               viewVector.normalize();
               Vector3f diffuse = diffuseTerm(diffuseColor, light, surface_normal);
               Vector3f specular = specularTerm(specularColor, surface_normal, light, viewVector);
               returnColor.rgb_vec += diffuse + specular; 
        }
        
        // Handle mirror reflection
        if (kr > 0) {
            Ray reflectRay = createReflectRay(intersect_point, ray, surface_normal);
            // Make a recursive call to trace the reflected ray
            Color temp = Trace(reflectRay, depth+1);
            returnColor.rgb_vec += temp.rgb_vec; 
        } 
    
        returnColor.reset(); 
       return returnColor; 
}


//****************************************************
// The Rendering Loop
//****************************************************
Color** render(int height, int width) {
    //insert values into ColorBuffer array
    Color** buffer = new Color*[width];
    for(int i=0; i<width; ++i)
        buffer[i] = new Color[height];

    for (int i= 0; i<width; i++) {
        for (int j = 0; j<height; j++) {
            Ray ray = GenerateRay(i, j);
            Color new_color = Trace(ray, depth);
            buffer[i][j] = new_color; 
        }
    }
    return buffer; 
           }


//****************************************************
// CImage output
//****************************************************
CImg<float> image(1000, 1000,1,3,0); 
void outputImage(Color** buffer){
    for (int j=height-1; j>=0; j--) {
        for (int i=0; i<width; i++) {
            image(i, j, 0) = buffer[i][j].r;
            image(i, j, 1) = buffer[i][j].g;
            image(i, j, 2) = buffer[i][j].b;
        }
    }
}


//****************************************************
// Main
//****************************************************

int main(int argc, char* argv[]){

    
    parseScene(argv[1]);
    Color** testbuffer = render(height, width);
    outputImage(testbuffer);
    outputImage(testbuffer);
    image.normalize(0,255);
    image.save("Reflection.ppm");
    image.display(); 
    
    return 0;
}






