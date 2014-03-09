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
                                Color(values[3], values[4], values[5]));
        AllLights[numLights] = new_DirLight;
        numLights++;

    }else if (operand.compare("point") == 0){
        readvals(ss, 6, values);
        Light new_pointLight(POINT, Vector3f(values[0], values[1], values[2]), 
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
        t = s1; 
        return true; 
    }
    t = s0; 
    return true;
    
    
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
        for(int i=0; i<numSpheres; i++){
            if((sphereIntersection(AllSpheres[i], ray))){
               // printf("do i detect an intersection?");
                intersection = true;
               break;
           }
       }
    if (!intersection) {
        // No intersection
        return black_pix;
    }
    
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




    

     
     /*
    //setting params to specific things just to test
    height = 2;
    width = 2;
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
    printf("\nUL: %f, %f, %f", cam.UL(0), cam.UL(1), cam.UL(2));
    printf("\nUR: %f, %f, %f", cam.UR(0), cam.UR(1), cam.UR(2));
    printf("\nLL: %f, %f, %f", cam.LL(0), cam.LL(1), cam.LL(2));
    printf("\nLR: %f, %f, %f", cam.LR(0), cam.LR(1), cam.LR(2));
    printf("\n");
    */
   

    //parseScene(argv[1]);
    
     parseScene(argv[1]);
    Color** testbuffer = render(myCamera, height, width);
    outputFrame(testbuffer);
    
    return 0;
}






