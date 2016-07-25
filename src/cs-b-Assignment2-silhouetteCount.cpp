#include <iostream>
#include "console.h"
#include "gwindow.h" // for GWindow
#include "simpio.h"  // for getLine
#include "vector.h"  // for Vector
#include "queue.h"
#include "gbufferedimage.h"
#include "gobjects.h"
using namespace std;

struct MyPoint{
    bool color = false;
    bool visited = false;
    int x = 0;
    int y = 0;
};

class PointGrid {
private:
    int width=0;
    int height=0;
    MyPoint *data;
public:

    PointGrid(int width, int height) {
        this->width = width;
        this->height = height;
        data = new MyPoint[width * height]();
    }

    ~PointGrid(){
        delete[] data;
    }

    int get_gridHeight(){return width;}
    int get_gridWidth(){return height;}
    MyPoint& get_Point(int x, int y){
        return data[y * width + x];
    }
    bool get_color(int x, int y) {return get_Point(x, y).color;}
    bool get_visited(int x, int y) {return get_Point(x, y).visited;}
    void set_color(int x, int y, bool z) { get_Point(x, y).color = z;}
    void set_visited(int x, int y, bool z) { get_Point(x, y).visited = z;}
    void set_coordinate(int x, int y){
        MyPoint& point = get_Point(x, y);
        point.x = x;
        point.y = y;
    }

};


void binaringImageToGrid(GBufferedImage* img, PointGrid &imageBinaring){
    int imgHaight = img->getHeight();
    int imgWidth = img->getWidth();
    int pixelColorRGB = 0;


    for(int x = 0; x < imgHaight; x++){
        for (int y = 0; y < imgWidth; y++){
            pixelColorRGB = img->getRGB(y,x);
            if ((img->getRed(pixelColorRGB) | img->getGreen(pixelColorRGB) | img->getBlue(pixelColorRGB)) > 128 )
            {
                imageBinaring.set_color(x,y,false);
                imageBinaring.set_coordinate(x,y);
            } else {
                imageBinaring.set_color(x,y,true);
                imageBinaring.set_coordinate(x,y);
            }
            //cout<<imageBinaring.get_color(x,y);
        }
        //cout<<endl;
    }
}

void selectOnePoint(int x, int y, PointGrid &imageBinaring, Queue<MyPoint> &findePoint){
    if (y >= imageBinaring.get_gridWidth()) return;
    if (x >= imageBinaring.get_gridHeight()) return;
    if (x < 0) return;
    if (y < 0) return;
    if (!imageBinaring.get_visited(x,y) && imageBinaring.get_color(x,y)){
        findePoint.enqueue(imageBinaring.get_Point(x,y));
        imageBinaring.set_visited(x, y, true);
    }
}
bool getSilhouetteIsValid(int xLength,
                          int yLength,
                          int imgHeight,
                          int imgWidth){

    if (xLength > (imgHeight / 5)) return true;
    return false;
}
void selectFindeArea(int x,
                     int y,
                     PointGrid &imageBinaring,
                     bool &silhouetteIsValid){

    Queue<MyPoint> findePoint;
    MyPoint tempPoint;
    int xMin, xMax, yMin, yMax;
    xMin = xMax = x;
    yMin = yMax = y;

    findePoint.enqueue(imageBinaring.get_Point(x,y));
    imageBinaring.set_visited(x,y,true);

    while (!findePoint.isEmpty()) {
        tempPoint = findePoint.dequeue();
        x = tempPoint.x;
        y = tempPoint.y;

        if (x < xMin) xMin = x;
        if (x > xMax) xMax = x;
        if (y < yMin) yMin = y;
        if (y > yMax) yMax = y;

        selectOnePoint(x,y-1,imageBinaring, findePoint);
        selectOnePoint(x,y+1,imageBinaring, findePoint);
        selectOnePoint(x+1,y,imageBinaring, findePoint);
        selectOnePoint(x-1,y,imageBinaring, findePoint);

    }

    silhouetteIsValid = getSilhouetteIsValid((xMax - xMin),
                                             (yMax - yMin),
                                             imageBinaring.get_gridHeight(),
                                             imageBinaring.get_gridWidth());
}

void silhouetteCount(string nameImageFile){

    int silhouetteCounter=0;
    GImage* imageFile = new GImage(nameImageFile);
    GBufferedImage* imgageInBuffer = new GBufferedImage(0,0,imageFile->getWidth(), imageFile->getHeight());
    imgageInBuffer->load(nameImageFile);

    int imgHaight = imgageInBuffer->getHeight();
    int imgWidth = imgageInBuffer->getWidth();

    GWindow gw(imgWidth, imgHaight);
    gw.add(imageFile);

    PointGrid imageBinaring(imgHaight, imgWidth);

    binaringImageToGrid(imgageInBuffer, imageBinaring);
    bool silhouetteIsValid = false;
    for(int x = 0; x < imgHaight; x++){
        for (int y = 0; y < imgWidth; y++){
            if (imageBinaring.get_color(x,y) == true && imageBinaring.get_visited(x,y) == false) {
                selectFindeArea(x,y, imageBinaring, silhouetteIsValid);
                if (silhouetteIsValid) silhouetteCounter++;
            }
        }
    }
    cout<<"in File: "<<nameImageFile<<"; found "<< silhouetteCounter<<" silhouettes."<<endl;
    cout<<endl;

    delete imgageInBuffer;
    delete imageFile;

}

int main() {
    string nameImageFile ="";
    Vector<string> testFileNameBase;
    testFileNameBase.push_back("s1_3.jpg");
    testFileNameBase.push_back("s2_11.jpg");
    testFileNameBase.push_back("s3_8.jpg");
    testFileNameBase.push_back("s4_6.jpg");
    testFileNameBase.push_back("s5_10.jpg");
    testFileNameBase.push_back("s6_8.jpg");

    cout<<"For the test mode program enter 1. For standard operation, enter 2."<<endl;
    cin>>nameImageFile;
    if (nameImageFile[0] == '1'){
        for (int i = 0; i < testFileNameBase.size(); i++){
            silhouetteCount(testFileNameBase[i]);
        }

    } else {
        cout<<"Enter the name of the source file: ";
        cin>>nameImageFile;
        silhouetteCount(nameImageFile);
    }

    return 0;
}
