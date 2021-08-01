#include <bits/stdc++.h>
#include <cmath>
#include "bitmap_image.hpp"

using namespace std;

#define pi (2*acos(0.0))

struct point
{
    double x,y,z;
};

struct color
{
    int R,G,B;
};

struct Triangle
{
    point p1,p2,p3;
    point t1,t2,t3;
    color ownColor;
};

struct Matrix
{
    double mat[4][4];
};

//stores the latest Matrix
double ScaleMatrix [4][4];
double RotationMatrix[4][4];
double TranslateMatrix [4][4];
double LastMatrix[4][4];
double Top[4][4];
double T[4][4], V[4][4],R [4][4], P[4][4];
int pushcount=0;
int popcount=0;
vector<Triangle> TriangleArray;
vector<Matrix> Stack;

void AddTriangle(Triangle t1)
{
    TriangleArray.push_back(t1);
}

point X_Intersection(point a,point b,double ys)
{
    point ret;
    double x = a.x + (ys - a.y)*(b.x - a.x)/(b.y - a.y);
    ret.x = x;
    ret.y=ys;
    ret.z=-1000;
    return ret;
}
point Z_Intersection(point a,point b,double ys,double x)
{
    point ret;
    double z = a.z + (ys - a.y)*(b.z - a.z)/(b.y - a.y);
    ret.x=x;ret.y=ys;ret.z=z;
    return ret;
}

double Distance(point a,point b)
{
    return sqrt(pow((a.x - b.x),2)+pow((a.y - b.y),2));
}

void UpdateLast(double x[4][4])
{
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            LastMatrix[i][j] = x[i][j];
        }
    }
}

void UpdateTop()
{
    double temp[4][4];
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            double sum=0;
            for(int k=0; k<4; k++)
            {
                sum+=Top[i][k] * LastMatrix[k][j];
                temp[i][j] = sum;
            }
        }
    }

    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            Top[i][j] = temp [i][j];
        }
    }
}

void PrintLastMatrix()
{
    cout<<"Printing LAST\n";
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            cout<<LastMatrix[i][j]<<" ";
        }
        cout<<"\n";
    }
}

void PrintTop()
{
    cout<<"Printing TOP\n";
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            cout<<Top[i][j]<<" ";
        }
        cout<<"\n";
    }
}

void Initialize()
{
    Matrix top;
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            if(i==j)
            {
                Top[i][j] = 1.0;
                top.mat[i][j] = 1.0;
            }
            else
            {
                Top[i][j] = 0.0;
                top.mat[i][j] = 0.0;
            }
        }
    }
    Stack.push_back(top);
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

void PrintPoint(point p)
{
    cout<<p.x<<" "<<p.y<<" "<<p.z<<endl;
}
point RotationVector(point x,point a,double angle)
{
    point temp,temp1,temp2;
    double angle_radian,cosA,sinA;
    int b=(int)angle/90;
    if((b%2) != 0 && (int)angle % 90 == 0)
    {
        cosA = 0;
        if(angle == 270.0 || angle == -90.0) sinA = -1;
        else sinA = 1;
    }
    else
    {
        angle_radian=(angle*pi)/180.0;
        cosA=cos(angle_radian);
        sinA=sin(angle_radian);
    }
    temp.x = x.x*cosA;
    temp.y = x.y*cosA;
    temp.z = x.z*cosA;

    double t1 = 1.0-cosA;
    double t2 = DotProduct(a,x);

    temp1.x = t1*t2*a.x;
    temp1.y = t1*t2*a.y;
    temp1.z = t1*t2*a.z;

    point cross;
    cross = CrossProduct(a,x);

    temp2.x = cross.x*sinA;
    temp2.y = cross.y*sinA;
    temp2.z = cross.z*sinA;

    point ret;

    ret.x = temp.x+temp1.x+temp2.x;
    ret.y = temp.y+temp1.y+temp2.y;
    ret.z = temp.z+temp1.z+temp2.z;


    return ret;
}

point UpdateTransformation(double current[4][1])
{
    double Transformed[4][1];
    point ret;
    for(int m=0; m<4; m++)
    {
        double sum=0.0;
        for(int n=0; n<4; n++)
        {
            sum+=Top[m][n] * current[n][0];
            Transformed[m][0] = sum;
        }
    }
    for(int i=0; i<4; i++)
    {
        Transformed[i][0] /=Transformed[3][0];
    }
    ret.x=Transformed[0][0];
    ret.y=Transformed[1][0];
    ret.z=Transformed[2][0];

    return ret;
}

point UpdateView(double current[4][1])
{
    double Transformed[4][1];
    point ret;
    for(int m=0; m<4; m++)
    {
        double sum=0.0;
        for(int n=0; n<4; n++)
        {
            sum+=V[m][n] * current[n][0];
            Transformed[m][0] = sum;
        }
    }
    for(int i=0; i<4; i++)
    {
        Transformed[i][0] /=Transformed[3][0];
    }
    ret.x=Transformed[0][0];
    ret.y=Transformed[1][0];
    ret.z=Transformed[2][0];

    return ret;
}

point UpdateProjection(double current[4][1])
{
    double Transformed[4][1];
    point ret;
    for(int m=0; m<4; m++)
    {
        double sum=0.0;
        for(int n=0; n<4; n++)
        {
            sum+=P[m][n] * current[n][0];
            Transformed[m][0] = sum;
        }
    }
    for(int i=0; i<4; i++)
    {
        Transformed[i][0] /=Transformed[3][0];
    }
    ret.x=Transformed[0][0];
    ret.y=Transformed[1][0];
    ret.z=Transformed[2][0];

    return ret;
}
int main()
{
    fstream newfile;
    newfile.open("scene.txt",ios::in);
    //Reading from the 4th line of the file

    if (newfile.is_open())
    {
        string tp;
        int i=0;
        Initialize();
        while(getline(newfile, tp))
        {
            i++;
            if(i<5) continue;
            if(tp == "triangle")
            {
                string t1,t2,t3;
                getline(newfile, t1);
                getline(newfile, t2);
                getline(newfile, t3);

                stringstream ss1,ss2,ss3;
                double temp1[3];
                double temp2[3];
                double temp3[3];
                int i=0;
                string v1,v2,v3;
                Triangle tri1;

                //spliting the line
                ss1 << t1;
                while (ss1.good() && i < 3)
                {
                    ss1 >> v1;
                    temp1[i++]=atof(v1.c_str());
                }
                //setting the co-ordinates of the triangle
                tri1.p1.x=temp1[0];
                tri1.p1.y=temp1[1];
                tri1.p1.z=temp1[2];

                i=0;
                ss2 << t2;
                while (ss2.good() && i < 3)
                {
                    ss2 >> v2;
                    temp2[i++]=atof(v2.c_str());
                }
                tri1.p2.x=temp2[0];
                tri1.p2.y=temp2[1];
                tri1.p2.z=temp2[2];

                i=0;
                ss3 << t3;
                while (ss3.good() && i < 3)
                {
                    ss3 >> v3;
                    temp3[i++]=atof(v3.c_str());
                }
                tri1.p3.x=temp3[0];
                tri1.p3.y=temp3[1];
                tri1.p3.z=temp3[2];

                double Current_Co_Ordinate[4][1];
                point transformed;

                Current_Co_Ordinate[0][0]=tri1.p1.x;
                Current_Co_Ordinate[1][0]=tri1.p1.y;
                Current_Co_Ordinate[2][0]=tri1.p1.z;
                Current_Co_Ordinate[3][0]=1.0;

                transformed = UpdateTransformation(Current_Co_Ordinate);
                tri1.t1.x = transformed.x;
                tri1.t1.y = transformed.y;
                tri1.t1.z = transformed.z;

                Current_Co_Ordinate[0][0]=tri1.p2.x;
                Current_Co_Ordinate[1][0]=tri1.p2.y;
                Current_Co_Ordinate[2][0]=tri1.p2.z;
                Current_Co_Ordinate[3][0]=1.0;

                transformed = UpdateTransformation(Current_Co_Ordinate);
                tri1.t2.x = transformed.x;
                tri1.t2.y = transformed.y;
                tri1.t2.z = transformed.z;

                Current_Co_Ordinate[0][0]=tri1.p3.x;
                Current_Co_Ordinate[1][0]=tri1.p3.y;
                Current_Co_Ordinate[2][0]=tri1.p3.z;
                Current_Co_Ordinate[3][0]=1.0;

                transformed = UpdateTransformation(Current_Co_Ordinate);
                tri1.t3.x = transformed.x;
                tri1.t3.y = transformed.y;
                tri1.t3.z = transformed.z;

                tri1.ownColor.R= rand()%255;
                tri1.ownColor.G= rand()%255;
                tri1.ownColor.B= rand()%255;

                AddTriangle(tri1);
                //PrintTriangle(tri1);

            }
            else if(tp == "translate")
            {
                //cout<<"translate "<<endl;
                string t;
                getline(newfile, t);

                stringstream ss;
                double temp[3];
                string v;
                int i=0;
                //spliting the line
                ss << t;
                while (ss.good() && i < 3)
                {
                    ss >> v;
                    temp[i++]=atof(v.c_str());
                }

                TranslateMatrix[0][0] = 1.0;
                TranslateMatrix[0][1] = 0.0;
                TranslateMatrix[0][2] = 0.0;
                TranslateMatrix[0][3] = temp[0];

                TranslateMatrix[1][0] = 0.0;
                TranslateMatrix[1][1] = 1.0;
                TranslateMatrix[1][2] = 0.0;
                TranslateMatrix[1][3] = temp[1];

                TranslateMatrix[2][0] = 0.0;
                TranslateMatrix[2][1] = 0.0;
                TranslateMatrix[2][2] = 1.0;
                TranslateMatrix[2][3] = temp[2];

                TranslateMatrix[3][0] = 0.0;
                TranslateMatrix[3][1] = 0.0;
                TranslateMatrix[3][2] = 0.0;
                TranslateMatrix[3][3] = 1.0;

                UpdateLast(TranslateMatrix);
                UpdateTop();
                //PrintTop();

            }
            else if(tp == "rotate")
            {
                //cout<<"rotation "<<endl;
                string t;
                getline(newfile, t);

                stringstream ss;
                double temp[4];
                string v;
                int m=0;
                //spliting the line
                ss << t;
                while (ss.good() && m < 4)
                {
                    ss >> v;
                    temp[m++]=atof(v.c_str());
                }
                double angle=temp[0];
                point a;
                a.x=temp[1];
                a.y=temp[2];
                a.z=temp[3];

                //Normalization
                double mod=Modulus(a);
                a.x=(a.x)/mod;
                a.y=(a.y)/mod;
                a.z=(a.z)/mod;

                point i,j,k;
                i.x=1;
                i.y=0;
                i.z=0;

                j.x=0;
                j.y=1;
                j.z=0;

                k.x=0;
                k.y=0;
                k.z=1;
                point c1=RotationVector(i,a,angle);
                point c2=RotationVector(j,a,angle);
                point c3=RotationVector(k,a,angle);

                RotationMatrix[0][0] = c1.x;
                RotationMatrix[0][1] = c2.x;
                RotationMatrix[0][2] = c3.x;
                RotationMatrix[0][3] = 0.0;

                RotationMatrix[1][0] = c1.y;
                RotationMatrix[1][1] = c2.y;
                RotationMatrix[1][2] = c3.y;
                RotationMatrix[1][3] = 0.0;

                RotationMatrix[2][0] = c1.z;
                RotationMatrix[2][1] = c2.z;
                RotationMatrix[2][2] = c3.z;
                RotationMatrix[2][3] = 0.0;

                RotationMatrix[3][0] = 0.0;
                RotationMatrix[3][1] = 0.0;
                RotationMatrix[3][2] = 0.0;
                RotationMatrix[3][3] = 1.0;

                UpdateLast(RotationMatrix);
                UpdateTop();
                //PrintTop();
            }
            else if(tp == "scale")
            {
                //cout<<"scale "<<endl;
                string t;
                getline(newfile, t);

                stringstream ss;
                double temp[3];
                string v;
                int i=0;
                //spliting the line
                ss << t;
                while (ss.good() && i < 3)
                {
                    ss >> v;
                    temp[i++]=atof(v.c_str());
                }

                ScaleMatrix[0][0]=temp[0];
                ScaleMatrix[0][1]=0.0;
                ScaleMatrix[0][2]=0.0;
                ScaleMatrix[0][3]=0.0;

                ScaleMatrix[1][0]=0.0;
                ScaleMatrix[1][1]=temp[1];
                ScaleMatrix[1][2]=0.0;
                ScaleMatrix[1][3]=0.0;

                ScaleMatrix[2][0]=0.0;
                ScaleMatrix[2][1]=0.0;
                ScaleMatrix[2][2]=temp[2];
                ScaleMatrix[2][3]=0.0;

                ScaleMatrix[3][0]=0.0;
                ScaleMatrix[3][1]=0.0;
                ScaleMatrix[3][2]=0.0;
                ScaleMatrix[3][3]=1.0;

                UpdateLast(ScaleMatrix);
                UpdateTop();
                //PrintTop();
            }
            else if(tp == "push")
            {
                //cout<<"push "<<endl;
                Matrix mat_top;
                if(pushcount !=0)
                {
                    for(int i=0; i<4; i++)
                    {
                        for(int j=0; j<4; j++)
                        {
                            mat_top.mat[i][j] = Top[i][j];
                        }
                    }
                    Stack.push_back(mat_top);

                }
                pushcount++;
                //cout<<"Top Matrix after push "<<pushcount<<endl;
                //PrintTop();

            }
            else if(tp == "pop")
            {
                //cout<<"pop"<<endl;
                popcount++;
                if(pushcount == popcount)
                {
                    Stack.clear();
                    Matrix I;
                    for(int i=0; i<4; i++)
                    {
                        for(int j=0; j<4; j++)
                        {
                            if(i==j)
                            {
                                Top[i][j] = 1.0;
                                I.mat[i][j] = 1.0;
                            }
                            else
                            {
                                Top[i][j] = 0.0;
                                I.mat[i][j] = 0.0;
                            }
                        }
                    }
                    Stack.push_back(I);
                }
                Matrix newTop=Stack.back();
                for(int i=0; i<4; i++)
                {
                    for(int j=0; j<4; j++)
                    {
                        Top[i][j] = newTop.mat[i][j];
                    }
                }
                //PrintLastMatrix();
                //cout<<"Top Matrix after pop "<<popcount<<endl;;
                //PrintTop();

            }
            else if(tp == "end")
            {
                //cout<<"end "<<endl;
            }
            else cout<<""<<endl;

        }
        newfile.close();
    }
    newfile.open("stage1.txt",ios::out);
    if(newfile.is_open())
    {
        for(int i=0; i<TriangleArray.size(); i++)
        {
            Triangle T = TriangleArray[i];
            newfile<<setprecision(7)<<fixed<<T.t1.x<<" "<<T.t1.y<<" "<<T.t1.z<<"\n";
            newfile<<setprecision(7)<<fixed<<T.t2.x<<" "<<T.t2.y<<" "<<T.t2.z<<"\n";
            newfile<<setprecision(7)<<fixed<<T.t3.x<<" "<<T.t3.y<<" "<<T.t3.z<<"\n\n";
        }
        newfile.close();
    }

    //Stage 2
    newfile.open("scene.txt",ios::in);
    //Reading the 1st 4 line
    string t1,t2,t3,t4;

    if (newfile.is_open())
    {
        getline(newfile, t1);
        getline(newfile, t2);
        getline(newfile, t3);
        getline(newfile, t4);

        newfile.close();
    }


    stringstream ss1,ss2,ss3;
    double temp1[3];
    double temp2[3];
    double temp3[3];
    int i=0;
    string v1,v2,v3;
    point eye,look,up;
    //spliting the line
    ss1 << t1;
    while (ss1.good() && i < 3)
    {
        ss1 >> v1;
        temp1[i++]=atof(v1.c_str());
    }
    //setting the co-ordinates of the triangle
    eye.x=temp1[0];
    eye.y=temp1[1];
    eye.z=temp1[2];

    i=0;
    ss2 << t2;
    while (ss2.good() && i < 3)
    {
        ss2 >> v2;
        temp2[i++]=atof(v2.c_str());
    }
    look.x=temp2[0];
    look.y=temp2[1];
    look.z=temp2[2];

    i=0;
    ss3 << t3;
    while (ss3.good() && i < 3)
    {
        ss3 >> v3;
        temp3[i++]=atof(v3.c_str());
    }
    up.x=temp3[0];
    up.y=temp3[1];
    up.z=temp3[2];

    point l,r,u;
    l.x = look.x - eye.x;
    l.y = look.y - eye.y;
    l.z = look.z - eye.z;

    l.x=(l.x)/Modulus(l);
    l.y=(l.y)/Modulus(l);
    l.z=(l.z)/Modulus(l);

    r = CrossProduct(l,up);
    r.x=(r.x)/Modulus(r);
    r.y=(r.y)/Modulus(r);
    r.z=(r.z)/Modulus(r);

    u=CrossProduct(r,l);

    T[0][0] = 1.0 ;
    T[0][1] = 0.0 ;
    T[0][2] = 0.0 ;
    T[0][3] = -(eye.x);
    T[1][0] = 0.0 ;
    T[1][1] = 1.0 ;
    T[1][2] = 0.0 ;
    T[1][3] = -(eye.y);
    T[2][0] = 0.0 ;
    T[2][1] = 0.0 ;
    T[2][2] = 1.0 ;
    T[2][3] = -(eye.z);
    T[3][0] = 0.0 ;
    T[3][1] = 0.0 ;
    T[3][2] = 0.0 ;
    T[3][3] = 1.0;

    R[0][0] = r.x ;
    R[0][1] = r.y ;
    R[0][2] = r.z ;
    R[0][3] = 0.0;
    R[1][0] = u.x ;
    R[1][1] = u.y ;
    R[1][2] = u.z ;
    R[1][3] = 0.0;
    R[2][0] = -(l.x) ;
    R[2][1] = -(l.y) ;
    R[2][2] = -(l.z) ;
    R[2][3] = 0.0;
    R[3][0] = 0.0 ;
    R[3][1] = 0.0 ;
    R[3][2] = 0.0 ;
    R[3][3] = 1.0;

    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            double sum=0;
            for(int k=0; k<4; k++)
            {
                sum+=R[i][k] * T[k][j];
                V[i][j] = sum;
            }
        }
    }

    for(int i=0; i<TriangleArray.size(); i++)
    {
        double current[4][1];
        point view_trans1,view_trans2,view_trans3;

        current[0][0] = TriangleArray[i].t1.x;
        current[1][0] = TriangleArray[i].t1.y;
        current[2][0] = TriangleArray[i].t1.z;
        current[3][0] = 1.0;
        view_trans1 = UpdateView(current);

        current[0][0] = TriangleArray[i].t2.x;
        current[1][0] = TriangleArray[i].t2.y;
        current[2][0] = TriangleArray[i].t2.z;
        current[3][0] = 1.0;
        view_trans2 = UpdateView(current);

        current[0][0] = TriangleArray[i].t3.x;
        current[1][0] = TriangleArray[i].t3.y;
        current[2][0] = TriangleArray[i].t3.z;
        current[3][0] = 1.0;
        view_trans3 = UpdateView(current);

        TriangleArray[i].t1.x = view_trans1.x;
        TriangleArray[i].t1.y = view_trans1.y;
        TriangleArray[i].t1.z = view_trans1.z;

        TriangleArray[i].t2.x = view_trans2.x;
        TriangleArray[i].t2.y = view_trans2.y;
        TriangleArray[i].t2.z = view_trans2.z;

        TriangleArray[i].t3.x = view_trans3.x;
        TriangleArray[i].t3.y = view_trans3.y;
        TriangleArray[i].t3.z = view_trans3.z;
    }

    newfile.open("stage2.txt",ios::out);
    if(newfile.is_open())
    {
        for(int i=0; i<TriangleArray.size(); i++)
        {
            Triangle T = TriangleArray[i];
            newfile<<setprecision(7)<<fixed<<T.t1.x<<" "<<T.t1.y<<" "<<T.t1.z<<"\n";
            newfile<<setprecision(7)<<fixed<<T.t2.x<<" "<<T.t2.y<<" "<<T.t2.z<<"\n";
            newfile<<setprecision(7)<<fixed<<T.t3.x<<" "<<T.t3.y<<" "<<T.t3.z<<"\n\n";
        }
        newfile.close();
    }

    //Stage 3
    stringstream ss4;
    double temp4[4];
    i=0;
    string v4;
    //spliting the line
    ss4 << t4;
    while (ss4.good() && i < 4)
    {
        ss4 >> v4;
        temp4[i++]=atof(v4.c_str());
    }
    double fovY = temp4[0];
    double aspectRatio = temp4[1];
    double near = temp4[2];
    double far = temp4[3];

    double fovX = fovY * aspectRatio;
    double angle1 = ((fovY/2) * pi)/180.0;
    double angle2 = ((fovX/2) * pi)/180.0;
    double t = near * tan(angle1);
    double r1 = near * tan(angle2);

    double f1 = -(far+near)/(far-near);
    double f2 = -(2.0*far*near)/(far-near);

    P[0][0] = near/r1 ;
    P[0][1] = 0.0 ;
    P[0][2] = 0.0 ;
    P[0][3] = 0.0;
    P[1][0] = 0.0 ;
    P[1][1] = near/t ;
    P[1][2] = 0.0 ;
    P[1][3] = 0.0;
    P[2][0] = 0.0 ;
    P[2][1] = 0.0 ;
    P[2][2] = f1 ;
    P[2][3] = f2;
    P[3][0] = 0.0 ;
    P[3][1] = 0.0 ;
    P[3][2] = -1.0 ;
    P[3][3] = 0.0;

    for(int i=0; i<TriangleArray.size(); i++)
    {
        double current[4][1];
        point projection_trans1,projection_trans2,projection_trans3;

        current[0][0] = TriangleArray[i].t1.x;
        current[1][0] = TriangleArray[i].t1.y;
        current[2][0] = TriangleArray[i].t1.z;
        current[3][0] = 1.0;
        projection_trans1 = UpdateProjection(current);

        current[0][0] = TriangleArray[i].t2.x;
        current[1][0] = TriangleArray[i].t2.y;
        current[2][0] = TriangleArray[i].t2.z;
        current[3][0] = 1.0;
        projection_trans2 = UpdateProjection(current);

        current[0][0] = TriangleArray[i].t3.x;
        current[1][0] = TriangleArray[i].t3.y;
        current[2][0] = TriangleArray[i].t3.z;
        current[3][0] = 1.0;
        projection_trans3 = UpdateProjection(current);

        TriangleArray[i].t1.x = projection_trans1.x;
        TriangleArray[i].t1.y = projection_trans1.y;
        TriangleArray[i].t1.z = projection_trans1.z;

        TriangleArray[i].t2.x = projection_trans2.x;
        TriangleArray[i].t2.y = projection_trans2.y;
        TriangleArray[i].t2.z = projection_trans2.z;

        TriangleArray[i].t3.x = projection_trans3.x;
        TriangleArray[i].t3.y = projection_trans3.y;
        TriangleArray[i].t3.z = projection_trans3.z;
    }

    newfile.open("stage3.txt",ios::out);
    if(newfile.is_open())
    {
        for(int i=0; i<TriangleArray.size(); i++)
        {
            Triangle T = TriangleArray[i];
            newfile<<setprecision(7)<<fixed<<T.t1.x<<" "<<T.t1.y<<" "<<T.t1.z<<"\n";
            newfile<<setprecision(7)<<fixed<<T.t2.x<<" "<<T.t2.y<<" "<<T.t2.z<<"\n";
            newfile<<setprecision(7)<<fixed<<T.t3.x<<" "<<T.t3.y<<" "<<T.t3.z<<"\n\n";
        }
        newfile.close();
    }

    newfile.open("config.txt",ios::in);
    //Reading the 1st 4 line
    string t5,t6,t7,t8;

    if (newfile.is_open())
    {
        getline(newfile, t5);
        getline(newfile, t6);
        getline(newfile, t7);
        getline(newfile, t8);

        newfile.close();
    }
    stringstream ss5,ss6,ss7,ss8;
    double temp5[2];
    double temp6[1];
    double temp7[1];
    double temp8[2];
    string v5,v6,v7,v8;
    //spliting the line
    i=0;
    ss5 << t5;
    while (ss5.good() && i < 2)
    {
        ss5 >> v5;
        temp5[i++]=atof(v5.c_str());
    }

    i=0;
    ss6 << t6;
    while (ss6.good() && i < 1)
    {
        ss6 >> v6;
        temp6[0]=atof(v6.c_str());
    }

    ss7 << t7;
    ss7 >> v7;
    temp7[0]=atof(v7.c_str());

    i=0;
    ss8 << t8;
    while (ss8.good() && i < 2)
    {
        ss8 >> v8;
        temp8[i++]=atof(v8.c_str());
    }

    double Left_X = temp6[0];
    double Bottom_Y = temp7[0];
    double Top_Y = -1.0*Bottom_Y;
    double Right_X = -1.0*Left_X;
    int Screen_Width = (int)temp5[0];
    int Screen_Height = (int)temp5[1];
    double Z_min = temp8[0];
    double Z_max = temp8[1];
    double dx = (2*Right_X) / Screen_Width;
    double dy = (2*Top_Y) / Screen_Height;

    Top_Y = Top_Y - (dy/2) ;
    Left_X = Left_X + (dx/2);

    double Z_Buffer[Screen_Width][Screen_Height];
    bitmap_image image(Screen_Width,Screen_Height);

    for(int i=0; i<Screen_Width; i++)
    {
        for(int j=0; j<Screen_Height; j++)
        {
            Z_Buffer[i][j] = Z_max;
            image.set_pixel(i,j,0,0,0);
        }
    }


    for(int i=0; i<TriangleArray.size(); i++)
    {
        double max_Y,min_Y,max_X,min_X,a,b,c;

        a=TriangleArray[i].t1.y,b=TriangleArray[i].t2.y,c=TriangleArray[i].t3.y;
        max_Y = max(a,max(b,c));
        min_Y = min(a,min(b,c));

        a=TriangleArray[i].t1.x,b=TriangleArray[i].t2.x,c=TriangleArray[i].t3.x;
        max_X = max(a,max(b,c));
        min_X = min(a,min(b,c));
        int top_Row,bottom_Row;
        int left_Col,right_Col;

        if(max_Y>Top_Y) top_Row = 0;
        else top_Row = round( (Top_Y - max_Y)/dy );

        if(min_Y<Bottom_Y) bottom_Row = round((Bottom_Y-Top_Y)/dy);
        else bottom_Row = round( (Top_Y - min_Y)/dy );

        for(int j = top_Row+1; j<bottom_Row; j++)
        {
            double ys = Top_Y -(j*dy);
            double t1,t2,t3;
            double xa,xb,za,zb;
            point Intersect[2];
            point E,F,G;
            E = X_Intersection(TriangleArray[i].t1,TriangleArray[i].t2,ys);
            F = X_Intersection(TriangleArray[i].t1,TriangleArray[i].t3,ys);
            G = X_Intersection(TriangleArray[i].t2,TriangleArray[i].t3,ys);
            //Calculating xa,xb
            int l=0;
            t1 = Distance(TriangleArray[i].t1,E);
            t2 = Distance(TriangleArray[i].t2,E);
            t3 = Distance(TriangleArray[i].t1,TriangleArray[i].t2);

            //cout<<"ys :"<<ys<<endl;

            if((t1+t2-t3<0.001))
            {
                //cout<<"t1,t2 intersected at E \n";
                double x = E.x;
                point E1 = Z_Intersection(TriangleArray[i].t1,TriangleArray[i].t2,ys,x);
                Intersect[l].x = E1.x;
                Intersect[l].y = E1.y;
                Intersect[l].z = E1.z;
                //cout<<"E1 : "<<E1.x<<" "<<E1.y<<" "<<E1.z<<endl;
                l++;
            }

            t1 = Distance(TriangleArray[i].t1,F);
            t2 = Distance(TriangleArray[i].t3,F);
            t3 = Distance(TriangleArray[i].t1,TriangleArray[i].t3);

            if((t1+t2-t3<0.001))
            {
                //cout<<"t1,t3 intersected at F \n";
                double x = F.x;
                point F1 = Z_Intersection(TriangleArray[i].t1,TriangleArray[i].t3,ys,x);
                Intersect[l].x = F1.x;
                Intersect[l].y = F1.y;
                Intersect[l].z = F1.z;
                //cout<<"F1 : "<<F1.x<<" "<<F1.y<<" "<<F1.z<<endl;
                l++;
            }

            t1 = Distance(TriangleArray[i].t2,G);
            t2 = Distance(TriangleArray[i].t3,G);
            t3 = Distance(TriangleArray[i].t2,TriangleArray[i].t3);

            if((t1+t2-t3<0.001))
            {
                //cout<<"t2,t3 intersected at G \n";
                double x = G.x;
                point G1 = Z_Intersection(TriangleArray[i].t2,TriangleArray[i].t3,ys,x);
                Intersect[l].x = G1.x;
                Intersect[l].y = G1.y;
                Intersect[l].z = G1.z;
                //cout<<"G1 : "<<G1.x<<" "<<G1.y<<" "<<G1.z<<endl;
                l++;
            }
            if(Intersect[0].x > Intersect[1].x)
            {
                xa=Intersect[1].x;
                xb=Intersect[0].x;
                za=Intersect[1].z;
                zb=Intersect[0].z;
            }
            else
            {
                xa=Intersect[0].x;
                xb=Intersect[1].x;
                za=Intersect[0].z;
                zb=Intersect[1].z;
            }
            //cout<<"xa "<<xa<<" xb "<<xb<<" za "<<za<<" zb "<<zb<<endl;

            if(xa<Left_X) left_Col = 0;
            else left_Col = round((xa - Left_X)/dx);

            if(xb>Right_X) right_Col = round((Right_X-Left_X)/dx);
            else right_Col = round((xb - Left_X)/dx);
            double zp,xp;
            for(int k=left_Col; k<right_Col; k++)
            {
                xp = Left_X + k*dx;
                zp=za+((xp-xa)*(zb-za)/(xb-xa));
                if(zp<Z_Buffer[j][k] && zp>Z_min)
                {

                    Z_Buffer[j][k] = zp;
                    color c ;
                    c.R  = TriangleArray[i].ownColor.R;
                    c.G  = TriangleArray[i].ownColor.G;
                    c.B  = TriangleArray[i].ownColor.B;
                    image.set_pixel(k,j,c.R,c.G,c.B);
                }
            }

        }
    }

    newfile.open("z_buffer.txt",ios::out);
    if(newfile.is_open())
    {
        for(int i=0;i<Screen_Width;i++)
        {
            for(int j=0;j<Screen_Height;j++)
            {
                if(Z_Buffer[i][j] < Z_max) newfile<<setprecision(7)<<fixed << Z_Buffer[i][j] << "   ";
                else newfile<<setprecision(7)<<fixed << "   ";
            }
            newfile<<"\n";
        }
        newfile.close();
    }


    image.save_image("output.bmp");;

    return 0;

}
