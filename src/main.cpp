#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <string>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>
#include <stdlib.h>
#include "RayTracer.h"

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

Vector2f imagePixelToCameraPoint(int width, int height, int pixelX, int pixelY){
    
    float coorX = (2*float(pixelX) / (width-1)) -1;
    float coorY = (2*float(pixelY) / (height-1)) -1;
    
    return Vector2f(coorX , coorY);
}

int checkGaussianOutOfBounds( int upperLim, int i, int delta){
    if ( (i+delta) <0 || (i+delta) >=upperLim ){
        return 0;
    } else {
        return i+delta;
    }
}


#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
    // Fill in your implementation here.
    char* filename = NULL;
    int size[2], depth[2];
    char* output = NULL;
    char* depthOutput = NULL;
    char* normalOutput = NULL;
    int max_bounces = 0;
    bool shadows = false;
    bool jitter = false;
    bool filter = false;
    
    // This loop loops over each of the input arguments.
    // argNum is initialized to 1 because the first
    // "argument" provided to the program is actually the
    // name of the executable (in our case, "a4").
    for( int argNum = 1; argNum < argc; ++argNum )
    {
        
        cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
        
        if (strcmp(argv[argNum],"-input") == 0){
            filename = argv[argNum+1];
            
        }
        
        if (strcmp(argv[argNum],"-size") == 0){
            size[0] = atoi(argv[argNum+1]);
            size[1] = atoi(argv[argNum+2]);
            
        }
        
        if (strcmp(argv[argNum],"-output") == 0){
            output = argv[argNum+1];
            
        }
        
        if (strcmp(argv[argNum],"-depth") == 0){
            depth[0] = atoi(argv[argNum+1]);
            depth[1] = atoi(argv[argNum+2]);
            depthOutput = argv[argNum+3];
        }
        
        if (strcmp(argv[argNum],"-normals") == 0){
            normalOutput = argv[argNum+1];
        }
        
        if (strcmp(argv[argNum],"-bounces") == 0){
            max_bounces = atoi(argv[argNum+1]);
        }
        
        if (strcmp(argv[argNum],"-shadows") == 0){
            shadows = true;
        }
        
        if (strcmp(argv[argNum],"-jitter") == 0){
            jitter = true;
        }
        
        if (strcmp(argv[argNum],"-filter") == 0){
            filter = true;
        }
        
    }
    
    
    
    // First, parse the scene using SceneParser.
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    
    /*
     Write the main function that reads the scene (using the parsing code provided),
     loops over the pixels in the image plane, generates a ray using your camera class,
     intersects it with the high-level Group that stores the objects of the scene, and
     writes the color of the closest intersected object.
     
     Up to this point, you may choose to render some spheres with a single color. If there is an intersection, use one color and if not, use another color.
     */
    
    SceneParser sceneParser = SceneParser(filename);
    Group* group = sceneParser.getGroup();
    RayTracer rayTracer = RayTracer( &sceneParser, max_bounces, shadows);
    
    if (jitter){
        
        // jitter sampling
        int jitter_x = size[0] * 3;
        int jitter_y = size[1] * 3;

        Image jitter_image = Image(jitter_x, jitter_y);
        Camera* camera = sceneParser.getCamera();
        
        for (int i=0 ; i<jitter_x ; i++){
            for (int j=0 ; j<jitter_y ; j++){
                Hit hit = Hit(FLT_MAX, NULL, Vector3f(0.0f,0.0f,0.0f));
                
                float r_i = (float)rand()/(float)RAND_MAX - 0.5f;
                float r_j = (float)rand()/(float)RAND_MAX - 0.5f;
                
                Vector2f jitter_pixel = imagePixelToCameraPoint(jitter_x, jitter_y, i+r_i, j+r_j);
                
                Ray ray = sceneParser.getCamera()->generateRay( jitter_pixel );
                group->intersect( ray, hit, camera->getTMin() );
                Vector3f c_pixel = rayTracer.traceRay(ray, camera->getTMin(), max_bounces, 1.0f, hit);
                
                jitter_image.SetPixel(i, j, c_pixel);
            }
        }
        
        // gaussian blur, Horizontal
        Image filter_image = Image(jitter_x, jitter_y);
        float K[] = {0.1201, 0.2339, 0.2931, 0.2339, 0.1201};
        for (int i=0 ; i<jitter_x ; i++){
            for (int j=0 ; j<jitter_y ; j++){
                // checkGaussianOutOfBounds( int upperLim, int i, int delta)
                Vector3f filterCol = K[0]*jitter_image.GetPixel(i, checkGaussianOutOfBounds( jitter_y, j, -2))
                                    + K[1]*jitter_image.GetPixel(i, checkGaussianOutOfBounds( jitter_y, j, -1))
                                    + K[2]*jitter_image.GetPixel(i, checkGaussianOutOfBounds( jitter_y, j, 0))
                                    + K[3]*jitter_image.GetPixel(i, checkGaussianOutOfBounds( jitter_y, j, +1))
                                    + K[4]*jitter_image.GetPixel(i, checkGaussianOutOfBounds( jitter_y, j, +2));
                filter_image.SetPixel(i, j, filterCol);
            }
        }
        
        // gaussian blur, Vertical
        for (int i=0 ; i<jitter_x ; i++){
            for (int j=0 ; j<jitter_y ; j++){
                // checkGaussianOutOfBounds(int upperLim, int i, int delta)
                Vector3f filterCol = K[0]*filter_image.GetPixel(checkGaussianOutOfBounds( jitter_x, i, -2), j)
                                    + K[1]*filter_image.GetPixel(checkGaussianOutOfBounds( jitter_x, i, -1), j)
                                    + K[2]*filter_image.GetPixel(checkGaussianOutOfBounds( jitter_x, i, 0), j)
                                    + K[3]*filter_image.GetPixel(checkGaussianOutOfBounds( jitter_x, i, +1), j)
                                    + K[4]*filter_image.GetPixel(checkGaussianOutOfBounds( jitter_x, i, +2), j);
                filter_image.SetPixel(i, j, filterCol);
            }
        }
        
        
        
        // down sampling
        Image image = Image(size[0], size[1]);
        for (int i=0 ; i<size[0] ; i++){
            for (int j=0 ; j<size[1] ; j++){
                Vector3f pixelColour =  filter_image.GetPixel((3*i)+0, (3*j)+0)
                                    +   filter_image.GetPixel((3*i)+0, (3*j)+1)
                                    +   filter_image.GetPixel((3*i)+0, (3*j)+2)
                                    +   filter_image.GetPixel((3*i)+1, (3*j)+0)
                                    +   filter_image.GetPixel((3*i)+1, (3*j)+1)
                                    +   filter_image.GetPixel((3*i)+1, (3*j)+2)
                                    +   filter_image.GetPixel((3*i)+2, (3*j)+0)
                                    +   filter_image.GetPixel((3*i)+2, (3*j)+1)
                                    +   filter_image.GetPixel((3*i)+2, (3*j)+2);
                
                image.SetPixel(i, j, pixelColour/9.0f);
            }
        }
        
        image.SaveImage( output);
        
    } else {
        Image image = Image(size[0], size[1]);
        Camera* camera = sceneParser.getCamera();
        
        for (int i=0 ; i<size[0] ; i++){
            for (int j=0 ; j<size[1] ; j++){
                Hit hit = Hit(FLT_MAX, NULL, Vector3f(0.0f,0.0f,0.0f));
                Ray ray = sceneParser.getCamera()->generateRay( imagePixelToCameraPoint(size[0], size[1], i, j) );
                group->intersect( ray, hit, camera->getTMin() );
                Vector3f c_pixel = rayTracer.traceRay(ray, camera->getTMin(), max_bounces, 1.0f, hit);
                
                image.SetPixel(i, j, c_pixel);
            }
        }
        image.SaveImage( output);
    }
    
    return 0;
}












