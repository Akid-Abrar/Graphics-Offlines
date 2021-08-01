#include "1605100_Header.h"

using namespace std;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle,angle2,angle3,angle4;

int window_height,window_width;
point pos,u,r,l;

extern vector <Object *> objects;
extern vector <Light> lights;

void drawAxes()
{
    if(drawaxes==1)
    {
        glColor3f(1.0, 0, 0);
        glBegin(GL_LINES);
        {
            glVertex3f( 200,0,0);
            glVertex3f(-200,0,0);
        }
        glEnd();

        glColor3f(0, 1.0, 0);
        glBegin(GL_LINES);
        {
            glVertex3f(0,-200,0);
            glVertex3f(0, 200,0);
        }
        glEnd();

        glColor3f(0, 0, 1.0);
        glBegin(GL_LINES);
        {
            glVertex3f(0,0, 200);
            glVertex3f(0,0,-200);
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

void capture()
{
    bitmap_image myimage(number_of_pixels,number_of_pixels);

    for(int i=0; i<number_of_pixels; i++)
    {
        for(int j=0; j<number_of_pixels; j++)
        {
            myimage.set_pixel(i,j,0,0,0);
        }
    }
    double planeDistance = (250.0)/tan((40.0*pi)/180.0);
    //window height,width =500,viewAngle=80
    point topleft;
    topleft.x=pos.x + l.x*planeDistance - r.x*250.0 + u.x*250.0;
    topleft.y=pos.y + l.y*planeDistance - r.y*250.0 + u.y*250.0;
    topleft.z=pos.z + l.z*planeDistance - r.z*250.0 + u.z*250.0;

    double du = 500.0/number_of_pixels;
    double dv = 500.0/number_of_pixels;

    topleft.x += r.x*0.5*du - u.x*0.5*dv;
    topleft.y += r.y*0.5*du - u.y*0.5*dv;
    topleft.z += r.z*0.5*du - u.z*0.5*dv;

    point curPixel, dir ;
    Ray ray;
    Object *O;
    //int nearest = -1;
    for(int i=0;i<number_of_pixels;i++)
    {

        for(int j=0;j<number_of_pixels;j++)
        {
            double t, tmin = (double)INT_MAX;
            curPixel.x = topleft.x + r.x*i*du - u.x*j*dv;
            curPixel.y = topleft.y + r.y*i*du - u.y*j*dv;
            curPixel.z = topleft.z + r.z*i*du - u.z*j*dv;

            point t1 = subtract(curPixel , pos);
            dir=Normalize(t1);
            ray.dir = dir;
            ray.start = pos;
            double *color = new double[3];
            double *dummyColor = new double[3];
            dummyColor[0]=0.0;
            dummyColor[1]=0.0;
            dummyColor[2]=0.0;
            int nearest = -1;
            for(int k=0;k<objects.size();k++)
            {
                //cout<<"ob "<<i<<" "<<j<<endl;
                t=objects[k]->intersect(ray,dummyColor,0);
                //cout<<"t is : "<<t<<endl;
                if(t<tmin && t>0)
                {
                    tmin = t;
                    nearest = k;
                }
            }
            if(nearest != -1)
            {
                double temp = objects[nearest]->intersect(ray,color,1);
                //cout<<"returned from int\n";
                myimage.set_pixel(i,j,color[0]*255,color[1]*255,color[2]*255);
            }
        }
    }
    myimage.save_image("C:\\Users\\User\\Desktop\\Ray Tracing Offline\\1605100\\output.bmp");
    cout<<"captured\n";
    return;
}


void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {
    case '0':
        capture();
        break;
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
        pos.x-=2.5*(l.x);
        pos.y-=2.5*(l.y);
        pos.z-=2.5*(l.z);
        break;
    case GLUT_KEY_UP:		// up arrow key
        //cameraHeight += 3.0;
        pos.x+=2.5*(l.x);
        pos.y+=2.5*(l.y);
        pos.z+=2.5*(l.z);

        break;

    case GLUT_KEY_RIGHT:
        //cameraAngle += 0.03;
        pos.x+=2.5*(r.x);
        pos.y+=2.5*(r.y);
        pos.z+=2.5*(r.z);
        break;
    case GLUT_KEY_LEFT:
        //cameraAngle -= 0.03;
        pos.x-=0.5*(r.x);
        pos.y-=0.5*(r.y);
        pos.z-=0.5*(r.z);
        break;

    case GLUT_KEY_PAGE_UP:
        pos.x+=2.5*(u.x);
        pos.y+=2.5*(u.y);
        pos.z+=2.5*(u.z);
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x-=2.5*(u.x);
        pos.y-=2.5*(u.y);
        pos.z-=2.5*(u.z);
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

    for(int i=0; i<objects.size(); i++)
    {
        objects[i]->draw();
    }
    for(int i=0; i<lights.size(); i++)
    {
        lights[i].draw();
    }

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

    pos.x=200.0;
    pos.y=200.0;
    pos.z=50.0;
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


void loadata(fstream &newfile)
{
    Object *floor;
    floor = new Floor(1000,20);
    double co_effs[4];
    co_effs[0]=0.4;co_effs[1]=0.2;co_effs[2]=0.2;co_effs[3]=0.2;
    floor->setCoEfficients(co_effs);
    floor->setShine(15);
    objects.push_back(floor);
    string file="C:\\Users\\User\\Desktop\\Ray Tracing Offline\\1605100\\scene.txt";
    newfile.open(file.c_str(),ios::in);
    int cnt=0;
    if (newfile.is_open())
    {
        int i=0;
        string temp;
        while(getline(newfile, temp) )
        {
            i++;
            while(temp.length() == 0)
            {
                getline(newfile, temp);
            }
            if(i == 1 ) recursion_level = atoi(temp.c_str());
            else if(i == 2 ) number_of_pixels = atoi(temp.c_str());
            else if(i == 3 ) number_of_obejcts = atoi(temp.c_str());
            else if(temp.length() != 1)
            {

                if(i==4 || temp == "sphere")
                {
                    cout<<"got sphere\n";
                    cnt++;
                    Object *Temp;
                    point center;double radius;int shine;
                    stringstream ss1,ss2,ss3;
                    double T1[3],T2[4];
                    string v1;
                    for(int j=0;j<5;j++)
                    {
                       getline(newfile, temp);
                       if(j==0)
                       {
                           int k=0;
                           ss1 << temp;
                           while (ss1.good() && k < 3)
                            {
                                ss1 >> v1;
                                T1[k++]=atof(v1.c_str());
                            }
                            center.x=T1[0];
                            center.y=T1[1];
                            center.z=T1[2];
                       }
                       else if(j==1)
                       {
                           radius=atof(temp.c_str());
                           Temp = new Sphere(center,radius);
                       }
                       else if(j==2)
                       {
                           int k=0;
                           ss3 << temp;
                           while (ss3.good() && k < 3)
                            {
                                ss3 >> v1;
                                T1[k++]=atof(v1.c_str());
                            }
                            Temp->setColor(T1);
                       }
                       else if(j==3)
                       {
                           int k=0;
                           ss2 << temp;
                           while (ss2.good() && k < 4)
                            {
                                ss2 >> v1;
                                T2[k++]=atof(v1.c_str());
                            }
                            Temp->setCoEfficients(T2);
                       }
                       else
                       {
                           shine=atoi(temp.c_str());
                           Temp->setShine(shine);
                       }


                    }
                    objects.push_back(Temp);
                    i+=5;
                }
                else if(temp == "triangle")
                {
                    cout<<"got triangle\n";
                    cnt++;
                    Object *Temp;
                    Temp=new Triangle();int shine;point co_ordinates[3];
                    stringstream ss1,ss2,ss3,ss4,ss5;
                    double T1[3],T2[4];
                    string v1;
                    for(int j=0;j<6;j++)
                    {
                        getline(newfile, temp);
                        if(j==0)
                        {
                           int k=0;
                           ss1 << temp;
                           while (ss1.good() && k < 3)
                            {
                                ss1 >> v1;
                                T1[k++]=atof(v1.c_str());
                            }
                            point p;
                            p.x=T1[0];
                            p.y=T1[1];
                            p.z=T1[2];
                            co_ordinates[0]=p;
                        }
                        else if(j==1)
                        {
                           int k=0;
                           ss2 << temp;
                           while (ss2.good() && k < 3)
                            {
                                ss2 >> v1;
                                T1[k++]=atof(v1.c_str());
                            }
                            point p;
                            p.x=T1[0];
                            p.y=T1[1];
                            p.z=T1[2];
                            co_ordinates[1]=p;
                        }
                        else if(j==2)
                        {
                           int k=0;
                           ss3 << temp;
                           while (ss3.good() && k < 3)
                            {
                                ss3 >> v1;
                                T1[k++]=atof(v1.c_str());
                            }
                            point p;
                            p.x=T1[0];
                            p.y=T1[1];
                            p.z=T1[2];
                            co_ordinates[2]=p;
                        }
                        else if(j==3)
                        {
                           int k=0;
                           ss4 << temp;
                           while (ss4.good() && k < 3)
                            {
                                ss4 >> v1;
                                T1[k++]=atof(v1.c_str());
                            }
                            Temp->setColor(T1);
                        }
                        else if(j==4)
                        {
                           int k=0;
                           ss5 << temp;
                           while (ss5.good() && k < 4)
                            {
                                ss5 >> v1;
                                T2[k++]=atof(v1.c_str());
                            }
                            Temp->setCoEfficients(T2);
                        }
                        else
                        {
                            shine=atoi(temp.c_str());
                            Temp->setShine(shine);
                        }
                    }
                    i+=6;
                    Temp->setCoOrdinates(co_ordinates);
                    objects.push_back(Temp);

                }
                else if(temp == "general")
                {
                    cout<<"got general\n";
                    cnt++;
                    Object *Temp;
                    Temp=new General();
                    point center;double radius;int shine;
                    point co_ordinates[3];
                    stringstream ss1,ss2,ss3,ss4,ss5;
                    double T1[3],T2[6],T3[10];
                    string v1;
                    for(int j=0;j<5;j++)
                    {
                        getline(newfile, temp);
                        if(j==0)
                        {
                           int k=0;
                           ss1 << temp;
                           while (ss1.good() && k < 10)
                            {
                                ss1 >> v1;
                                T3[k++]=atof(v1.c_str());
                            }
                            Temp->setEqnCoEfficients(T3);
                        }
                        else if(j==1)
                        {
                           int k=0;
                           ss2 << temp;
                           while (ss2.good() && k < 6)
                            {
                                ss2 >> v1;
                                T2[k++]=atof(v1.c_str());
                            }
                            point ref_p;
                            ref_p.x=T2[0];
                            ref_p.y=T2[1];
                            ref_p.z=T2[2];
                            Temp->set_ref_h_w_l(ref_p,T2[3],T2[4],T2[5]);
                        }
                        else if(j==2)
                        {
                           int k=0;
                           ss3 << temp;
                           while (ss3.good() && k < 3)
                            {
                                ss3 >> v1;
                                T1[k++]=atof(v1.c_str());
                            }
                            Temp->setColor(T1);
                        }
                        else if(j==3)
                        {
                           int k=0;
                           ss4 << temp;
                           while (ss4.good() && k < 4)
                            {
                                ss4 >> v1;
                                T1[k++]=atof(v1.c_str());
                            }
                            Temp->setCoEfficients(T1);
                        }
                        else
                        {
                            shine=atoi(temp.c_str());
                            Temp->setShine(shine);
                        }
                    }
                    i+=5;
                    objects.push_back(Temp);
                }
                else{}
            }
            else if(temp.length() == 1)
            {
                string temp1,temp2;
                stringstream ss1,ss2;
                double T1[3],T2[3];
                string v1;
                int n = atoi(temp.c_str());
                for(i=0;i<n;i++)
                {
                    string temp1,temp2;
                    stringstream ss1,ss2;
                    double T1[3],T2[3];
                    string v1;
                    Light light;
                    point p;
                    getline(newfile, temp1);
                    getline(newfile, temp2);
                    int k=0;
                    ss1 << temp1;
                    while (ss1.good() && k < 3)
                    {
                        ss1 >> v1;
                        T1[k++]=atof(v1.c_str());
                    }
                    p.x=T1[0];p.y=T1[1];p.z=T1[2];
                    light.setPoint(p);
                    k=0;
                    ss2 << temp2;
                    while (ss2.good() && k < 3)
                    {
                        ss2 >> v1;
                        T2[k++]=atof(v1.c_str());
                    }
                    light.setColor(T2);
                    //cout<<"n "<<n<<" i "<<i<<" "<<temp1<<" "<<temp2<<endl;
                    lights.push_back(light);
                }
                //i+=(2*n+1);
                newfile.close();

            }
            else cout<<"invalid"<<endl;
        }
        cout<<recursion_level<<" "<<number_of_pixels<<" "<<number_of_obejcts<<endl;
        //newfile.close();
        cout<<"total obj :"<<cnt<<endl;
        cout<<"vector size "<<objects.size()<<endl;
    }
}
int main(int argc, char **argv)
{
    fstream newfile;
    loadata(newfile);
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

    for(int i=0; i<objects.size(); i++)
    {
        delete(objects[i]);
    }
    objects.clear();
    lights.clear();

    return 0;
}
