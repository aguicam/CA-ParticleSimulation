#ifndef _GEOMETRYP_H
#define _GEOMETRYP_H

#include <glm\glm.hpp>

struct GeometryP{
	virtual void setPosition(const glm::vec3& newPos) = 0;
	virtual bool isInside(const glm::vec3& point) = 0;
};

struct Plane : public GeometryP {
	glm::vec3 normal;
	float dconst;
	Plane(){};
	~Plane() {};
	Plane(const glm::vec3& point, const glm::vec3& normalVect);
	Plane(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2);

	void setPosition(const glm::vec3& newPos);
    void setNormal(const glm::vec3& newNorm);
	bool isInside(const glm::vec3& point);
	float distPoint2Plane(const glm::vec3& point);
	glm::vec3 closestPointInPlane(const glm::vec3& point);
    glm::vec3 intersecSegment(glm::vec3 point1, glm::vec3 point2);
    glm::vec3 intersecPos(glm::vec3 p1,float bounce);
    glm::vec3 intersecVelocity(glm::vec3 p1, glm::vec3 v1, float bounce, float isVerlet, float dt);
    void setPosAndNorm(const glm::vec3& newPos,const glm::vec3& newNorm);
};	

struct TriangleP : public Plane {
    glm::vec3 vertex1, vertex2, vertex3;
    float area;
    TriangleP();
    virtual ~TriangleP();
    TriangleP(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2);
    void setNewVertices(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2);
    bool isInsideArea(const glm::vec3& point);
    bool intersecSegment(const glm::vec3& point1, const glm::vec3& point2, glm::vec3& pTall);
    glm::vec3 intersecSegmentP(const glm::vec3& point1,const glm::vec3& point2);
};

struct Sphere : public GeometryP {
    glm::vec3 center;
    float radi;
    Sphere();
    virtual ~Sphere();
    Sphere(const glm::vec3& point, const float& radious);
    void setPosition(const glm::vec3& newPos);
    bool isInside(const glm::vec3& point);
    bool intersecSegment(const glm::vec3& point1, const glm::vec3& point2, glm::vec3& pTall);
    float distPointToCenter(const glm::vec3& point);
    glm::vec3 intersectingPointSphere(const glm::vec3& pa,const glm::vec3& pn );
};

#endif
