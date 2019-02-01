#ifndef GLWIDGET_H
#define GLWIDGET_H


#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "trianglemesh.h"
#include <QGLShaderProgram>
#include <GeometryP.h>
#include <Particle.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <QOpenGLTimerQuery>


class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{

public:
	GLWidget(QWidget *parent);
	~GLWidget();

	void loadMesh(const QString &filename);
    void loadSphere(const QString &filename);
	void setPolygonMode(bool bFill);
    void changeHomework(int index);
    void changeHomeworkState(int index);


    void setXPosr(float a);
    void setYPosr(float a);
    void setZPosr(float a);


    void initParticleSpring();
    void change_ke(float new_ke);
    void change_kd(float new_kd);
    void change_keB(float new_keB);
    void change_kdB(float new_kdB);


protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	void setProjection(float aspect);
	void setModelview();
    QMatrix4x4 getModelview();
    void resetParticle(int k);

    void renderParticleSystem();
    void renderParticleSpring();
    void initParticleSystem();
    void collisionSphere(int k, glm::vec3 prevPos);
    void collisionPlane(int k, glm::vec3 prevPos);
    void collisionTriangle(int k, glm::vec3 prevPos);



private:
	bool bPolygonFill;
	float angleX, angleY, distance;
    QPoint lastMousePos;


    int homework;
    float initLenghtH;
    float initLenghtV;
    float initDiag;
    int HW2state=0;
    int xParticles;
    int yParticles;

    //QOpenGLShaderProgram *program;
    QGLShaderProgram *program;
	TriangleMesh mesh;

    TriangleMesh room;
    TriangleMesh trianglesMesh;
    TriangleMesh floor;

    bool SemiEuler=true;

    //Particles
    Particle p;
    std::vector<Particle> particles;
    int numParticles;
    glm::vec3 initPPos;
    bool fountain=true;

    float dt = 0.03f;
    //Planes
    std::vector<Plane> planes;
//    Plane planeFloor;
//    Plane planeBack;
//    Plane planeFront;
//    Plane planeRight;
//    Plane planeLeft;


    //Spheres
    std::vector<Sphere> spheres;

    //Triangles
    std::vector<TriangleP> triangles;



    //SPRINGS
    float ke=100;
    float kd=0.5;
    float keB=0.02;
    float kdB=0.5;
    float gravSpring= -1;

    std::vector<std::vector<bool>> springForceM;



public slots:
    void resetScene();
    void changeSolver(bool b);

    void changeFountain(bool b);



};

#endif // GLWIDGET_H
