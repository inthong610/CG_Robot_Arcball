//
//  modified_robot.cpp
//  CG_HW4
//
//  Created by Jungsoo on 24/10/2018.
//  Copyright © 2018 Jungsoo. All rights reserved.
//
 

#include <cstdlib>
#include <cmath>
#include "arcball.h"
//#include <math.h>
//#include <stdlib.h>

#ifdef __APPLE__ // in Mac os, <GLUT/glut.h>, in Window os, <GL/glut.h>
#include <GLUT/glut.h> // for mac
#include <OpenGL/OpenGL.h> // for mac
#include <OpenGL/gl3.h> // for mac
#include <OpenGL/glu.h> // for mac
#define __gl_h_ // for mac
#else
#include <GL/glut.h> /* glut.h includes gl.h and glu.h*/
#endif


// =================
// Drawing the Scene
// =================


static int width, height; // for HW4.
const vec eye( 0.0f, 0.0f, -20.0f ); // for HW4.
const vec up( 0.0f, 1.0f, 0.0f );// for HW4.
const float SPHERE_RADIUS = 1.0f; // for HW4. 숫자가 작아질수록 회전 반경이 커짐(회전 속도 커짐)



// draw->T/R
// finger 할 때 push 하고 pop하고 next finger팁에 하고. 다른 finger tip에 relative한게 아니라 그 전 joint에 relative

// ROBOT ARM CONTROLS
float BaseTransX=-0.5f;  // 0
float BaseTransZ=0;
float BaseSpin=0;        // 1
float ShoulderAng=-10;   // 2
float ElbowAng=-120;
float WristAng=90;       // 3
float WristTwistAng=10;
float FingerAng1=45;     // 4
float FingerAng2=-90;

// ROBOT COLORS
GLubyte Arms[] = { 128,128,128 };
GLubyte Joints[] = { 0,68,119 };
GLubyte Fingers[] = { 150,0,24 };
GLubyte FingerJoints[] = { 128,128,128 };

// USER INTERFACE GLOBALS
int LeftButtonDown=0;    // MOUSE STUFF
int RightButtonDown=0; // for right button
int OldX,OldY,NewX,NewY;
int RobotControl=1;

void DrawUnitCylinder(int NumSegs)  // x,y,z in [0,1], Y-axis is up
{
    int i;
    float *Px = new float[NumSegs];
    float *Py = new float[NumSegs];
    float AngIncr = (2.0f*3.1415927f) / (float)NumSegs;
    float Ang = AngIncr;
    Px[0]=1;
    Py[0]=0;
    for (i=1; i<NumSegs; i++, Ang+=AngIncr)
    {
        Px[i]=(float)cos(Ang);
        Py[i]=(float)sin(Ang);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0.5f,0.5f,0.5f);
    glScalef(0.5f,0.5f,0.5f);
    
    // TOP
    glNormal3f(0,1,0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0,1,0);
    for (i=0; i<NumSegs; i++)
        glVertex3f(Px[i],1,-Py[i]);
    glVertex3f(Px[0],1,-Py[0]);
    glEnd();
    
    // BOTTOM
    glNormal3f(0,-1,0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0,-1,0);
    for (i=0; i<NumSegs; i++)
        glVertex3f(Px[i],-1,Py[i]);
    glVertex3f(Px[0],-1,Py[0]);
    glEnd();
    
    // SIDES
    glBegin(GL_QUAD_STRIP);
    for (i=0; i<NumSegs; i++)
    {
        glNormal3f(Px[i],0,-Py[i]);
        glVertex3f(Px[i],1,-Py[i]);
        glVertex3f(Px[i],-1,-Py[i]);
    }
    glNormal3f(Px[0],0,-Py[0]);
    glVertex3f(Px[0],1,-Py[0]);
    glVertex3f(Px[0],-1,-Py[0]);
    glEnd();
    
    glPopMatrix();
    
    delete[] Px;
    delete[] Py;
}

void DrawUnitSphere(int NumSegs)  // x,y,z in [0,1]
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0.5f,0.5f,0.5f);
    glutSolidSphere(0.5f,NumSegs,NumSegs);
    glPopMatrix();
}

void DrawUnitCone(int NumSegs)  // x,y,z in [0,1], apex is in +Y direction
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0.5f,0,0.5f);
    glRotatef(-90, 1,0,0);
    glutSolidCone(0.5f,1,NumSegs,NumSegs);
    glPopMatrix();
}

void DrawGroundPlane(int NumSegs)
{

    glColor3f(0.7f,0.7f,0.7f);
    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glVertex3f(-1,0,1);
    glVertex3f(1,0,1);
    glVertex3f(1,0,-1);
    glVertex3f(-1,0,-1);
    glEnd();
}

void DrawJoint(int NumSegs)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glScalef(0.15f,0.15f,0.12f);
    glRotatef(90,1,0,0);
    glTranslatef(-0.5f,-0.5f,-0.5f);
    glColor3ubv(Joints);
    DrawUnitCylinder(NumSegs);
    glPopMatrix();
}

void DrawBase(int NumSegs)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glScalef(0.2f,0.025f,0.2f);
    glTranslatef(-0.5f,0,-0.5f);
    glColor3ubv(Joints);
    DrawUnitCylinder(NumSegs);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.05f,0,-0.05f);
    glScalef(0.1f,0.4f,0.1f);
    glColor3ubv(Arms);
    DrawUnitCylinder(NumSegs);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,0.4f,0);
    DrawJoint(NumSegs);
    glPopMatrix();
}

void DrawArmSegment(int NumSegs)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(-0.05f,0,-0.05f);
    glScalef(0.1f,0.5f,0.1f);
    glColor3ubv(Arms);
    DrawUnitCylinder(NumSegs);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,0.5f,0);
    DrawJoint(NumSegs);
    glPopMatrix();
}

void DrawWrist(int NumSegs)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(-0.04f,0,-0.04f);
    glScalef(0.08f,0.2f,0.08f);
    glColor3ubv(Fingers);
    DrawUnitCylinder(NumSegs);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,0.2f,0);
    glScalef(0.12f,0.12f,0.12f);
    glTranslatef(-0.5f,-0.5f,-0.5f);
    glColor3ubv(FingerJoints);
    DrawUnitSphere(NumSegs);
    glPopMatrix();
}

void DrawFingerBase(int NumSegs)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(-0.025f,0,-0.025f);
    glScalef(0.05f,0.3f,0.05f);
    glColor3ubv(Fingers);
    DrawUnitCylinder(NumSegs);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,0.3f,0);
    glScalef(0.08f,0.08f,0.08f);
    glTranslatef(-0.5f,-0.5f,-0.5f);
    glColor3ubv(FingerJoints);
    DrawUnitSphere(NumSegs);
    glPopMatrix();
}

void DrawFingerTip(int NumSegs)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glScalef(0.05f,0.25f,0.05f);
    glTranslatef(-0.5f,0,-0.5f);
    glColor3ubv(Fingers);
    DrawUnitCone(NumSegs);
    glPopMatrix();
}

void DrawRobotArm(int NumSegs)
{
    
    // 과제 영상 7분 50초 : these variables are should be use in translation and rolation function to have proper motion.
    
    //case 0: BaseTransX+=RelX; BaseTransZ+=RelY; break;
    //case 1: BaseSpin+=RelX*180; break;
    //case 2: ShoulderAng+=RelY*-90; ElbowAng+=RelX*90; break;
    //case 3: WristAng+=RelY*-180; WristTwistAng+=RelX*180; break;
    //case 4: FingerAng1+=RelY*90; FingerAng2+=RelX*180; break;
    
    // ROBOT ARM CONTROLS
    //  float BaseTransX=-0.5f;  // 0
    //  float BaseTransZ=0;
    //  float BaseSpin=0;        // 1
    //  float ShoulderAng=-10;   // 2
    //  float ElbowAng=-120;
    //  float WristAng=90;       // 3
    //  float WristTwistAng=10;
    //  float FingerAng1=45;     // 4
    //  float FingerAng2=-90;
    
    
    glMatrixMode(GL_MODELVIEW);
    
    // ADD YOUR DISPLAY ROUTINE HERE
    
    // rotate을 먼저 하면 크게 돌고 translate을 먼저 하면 작게 돈다.


    // 1번 + 2번

    glTranslated(BaseTransX, 0.00f, BaseTransZ); // 1번 누르면 첫 번째 arm plane에 붙어서 움직임.
    glRotated(BaseSpin, 0, 1, 0); // 2번 누르면 Base의 파란 동그라미 움직임.  (1,0,0)은 앞뒤, (0,1,0)은 좌우, (0,0,1)은 시계처럼 움직임.
    DrawBase(100); //이건 첫 번째 arm 그리기. 도형이 원형으로 매끄러워 보이도록 100각형.
    

    // 3번
    
    glTranslated(0.00f, 0.40f, 0.00f);
    glRotated(ShoulderAng, 0, 0, 1);// 3번 누르면 두 번째 arm 움직임. (1,0,0)은 앞뒤, (0,1,0)은 좌우(아래 베이스랑 같은 방향), (0,0,1)은 시계처럼(크레인 처럼) 움직임.
    DrawArmSegment(100); //이건 두 번째 arm 그리기.
    
    glTranslated(0.00f, 0.50f, 0.00f);
    glRotated(ElbowAng, 0, 0, 1); // 3번 누르면 두 번째 arm과 같이 움직임. 크레인처럼.
    DrawArmSegment(100); //이건 세 번째 arm 그리기.
    
    
    // 4번
    
    glTranslated(-0.02f, 0.50f, 0.00f); // 밑에 두개 합쳐져서 사방으로 돈다.
    glRotated(WristAng, 0, 0, 1); // 4번 누르면 시계방향으로 움직임.
    glRotated(-WristTwistAng, 1, 0, 0); // 4번 누르면 좌우(수평으로 360도)로 움직임. - 안 붙이면 마우스 반대 방향으로 돌아감.
    DrawWrist(100); // 이건 빨강 첫 번째.
    
    
    // 5번 // push한거 pop 안 하면 회오리 처럼 돌다가 base 빼고 사라진다.
    
    glPushMatrix(); // 맨 끝 fingerTip은 갈라지는 막대가 아니라 처음 빨간 막대를 기준으로 해야해서 push & pop
    
    // 윗쪽 빨간 finger
    glTranslated(0.00f, 0.20f, 0.00f);
    glRotated(FingerAng1, 0, 0, 1); // 5번 누르면 가위처럼 움직임.
    DrawFingerBase(100); // 갈라지는 빨강 막대.
    
    glTranslated(0.00f,0.30f, 0.00f);
    glRotated(FingerAng2, 0, 0, 1); // 5번 누르면 가위처럼 움직임.
    DrawFingerTip(100); // 맨끝. 원뿔이 매끄럽게 보이도록 100각형
    
    glPopMatrix();
    
    // 아래쪽 빨간 finger
    glPushMatrix();
    
    glTranslated(0.00f, 0.20f, 0.00f);
    glRotated(-FingerAng1, 0, 0, 1); // 5번 누르면 가위처럼 움직임.
    DrawFingerBase(100);
    
    glTranslated(0.00f,0.30f, 0.00f);
    glRotated(-FingerAng2, 0, 0, 1); // 5번 누르면 가위처럼 움직임.
    DrawFingerTip(100);

    glPopMatrix();

}

void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,2,4, 0,0.5f,0, 0,1,0);

    arcball_rotate(); // for HW4
    DrawGroundPlane(16);
    DrawRobotArm(16);

    glutSwapBuffers();
}

void myReshape(int w, int h)
{
    width = w; // for HW4
    height = h;
    
    glViewport(0,0,w,h);
    
    glMatrixMode(GL_PROJECTION);
    arcball_setzoom(SPHERE_RADIUS, eye, up); // for HW4

    
    glLoadIdentity();
    gluPerspective(30,(float)w/h,0.1,10);

}

void myIdle()
{
    glutPostRedisplay();
}

void KeyboardFunc(unsigned char Key, int x, int y)
{
    if (Key>='1' && Key<='5') RobotControl=Key-'1';
    if (Key==27) exit(0);         // ESC
}

void MouseFunc(int button, int state, int x, int y)
{
    NewX=x; NewY=y;
    if (button==GLUT_LEFT_BUTTON){
        LeftButtonDown=!LeftButtonDown;
    }
    
    if (button==GLUT_RIGHT_BUTTON){
    
        RightButtonDown=!RightButtonDown; // for HW4

        
        int invert_x = (width - NewX)-1; // for HW4
        arcball_start(invert_x, NewY/2);// for HW4. NewY를 /2 안 해주면 지정 방향에서 휘어져서 돈다.
        
        // for HW4. 기존 sample.cpp처럼 invert_y로 하면 반대로 돈다.
        // int invert_y = (height - NewY) - 1; // OpenGL viewport coordinates are Cartesian
        //    arcball_start(NewX,invert_y);
        
    }
       
}

void MotionFunc(int x, int y) // Translation이나 rotation function 안에 쓰여야 한다.
{
    OldX=NewX; OldY=NewY;
    NewX=x;    NewY=y;
    float RelX = (NewX-OldX) / (float)glutGet((GLenum)GLUT_WINDOW_WIDTH);
    float RelY = (NewY-OldY) / (float)glutGet((GLenum)GLUT_WINDOW_HEIGHT);
    if (LeftButtonDown) // 왼쪽 마우스 클릭시
        switch(RobotControl)
        
    { // 키보드 function이나 마우스 클릭에 따라 달라짐.
        case 0: BaseTransX+=RelX; BaseTransZ+=RelY; break;
        case 1: BaseSpin+=RelX*180; break;
        case 2: ShoulderAng+=RelY*-90; ElbowAng+=RelX*90; break;
        case 3: WristAng+=RelY*-180; WristTwistAng+=RelX*180; break;
        case 4: FingerAng1+=RelY*90; FingerAng2+=RelX*180; break;
    }else if(RightButtonDown){ // for HW4. 오른쪽 마우스 클릭시
        
        int invert_x = (width - NewX) - 1; // for HW4.
        arcball_move(invert_x, NewY/2); // for HW4. NewY를 /2 안 해주면 지정 방향에서 휘어져서 돈다.
    }
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(512,512);
    glutInitWindowPosition(180,100);
    glutCreateWindow("The Robot Arm");
    
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutIdleFunc(myIdle); // 반복할 수 있게
    
    glutKeyboardFunc(KeyboardFunc); // 버튼 컨트롤
    glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    
    glutMainLoop();
    return 0;
}

