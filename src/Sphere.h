#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include "vecmath.h"
#include <cmath>

#include <iostream>
using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere: public Object3D
{
public:
	Sphere(){ 
		//unit ball at the center
	}

	Sphere( Vector3f center , float radius , Material* material ):Object3D(material){
        this->center = center;
        this->radius = radius;
	}
	

	~Sphere(){}

	virtual bool intersect( const Ray& r , Hit& h , float tmin){
		
        
        /*
            check ray intersect with this sphere or not.
            if it does, store t values.
         
         parameters: 
            ray contains the information about the ray
            hit contains information about hits
            tmin is used to restrict the range of intersection. If an intersection is found such that t > tmin and t is less than the value of the intersection currently stored in the Hit data structure, Hit is updated as necessary.
         */
        
        float a = r.getDirection().absSquared();
        float b = 2.0*(Vector3f::dot(r.getDirection(), (r.getOrigin() - this->center) ));
        float c = (r.getOrigin() - this->center).absSquared() - (radius*radius);
        
        float checkPositive = b*b - 4*a*c;
        if (checkPositive >= 0){
            float t_positive = ( -1.0*b + sqrt(checkPositive) )/( 2*a );
            float t_negative = ( -1.0*b - sqrt(checkPositive) )/( 2*a );
            bool update = false;
            
            if (t_positive > tmin && t_positive < h.getT()){
                Vector3f normal = (r.pointAtParameter(t_positive) - this->center).normalized();
                h.set(t_positive, this->material, normal);
                update = true;
            }
            
            if (t_negative > tmin && t_negative < h.getT()){
                Vector3f normal = ( r.pointAtParameter(t_negative) - this->center ).normalized();
                h.set(t_negative, this->material, normal);
                update = true;
            }
            
            if (update){
                return true;
            }
            
        }
        
        return false;
	}

protected:
    Vector3f center;
    float radius;

};


#endif
