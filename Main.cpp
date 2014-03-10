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
using namespace std;
using namespace Eigen;



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
Vector3f vertices[100]; int numVerts;
#define PI 3.14159265  
Vector3f intersect_point;
Vector3f surface_normal;

//****************************************************
// Ray
//****************************************************
class Ray{
public:
    Vector3f dir;
    float t_min, t_max;
    Vector3f start;
    Vector3f p0; 
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
    float aspect_ratio, verticle_dist_from_center, horizontal_dist_from_center, 
              vert_length, hor_length, pixel_length, half_pixel_offset, FOVY;

    Eigen::Vector3f look_from, up, right, UL, UR, 
                    LL, LR, look_at, center_screen_coord;

   // Vector3f center_screen_coord(0,0,0);

  //  Eigen::Vector3f center_screen_coord = Eigen::Vector3f(0,0,0);

    Camera(){};
   
  Camera(Vector3f _eye, Vector3f view_dir, Vector3f up_dir, float field_of_view){
       //Vector3f look_at(0, 0, 0);
       Vector3f ZER_VEC(0, 0, 0);
       look_from = _eye;
       look_at = view_dir;
       up = up_dir;
       FOVY = field_of_view;

       /*
      // if(!(look_at == ZER_VEC))
      look_at.normalize(); //normalize the viewing direction      
      // if(!(up == ZER_VEC))
       up.normalize(); //normalize up vector
       aspect_ratio = (width/height); // give ratio of horizontal screen size to verticle
       right = look_at.cross(up); //create horizontal direction based on hand rule

       Vector3f center_screen_coord(0, 0, 0);
       center_screen_coord = look_from + look_at; //create pixel at center of "screen"

       verticle_dist_from_center = tan(FOVY/2);
       horizontal_dist_from_center = verticle_dist_from_center * aspect_ratio;

       Vector3f test = center_screen_coord + (verticle_dist_from_center * up) - (horizontal_dist_from_center * right);

       UL = center_screen_coord + (verticle_dist_from_center * up) - (horizontal_dist_from_center * right);
       UR = center_screen_coord + (verticle_dist_from_center * up) + (horizontal_dist_from_center * right);
       LL = center_screen_coord - (verticle_dist_from_center * up) - (horizontal_dist_from_center * right);
       LR = center_screen_coord - (verticle_dist_from_center * up) + (horizontal_dist_from_center * right);


       vert_length = verticle_dist_from_center * 2;
       hor_length = horizontal_dist_from_center * 2;
       pixel_length = vert_length/height;
       half_pixel_offset = pixel_length/2;
    */
   }

    
};


Camera myCamera;


//****************************************************
// GenerateRay
//****************************************************
Ray GenerateRay(Camera cam, int pix_i, int pix_j){
    //THIS IS WHERE WE SHOULD DO THOSE DANK EQUATIONS

    Vector3f w = cam.look_from - cam.look_at;
    w.normalize();
    Vector3f u =  cam.up.cross(w);
    u.normalize();
    Vector3f v = w.cross(u);

    float fovy = cam.FOVY * PI / 180.0; // degree to radian
    float x_range = tan(fovy / 2.0) * width / height;
    float b = tan(fovy / 2.0) * (height/2.0 - pix_i) / (height / 2.0);
    float a =  x_range * (pix_j - width/2.0) / (width / 2.0);
    return Ray(cam.look_from, -w + u*a + v*b, 0, 30);

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
    d = fmax(d, 0);
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
    s_color = s_color* pow(fmax(r_dotV, 0), sp_v);
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
        Sphere newSphere(Vector3f(values[0], values[1], values[2]), values[3]);
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
        surface_normal = (intersect_point - sphere.pos)/sphere.radius;
        return true; 
    }
    intersect_point = p0 + (dir-p0)*s0; 
    surface_normal = (intersect_point - sphere.pos)/sphere.radius;
    return true;
    
    
}

bool triIntersection(Tri tri, Ray ray){
    Vector3f p0 = tri.v1;
    Vector3f p1 = tri.v2;
    Vector3f p2 = tri.v3; 
    Vector3f n = (p1 -p0).cross(p2 - p0);
    Vector3f o = ray.start;
    Vector3f d = ray.dir; 
    float t = ((o-p0).dot(n))/(d.dot(n));
    Vector3f x = o + t*d;
    float case1 = ((p1 -p0).cross(x-p0)).dot(n);
    float case2 = ((p2-p1).cross(x-p1)).dot(n);
    float case3 = ((p0 - p2).cross(x-p2)).dot(n);
    if(case1 >= 0 and case2 >= 0 and case3 >= 0){
        return true; 
    }
    return false; 
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
             // cout<<light.LightRay;
            }

            // Check if the light is blocked or not
           // if (!primitive->intersectP(lray))
                
                Vector3f viewVector = cam.look_from - intersect_point;
                viewVector.normalize();
                Vector3f ambient = ambientTerm(ambientColor, light);
                Vector3f diffuse = diffuseTerm(diffuseColor, light, surface_normal);
                Vector3f specular = specularTerm(specularColor, surface_normal, light, viewVector);
                returnColor.rgb_vec =  ambient + diffuse + specular; 
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
       // return Color(1, 0, 0);
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
    Color** testbuffer = render(myCamera, height, width);
    outputFrame(testbuffer);
    
    return 0;
}






