//#pragma once
#include "GeometryP.h"
#include <iostream>

//****************************************************
// Plane
//****************************************************

Plane::Plane(const glm::vec3& point, const glm::vec3& normalVect){
	normal = glm::normalize(normalVect);
	dconst = -glm::dot(point, normal);
}

Plane::Plane(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2){
	glm::vec3 v1 = point1 - point0;
	glm::vec3 v2 = point2 - point0;
	normal = glm::normalize(glm::cross(v1, v2));
	dconst = -glm::dot(point0, normal);
}

void Plane::setPosition(const glm::vec3& newPos){
	dconst = -glm::dot(newPos, normal);
}
void Plane::setNormal(const glm::vec3& newNorm){

    normal=glm::normalize(newNorm);
  //  std::cout<<" The normal is "<<normal.x<< " "<< normal.y<< " " <<normal.z<<" the d is "<<dconst<<std::endl;
}
void Plane::setPosAndNorm(const glm::vec3& newPos,const glm::vec3& newNorm){
    normal = glm::normalize(newNorm);
    dconst = -glm::dot(newPos, normal);
}


bool Plane::isInside(const glm::vec3& point){
	float dist;
	dist = glm::dot(point, normal) + dconst;
	if (dist > 1.e-7)
		return false;
	else
		return true;
}

float Plane::distPoint2Plane(const glm::vec3& point){
	float dist;
	return dist = glm::dot(point, normal) + dconst;
}

glm::vec3 Plane::closestPointInPlane(const glm::vec3& point){
	glm::vec3 closestP;
	float r = (-dconst - glm::dot(point, normal));
	return closestP = point + r*normal;
}

glm::vec3 Plane::intersecSegment(glm::vec3 point1,glm::vec3 point2){
	float r = (-dconst - glm::dot(point1, normal)) / glm::dot((point2 - point1), normal);
    return (1 - r)*point1 + r*point2;
}

glm::vec3 Plane::intersecPos(glm::vec3 p1,float bounce){
return p1-(1+bounce)*(glm::dot(p1,normal)+dconst)*normal;
}

glm::vec3 Plane::intersecVelocity(glm::vec3 p1, glm::vec3 v1, float bounce, float isVerlet, float dt){

    if(isVerlet){
       // intersecSegment(p1,intersecPos(p1,bounce))
        return (intersecPos(p1,bounce)-p1)/dt;
    }
    else{
        return v1-(1+bounce)*glm::dot(v1,normal)*normal;
    }
}


//****************************************************
// Triangle
//****************************************************
TriangleP::TriangleP(){}
TriangleP::~TriangleP(){}

TriangleP::TriangleP(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2){
    vertex1=point0;
    vertex2=point1;
    vertex3=point2;
    normal=glm::normalize(glm::cross(vertex2-vertex1,vertex3-vertex1));
    dconst= -glm::dot(point0, normal);
    area= 0.5*glm::length(glm::cross(vertex2-vertex1,vertex3-vertex2));
}


void TriangleP::setNewVertices(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2){
    vertex1=point0;
    vertex2=point1;
    vertex3=point2;
    normal=glm::normalize(glm::cross(vertex2-vertex1,vertex3-vertex1));
    dconst= -glm::dot(point0, normal);
    area= 0.5*glm::length(glm::cross(vertex2-vertex1,vertex3-vertex2));
}

bool TriangleP::isInsideArea(const glm::vec3& point){
    float areaT1,areaT2,areaT3,dist;
    areaT1=0.5*glm::length(glm::cross(vertex2-point,vertex3-vertex2));
    areaT2=0.5*glm::length(glm::cross(point-vertex1,vertex3-point));
    areaT3=0.5*glm::length(glm::cross(vertex2-vertex1,point-vertex2));

    dist = areaT1+areaT2+areaT3-area;
    if (dist > 1.e-7)
        return false;
    else
        return true;
}

glm::vec3 TriangleP::intersecSegmentP(const glm::vec3& point1,const glm::vec3& point2){
    float r = (-dconst - glm::dot(point1, normal)) / glm::dot((point2 - point1), normal);
    return (1 - r)*point1 + r*point2;
}

//****************************************************
// Sphere
//****************************************************
Sphere::Sphere(){}
Sphere::~Sphere(){}

Sphere::Sphere(const glm::vec3& point, const float& radious){
    center=point;
    radi=radious;
}
void Sphere::setPosition(const glm::vec3& newPos){
    center=newPos;
}

float Sphere::distPointToCenter(const glm::vec3& point){
    return glm::length(point-center);
}

bool Sphere::isInside(const glm::vec3& point){
    if(distPointToCenter(point)>radi)return false;
    else return true;
}

glm::vec3 Sphere::intersectingPointSphere(const glm::vec3& pa,const glm::vec3& pn ){
    glm::vec3 v= pn-pa;
    float a = glm::dot(v,v);
    float b = glm::dot(2.0f*v,pa-center);
    float c = glm::dot(center,center)+glm::dot(pa,pa)-2.0*glm::dot(pa,center)-radi*radi;

    float lambda1 = (-b+glm::sqrt(b*b-4*a*c))/(2*a);
    float lambda2 = (-b-glm::sqrt(b*b-4*a*c))/(2*a);
    if(lambda1>=0 && lambda1<=1){
        return (1-lambda1)*pa+lambda1*pn;
    }
    if(lambda2>=0 && lambda2<=1){
        return (1-lambda2)*pa+lambda2*pn;
    }

}
