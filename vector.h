#ifndef VECTOR_H
#define VECTOR_H
#include <math.h>

struct vector3 {
    float x,y,z; 

    static vector3 normal(vector3 a, vector3 b, vector3 c){
            vector3 ret = cross(b-a, c-b);
            normalize(ret);
            return ret;
    }
    
    static inline vector3 cross(vector3 a, vector3 b){
            vector3 ret;
            ret.x = a.y*b.z - a.z*b.y;
            ret.y = a.x*b.z - a.z*b.x;
            ret.z = a.x*b.y - a.y*b.x;
    
            return ret;
    }
    
    static inline void normalize( vector3 &foo ){
            double magnitude = foo.x*foo.x + foo.y*foo.y + foo.z*foo.z;
            magnitude = sqrt(magnitude);
            foo.x /= magnitude;
            foo.y /= magnitude;
            foo.z /= magnitude;
    }
    
    vector3 operator-(const vector3 &b){
            vector3 ret = { x - b.x , y - b.y, z - b.z };
            return ret;
    }
};
#endif//VECTOR_H
