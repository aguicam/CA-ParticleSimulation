#include "glwidget.h"
#include <iostream>
#include <QApplication>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <plyreader.h>
#include <random>

#define rand01() ((float)std::rand()/RAND_MAX)


using namespace std;




const float rotationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 1.0f;
const float maxDistanceCamera = 50.0f;


GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), bPolygonFill(true), angleX(45.0f), angleY(60.0f), distance(15.0f)
{
    program = NULL;
}

GLWidget::~GLWidget()
{
    if(program) delete program;
}


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    program = new QGLShaderProgram();
    program->addShaderFromSourceFile(QGLShader::Vertex, ":/shaders/simpleshader.vert");
    program->addShaderFromSourceFile(QGLShader::Fragment, ":/shaders/simpleshader.frag");

    program->link();
    if(!program->isLinked())
    {
        cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
        QApplication::quit();
    }
    program->bind();

    mesh.buildCube();
    room.buildRoom();
    trianglesMesh.buildTriangles();
    floor.buildFloor();

    if(!mesh.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    if(!room.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    if(!trianglesMesh.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    if(!floor.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }


    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);


    loadSphere(QString("sphere.ply"));

    switch(homework){
    case 0:
        initParticleSystem();
        break;
    case 1:
        initParticleSpring();
        break;

    }



}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    setProjection((float)w/h);
    setModelview();
}

void GLWidget::paintGL()
{

    switch(homework){
    case 0:
        renderParticleSystem();
        break;
    case 1:
        renderParticleSpring();
        break;

    }

    update();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    // Rotation
    if(event->buttons() & Qt::LeftButton)
    {
        angleX += rotationFactor * (event->y() - lastMousePos.y());
        angleX = max(-maxRotationCamera, min(angleX, maxRotationCamera));
        angleY += rotationFactor * (event->x() - lastMousePos.x());
    }
    // Zoom
    if(event->buttons() & Qt::RightButton)
    {
        distance += 0.01f * (event->y() - lastMousePos.y());
        distance = max(minDistanceCamera, min(distance, maxDistanceCamera));
    }

    lastMousePos = event->pos();

    makeCurrent();
    setModelview();
    doneCurrent();
    update();
}

void GLWidget::setProjection(float aspect)
{
    QMatrix4x4 projectionMatrix;

    projectionMatrix.perspective(60, aspect, 0.01, 100.0);
    program->bind();
    program->setUniformValue("projection", projectionMatrix);
    program->release();
}

void GLWidget::setModelview()
{
    QMatrix4x4 modelviewMatrix;

    modelviewMatrix.translate(0, 0, -distance);
    modelviewMatrix.rotate(angleX, 1.0f, 0.0f, 0.0f);
    modelviewMatrix.rotate(angleY, 0.0f, 1.0f, 0.0f);
    program->bind();
    program->setUniformValue("modelview", modelviewMatrix);
    program->setUniformValue("normalMatrix", modelviewMatrix.normalMatrix());
    program->release();
}

QMatrix4x4 GLWidget::getModelview()
{
    QMatrix4x4 modelviewMatrix;

    modelviewMatrix.translate(0, 0, -distance);
    modelviewMatrix.rotate(angleX, 1.0f, 0.0f, 0.0f);
    modelviewMatrix.rotate(angleY, 0.0f, 1.0f, 0.0f);
    //	program->bind();
    //	program->setUniformValue("modelview", modelviewMatrix);
    //	program->setUniformValue("normalMatrix", modelviewMatrix.normalMatrix());
    //	program->release();
    return modelviewMatrix;

}

void GLWidget::setPolygonMode(bool bFill)
{
    bPolygonFill = bFill;

    makeCurrent();
    if(bFill)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    doneCurrent();
    update();
}

void GLWidget::loadMesh(const QString &filename)
{
    PLYReader reader;

    mesh.destroy();
    reader.readMesh(filename, mesh);
    makeCurrent();
    if(!mesh.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    doneCurrent();
    update();
}
void GLWidget::loadSphere(const QString &filename)
{
    PLYReader reader;

    mesh.destroy();
    reader.readMesh(filename, mesh);
    makeCurrent();
    if(!mesh.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    doneCurrent();
}

void GLWidget::resetParticle(int k){
    particles[k].setPosition(initPPos);
    if(fountain){
        particles[k].setVelocity(rand01()-0.5, 10, rand01()-0.5);
    }else{
        particles[k].setVelocity(rand01()-0.5, 0, rand01()-0.5);
    }
    particles[k].setLifetime(10.0f);
    if(!SemiEuler){
        particles[k].setPreviousPosition(initPPos-dt*particles[k].getVelocity());
    }

}


void GLWidget::resetScene(){
    particles.clear();
    // p.setPosition(glm::vec3(0.0f,15.0f,0.0f));
    //  p.setVelocity(glm::vec3(0.0f,0.0f,0.0f));
    // update();
}
void GLWidget::changeSolver(bool b){
    SemiEuler=b;
}

void GLWidget::setXPosr(float a){
    initPPos=glm::vec3(a,initPPos.y,initPPos.z);
    p.setPosition(initPPos);
}
void GLWidget::setYPosr(float a){
    initPPos=glm::vec3(initPPos.x,a,initPPos.z);
    p.setPosition(initPPos);
}
void GLWidget::setZPosr(float a){
    initPPos=glm::vec3(initPPos.z,initPPos.y,a);
    p.setPosition(initPPos);
}

void GLWidget::changeFountain(bool b){
    fountain=b;
}

void GLWidget::changeHomework(int index){

    homework=index;
    switch(homework){
    case 0:
        initParticleSystem();
        break;
    case 1:
        initParticleSpring();
        break;

    }

    update();
}


void GLWidget::changeHomeworkState(int index){
    HW2state=index;
    initParticleSpring();
}







//RENDERING FUNCTIONS
void GLWidget::renderParticleSystem(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->bind();
    program->setUniformValue("bLighting", bPolygonFill);
    program->setUniformValue("color", QVector4D(1, 0.0, 0.0, 1.0));

    QMatrix4x4 model = getModelview();
    model = getModelview();

    model.scale(2);
    program->setUniformValue("modelview", model);
    program->setUniformValue("normalMatrix", model.normalMatrix());
    program->setUniformValue("color", QVector4D(0, 0.8, 0, 1.0));
    trianglesMesh.render(*this);

    program->setUniformValue("color", QVector4D(0.8, 0.8, 0.8, 1.0));
    //QMatrix4x4 model = getModelview();
    model = getModelview();

    model.scale(30);
    program->setUniformValue("modelview", model);
    program->setUniformValue("normalMatrix", model.normalMatrix());

    room.render(*this);

    //RENDER PARTICLES

    if((int)particles.size()<numParticles){
        if(fountain){
            p.setVelocity(rand01()-0.5, 10, rand01()-0.5);
        }else{
            p.setVelocity(rand01()-0.5, 0, rand01()-0.5);
        }
        particles.push_back(p);
    }

    for(int k= 0;k<(int)particles.size();k++){

        if(particles[k].getLifetime()<=0){
            resetParticle(k);
        }

        model = getModelview();
        glm::vec3 prevPos=particles[k].getCurrentPosition();
        if(SemiEuler){
            particles[k].updateParticle(dt, Particle::UpdateMethod::EulerSemi);
        }else{
            particles[k].updateParticle(dt, Particle::UpdateMethod::Verlet);
        }


        particles[k].setLifetime(particles[k].getLifetime() - dt);

        collisionPlane(k,prevPos);
        collisionTriangle(k,prevPos);
        collisionSphere(k,prevPos);

        glm::vec3 currentParticlePos = particles[k].getCurrentPosition();

        model.translate(currentParticlePos.x,currentParticlePos.y,currentParticlePos.z);
        model.scale(0.3);
        program->setUniformValue("modelview", model);
        program->setUniformValue("normalMatrix", model.normalMatrix());
        program->setUniformValue("color", QVector4D(0.8,0.7,0.1, 1.0));
        mesh.render(*this);

    }





    //Ground Sphere
    model = getModelview();
    model.scale(4);

    program->setUniformValue("modelview", model);
    program->setUniformValue("normalMatrix", model.normalMatrix());
    program->setUniformValue("color", QVector4D(0.8, 0, 0, 1.0));
    mesh.render(*this);

    //Air Sphere
    model = getModelview();
    model.translate(1.0f,5.0f,1.0f);
    model.scale(4);

    program->setUniformValue("modelview", model);
    program->setUniformValue("normalMatrix", model.normalMatrix());
    program->setUniformValue("color", QVector4D(0.8, 0, 0, 1.0));
    mesh.render(*this);

    setModelview();





    program->release();
}
void GLWidget::renderParticleSpring(){


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->bind();
    program->setUniformValue("bLighting", bPolygonFill);
    program->setUniformValue("color", QVector4D(1, 0.0, 0.0, 1.0));

    QMatrix4x4 model = getModelview();
    model = getModelview();

    model.scale(2);
    program->setUniformValue("modelview", model);
    program->setUniformValue("normalMatrix", model.normalMatrix());
    program->setUniformValue("color", QVector4D(0.6, 0.6, 0.6, 1.0));
    floor.render(*this);

    //Air Sphere
    model = getModelview();
    model.translate(0.0f,3.0f,0.0f);
    model.scale(4);

    program->setUniformValue("modelview", model);
    program->setUniformValue("normalMatrix", model.normalMatrix());
    program->setUniformValue("color", QVector4D(0.8, 0, 0, 1.0));
    mesh.render(*this);

    //RENDER PARTICLES
    //HW2 1D
    if(HW2state==0){
        glm::vec3 distVec = particles[1].getCurrentPosition()-particles[0].getCurrentPosition();
        glm::vec3 f1= (ke*(glm::length(distVec)-initLenghtH)+
                       kd*glm::dot((particles[1].getVelocity()-particles[0].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                       //kd*(particles[1].getVelocity()-particles[0].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
        particles[0].setForce(f1);
        glm::vec3 prevSF=f1;
        for(int k= 1;k<(int)particles.size()-1;k++){
            prevSF=-f1;
            distVec = particles[k+1].getCurrentPosition()-particles[k].getCurrentPosition();
            f1= (ke*(glm::length(distVec)-initLenghtH)+
                 kd*glm::dot((particles[k+1].getVelocity()-particles[k].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                 //kd*(particles[k+1].getVelocity()-particles[k].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            particles[k].setForce(prevSF+f1+glm::vec3(0.0f,gravSpring,0.0f));//Añado la gravedad
        }
        particles[(int)particles.size()-1].setForce(-f1+glm::vec3(0.0f,gravSpring,0.0f));//Añado la gravedad
    }
    else if(HW2state==1){
        //HW2 2D
        glm::vec3 distVec;
        //FIRST ALL CORNERS
        glm::vec3 f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12;
        for(int index=0;index<particles.size();index++){
            distVec = particles[index-1].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][0]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){//>1.e-7
                f1=(ke*(glm::length(distVec)-initLenghtH)+
                    kd*glm::dot((particles[index-1].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                    //kd*(particles[index-1].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f1=glm::vec3(0.0f);}
            distVec = particles[index-2].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][1]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){
                f2=(ke*(glm::length(distVec)-2*initLenghtH)+
                    kd*glm::dot((particles[index-2].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                    //kd*(particles[index-2].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f2=glm::vec3(0.0f);}

            distVec = particles[index+1].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][2]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){
                f3=(ke*(glm::length(distVec)-initLenghtH)+
                    kd*glm::dot((particles[index+1].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                    //kd*(particles[index+1].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f3=glm::vec3(0.0f);}

            distVec = particles[index+2].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][3]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){
                f4=(ke*(glm::length(distVec)-2*initLenghtH)+
                    kd*glm::dot((particles[index+2].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                    //kd*(particles[index+2].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f4=glm::vec3(0.0f);}

            distVec = particles[index-xParticles].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][4]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){
                f5=(ke*(glm::length(distVec)-initLenghtV)+
                    kd*glm::dot((particles[index-xParticles].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                    //kd*(particles[index-xParticles].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f5=glm::vec3(0.0f);}

            distVec = particles[index-2*xParticles].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][5]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){
                f6=(ke*(glm::length(distVec)-2*initLenghtV)+
                    kd*glm::dot((particles[index-2*xParticles].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                    //kd*(particles[index-2*xParticles].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f6=glm::vec3(0.0f);}

            distVec = particles[index+xParticles].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][6]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){
                f7=(ke*(glm::length(distVec)-initLenghtV)+
                    kd*glm::dot((particles[index+xParticles].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                    //kd*(particles[index+xParticles].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f7=glm::vec3(0.0f);}

            distVec = particles[index+2*xParticles].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][7]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){
                f8=(ke*(glm::length(distVec)-2*initLenghtV)+
                    kd*glm::dot((particles[index+2*xParticles].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                    //kd*(particles[index+2*xParticles].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f8=glm::vec3(0.0f);}

            distVec = particles[index-xParticles-1].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][8]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){
                f9=(keB*(glm::length(distVec)-initDiag)+
                    kdB*glm::dot((particles[index-xParticles-1].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                    //kd*(particles[index-xParticles-1].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f9=glm::vec3(0.0f);}

            distVec = particles[index-xParticles+1].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][9]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){
                f10=(keB*(glm::length(distVec)-initDiag)+
                     kdB*glm::dot((particles[index-xParticles+1].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                     //kd*(particles[index-xParticles+1].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f10=glm::vec3(0.0f);}

            distVec = particles[index+xParticles-1].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][10]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){
                f11=(keB*(glm::length(distVec)-initDiag)+
                     kdB*glm::dot((particles[index+xParticles-1].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                     //kd*(particles[index+xParticles-1].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f11=glm::vec3(0.0f);}

            distVec = particles[index+xParticles+1].getCurrentPosition()-particles[index].getCurrentPosition();
            if(springForceM[index][11]&&!isinf(glm::length(glm::abs(distVec)))&&glm::length(glm::abs(distVec))>1.e-7){
                f12=(keB*(glm::length(distVec)-initDiag)+
                     kdB*glm::dot((particles[index+xParticles+1].getVelocity()-particles[index].getVelocity()),glm::normalize(distVec)))*glm::normalize(distVec);
                     //kd*(particles[index+xParticles+1].getVelocity()-particles[index].getVelocity())*distVec/glm::length(distVec))*distVec/glm::length(distVec);
            }else{f12=glm::vec3(0.0f);}

            glm::vec3 force =f1+f3+f5+f7+glm::vec3(0.0f,gravSpring-1,0.0f);

            particles[index].setForce(force);//f1+f2+f3+f4+f5+f6+f7+f8+f9+f10+f11+f12
        }

    }

    //CHECK COLLISIONS
    for(int k= 0;k<(int)particles.size();k++){
        model = getModelview();

        if(!particles[k].isFixed()){
            glm::vec3 prevPos=particles[k].getCurrentPosition();
            particles[k].updateParticle(dt, Particle::UpdateMethod::Verlet);
            collisionPlane(k,prevPos);
            collisionSphere(k,prevPos);
        }
        glm::vec3 currentParticlePos = particles[k].getCurrentPosition();
        model.translate(currentParticlePos.x,currentParticlePos.y,currentParticlePos.z);
        model.scale(0.3);
        program->setUniformValue("modelview", model);
        program->setUniformValue("normalMatrix", model.normalMatrix());
        program->setUniformValue("color", QVector4D(0.8,0.7,0.1, 1.0));
        mesh.render(*this);

    }

    setModelview();
    program->release();
}

//INITIALIZING FUNCTIONS
void GLWidget::initParticleSystem(){
    particles.clear();
    triangles.clear();
    spheres.clear();
    planes.clear();


    //INITIALIZATION OF PARTICLES
    numParticles = 500;
    initPPos=glm::vec3(0.0f, 8.0f, 0.0f);
    Particle ps(0.0f, 8.0f, 0.0f);
    p=ps;
    p.setLifetime(10.0f);
    std::cout << "Lifetime =" << p.getLifetime() << std::endl;
    p.setBouncing(0.8f);
    p.setVelocity(rand01()-0.5, 0, rand01()-0.5);
    p.addForce(0, -9.8f, 0);
    if(!SemiEuler){
        p.setPreviousPosition(initPPos-dt*p.getVelocity());
    }


    //We define the planes

    // define a floor plane for collision
    glm::vec3 punt(0.0f);
    glm::vec3 normal(0, 1, 0);
    Plane pl(punt, normal);
    planes.push_back(pl);
    //FrontPlane (z+)
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,15.0f),glm::vec3(0,0,-1));
    planes.push_back(pl);
    //Back plane (z-)
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,-15.0f),glm::vec3(0,0,1));
    planes.push_back(pl);
    //Right plane (x+)
    pl.setPosAndNorm(glm::vec3(15.0f,0.0f,0.0f),glm::vec3(-1,0,0));
    planes.push_back(pl);
    //Left plane (x-)
    pl.setPosAndNorm(glm::vec3(-15.0f,0.0f,0.0f),glm::vec3(1,0,0));
    planes.push_back(pl);


    //Definde Spheres
    glm::vec3 cent(0.0f,0.0f,0.0f);
    float radious= 2;
    Sphere sp(cent,radious);
    spheres.push_back(sp);
    sp.setPosition(glm::vec3(1.0f, 5.0f,1.0f));
    spheres.push_back(sp);


    //Define Triangles
    glm::vec3 v1(-3.0f, 0.0f, -6.0f);
    glm::vec3 v2(-10.0f, 0.0f, 2.0f);
    glm::vec3 v3(-12.0f,  12.0f, -12.0f);

    TriangleP tri(v2,v1,v3);
    triangles.push_back(tri);
    tri.setNewVertices(glm::vec3(-2.5f, 4.0f, 0.5f),glm::vec3(-5.0f, 5.0f, 2.0f),glm::vec3(-1.5f, 1.0f, 4.0f));
    triangles.push_back(tri);
}
void GLWidget::initParticleSpring(){

    particles.clear();
    triangles.clear();
    spheres.clear();
    planes.clear();

    //INITIALIZATION OF PARTICLES
    numParticles = 100;
    glm::vec3 pospart(-5.0f, 8.0f, 0.0f);
    Particle ps(-5.0f, 8.0f, 0.0f);
    p=ps;
    p.setLifetime(100.0f);
    //std::cout << "Lifetime =" << p.getLifetime() << std::endl;
    p.setBouncing(0.8f);
    p.addForce(0, -9.8f, 0);
    p.setFixed(true);

    //HW2 1D
    if(HW2state==0){
        particles.push_back(p);
        p.setFixed(false);
        initLenghtH = 10.0f/numParticles;
        for(int i =0;i<numParticles-1;i++){
            pospart=pospart +glm::vec3(initLenghtH,0.0f,0.0f);
            p.setPreviousPosition(pospart-dt*glm::vec3(0.0f,gravSpring,0.0f));
            p.setPosition(pospart);
            particles.push_back(p);
        }
    }
    else if(HW2state==1){
        //HW2 2D
        xParticles=50;
        yParticles=25;
        numParticles= yParticles*xParticles;
        initLenghtH=15.0f/xParticles;
        initLenghtV=10.0f/yParticles;
        initDiag = sqrt(initLenghtH*initLenghtH+initLenghtV*initLenghtV);

        pospart=glm::vec3(-7.0f, 8.0f, 2.0f);
        p.setFixed(true);
        for(int i=0;i<xParticles;i++){
            pospart=pospart +glm::vec3(initLenghtH,0.0f,0.0f);
            p.setPreviousPosition(pospart-dt*glm::vec3(0.0f,gravSpring,0.0f));
            p.setPosition(pospart);
            particles.push_back(p);
        }
        p.setFixed(false);
        for(int j=1;j<yParticles;j++){
            pospart=pospart-glm::vec3(xParticles*initLenghtH,0.0f,initLenghtV);
            for(int i=0;i<xParticles;i++){
                pospart=pospart +glm::vec3(initLenghtH,0.0f,0.0f);
                p.setPreviousPosition(pospart-dt*glm::vec3(0.0f,gravSpring,0.0f));
                p.setPosition(pospart);
                particles.push_back(p);
            }
        }

        for(int j=0;j<yParticles;j++){
            for(int i=0;i<xParticles;i++){

                std::vector<bool> aux;
                //F1
                aux.push_back(!(i==0));
                //F2
                aux.push_back( !(i<2));
                //F3
                aux.push_back(!(i==xParticles-1));
                //F4
                aux.push_back(!(i>=xParticles-3));

                //F5
                aux.push_back(!(j==0));
                //F6
                aux.push_back(!(j<2));
                //F7
                aux.push_back(!(j==yParticles-1));
                //F8
                aux.push_back(!(j>=yParticles-3));

                //F9
                aux.push_back(!(j==0||i==xParticles-1));
                //F10
                aux.push_back(!(j==0||i==0));
                //F11
                aux.push_back(!(i==xParticles-1||j==yParticles-1));
                //F12
                aux.push_back(!(i==0||j==yParticles-1));

                springForceM.push_back(aux);

            }
        }

    }





    //We define the planes

    // define a floor plane for collision
    glm::vec3 punt(0.0f);
    glm::vec3 normal(0, 1, 0);
    Plane pl(punt, normal);
    planes.push_back(pl);

    //Definde Spheres
    glm::vec3 cent(0.0f,3.0f,0.0f);
    float radious= 2;
    Sphere sp(cent,radious);
    spheres.push_back(sp);

    SemiEuler=false;
}


void GLWidget::collisionPlane(int k, glm::vec3 prevPos){
    for(int i=0;i<(int)planes.size();i++){
        if(planes[i].distPoint2Plane(particles[k].getCurrentPosition())*planes[i].distPoint2Plane(prevPos)< 0){
            glm::vec3 interPos = particles[k].getCurrentPosition();
            glm::vec3 pointPos = planes[i].intersecPos(particles[k].getCurrentPosition(),particles[k].getBouncing());
            glm::vec3 pointVel = planes[i].intersecVelocity(interPos,particles[k].getVelocity(),particles[k].getBouncing(),!SemiEuler,dt);
            particles[k].setPosition(pointPos.x, pointPos.y, pointPos.z);
            particles[k].setVelocity(pointVel.x,pointVel.y, pointVel.z);
        }
    }
}
void GLWidget::collisionTriangle(int k, glm::vec3 prevPos){
    for(int i=0;i<(int)triangles.size();i++){
        if (triangles[i].distPoint2Plane(particles[k].getCurrentPosition())*triangles[i].distPoint2Plane(prevPos)< 0) {
            // std::cout<<" Current position: "<<particles[k].getCurrentPosition().x <<" "<<particles[k].getCurrentPosition().y <<" "<<particles[k].getCurrentPosition().z <<std::endl;
            glm::vec3 interPos = particles[k].getCurrentPosition();
            glm::vec3 pointPos = triangles[i].intersecPos(particles[k].getCurrentPosition(),particles[k].getBouncing());
            glm::vec3 pointVel = triangles[i].intersecVelocity(interPos,particles[k].getVelocity(),particles[k].getBouncing(),!SemiEuler,dt);
            if(triangles[i].isInsideArea(triangles[i].intersecSegmentP(particles[k].getPreviousPosition(),particles[k].getCurrentPosition()))){
                particles[k].setPosition(pointPos.x, pointPos.y, pointPos.z);
                particles[k].setVelocity(pointVel.x,pointVel.y, pointVel.z);
            }
        }
    }
}
void GLWidget::collisionSphere(int k, glm::vec3 prevPos){
    for(int i=0;i<(int)spheres.size();i++){

        if(spheres[i].isInside(particles[k].getCurrentPosition())){
            Plane tangPlane(spheres[i].intersectingPointSphere(prevPos,particles[k].getCurrentPosition()),glm::normalize(spheres[i].intersectingPointSphere(prevPos,particles[k].getCurrentPosition())-spheres[i].center));
            glm::vec3 interPos = particles[k].getCurrentPosition();
            glm::vec3 pointPos = tangPlane.intersecPos(particles[k].getCurrentPosition(),particles[k].getBouncing());
            glm::vec3 pointVel = tangPlane.intersecVelocity(interPos,particles[k].getVelocity(),particles[k].getBouncing(),!SemiEuler,dt);
            particles[k].setPosition(pointPos.x, pointPos.y, pointPos.z);
            particles[k].setVelocity(pointVel.x,pointVel.y, pointVel.z);
        }
    }
}


void GLWidget::change_ke(float new_ke){
    ke=new_ke;

}
void GLWidget::change_kd(float new_kd){

    kd=new_kd;
}
void GLWidget::change_keB(float new_keB){
    keB=new_keB;

}
void GLWidget::change_kdB(float new_kdB){

    kdB=new_kdB;
}
