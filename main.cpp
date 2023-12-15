#include "space.h"
#include "EngineFunctions.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <algorithm>
#define fori(n) for (int i = 0; i < n; i++)
#define forj(n) for (int j = 0; j < n; j++)
#define fork(n) for (int k = 0; k < n; k++)
#include <SFML/Audio.hpp>
void rotate_point(num cx,num cy,num angle,num &x,num &y)
{
    num s = sin(angle);
    num c = cos(angle);
    // translate point back to origin:
    x -= cx;
    y -= cy;

    // rotate point
    num xnew = x * c - y * s;
    num ynew = x * s + y * c;

    // translate point back:
    x = xnew + cx;
    y = ynew + cy;
}

num ViewAngle = M_PI / 2;
num ZFar = 10, ZNear = 0;
num Height = 500, Width = 500;
Point View{-20, -5, 0};
Vector U{1, 0,0}, V{0,-1, 0}, W{0,0,1},normalV,directv;

//original vectors
num Tr=0,Tu=0;//t right ,t up
vector<Point> points;
vector<Point> Triangles;
vector<sf::Color> TriangleColor;
vector<int> sortedTriangles;

vector<object3d> objects;
vector<object3d> dadObjects;
#define sT sortedTriangles

bool sameDirection = 0;

void loadFile(string file,Point scale={1,1,1},Point shift={0,0,0}){//not tested
ifstream in(file);
int Np=points.size(),
Nt=Triangles.size(),
No=objects.size();
char enter;
int get,alpha;
object3d getObj;
Point get3;
sf::Color col;
while(in>>enter){
switch(enter)
{case 'o':{//object
in>>getObj.start>>getObj.len>>getObj.dad;
getObj.start+=Nt;
if(getObj.dad!=-1&&
  getObj.dad<objects.size()-No){
 objects[No+getObj.dad].son.pb(objects.size());
    }else{dadObjects.pb(getObj);}
objects.pb(getObj);
    break;}
case 't':{//triangle
in>>get3.x>>get3.y>>get3.z;
get3.x+=Np;
get3.y+=Np;
get3.z+=Np;
Triangles.pb(get3);
in>>get3.x>>get3.y>>get3.z>>alpha;
TriangleColor.pb(sf::Color(get3.x,get3.y,get3.z,alpha));
   break;}
case 'p':{//point
in>>get3.x>>get3.y>>get3.z;
get3=
Point{
get3.x*scale.x+shift.x,
get3.y*scale.y+shift.y,
get3.z*scale.z+shift.z
};
points.pb(get3);
    break;}
case 'r':{//rectangle
in>>get3.x>>get3.y>>get3.z;
get3.x+=Np;get3.y+=Np;get3.z+=Np;
Triangles.pb(get3);
get3.y=get3.z;
in>>get3.z;get3.z+=Np;
Triangles.pb(get3);
in>>get3.x>>get3.y>>get3.z>>alpha;
fori(2)
TriangleColor.pb(sf::Color(get3.x,get3.y,get3.z,alpha));

break;}
case 'c':{
    in>>get;
    in>>get3.x>>get3.y>>get3.z;
    get3.x+=Np;
    get3.y+=Np;
    get3.z+=Np;
    Triangles.pb(get3);
    fori(get-3){
        get3.y=get3.z;
        in>>get3.z;
        get3.z+=Np;
        Triangles.pb(get3);
        }
in>>get3.x>>get3.y>>get3.z>>alpha;
col=sf::Color(get3.x,get3.y,get3.z,alpha);
fori(get-2)
TriangleColor.pb(col);
    break;}
default:break;}    
    }
}



Point transform1(Point A, Point B)
{ //view from A to B
    Point res;
    num t = U.a * (B.x - A.x) + U.b * (B.y - A.y) + U.c * (B.z - A.z);
    t /= (U.a * U.a + U.b * U.b + U.c * U.c);
    Point C{U.a * t + A.x, U.b * t + A.y, U.c * t + A.z};
    res.z = A.length(C);
    Vector CB{B.x - C.x, B.y - C.y, B.z - C.z},
        AB{B.x - A.x, B.y - A.y, B.z - A.z};
    res.x = (W.a * CB.b - CB.a * W.b) / (W.a * V.b - V.a * W.b);
    res.y = (V.a * CB.b - CB.a * V.b) / (W.a * V.b - V.a * W.b);
    sameDirection = 1;
    directv=AB;
  
    return res;
}

Point transform2(Point ob)
{
    num w = ob.z;
    ob.x *= (Height / Width) / tan(ViewAngle / 2);
    ob.y /= tan(ViewAngle / 2);
    ob.z = (ob.z - ZNear) * (ZFar / (ZFar - ZNear));
    if (w == 0)
    {
        w = 1;
    }
   
    
        ob.z /= w;
        ob.x = (ob.x * Width) / (2.0 * w) + Width / 2;
        ob.y = (ob.y * Height) / (2.0 * w) + Height / 2;
    
    return ob;
}

bool closer(int a1,int b1){
    //if a is closer than b;
    #define T points
    Point a=Triangles[a1],
              b=Triangles[b1];    
    
    Point ca{(T[a.x].x+T[a.y].x+T[a.z].x)/3,
                    (T[a.x].y+T[a.y].y+T[a.z].y)/3,
                    (T[a.x].z+T[a.y].z+T[a.z].z)/3},
               cb{(T[b.x].x+T[b.y].x+T[b.z].x)/3,
                    (T[b.x].y+T[b.y].y+T[b.z].y)/3,
                    (T[b.x].z+T[b.y].z+T[b.z].z)/3};                  
return (View.length(ca)>View.length(cb));  
    }
    
   
num fix(num a){
   a*=1000000;
   a=int(a);
   a/=1000000;
   return a;
    }
void setUVW(){
    U=Vector{
       fix(cos(Tu)*cos(Tr)),
       fix(cos(Tu)*sin(Tr)),
       fix(sin(Tu))
        };
    V=Vector{
      fix(cos(Tu)*cos(Tr-num(M_PI/2))),
      fix(cos(Tu)*sin(Tr-num(M_PI/2))),
       fix(sin(Tu))
        };
  W=Vector{
       fix(cos(Tu-num(M_PI/2))*cos(Tr)),
        fix(cos(Tu-num(M_PI/2))*sin(Tr)),
      fix(sin(Tu-num(M_PI/2)))
        };
    
    }



int main()
{
    num speed=0.1;
 
    
 Button but[10]={{250,800,300,850,sf::Color::Blue},
                           {300,750,350,800,sf::Color::Red},
                          {350,800,400,850,sf::Color::Green},
                        {300,850,350,900,sf::Color::Yellow},
                           {50,800,100,850,sf::Color::Blue},
                           {100,750,150,800,sf::Color::Red},
                          {150,800,200,850,sf::Color::Green},
                      {100,850,150,900,sf::Color::Yellow},
                     {400,800,450,850,sf::Color::Red},
                        {400,850,450,900,sf::Color::Green}
 };
    
    ofstream out("output.txt");
    bool get;
    Point a,b,c;
 loadFile("cube.txt",{2,2,2},{1,1,0});
loadFile("diamond.txt",{1,1,1},{0,0,0});
loadFile("pyramid.txt",{1,1,1},{-1,-2,0});
loadFile("diamond.txt",{1,1,1},{5,-5,0});
loadFile("diamond.txt",{1,1,1},{5,5,0});
loadFile("diamond.txt",{1,1,1},{-5,-5,0});


    sf::ConvexShape ver;
  
    fori(Triangles.size())sT.pb(i);

 sf::RenderWindow win(sf::VideoMode(500, 1000), "window");

#define sT sortedTriangles
//music.play();
    while (win.isOpen())
    {
        sf::Event ev;
        while (win.pollEvent(ev))
        {}
        forj(2)
            if(sf::Touch::isDown(j)){
                
sf::Vector2i vget1=sf::Touch::getPosition(j);
sf::Vector2f vget=win.mapPixelToCoords(vget1);
        out<<vget.x<<"    "<<vget.y<<endl;
        
        fori(10)
        {if(but[i].pushed(vget.x,vget.y)){
            if(i==0){Tr+=0.01;}else
            if(i==1){Tu+=0.01;}else
            if(i==2){Tr-=0.01;}else
            if(i==3){Tu-=0.01;}else
        if(i==4){
 View=Point{
         View.x-speed*V.a,
         View.y-speed*V.b,
         View.z
         };
         }else
            if(i==5){
 View=Point{
         View.x+speed*U.a,
         View.y+speed*U.b,
         View.z
         };
                }else
            if(i==6){ 
 View=Point{
         View.x+speed*V.a,
         View.y+speed*V.b,
         View.z
         };            
}else
  if(i==7){ 
 View=Point{
         View.x-speed*U.a,
         View.y-speed*U.b,
         View.z
         };
  }
  else if(i==8){
  View=Point{
         View.x,
         View.y,
         View.z-speed*W.c
         };     
      } else if(i==9){
 View=Point{
         View.x,
         View.y,
         View.z+speed*W.c
         };          }
            }}
       }
     
       
sort(sT.begin(),sT.end(),&closer);


        win.clear();
           ver.setPointCount(3);
        fori(sT.size())
        {
            get = 0;
            a = points[Triangles[sT[i]].x];
            b = points[Triangles[sT[i]].y];
            c = points[Triangles[sT[i]].z];
normalV=createVector(a,b).cross(createVector(a,c));
;
                       
                     
                      
           
            a = transform1(View, a);
            a = transform2(a);
     get=(get)||directv.dot(normalV)<=0;
          
            b = transform1(View, b);
            b = transform2(b);
      get=(get)||directv.dot(normalV)<=0;
            c = transform1(View, c);
            c = transform2(c);
 get=(get)||directv.dot(normalV)<=0;

            
 if (( get))
            {
     ver.setPoint(0, sf::Vector2f(a.x, Height - a.y));
     ver.setPoint(1, sf::Vector2f(b.x, Height - b.y));
      ver.setPoint(2, sf::Vector2f(c.x, Height - c.y));
     ver.setFillColor(TriangleColor[sT[i]]);
     win.draw(ver);
        }
        
        }
        ver.setPointCount(4);
        
        fori(10){
  ver.setPoint(0, sf::Vector2f(but[i].x1, but[i].y1));
   ver.setPoint(1, sf::Vector2f(but[i].x2, but[i].y1));
  ver.setPoint(2, sf::Vector2f(but[i].x2, but[i].y2));
   ver.setPoint(3, sf::Vector2f(but[i].x1, but[i].y2));
   ver.setFillColor(but[i].col);
   win.draw(ver);
        }
        win.display();
   
        
        
      //Tu+=0.01;
     //  Tr+=0.01;
     
        setUVW();
   
        //out<<Tr<<"     "<<Tu<<endl;
       // out<<U.a<<"  "<<U.b<<"     "<<U.c<<endl;
       // out<<V.a<<"  "<<V.b<<"     "<<V.c<<endl;
        //out<<W.a<<"  "<<W.b<<"     "<<W.c<<endl;
//fori(points.size()) { rotate_point(0.5, 0.5, 0.01, points[i].x, points[i].y); }

//fori(points.size()) { rotate_point(0.5, 0.5, 0.01, points[i].x, points[i].z); }
//fori(points.size()) { rotate_point(0.5, 0.5, 0.01, points[i].y, points[i].z); }
        
     
        
      //  rotate_point(0, 0, 0.1, U.a, U.c);
     //    rotate_point(0, 0, 0.1, V.a, V.c);

        //rotate_point(0.5,0.5,0.1,View.x,View.y);
    }
}