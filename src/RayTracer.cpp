#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"

#define EPSILON 0.001

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.
Vector3f mirrorDirection( const Vector3f& normal, const Vector3f& incoming) {
    
    float vDotN = Vector3f::dot(incoming, normal);
    Vector3f reflectedDir = incoming - (2*vDotN*normal);
    
    return reflectedDir;
}

bool transmittedDirection( const Vector3f& normal, const Vector3f& incoming, float index_n, float index_nt, Vector3f& transmitted) {
    
    /*
     check if values inside square root is positive.
     if square root is positive then update Vector3f transmitted and return true
     else return false
     */
    
    float index_n_square = pow(index_n, 2);
    float index_nt_square = pow(index_nt,2);
    float dDotN = Vector3f::dot(incoming, normal);
    float checkPositive = 1.0f - (index_n_square*(1-pow(dDotN,2)))/index_nt_square;
    
    if (checkPositive > 0){
        transmitted = (( (index_n*(incoming-normal*dDotN))/index_nt ) - normal*sqrt(checkPositive));
        
        return true;
    }
    
    return false;
}

float computeR(float index_n, float index_nt, Vector3f incoming, Vector3f Normal, Vector3f transmittedDir){
    float c;
    
    if (index_n > index_nt){
        c = abs(Vector3f::dot(transmittedDir, Normal));
    } else {
        c = abs(Vector3f::dot(incoming, Normal));
    }
    
    float r0 = pow( (index_nt-index_n) / (index_nt + index_n) , 2);
    float R = r0 + (1-r0)*pow( (1-c) , 5);
    
    return R;
    
}


RayTracer::RayTracer( SceneParser * scene, int max_bounces, bool shadows
//more arguments if you need...
) :
m_scene(scene) {
    g=scene->getGroup();
    m_maxBounces = max_bounces;
    m_shadow = shadows;
}

RayTracer::~RayTracer()
{
}

Vector3f RayTracer::traceRay( Ray& ray, float tmin, int bounces, float refr_index, Hit& hit ) const {
//    cout << "\t\tbounces:\t" << bounces << endl;
    if (hit.getMaterial() != NULL){
        Vector3f c_pixel = hit.getMaterial()->getDiffuseColor()*m_scene->getAmbientLight();
        
        for (int l=0 ; l<m_scene->getNumLights() ; l++){
            Light* light = m_scene->getLight(l);
            Vector3f dir; // direction to light
            Vector3f col; // colour of light
            float distanceToLight;
            
            light->getIllumination(ray.pointAtParameter(hit.getT()), dir, col, distanceToLight); // this function updates direction to light (dir) and colour of light (col)
            
        
            /*
             check whether there is intersection between point and light.
             if there is, ignore light colour.
             else, add light colour.
             */
            Hit pointToLightHit = Hit(FLT_MAX, NULL, Vector3f(0.0f,0.0f,0.0f));
            Vector3f currentPoint = ray.pointAtParameter(hit.getT());
            Ray pointToLightRay = Ray(currentPoint, dir);
            bool pointToLightIntersect = g->intersect(pointToLightRay, pointToLightHit, EPSILON);
            
            if (pointToLightIntersect){
                ;
            } else {
                Vector3f shade = hit.getMaterial()->Shade(ray, hit, dir, col);
                c_pixel += shade;
            }
            
        }
        
        if (bounces > 0){
            // Add reflection/mirror colour for every pixel
            Vector3f reflectionPoint = ray.pointAtParameter(hit.getT());
            Vector3f reflectedDir = mirrorDirection(hit.getNormal().normalized(), ray.getDirection().normalized());
            Ray reflectedRay = Ray(reflectionPoint, reflectedDir);
            Hit reflectionHit = Hit(FLT_MAX, NULL, Vector3f(0.0f,0.0f,0.0f));
            g->intersect(reflectedRay, reflectionHit, EPSILON);
            
            Vector3f reflectionColour = hit.getMaterial()->getSpecularColor()*traceRay(reflectedRay, EPSILON, bounces-1, hit.getMaterial()->getRefractionIndex(), reflectionHit);
            
            
            // check if material is transparent
            
            if (hit.getMaterial()->getRefractionIndex() > 0){
            
                /*
                 Add refraction colour
                 - check if the ray is inside the object. If ray is inside object, (d.N)>0. d=direction of incoming ray, N=normal
                 - check whether theres refraction
                 */
                float dDotN = Vector3f::dot(ray.getDirection(), hit.getNormal());
                float index_n;
                float index_nt;
                Vector3f transmittedDir;
                Vector3f normal;
                
                if (dDotN > 0){
                    /*
                     Ray going from inside object to outside object
                     index_n = refraction index of current object (object)
                     index_nt = refraction index of object ray is going into (air)
                     */
                    
                    index_n = hit.getMaterial()->getRefractionIndex();
                    index_nt = 1.0f;
                    normal = -1.0f*hit.getNormal().normalized();
                    
                } else {
                    /*
                     Ray going from outside object to inside object
                     index_n = refraction index of current object (air)
                     index_nt = refraction index of object ray is going into (object)
                     */
                    
                    index_n = 1.0f;
                    index_nt = hit.getMaterial()->getRefractionIndex();
                    normal = hit.getNormal().normalized();
                }
                
                /*
                 if (checkRefraction == true), there is refraction
                 else, there is reflection.
                 */
                bool checkRefraction = transmittedDirection( normal, ray.getDirection(), index_n, index_nt, transmittedDir);
                if (checkRefraction){
                    Ray refractionRay = Ray(ray.pointAtParameter(hit.getT()), transmittedDir);
                    Hit refractionHit = Hit(FLT_MAX, NULL, Vector3f(0.0f,0.0f,0.0f));
                    g->intersect(refractionRay, refractionHit, EPSILON);
                    Vector3f refractionColour = hit.getMaterial()->getSpecularColor()*traceRay(refractionRay, EPSILON, bounces-1, hit.getMaterial()->getRefractionIndex(), refractionHit);
                    float R = computeR(index_n, index_nt, ray.getDirection(), normal, transmittedDir);
                    
                    c_pixel += R*reflectionColour + (1-R)*refractionColour;
                    
                } else {
                    c_pixel += reflectionColour;
                }

            } else {
                // only reflection, no refraction
                c_pixel += reflectionColour;
            }
            
        }
        
        return c_pixel;
        
    } else {
        
        return m_scene->getBackgroundColor(ray.getDirection());
    }
}








