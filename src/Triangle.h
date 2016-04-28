#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include "vecmath.h"
#include <cmath>
#include <iostream>

using namespace std;
///TODO: implement this class.
///Add more fields as necessary,
///but do not remove hasTex, normals or texCoords
///they are filled in by other components
class Triangle: public Object3D
{
public:
	Triangle();
    
    ///@param a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m):Object3D(m){
        this->a = a;
        this->b = b;
        this->c = c;
        this->material = m;
        hasTex = false;
	}

	virtual bool intersect( const Ray& ray,  Hit& hit , float tmin){
        
        Vector3f ro = ray.getOrigin();
        Vector3f rd = ray.getDirection();
        
        Matrix3f A(a.x()-b.x(), a.x()-c.x(), rd.x(),
                   a.y()-b.y(), a.y()-c.y(), rd.y(),
                   a.z()-b.z(), a.z()-c.z(), rd.z());
        
        Matrix3f beta_top(a.x()-ro.x(), a.x()-c.x(), rd.x(),
                         a.y()-ro.y(), a.y()-c.y(), rd.y(),
                         a.z()-ro.z(), a.z()-c.z(), rd.z());

        float beta = beta_top.determinant() / A.determinant();
        
        Matrix3f gamma_top(a.x()-b.x(), a.x()-ro.x(), rd.x(),
                          a.y()-b.y(), a.y()-ro.y(), rd.y(),
                          a.z()-b.z(), a.z()-ro.z(), rd.z());
        
        float gamma = gamma_top.determinant() / A.determinant();
        
        
        Matrix3f t_top(a.x()-b.x(), a.x()-c.x(), a.x()-ro.x(),
                      a.y()-b.y(), a.y()-c.y(), a.y()-ro.y(),
                      a.z()-b.z(), a.z()-c.z(), a.z()-ro.z());
        
        float t = t_top.determinant() / A.determinant();
        
        if ( (beta+gamma)<=1 && beta>=0 && gamma>=0 ){
            if ( t<hit.getT() && t>tmin ){
                float alpha = 1.0f - beta - gamma;
                Vector3f normal = alpha*normals[0] + beta*normals[1] + gamma*normals[2];
                
                hit.set(t, material, normal);
                
                Vector2f newTexCoord = (alpha*this->texCoords[0] + beta*this->texCoords[1] + gamma*this->texCoords[2]);
                hit.setTexCoord(newTexCoord);
                
                return true;
            }
        }
        
        
		return false;
	}
    
	bool hasTex;
	Vector3f normals[3];
	Vector2f texCoords[3];
protected:
    Vector3f a, b, c;
    Material* material;

};

#endif //TRIANGLE_H
