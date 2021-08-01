#include <bits/stdc++.h>
#include <cmath>
#include "bitmap_image.hpp"

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

using namespace std;

int recursion_level,number_of_pixels,number_of_obejcts;
struct point
{
    double x,y,z;
};

double Distance(point p,point q)
{
    return sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y)+(p.z-q.z)*(p.z-q.z));
}
double Modulus(point p)
{
    return (sqrt((p.x*p.x)+(p.y*p.y)+(p.z*p.z)));
}

point Normalize(point p)
{
    double mod = Modulus(p);
    p.x /= mod;p.y /= mod;p.z /= mod;
    return p;
}

point add(point p,point q)
{
    point ret;
    ret.x = p.x + q.x ;
    ret.y = p.y + q.y ;
    ret.z = p.z + q.z ;

    return ret;
}

point scalerProduct(double m,point p)
{
    p.x = p.x * m;
    p.y = p.y * m;
    p.z = p.z * m;

    return p;
}

point subtract(point p,point q)
{
    point ret;
    ret.x = p.x - q.x ;
    ret.y = p.y - q.y ;
    ret.z = p.z - q.z ;

    return ret;
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

void drawSphere(double radius,int slices,int stacks,double color[3])
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
        glColor3f(color[0],color[1],color[2]);
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

class Ray
{
public:

    point start;
    point dir; // normalize for easier calculations

};

class Light
{
public:
    point light_pos;
    double color[3];
    void setPoint(point p)
    {
        light_pos = p;
    };
    void setColor(double c[3])
    {
        for(int i=0;i<3;i++) color[i]=c[i];
    }
    void draw()
    {
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_POINTS);
        glVertex3f(light_pos.x,light_pos.y,light_pos.z);
        glEnd();

    }

};

class Object
{
public :
    point reference_point; // should have x, y, z
    double height, width, length;
    point coOrdinates[3];
    double color[3];
    double coEfficients[4]; // reflection coefficients
    double eqn_co_effs[10];
    int shine; // exponent term of specular component
    void setColor(double c[3])
    {
        color[0] = c[0];
        color[1] = c[1];
        color[2] = c[2];
    };
    void setShine(int s)
    {
        shine=s;
    };
    void set_ref_h_w_l(point p,double l,double w,double h)
    {
        reference_point=p;
        height=h;width=w;length=l;
    };
    void setCoEfficients(double c[4])
    {
        coEfficients[0]=c[0];
        coEfficients[1]=c[1];
        coEfficients[2]=c[2];
        coEfficients[3]=c[3];
    };

    object(){};
    virtual void draw(){};
    virtual void setCoOrdinates(point c[3]){};
    virtual void setEqnCoEfficients(double d[10]){};
    virtual double intersect(Ray r,double *Color, int level){};
    virtual double get_intersection_t(Ray r){};

};

vector <Object *> objects;
vector <Light> lights;

class Sphere : public Object
{
public:

    Sphere(point center,double radius)
    {
        reference_point = center;
        length = radius;
    }
    void draw()
    {
        glPushMatrix();
        glTranslatef(reference_point.x,reference_point.y,reference_point.z);
        drawSphere(length,50,50,color);
        glPopMatrix();
    }

    double get_intersection_t(Ray r)
    {

        return -1;
    }

    double intersect(Ray r ,double *Color, int level)
    {
        //double t = get_intersection_t(r);
        //cout<<"t "<<t<<endl;
        point R0= subtract(r.start , reference_point);
        point Rd;
        Rd.x=r.dir.x;
        Rd.y=r.dir.y;
        Rd.z=r.dir.z;
        //cout<<R0.x<<" "<<R0.y<<" "<<R0.z<<endl;
        //cout<<Rd.x<<" "<<Rd.y<<" "<<Rd.z<<endl;
        double a,b,c,D,t1,t2,t;
        a = 1.0;
        b = 2.0 * DotProduct(Rd,R0);
        c = DotProduct(R0,R0) - (length*length);
        D = (b*b)-(4.0*a*c);
        //cout<<a<<" "<<b<<" "<<c<<" "<<D<<endl;
        if (D < 0)
        {
            //cout<<"from D : a "<<a<<" "<<"b "<<b<<"c "<<c<<"D "<<D<<endl;
            return -1;
        }
        t1 = (-b+sqrt(D))/(2.0*a);
        t2 = (-b-sqrt(D))/(2.0*a);
        //cout<<t1<<" "<<t2<<endl;
        if(t1>0.0)
        {
            if(t2<0.0) t=t1;
            else t1<t2?t=t1:t=t2;
        }
        else if(t2>0.0)
        {
            t = t2;
        }
        else
        {
            t = -1;
            return t;
        }
        //cout<<"t "<<t<<endl;
        if(level == 0)
        {
            return t;
        }
        //point intersectionPoint = add(subtract(r.start,reference_point) , scalerProduct(t,r.dir));
        point intersectionPoint = add(r.start , scalerProduct(t,r.dir));
        point normal = subtract(intersectionPoint,reference_point);
        point n=Normalize(normal);
        Color[0] = color[0] * coEfficients[0];
        Color[1] = color[1] * coEfficients[0];
        Color[2] = color[2] * coEfficients[0];

        //cout<<"amb: "<<"R "<<Color[0]<<" G "<<Color[1]<<" B "<<Color[2]<<endl;
        for(int i=0 ; i <lights.size();i++)
        {
            Ray light_ray;
            point p1 = subtract(lights[i].light_pos,intersectionPoint);
            point dir = Normalize(p1);
            light_ray.dir = dir;
            //light_ray.start = add(intersectionPoint , scalerProduct(1.0,dir)) ;
            light_ray.start = intersectionPoint  ;

            bool is_in_shadow=false;
            for(int j=0;j<objects.size();j++)
            {
                double ti= objects[j]->intersect(light_ray,Color,0);
                if(ti>0.0 && ti<Distance(intersectionPoint,lights[i].light_pos))
                {
                    is_in_shadow=true;
                    break;
                }
            }
            if(true/*!is_in_shadow*/)
            {
                double phongValue=0.0,lambertValue=0.0;
                lambertValue=DotProduct(light_ray.dir,n);

                lambertValue=max(0.0,lambertValue);
                //cout<<"lam"<<lambertValue<<endl;
                Color[0] += lights[i].color[0]*coEfficients[1]*lambertValue*color[0];
                Color[1] += lights[i].color[1]*coEfficients[1]*lambertValue*color[1];
                Color[2] += lights[i].color[2]*coEfficients[1]*lambertValue*color[2];
                //cout<<"diff: "<<"R "<<Color[0]<<" G "<<Color[1]<<" B "<<Color[2]<<endl;
                point R;
                //double temp= 2.0*DotProduct(r.dir,n);
                double temp= 2.0*DotProduct(light_ray.dir,n);
                R=scalerProduct(temp,n);
                //R=subtract(R,r.dir);
                R=subtract(R,light_ray.dir);
                //R=Normalize(R);


                //phongValue=pow(DotProduct(R,r.dir),shine);
                point p;
                phongValue=DotProduct(R,scalerProduct(-1.0,r.dir));
                phongValue=max(0.0,phongValue);
                //cout<<"phong"<<phongValue<<endl;
                //cout<<"phong : "<<phongValue<<endl;
                Color[0] += lights[i].color[0]*coEfficients[2]*pow(phongValue,shine)*color[0];
                Color[1] += lights[i].color[1]*coEfficients[2]*pow(phongValue,shine)*color[1];
                Color[2] += lights[i].color[2]*coEfficients[2]*pow(phongValue,shine)*color[2];

                //cout<<"spec: "<<"R "<<Color[0]<<" G "<<Color[1]<<" B "<<Color[2]<<endl;

                for(int k=0;k<3;k++)
                {
                    if(Color[k]<0) Color[k]=0;
                    if(Color[k]>1) Color[k]=1;
                }

            }

        }
        if (level >= recursion_level) return t;
        point Reflected;
        double temp1 = DotProduct(r.dir,n)*2.0;
        Reflected = scalerProduct(temp1,n);
        Reflected = subtract(r.dir,Reflected);
        Reflected = Normalize(Reflected);
        point start = add(intersectionPoint,scalerProduct(1.0,Reflected));

        Ray reflectedRay;
        reflectedRay.start=start;
        reflectedRay.dir=Reflected;

        double t_calc, tmin = (double)INT_MAX;
        double *reflected_color = new double[3];
        int nearest = -1;
        for(int k=0; k<objects.size(); k++)
        {
            t_calc=objects[k]->intersect(reflectedRay,reflected_color,0);
            if(t_calc<tmin && t_calc>0)
            {
                tmin = t_calc;
                nearest = k;
            }
        }
        if(nearest != -1)
        {
            double temp2 = objects[nearest]->intersect(reflectedRay,reflected_color,level+1);
            for(int p=0;p<3;p++)
            {
                Color[p] += coEfficients[3]*reflected_color[p];
            }
        }

        delete reflected_color;

        return t;
    };
};

class General : public Object
{
public:

    General(){};
    void setEqnCoEfficients(double d[10])
    {
        for(int i=0;i<10;i++)
        {
           eqn_co_effs[i] = d[i];
        }
    };

    bool is_inside(double t,Ray r)
    {
        point intersectionPoint = add(r.start , scalerProduct(t,r.dir));
        double l0,w0,h0;
        double l1,w1,h1;
        bool ret=true;
        l0=reference_point.x;
        l1=reference_point.x+length;
        w0=reference_point.y;
        w1=reference_point.y+width;
        h0=reference_point.z;
        h1=reference_point.z+height;
        if(length != 0.0)
        {
            //cout<<"for l\n";
            if(intersectionPoint.x<l0 || intersectionPoint.x>l1) ret=false;
        }
        if(width != 0.0)
        {
            //cout<<"for w\n";
            if(intersectionPoint.y<w0 || intersectionPoint.y>w1) ret=false;
        }
        if(height != 0.0)
        {
            //cout<<"for h\n";
            if(intersectionPoint.z<h0 || intersectionPoint.z>h1) ret=false;
        }

        return ret;
    }
    double intersect(Ray r ,double *Color, int level)
    {
        double a,b,c,t,t1,t2,D;
        double xd,yd,zd,xo,yo,zo;
        xd=r.dir.x;
        yd=r.dir.y;
        zd=r.dir.z;

        xo=r.start.x;
        yo=r.start.y;
        zo=r.start.z;

        a=eqn_co_effs[0]*xd*xd+eqn_co_effs[1]*yd*yd+eqn_co_effs[2]*zd*zd+eqn_co_effs[3]*xd*yd+eqn_co_effs[4]*xd*zd+eqn_co_effs[5]*yd*zd;
        b=2*eqn_co_effs[0]*xo*xd + 2*eqn_co_effs[1]*yo*yd + 2*eqn_co_effs[2]*zo*zd + eqn_co_effs[3]*(xo*yd + yo*xd) + eqn_co_effs[4]*(xo*zd + zo*xd) + eqn_co_effs[5]*(yo*zd + yd*zo) + eqn_co_effs[6]*xd + eqn_co_effs[7]*yd + eqn_co_effs[8]*zd;
        c=eqn_co_effs[0]*xo*xo + eqn_co_effs[1]*yo*yo + eqn_co_effs[2]*zo*zo + eqn_co_effs[3]*xo*yo + eqn_co_effs[4]*xo*zo + eqn_co_effs[5]*yo*zo + eqn_co_effs[6]*xo + eqn_co_effs[7]*yo + eqn_co_effs[8]*zo + eqn_co_effs[9];
        D = (b*b)-(4.0*a*c);
        if(a == 0.0)
        {
            t=-c/b;
            return t;
        }
        if(D<0) return -1;
        t1 = (-b+sqrt(D))/(2*a);
        t2 = (-b-sqrt(D))/(2*a);

        bool inside_t1,inside_t2;
        if(t1<0.0 && t2<0.0)
        {
            return -1;
        }
        else if(t2<0.0 && t1>0.0)
        {
            inside_t1=is_inside(t1,r);
            inside_t2=false;
        }
        else if(t2>0.0 && t1<0.0)
        {
            inside_t1=false;
            inside_t2=is_inside(t2,r);
        }
        else
        {
            inside_t1=is_inside(t1,r);
            inside_t2=is_inside(t2,r);
        }

        if(inside_t1)
        {
            if(inside_t2) t=t1<t2?t1:t2;
            else t=t1;
        }
        else if(inside_t2)
        {
           t=t2;
        }
        else return -1;

        if(level == 0)
        {
            return t;
        }
        point intersectionPoint = add(r.start , scalerProduct(t,r.dir));
        point n;
        double xi,yi,zi;
        xi=intersectionPoint.x;
        yi=intersectionPoint.y;
        zi=intersectionPoint.z;


        n.x = 2*eqn_co_effs[0]*xi + eqn_co_effs[3]*yi + eqn_co_effs[4]*zi + eqn_co_effs[6];
        n.y = 2*eqn_co_effs[1]*yi + eqn_co_effs[3]*xi + eqn_co_effs[5]*zi + eqn_co_effs[7];
        n.z = 2*eqn_co_effs[2]*zi + eqn_co_effs[4]*xi + eqn_co_effs[5]*yi + eqn_co_effs[8];

        n=Normalize(n);
        Color[0] = color[0] * coEfficients[0];
        Color[1] = color[1] * coEfficients[0];
        Color[2] = color[2] * coEfficients[0];

        for(int i=0 ; i <lights.size();i++)
        {
            Ray light_ray;
            point p1 = subtract(lights[i].light_pos,intersectionPoint);
            point dir = Normalize(p1);
            light_ray.dir = dir;
            //light_ray.start = add(intersectionPoint , scalerProduct(1.0,dir)) ;
            light_ray.start = intersectionPoint  ;

            bool is_in_shadow=false;
            for(int j=0;j<objects.size();j++)
            {
                double t= objects[j]->intersect(light_ray,Color,0);
                if(t>0.0 && t<Distance(intersectionPoint,lights[i].light_pos))
                {
                    is_in_shadow=true;
                    break;
                }
            }
            if(true/*!is_in_shadow*/)
            {
                double phongValue=0.0,lambertValue=0.0;
                lambertValue=DotProduct(light_ray.dir,n);

                lambertValue=max(0.0,lambertValue);
                //cout<<"lam"<<lambertValue<<endl;
                Color[0] += lights[i].color[0]*coEfficients[1]*lambertValue*color[0];
                Color[1] += lights[i].color[1]*coEfficients[1]*lambertValue*color[1];
                Color[2] += lights[i].color[2]*coEfficients[1]*lambertValue*color[2];
                //cout<<"diff: "<<"R "<<Color[0]<<" G "<<Color[1]<<" B "<<Color[2]<<endl;
                point R;
                //double temp= 2.0*DotProduct(r.dir,n);
                double temp= 2.0*DotProduct(light_ray.dir,n);
                R=scalerProduct(temp,n);
                //R=subtract(R,r.dir);
                R=subtract(R,light_ray.dir);
                //R=Normalize(R);


                //phongValue=pow(DotProduct(R,r.dir),shine);
                point p;
                phongValue=DotProduct(R,scalerProduct(-1.0,r.dir));
                phongValue=max(0.0,phongValue);
                //cout<<"phong"<<phongValue<<endl;
                //cout<<"phong : "<<phongValue<<endl;
                Color[0] += lights[i].color[0]*coEfficients[2]*pow(phongValue,shine)*color[0];
                Color[1] += lights[i].color[1]*coEfficients[2]*pow(phongValue,shine)*color[1];
                Color[2] += lights[i].color[2]*coEfficients[2]*pow(phongValue,shine)*color[2];

                //cout<<"spec: "<<"R "<<Color[0]<<" G "<<Color[1]<<" B "<<Color[2]<<endl;

                for(int k=0;k<3;k++)
                {
                    if(Color[k]<0) Color[k]=0;
                    if(Color[k]>1) Color[k]=1;
                }

            }

        }
        if (level >= recursion_level) return t;
        point Reflected;
        double temp1 = DotProduct(r.dir,n)*2.0;
        Reflected = scalerProduct(temp1,n);
        Reflected = subtract(r.dir,Reflected);
        Reflected = Normalize(Reflected);
        point start = add(intersectionPoint,scalerProduct(1.0,Reflected));

        Ray reflectedRay;
        reflectedRay.start=start;
        reflectedRay.dir=Reflected;

        double t_calc, tmin = (double)INT_MAX;
        double *reflected_color = new double[3];
        int nearest = -1;
        for(int k=0; k<objects.size(); k++)
        {
            t_calc=objects[k]->intersect(reflectedRay,reflected_color,0);
            if(t_calc<tmin && t_calc>0)
            {
                tmin = t_calc;
                nearest = k;
            }
        }
        if(nearest != -1)
        {
            double temp2 = objects[nearest]->intersect(reflectedRay,reflected_color,level+1);
            for(int p=0;p<3;p++)
            {
                Color[p] += coEfficients[3]*reflected_color[p];
            }
        }
        delete reflected_color;
        return t;
    }


};

class Triangle : public Object
{
public:
    Triangle(){};
    void draw()
    {
        glColor3f(color[0],color[1],color[2]); //Green

        glBegin(GL_TRIANGLES);
        glVertex3f(coOrdinates[0].x,coOrdinates[0].y,coOrdinates[0].z);
        glVertex3f(coOrdinates[1].x,coOrdinates[1].y,coOrdinates[1].z);
        glVertex3f(coOrdinates[2].x,coOrdinates[2].y,coOrdinates[2].z);

        glEnd();
    };
    void setCoOrdinates(point c[3])
    {
        for(int i=0;i<3;i++)
        {
            coOrdinates[i].x = c[i].x;
            coOrdinates[i].y = c[i].y;
            coOrdinates[i].z = c[i].z;
        }
    };

    double get_intersection_t(Ray r)
    {
        return -1;
    }
    double intersect(Ray r,double *Color, int level)
    {
        double EPSILON = 0.0000001;
        point vertex0 = coOrdinates[0];
        point vertex1 = coOrdinates[1];
        point vertex2 = coOrdinates[2];
        point edge1, edge2, h, s, q;
        double a,f,u,v,t;
        edge1 = subtract(vertex1 , vertex0);
        edge2 = subtract(vertex2 , vertex0);
        h = CrossProduct(r.dir,edge2);
        a = DotProduct(edge1,h);
        if (a > -EPSILON && a < EPSILON)
        {
                //cout<<"a er condition\n";
            t=-1;
            return t;
        }
        f = 1.0/a;
        s = subtract(r.start,vertex0);
        u = f * DotProduct(s,h);
            //cout<<"u : "<<u<<endl;
        if (u < 0.0 || u > 1.0)
        {
                //cout<<"u er condition\n";
            t=-1;
            return t;
        }
        q = CrossProduct(s,edge1);
        v = f * DotProduct(r.dir,q);
        if (v < 0.0 || u + v > 1.0)
        {
                //cout<<"u+v er condition\n";
            t=-1;
            return t;
        }
        t = f * DotProduct(edge2,q);
        if (t > EPSILON )
        {
            if(level == 0) return t;
        }
        else
        {
            t=-1;
            return t;
        }

        point intersectionPoint = add(r.start , scalerProduct(t,r.dir));
        point n = CrossProduct(edge1,edge2);
        n=Normalize(n);
        Color[0] = color[0] * coEfficients[0];
        Color[1] = color[1] * coEfficients[0];
        Color[2] = color[2] * coEfficients[0];

        for(int i=0 ; i <lights.size();i++)
        {
            Ray light_ray;
            point p1 = subtract(lights[i].light_pos,intersectionPoint);
            point dir = Normalize(p1);
            light_ray.dir = dir;
            //light_ray.start = add(intersectionPoint , scalerProduct(1.0,dir)) ;
            light_ray.start = intersectionPoint  ;

            bool is_in_shadow=false;
            for(int j=0;j<objects.size();j++)
            {
                double t= objects[j]->intersect(light_ray,Color,0);
                if(t>0.0 && t<Distance(intersectionPoint,lights[i].light_pos))
                {
                    is_in_shadow=true;
                    break;
                }
            }
            if(true/*!is_in_shadow*/)
            {
                double phongValue=0.0,lambertValue=0.0;
                lambertValue=DotProduct(light_ray.dir,n);

                lambertValue=max(0.0,lambertValue);
                //cout<<"lam"<<lambertValue<<endl;
                Color[0] += lights[i].color[0]*coEfficients[1]*lambertValue*color[0];
                Color[1] += lights[i].color[1]*coEfficients[1]*lambertValue*color[1];
                Color[2] += lights[i].color[2]*coEfficients[1]*lambertValue*color[2];
                //cout<<"diff: "<<"R "<<Color[0]<<" G "<<Color[1]<<" B "<<Color[2]<<endl;
                point R;
                //double temp= 2.0*DotProduct(r.dir,n);
                double temp= 2.0*DotProduct(light_ray.dir,n);
                R=scalerProduct(temp,n);
                //R=subtract(R,r.dir);
                R=subtract(R,light_ray.dir);
                //R=Normalize(R);


                //phongValue=pow(DotProduct(R,r.dir),shine);
                point p;
                phongValue=DotProduct(R,scalerProduct(-1.0,r.dir));
                phongValue=max(0.0,phongValue);
                //cout<<"phong"<<phongValue<<endl;
                //cout<<"phong : "<<phongValue<<endl;
                Color[0] += lights[i].color[0]*coEfficients[2]*pow(phongValue,shine)*color[0];
                Color[1] += lights[i].color[1]*coEfficients[2]*pow(phongValue,shine)*color[1];
                Color[2] += lights[i].color[2]*coEfficients[2]*pow(phongValue,shine)*color[2];

                //cout<<"spec: "<<"R "<<Color[0]<<" G "<<Color[1]<<" B "<<Color[2]<<endl;

                for(int k=0;k<3;k++)
                {
                    if(Color[k]<0) Color[k]=0;
                    if(Color[k]>1) Color[k]=1;
                }

            }

        }
        if (level >= recursion_level) return t;
        point Reflected;
        double temp1 = DotProduct(r.dir,n)*2.0;
        Reflected = scalerProduct(temp1,n);
        Reflected = subtract(r.dir,Reflected);
        Reflected = Normalize(Reflected);
        point start = add(intersectionPoint,scalerProduct(1.0,Reflected));

        Ray reflectedRay;
        reflectedRay.start=start;
        reflectedRay.dir=Reflected;

        double t_calc, tmin = (double)INT_MAX;
        double *reflected_color = new double[3];
        int nearest = -1;
        for(int k=0; k<objects.size(); k++)
        {
            t_calc=objects[k]->intersect(reflectedRay,reflected_color,0);
            if(t_calc<tmin && t_calc>0)
            {
                tmin = t_calc;
                nearest = k;
            }
        }
        if(nearest != -1)
        {
            double temp2 = objects[nearest]->intersect(reflectedRay,reflected_color,level+1);
            for(int p=0;p<3;p++)
            {
                Color[p] += coEfficients[3]*reflected_color[p];
            }
        }

        return t;

    };

};

class Floor: public Object
{
public:
    Floor(double floorWidth , double tileWidth)
    {
        reference_point.x = -(floorWidth*1.0/2);
        reference_point.y = -(floorWidth*1.0/2);
        reference_point.z = 0.0;
        length=tileWidth*1.0;
    }
    void draw()
    {
        glBegin(GL_QUADS);
        int m = (int)((reference_point.x)/length);
        int n = (int)((reference_point.y)/length);
        int l = (int) length;
        for (int i = m; i < -m; i++)
        {
			for (int j = n; j < -n; j++)
			{
				if ((i + j) % 2 == 0) glColor3f(1.0, 1.0, 1.0);
				else glColor3f(0, 0, 0);
				glVertex3f(i * l, j * l, 0);
				glVertex3f(i * l + l, j * l, 0);
				glVertex3f(i * l + l, j * l + l, 0);
				glVertex3f(i * l, j * l + l, 0);
			}
		}
		glEnd();
    }

    double get_intersection_t(Ray r)
    {
        return -1;
    }
    double intersect(Ray r ,double *Color, int level)
    {
        point n ;
        n.x=0;n.y=0;n.z=1.0;
        double t;
        if(DotProduct(n,r.dir) == 0.0) return -1;
        t=-1.0*(DotProduct(n,r.start)/DotProduct(n,r.dir));
        if(t<0) return -1;
        point intersectionPoint = add(r.start , scalerProduct(t,r.dir));
        double x1=reference_point.x;
        double y1=reference_point.y;    //these are top right and bottom left corner
        double x2=-reference_point.x;
        double y2=-reference_point.y;
        bool is_inside = true;
        if(intersectionPoint.x < x1 || intersectionPoint.x> x2 || intersectionPoint.y <y1 || intersectionPoint.y > y2)
        {
            t=-1;
            is_inside=false;
            return t;
        }
        if(level == 0) return t;
        double dx =  intersectionPoint.x - reference_point.x;
        double dy =  intersectionPoint.y - reference_point.y;
        int i = (int)((intersectionPoint.x - reference_point.x)/length);
        int j = (int)((intersectionPoint.y - reference_point.y)/length);

        double color[3];

        if((i + j) % 2 != 0)
        {
            for(int i=0;i<3;i++)
            {
                Color[i]=1.0;
                color[i]=1.0;
            }
        }
        else
        {
            for(int i=0;i<3;i++)
            {
                Color[i]=0.0;
                color[i]=0.0;
            }
        }
        Color[0] *= coEfficients[0];
        Color[1] *= coEfficients[0];
        Color[2] *= coEfficients[0];

        for(int i=0 ; i <lights.size();i++)
        {
            Ray light_ray;
            point p1 = subtract(lights[i].light_pos,intersectionPoint);
            point dir = Normalize(p1);
            light_ray.dir = dir;
            //light_ray.start = add(intersectionPoint , scalerProduct(1.0,dir)) ;
            light_ray.start = intersectionPoint  ;

            bool is_in_shadow=false;
            for(int j=0;j<objects.size();j++)
            {
                double t= objects[j]->intersect(light_ray,Color,0);
                if(t>0.0 && t<Distance(intersectionPoint,lights[i].light_pos))
                {
                    is_in_shadow=true;
                    break;
                }
            }
            if(true/*!is_in_shadow*/)
            {
                double phongValue=0.0,lambertValue=0.0;
                lambertValue=DotProduct(light_ray.dir,n);

                lambertValue=max(0.0,lambertValue);
                //cout<<"lam"<<lambertValue<<endl;
                Color[0] += lights[i].color[0]*coEfficients[1]*lambertValue*color[0];
                Color[1] += lights[i].color[1]*coEfficients[1]*lambertValue*color[1];
                Color[2] += lights[i].color[2]*coEfficients[1]*lambertValue*color[2];
                //cout<<"diff: "<<"R "<<Color[0]<<" G "<<Color[1]<<" B "<<Color[2]<<endl;
                point R;
                //double temp= 2.0*DotProduct(r.dir,n);
                double temp= 2.0*DotProduct(light_ray.dir,n);
                R=scalerProduct(temp,n);
                //R=subtract(R,r.dir);
                R=subtract(R,light_ray.dir);
                //R=Normalize(R);


                //phongValue=pow(DotProduct(R,r.dir),shine);
                point p;
                phongValue=DotProduct(R,scalerProduct(-1.0,r.dir));
                phongValue=max(0.0,phongValue);
                //cout<<"phong"<<phongValue<<endl;
                //cout<<"phong : "<<phongValue<<endl;
                Color[0] += lights[i].color[0]*coEfficients[2]*pow(phongValue,shine)*color[0];
                Color[1] += lights[i].color[1]*coEfficients[2]*pow(phongValue,shine)*color[1];
                Color[2] += lights[i].color[2]*coEfficients[2]*pow(phongValue,shine)*color[2];

                //cout<<"spec: "<<"R "<<Color[0]<<" G "<<Color[1]<<" B "<<Color[2]<<endl;

                for(int k=0;k<3;k++)
                {
                    if(Color[k]<0) Color[k]=0;
                    if(Color[k]>1) Color[k]=1;
                }

            }

        }
        if (level >= recursion_level) return t;
        point Reflected;
        double temp1 = DotProduct(r.dir,n)*2.0;
        Reflected = scalerProduct(temp1,n);
        Reflected = subtract(r.dir,Reflected);
        Reflected = Normalize(Reflected);
        point start = add(intersectionPoint,scalerProduct(1.0,Reflected));

        Ray reflectedRay;
        reflectedRay.start=start;
        reflectedRay.dir=Reflected;

        double t_calc, tmin = (double)INT_MAX;
        double *reflected_color = new double[3];
        int nearest = -1;
        for(int k=0; k<objects.size(); k++)
        {
            t_calc=objects[k]->intersect(reflectedRay,reflected_color,0);
            if(t_calc<tmin && t_calc>0)
            {
                tmin = t_calc;
                nearest = k;
            }
        }
        if(nearest != -1)
        {
            double temp2 = objects[nearest]->intersect(reflectedRay,reflected_color,level+1);
            for(int p=0;p<3;p++)
            {
                Color[p] += coEfficients[3]*reflected_color[p];
            }
        }
        delete reflected_color;
        return t;

    }
};







