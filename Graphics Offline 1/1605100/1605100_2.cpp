#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <time.h>
#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

struct point
{
    double x,y,z;
};

int counter=0,numberOfBubbles=0;
double m=0.01,mTemp;
bool pause=false;
struct bubble
{
    point center;
    point velocity;
    bool isInside;
    int overlapping;
};

bubble bubbleArray[5];

void drawAxes()
{
    if(drawaxes==1)
    {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        {
            glVertex3f( 100,0,0);
            glVertex3f(-100,0,0);

            glVertex3f(0,-100,0);
            glVertex3f(0, 100,0);

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
    glColor3f(0.0,1.0,0.0);
    glBegin(GL_LINES);
    {
        glVertex3f( a, a,2);
        glVertex3f( a,-a,2);
        glVertex3f(-a,-a,2);
        glVertex3f(-a, a,2);
        glVertex3f( a, a,2);
        glVertex3f( -a,a,2);
        glVertex3f(-a,-a,2);
        glVertex3f(a, -a,2);
    }
    glEnd();
}


void drawCircle(double radius,int segments,double centx,double centy)
{
    int i;
    struct point points[100];
    glColor3f(1,0,0);
    //generate points
    for(i=0; i<=segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi)+centx;
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi)+centy;
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



void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {

    case 'p':
        pause =!pause;
        if(pause)
        {
            mTemp=m;
            m=0.0;
        }
        else m=mTemp;

        break;
    case '2':
        break;
    case '3':
        break;
    case '4':
        break;
    case '5':
        break;
    case '6':
        break;

    default:
        break;
    }
}




void specialKeyListener(int key, int x,int y)
{
    switch(key)
    {
    case GLUT_KEY_DOWN:
        m-=0.005;
        if(m<0.005) m=0.005;
        break;
    case GLUT_KEY_UP:
        m+=0.005;
        if(m>1.0) m=1.0;
        break;

    case GLUT_KEY_RIGHT:
        break;
    case GLUT_KEY_LEFT:
        break;

    case GLUT_KEY_PAGE_UP:
        break;
    case GLUT_KEY_PAGE_DOWN:
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
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
            drawaxes=1-drawaxes;
        }
        break;

    case GLUT_RIGHT_BUTTON:
        //........
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
    counter++;
    if(counter == 1000) numberOfBubbles=1;
    if(counter == 2000) numberOfBubbles=2;
    if(counter == 3000) numberOfBubbles=3;
    if(counter == 4000) numberOfBubbles=4;
    if(counter == 5000) numberOfBubbles=5;

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

    gluLookAt(0,0,200,	0,0,0,	0,1,0);
    //gluLookAt(pos.x, pos.y, pos.z,   pos.x+l.x,pos.y + l.y, pos.z + l.z,  u.x, u.y, u.z);



    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    //add objects

    //drawAxes();
    drawGrid();

    //glColor3f(1,0,0);
    drawSquare(90);

    drawCircle(50,24,0,0);
    int i=0;
    for(i=0; i<numberOfBubbles; i++)
    {
        drawCircle(10,24,bubbleArray[i].center.x,bubbleArray[i].center.y);
    }
    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}

point AddPoints(point a,point b)
{
    point ret;
    ret.x=a.x+b.x;
    ret.y=a.y+b.y;
    ret.z=a.z+b.z;

    return ret;
}

double Modulus(point p)
{
    return (sqrt((p.x*p.x)+(p.y*p.y)+(p.z*p.z)));
}

double DotProduct(point p,point q)
{
    return (p.x*q.x)+(p.y*q.y)+(p.z*q.z);
}

double Distance(point p,point q)
{
    return sqrt((p.x-q.x)*(p.x-q.x) + (p.y-q.y)*(p.y-q.y)+ (p.z-q.z)*(p.z-q.z));
}

point ReflectCircleVector(point velocity,point p)
{
    point vp,vpl,ret;

    double val=DotProduct(p,velocity)/DotProduct(p,p);
    vp.x=val*p.x;
    vp.y=val*p.y;
    vp.z=val*p.z;//Component of V across AB

    vpl.x=velocity.x-vp.x;
    vpl.y=velocity.y-vp.y;
    vpl.z=velocity.z-vp.z;//Component of V perpendicular to AB

    ret.x=-1.0*vp.x+vpl.x;
    ret.y=-1.0*vp.y+vpl.y;
    ret.z=-1.0*vp.z+vpl.z;

    return ret;
}

point ReflecBubbleVector(point velocity,point p)
{
    point vp,vpl,ret;

    double val=DotProduct(p,velocity)/DotProduct(p,p);
    vp.x=val*p.x;
    vp.y=val*p.y;
    vp.z=val*p.z;//Component of V across AB

    vpl.x=velocity.x-vp.x;
    vpl.y=velocity.y-vp.y;
    vpl.z=velocity.z-vp.z;//Component of V perpendicular to AB

    ret.x=-1.0*vp.x+vpl.x;
    ret.y=-1.0*vp.y+vpl.y;
    ret.z=-1.0*vp.z+vpl.z;

    return ret;
}
bool Inside(point a,point b)
{
    double d=Distance(a,b);
    if(d<20) return true;
    else return false;
}
void animate()
{
    angle+=0.05;
    int i;
    for(i=0; i<numberOfBubbles; i++)
    {
        if((bubbleArray[i].center.y + 10) >= 90) bubbleArray[i].velocity.y *= -1.0;
        if((bubbleArray[i].center.y - 10) < -90) bubbleArray[i].velocity.y *= -1.0;
        if((bubbleArray[i].center.x + 10) >= 90) bubbleArray[i].velocity.x *= -1.0;
        if((bubbleArray[i].center.x - 10) < -90) bubbleArray[i].velocity.x *= -1.0;

        point BigCircleCenter;
        BigCircleCenter.x=0;
        BigCircleCenter.y=0;
        BigCircleCenter.z=0;

        if((Distance(BigCircleCenter,bubbleArray[i].center)) <= 40)
        {
            bubbleArray[i].isInside = true;

        }
        if(bubbleArray[i].isInside)
        {
            //Circle bubble collision
            if((Distance(BigCircleCenter,bubbleArray[i].center)) >= 40)
            {
                point A,B,AB,vp,vpl;
                A.x=0;
                A.y=0;
                A.z=0;
                B=bubbleArray[i].center;
                AB.x=B.x;
                AB.y=B.y;
                AB.z=B.z;//Finding the vector going through Circle's and bubble's radius

                bubbleArray[i].velocity = ReflectCircleVector(bubbleArray[i].velocity, AB);
            }
            int j,k;
            for(j=0; j<numberOfBubbles; j++)
            {

                if(i==j || bubbleArray[i].overlapping==j || bubbleArray[j].overlapping==i)
                {
                    bubbleArray[i].overlapping=-1;
                    bubbleArray[j].overlapping=-1;
                    continue;
                }
                bool a=Inside(bubbleArray[i].center,bubbleArray[j].center);
                //if(a) continue;
                double D=Distance(bubbleArray[i].center,bubbleArray[j].center);
                if(bubbleArray[j].isInside == true)
                {
                    if(D <= 20.0)
                    {
                        point p;
                        p.x=bubbleArray[j].center.x-bubbleArray[i].center.x;
                        p.y=bubbleArray[j].center.y-bubbleArray[i].center.y;
                        p.z=bubbleArray[j].center.z-bubbleArray[i].center.z;


                        point vp,vpL,vp1,vp1L;
                        double val=DotProduct(p,bubbleArray[i].velocity)/DotProduct(p,p);
                        vp.x=val*p.x;
                        vp.y=val*p.y;
                        vp.z=val*p.z;//Component of V across AB

                        vpL.x=bubbleArray[i].velocity.x-vp.x;
                        vpL.y=bubbleArray[i].velocity.y-vp.y;
                        vpL.z=bubbleArray[i].velocity.z-vp.z;

                        val=DotProduct(p,bubbleArray[j].velocity)/DotProduct(p,p);
                        vp1.x=val*p.x;
                        vp1.y=val*p.y;
                        vp1.z=val*p.z;//Component of V across AB

                        vp1L.x=bubbleArray[j].velocity.x-vp1.x;
                        vp1L.y=bubbleArray[j].velocity.y-vp1.y;
                        vp1L.z=bubbleArray[j].velocity.z-vp1.z;

                        bubbleArray[i].velocity.x= 1.0*vp1.x+vpL.x;
                        bubbleArray[i].velocity.y= 1.0*vp1.y+vpL.y;
                        bubbleArray[i].velocity.z= 1.0*vp1.z+vpL.z;

                        bubbleArray[j].velocity.x= 1.0*vp.x+vp1L.x;
                        bubbleArray[j].velocity.y= 1.0*vp.y+vp1L.y;
                        bubbleArray[j].velocity.z= 1.0*vp.z+vp1L.z;
                    }
                }

            }
        }
        bubbleArray[i].center.x += m*bubbleArray[i].velocity.x;
        bubbleArray[i].center.y += m*bubbleArray[i].velocity.y;
        bubbleArray[i].center.z += m*bubbleArray[i].velocity.z;

        int j;
        for(j=0;j<5;j++)
        {
            if(Inside(bubbleArray[i].center,bubbleArray[j].center))
            {
                bubbleArray[i].overlapping=j;
                bubbleArray[j].overlapping=i;
            }
        }
    }
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

    int i;
    for(i=0; i<5; i++)
    {
        bubbleArray[i].center.x=-80;
        bubbleArray[i].center.y=-80;
        bubbleArray[i].center.z=0;


        //srand(time(0));
        bubbleArray[i].velocity.x=((rand()%10)+1.0)/500;
        bubbleArray[i].velocity.y=((rand()%10)+1.0)/500;
        bubbleArray[i].velocity.z=0;

        bubbleArray[i].isInside=false;

        double mod=Modulus(bubbleArray[i].velocity);

        bubbleArray[i].velocity.x /= mod;
        bubbleArray[i].velocity.y /= mod;
        bubbleArray[i].velocity.z /= mod;


        //printf("rands are %lf %lf %lf : \n",bubbleArray[i].velocity.x,bubbleArray[i].velocity.y,bubbleArray[i].velocity.z);
    }

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
