#ifndef GROUP_H
#define GROUP_H


#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>


using  namespace std;

///TODO:
///Implement Group
///Add data structure to store a list of Object*
class Group:public Object3D
{
public:
	
	Group(){
		
	}
	
	Group( int num_objects ){
        this->num_objects = num_objects;
	}
	
	~Group(){
		
	}
	
	virtual bool intersect( const Ray& r , Hit& h , float tmin ) {
        bool update = false;
        
        for (int i=0 ; i<listOfObjects.size() ; i++){
            if( listOfObjects[i]->intersect(r, h, tmin) ){
                update = true;
            }
        }
		return update;
	}
	
	void addObject( int index , Object3D* obj ){
        (this->listOfObjects).push_back(obj);
	}
	
	int getGroupSize(){
        return this->listOfObjects.size();
	}
	
private:
    vector<Object3D*> listOfObjects;
    int num_objects;
};

#endif