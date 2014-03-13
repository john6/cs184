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
    Sphere(Vector3f _pos, float _radius){
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
    Vector3f v1, v2, v3;
    Tri(Vector3f _v1, Vector3f _v2, Vector3f _v3){
        v1 = _v1;
        v2 = _v2;
        v3 = _v3; 
    }
    Tri(){}
};

//****************************************************
// Global Variabels
//****************************************************
int height, width, depth = 5, threshhold;
Color ambientColor, specularColor, diffuseColor; 
Color black_pix(0.0, 0.0, 0.0);
float sp_v;
string out_file; 
int maxvertsnorms;
const int DIR = 1;
const int POINT = 0; 
Light AllLights[10]; int numLights; 
Sphere AllSpheres[100]; int numSpheres;
Tri AllTri[100]; int numTri;
Vector3f vertices[100]; int numVerts;
#define PI 3.14159265  
Vector3f intersect_point = Vector3f(0,0,0);
Vector3f surface_normal = Vector3f(0,0,0);
Vector3f eye = Vector3f(0,0,0);
Vector3f UL = Vector3f(1,0,0);
Vector3f UR = Vector3f(0,0,0);
Vector3f LR = Vector3f(0,1,0);
Vector3f LL = Vector3f(1,1,0);


//****************************************************
// Ray
//****************************************************
class Ray{
public:
    Vector3f dir;
    float t_min, t_max;
    Vector3f start;
    Ray(Eigen::Vector3f _pos, Eigen::Vector3f _dir, float _t_min, float _t_max){
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
// Make the Global Camera
//****************************************************

Camera myCamera;


//****************************************************
// GenerateRay
//****************************************************
Ray GenerateRay(Camera cam, int pix_i, int pix_j){
/*
    Vector3f w = cam.look_from - cam.look_at;
    w.normalize();
    Vector3f u =  cam.up.cross(w);
    u.normalize();
    Vector3f v = w.cross(u);

    float x_range = tan(cam.FOVY / 2.0) * width / height;
    float b = tan(cam.FOVY / 2.0) * (height/2.0 - pix_i) / (height / 2.0);
    float a =  x_range * (pix_j - width/2.0) / (width / 2.0);
    return Ray(cam.look_from, u*a + v*b -w, 20, 30);
    */
    float u = 1 - pix_j/height;
    float v = pix_i/width;
    Vector3f P;
    P = (u*(v*LL + (1-v)*UL)) + ((1-u)*(v*LR + (1-v)*UR));
    return Ray(eye, P-eye, 20, 30);

}



//****************************************************
// Shading Terms
//****************************************************
Vector3f ambientTerm(Color color, Light myLight){
    Eigen::Vector3f ambient = Eigen::Vector3f(color.r, color.g, color.b);
    ambient = ambient.cross(myLight.LightRGB);
    return ambient;
}

Vector3f diffuseTerm(Color color, Light myLight, Vector3f surf_norm){
    float d;
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

    //CAMERA
    }else if (operand.compare("camera") == 0){
         readvals(ss, 10, values);
         Camera temp(Vector3f(values[0], values[1], values[2]), 
                     Vector3f(values[3], values[4], values[5]), 
                     Vector3f(values[6], values[7], values[8]),
                     values[9]);
         myCamera = temp;

    //GEOMETRY
    }else if (operand.compare("sphere") == 0){
        readvals(ss, 4, values);
        Vector3f temp = Vector3f(values[0], values[1], values[2]);
        Sphere newSphere(temp, values[3]);
        AllSpheres[numSpheres] = newSphere;
        numSpheres++;

    }else if (operand.compare("maxverts") == 0){
        ss>>maxvertsnorms;
    }else if (operand.compare("vertex") == 0){
        readvals(ss, 3, values);
        Vector3f newVertex(values[0], values[1], values[2]);
        vertices[numVerts] = newVertex;
        numVerts++; 

    }else if (operand.compare("vertexnormal") == 0){
        readvals(ss, 6, values);

    }else if (operand.compare("tri") == 0){
        readvals(ss, 3 ,values);
        Tri newTri(vertices[(int)values[0]], vertices[(int)values[1]], vertices[(int)values[2]]);
        AllTri[numTri] = newTri;
        numTri++;

    }else if (operand.compare("UL") == 0){
        readvals(ss, 3, values);
      //  Vector3f temp(values[0], values[1], values[2]);

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
        Light new_DirLight(DIR, Vector3f(values[0], values[1], values[2]), 
                                Vector3f(values[3], values[4], values[5]));
        AllLights[numLights] = new_DirLight;
        numLights++;

    }else if (operand.compare("point") == 0){
        readvals(ss, 6, values);
        Light new_pointLight(POINT, Vector3f(values[0], values[1], values[2]), 
                                    Vector3f(values[3], values[4], values[5]));
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
    float b = 2*p0.dot(dir);
    float c = p0.dot(p0) - (sphere.radius*sphere.radius);
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
       // intersect_point.normalize();
        surface_normal = (intersect_point - center)/sphere.radius;
        return true; 
    }
    intersect_point = p0 + (dir-p0)*s0; 
    //intersect_point.normalize();
    surface_normal = (intersect_point - center)/sphere.radius;
    return true;
    
}

bool triIntersection(Tri tri, Ray ray){
    Vector3f p0 = tri.v1; 
    Vector3f p1 = tri.v2;
    Vector3f p2 = tri.v3; 
    Vector3f n = (p1 -p0).cross(p2 - p0);
    n.normalize();
    Vector3f o = ray.start;
    float NdotRaydir = n.dot(ray.dir);
    if(NdotRaydir == 0){
        return false; 
    }
    float d = n.dot(p0);
    float t = -(n.dot(o) + d) / NdotRaydir;
    if(t<0){
        return false;
    }
    Vector3f P = o + t*ray.dir;

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
    Vector3f Vp2 = P - p1;
    C = edge2.cross(Vp2);
    if(n.dot(C) < 0){
        return false;
    }
    printf("any true?");
    return true; 
}



//****************************************************
// The Trace Function 
//****************************************************

Color Trace(Ray ray, int depth, Camera cam) {
   
   bool intersection = false; 
   Color returnColor = black_pix;
 //   if (depth > threshhold) {
   //     return black_pix;
  // }
   
        for(int i=0; i<numSpheres; i++){
            if((sphereIntersection(AllSpheres[i], ray))){
                intersection = true;
               break;
           }
       }
    
    /*   
       for(int i=0; i<numTri; i++){
            if(triIntersection(AllTri[i], ray)){
                intersection = true;
                return Color(1, 0, 0);
                break; 
            }
       }
       */
    
    if (!intersection) {
        // No intersection
        return black_pix;
    }    
        for (int i=0; i<numLights; i++) {
            Light light = AllLights[i];
            if(light.type == DIR){
                light.LightRay = -1*(light.LightCord);
                light.LightRay.normalize();
                
            }
            else{
              light.LightRay = light.LightCord- intersect_point;
              light.LightRay.normalize();
            }

            // Check if the light is blocked or not
           // if (!primitive->intersectP(lray))
                
             //   Vector3f viewVector = cam.look_from - intersect_point;
             //   viewVector.normalize();
             //   Vector3f ambient = ambientTerm(ambientColor, light);
                Vector3f diffuse = diffuseTerm(diffuseColor, light, surface_normal);
             //   Vector3f specular = specularTerm(specularColor, surface_normal, light, viewVector);
               // returnColor.rgb_vec =  ambient + diffuse + specular; 
                returnColor.rgb_vec = diffuse;
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
        return Color(1, 0, 0);
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
            Ray ray = GenerateRay(camera, i, j);
            Color new_color = Trace(ray, depth, camera);
            buffer[i][j] = new_color; 
        }
    }
    return buffer; 
           }


//****************************************************
// CImage output
//****************************************************
CImg<float> image(400, 400,1,3,0); 
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
    Color** testbuffer = render(myCamera, height, width);
    outputImage(testbuffer);
    image.display(); 
    
    return 0;
}






