#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vecmath.h"
#include "Object3D.h"
#include "VecUtils.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public:
    
    /*
     Fill in subclass Transform from Object3D. Similar to a Group, a Transform will store a pointer to an Object3D (but only one, not an array). The constructor of a Transform takes a 4 × 4 matrix as input and a pointer to the Object3D modified by the transformation: Transform( const Matrix4f& m, Object3D* o ); The intersect routine will first transform the ray, then delegate to the intersect routine of the contained object. Make sure to correctly transform the resulting normal according to the rule seen in lecture. You may choose to normalize the direction of the transformed ray or leave it un-normalized. If you decide not to normalize the direction, you might need to update some of your intersection code. Instancing.
     */
    
    Transform(){}
    
    Transform( const Matrix4f& m, Object3D* obj ):o(obj){
        this->o = obj;
        this->trans = m;
    }
    
    ~Transform(){}
    
    virtual bool intersect( const Ray& r , Hit& h , float tmin){
        Vector3f origin_ws = r.getOrigin();
        Vector3f origin_os = VecUtils::transformPoint(trans.inverse(), origin_ws);
        
        Vector3f direction_ws = r.getDirection();
        Vector3f direction_os = VecUtils::transformDirection(trans.inverse(), direction_ws);
        
        Ray transformedRay = Ray(origin_os, direction_os);
        
        bool update = this->o->intersect(transformedRay, h, tmin);
        
        if (update){
            Vector3f transformedNormal = VecUtils::transformDirection( this->trans.inverse().transposed() ,h.getNormal());
            h.set(h.getT(), h.getMaterial(), transformedNormal);
        }
        
        return update;
    }

protected:
    Object3D* o; //un-transformed object
    Matrix4f trans;

};

#endif //TRANSFORM_H
