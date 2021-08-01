#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle,angle2,angle3,angle4;
struct point
{
    double x,y,z;
};

point pos,u,r,l;

void drawAxes()
{
    if(drawaxes==1)
    {
        glColor3f(1.0, 0, 0);
        glBegin(GL_LINES);
        {
            glVertex3f( 100,0,0);
            glVertex3f(-100,0,0);
        }
        glEnd();

        glColor3f(0, 1.0, 0);
        glBegin(GL_LINES);
        {
            glVertex3f(0,-100,0);
            glVertex3f(0, 100,0);
        }
        glEnd();

        glColor3f(0, 0, 1.0);
        glBegin(GL_LINES);
        {
            glVertex3f(0,0, 100);
            glVertex3f(0,0,-100);
        }
        glEnd();
    }
}


void drawGrid()
{
    int i;
    if(drawgrid==1)
    {
        glColor3f(0.6, 0.6, 0.6);	//grey
        glBegin(GL_LINES);
        {
            for(i=-8; i<=8; i++)
            {

                if(i==0)
                    continue;	//SKIP the MAIN axes

                //lines parallel to Y-axis
                glVertex3f(i*10, -90, 0);
                glVertex3f(i*10,  90, 0);

                //lines parallel to X-axis
                glVertex3f(-90, i*10, 0);
                glVertex3f( 90, i*10, 0);
            }
        }
        glEnd();
    }
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f( a, a,2);
        glVertex3f( a,-a,2);
        glVertex3f(-a,-a,2);
        glVertex3f(-a, a,2);
    }
    glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0; i<=segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0; i<segments; i++)
    {
        glBegin(GL_LINES);
        {
            glVertex3f(points[i].x,points[i].y,0);
            glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0; i<=segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0; i<segments; i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
            glVertex3f(points[i].x,points[i].y,0);
            glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}
void drawInverseHalfSphere(double radius,int slices,int stacks,int col)
{
    int color=col;
    struct point points[100][100];
    int i,j;
    double h,r;
    //generate points
    for(i=0; i<=stacks; i++)
    {
        h=radius*sin(((double)i/(double)stacks)*(pi/2));
        r=radius*cos(((double)i/(double)stacks)*(pi/2));
        r=(2*radius)-r;
        for(j=0; j<=slices; j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].z=h;
        }
    }

    for(i=0; i<stacks; i++)
    {

        for(j=0; j<slices; j++)
        {
            glColor3f(color,color,color);
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                //glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                //glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                //glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                //glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
            }
            glEnd();
            color=(color+1)%2;
        }

    }
}
void drawHalfSphere(double radius,int slices,int stacks,int col)
{
    int color=col;
    struct point points[100][100];
    int i,j;
    double h,r;
    //generate points
    for(i=0; i<=stacks; i++)
    {
        h=radius*sin(((double)i/(double)stacks)*(pi/2));
        r=radius*cos(((double)i/(double)stacks)*(pi/2));
        for(j=0; j<=slices; j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].z=r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].y=h;
        }
    }

    for(i=0; i<stacks; i++)
    {
        //glColor3f(color,color,color);
        for(j=0; j<slices; j++)
        {
            glColor3f(color,color,color);
            glBegin(GL_QUADS);
            {
                glColor3f(color,color,color);
                //upper hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                //glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                //glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                //glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                //glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
            }
            glEnd();
            color=(color+1)%2;
        }
        //color=(color+1)%2;
    }
    //color=(color+1)%2;
}

void  drawCylinder(double radius,double height,int slices,int col)
{
    int color=col;
    struct point points[100][100];
    int i,j;
    int stacks=40;
    double h,r=radius;
    //generate points
    for(i=0; i<=stacks; i++)
    {
        //h=radius*sin(((double)i/(double)stacks)*(pi/2));
        h=height*(i/stacks);
        //r=radius*cos(((double)i/(double)stacks)*(pi/2));
        for(j=0; j<=slices; j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].z=h;
        }
    }

    for(i=0; i<stacks; i++)
    {
        //glColor3f(color,color,color);

        for(j=0; j<slices; j++)
        {
            glColor3f(color,color,color);
            glBegin(GL_QUADS);
            {
                    //upper hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                    //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
            }
            glEnd();
            color=(color+1)%2;
        }

    }

}
void drawSphere(double radius,int slices,int stacks)
{
    struct point points[100][100];
    int i,j;
    double h,r;
    //generate points
    for(i=0; i<=stacks; i++)
    {
        h=radius*sin(((double)i/(double)stacks)*(pi/2));
        r=radius*cos(((double)i/(double)stacks)*(pi/2));
        for(j=0; j<=slices; j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].z=h;
        }
    }
    //draw quads using generated points
    for(i=0; i<stacks; i++)
    {
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        for(j=0; j<slices; j++)
        {
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
            }
            glEnd();
        }
    }
}


void drawSS()
{
    glColor3f(1,0,0);
    drawSquare(20);

    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);
}

void drawRiffel()
{

}

double Modulus(point p)
{
    return (sqrt((p.x*p.x)+(p.y*p.y)+(p.z*p.z)));
}

double DotProduct(point p,point q)
{
    return (p.x*q.x)+(p.y*q.y)+(p.z*q.z);
}

point CrossProduct(point p,point q)
{
    double px=p.x;
    double py=p.y;
    double pz=p.z;
    double qx=q.x;
    double qy=q.y;
    double qz=q.z;

    struct point ret;
    ret.x=(py*qz)-(pz*qy);
    ret.y=(pz*qx)-(px*qz);
    ret.z=(px*qy)-(py*qx);

    return ret;
}
point Rotation(double fx,double fy,double fz,point rotating,double angle)
{
    point k,temp1,temp2;
    k.x=fx;
    k.y=fy;
    k.z=fz;

    temp1.x=rotating.x*cos(angle);
    temp1.y=rotating.y*cos(angle);
    temp1.z=rotating.z*cos(angle);


    temp2=CrossProduct(k,rotating);

    temp2.x*=sin(angle);
    temp2.y*=sin(angle);
    temp2.z*=sin(angle);

    temp1.x+=temp2.x;
    temp1.y+=temp2.y;
    temp1.z+=temp2.z;
    return temp1;
}

void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {

    case '1':
        r=Rotation(u.x,u.y,u.z,r,0.0085);
        l=Rotation(u.x,u.y,u.z,l,0.0085);//not working
        break;
    case '2':
        r=Rotation(u.x,u.y,u.z,r,-0.0085);
        l=Rotation(u.x,u.y,u.z,l,-0.0085);
        break;
    case '3':
        l=Rotation(r.x,r.y,r.z,l,0.0085);
        u=Rotation(r.x,r.y,r.z,u,0.0085);//not working
        break;
    case '4':
        l=Rotation(r.x,r.y,r.z,l,-0.0085);
        u=Rotation(r.x,r.y,r.z,u,-0.0085);//not working
        break;
    case '5':
        r=Rotation(l.x,l.y,l.z,r,0.0085);//not working
        u=Rotation(l.x,l.y,l.z,u,0.0085);
        break;
    case '6':
        r=Rotation(l.x,l.y,l.z,r,-0.0085);//not working
        u=Rotation(l.x,l.y,l.z,u,-0.0085);
        break;
    case 'q':
        angle2+=10;
        if(angle2 >=70) angle2=70;
        break;
    case 'w':
        angle2-=10;
        if(angle2 <= -70) angle2=-70;
        break;
    case 'e':
        angle3+=10;
        if(angle3 >= 70) angle3=70;
        break;
    case 'r':
        angle3-=10;
        if(angle3 <= -70) angle3=-70;
        break;
    case 'a':
        angle4+=10;
        if(angle4 >= 70) angle4=70;
        break;
    case 's':
        angle4-=10;
        if(angle4 <= -70) angle4=-70;
        break;

    default:
        break;
    }
}




void specialKeyListener(int key, int x,int y)
{
    switch(key)
    {
    case GLUT_KEY_DOWN:		//down arrow key
        //cameraHeight -= 3.0;
        pos.x-=0.5*(l.x);
        pos.y-=0.5*(l.y);
        pos.z-=0.5*(l.z);
        break;
    case GLUT_KEY_UP:		// up arrow key
        //cameraHeight += 3.0;
        pos.x+=0.5*(l.x);
        pos.y+=0.5*(l.y);
        pos.z+=0.5*(l.z);

        break;

    case GLUT_KEY_RIGHT:
        //cameraAngle += 0.03;
        pos.x+=0.5*(r.x);
        pos.y+=0.5*(r.y);
        pos.z+=0.5*(r.z);
        break;
    case GLUT_KEY_LEFT:
        //cameraAngle -= 0.03;
        pos.x-=0.5*(r.x);
        pos.y-=0.5*(r.y);
        pos.z-=0.5*(r.z);
        break;

    case GLUT_KEY_PAGE_UP:
        pos.x+=0.5*(u.x);
        pos.y+=0.5*(u.y);
        pos.z+=0.5*(u.z);
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x-=0.5*(u.x);
        pos.y-=0.5*(u.y);
        pos.z-=0.5*(u.z);
        break;

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:
        break;
    case GLUT_KEY_END:
        break;

    default:
        break;
    }
}


void mouseListener(int button, int state, int x, int y) 	//x, y is the x-y of the screen (2D)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        break;

    case GLUT_RIGHT_BUTTON:
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
            drawaxes=1-drawaxes;
        }
        break;

    case GLUT_MIDDLE_BUTTON:
        //........
        break;

    default:
        break;
    }
}



void display()
{

    //clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);	//color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
    / set-up camera here
    ********************/
    //load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    //initialize the matrix
    glLoadIdentity();

    //now give three info
    //1. where is the camera (viewer)?
    //2. where is the camera looking?
    //3. Which direction is the camera's UP direction?

    //gluLookAt(100,100,100,	0,0,0,	0,0,1);
    //gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);

    //gluLookAt(0,0,300,	0,0,0,	0,1,0);
    gluLookAt(pos.x, pos.y, pos.z,   pos.x+l.x,pos.y + l.y, pos.z + l.z,  u.x, u.y, u.z);



    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    //add objects

    drawAxes();
    drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    //drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

    //drawSphere(70,24,20);
    //glRotatef(90,1,0,0);
    glRotatef(angle2,0,0,1);
    glRotatef(180,1,0,0);
    drawHalfSphere(50,50,40,0);

    glRotatef(180,1,0,0);
    glRotatef(angle3,1,0,0);
    drawHalfSphere(50,50,40,1);
    glTranslatef(0,70,0);
    glRotatef(180,1,0,0);
    glRotatef(angle4,1,0,0);
    drawHalfSphere(20,50,40,1);

    glTranslatef(0,-50,0);
    glRotatef(90,1,0,0);
    drawCylinder(20,50,50,1);
    glTranslatef(0,0,50);
    //glRotatef(-180,0,1,0);
    drawInverseHalfSphere(20,50,40,1);
    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}


void animate()
{
    angle+=0.05;
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}


void init()
{
    //codes for initialization
    drawgrid=0;
    drawaxes=1;
    cameraHeight=150.0;
    cameraAngle=1.0;
    angle=0;
    angle2=0;
    angle3=0;
    angle4-0;

    pos.x=100.0;
    pos.y=100.0;
    pos.z=0.0;
    u.x=0.0;
    u.y=0.0;
    u.z=1.0;
    r.x=(-1/sqrt(2));
    r.y=(1/sqrt(2));
    r.z=0.0;
    l.x=(-1/sqrt(2));
    l.y=(-1/sqrt(2));
    l.z=0.0;

    //clear the screen
    glClearColor(0,0,0,0);

    /************************
    / set-up projection here
    ************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(80,	1,	1,	1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
}

int main(int argc, char **argv)
{
    glutInit(&argc,argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Program");

    init();

    glEnable(GL_DEPTH_TEST);	//enable Depth Testing

    glutDisplayFunc(display);	//display callback function
    glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop();		//The main loop of OpenGL

    return 0;
}
