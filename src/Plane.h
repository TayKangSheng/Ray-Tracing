#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include "vecmath.h"
#include <cmath>
using namespace std;
///TODO: Implement Plane representing an infinite plane
///choose your representation , add more fields and fill in the functions
class Plane: public Object3D
{
public:
	Plane() {}
    
	Plane( const Vector3f& normal , float d , Material* m):Object3D(m) {
        
        this->normal = normal;
        this->D = d;
        this->m = m;
        
	}
    
	~Plane() {}
    
	virtual bool intersect( const Ray& r , Hit& h , float tmin) {
        Vector3f r_o = r.getOrigin();
        Vector3f r_d = r.getDirection();
        
        float t_value = -1.0f * ( -1.0f*D + Vector3f::dot(normal, r_o) ) / (Vector3f::dot(normal, r_d));
        
        if (t_value > tmin && t_value < h.getT()){
            h.set(t_value, m, normal);
            return true;
        }
        
        
        return false;
	}
	
protected:
    Vector3f normal;
    float D;
    Material* m;
    
};
#endif //PLANE_H


