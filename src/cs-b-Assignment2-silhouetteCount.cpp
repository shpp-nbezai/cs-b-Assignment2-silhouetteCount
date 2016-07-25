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

    int getHeight(){return width;}
    int getWidth(){return height;}
    MyPoint* getPoint(int x, int y){
        return data + getPointIndex(x, y);
    }
    MyPoint* getPoint(size_t index){
        return data + index;
    }
    void getPointPosition(size_t index, int& x, int& y){
        x = index % width;
        y = index / width;
    }
    size_t getPointIndex(int x, int y){ return y * width + x; }

};


void loadPointGrid(GBufferedImage* img, PointGrid &pointGrid){
    int imgHeight = img->getHeight();
    int imgWidth = img->getWidth();
    int pixelColorRGB = 0;

    MyPoint* point;
    for(int x = 0; x < imgHeight; x++){
        for (int y = 0; y < imgWidth; y++){
            pixelColorRGB = img->getRGB(y,x);
            point = pointGrid.getPoint(x, y);
            if ((img->getRed(pixelColorRGB) | img->getGreen(pixelColorRGB) | img->getBlue(pixelColorRGB)) > 128 )
            {
                point->color = false;
            } else {
                point->color = true;
            }
        }
    }
}

void enqueuePoint(int x, int y, PointGrid &pointGrid, Queue<size_t> &processQueue){
    if (x < 0 || x >= pointGrid.getHeight())
        return;
    if (y < 0 || y >= pointGrid.getWidth())
        return;

    MyPoint* point = pointGrid.getPoint(x, y);
    if (!point->visited && point->color){
        processQueue.enqueue(pointGrid.getPointIndex(x, y));
        point->visited = true;
    }
}
bool isSilhouette(int xLength, int yLength, int imgHeight, int imgWidth){
    return xLength > (imgHeight / 5);
}
void selectFindeArea(int x,
                     int y,
                     PointGrid &pointGrid,
                     bool &silhouetteIsValid){

    Queue<size_t> processQueue;
    size_t pointIndex;
    int pointX, pointY;
    int xMin, xMax, yMin, yMax;
    xMin = xMax = x;
    yMin = yMax = y;

    processQueue.enqueue(pointGrid.getPointIndex(x, y));
    pointGrid.getPoint(x, y)->visited = true;

    while (!processQueue.isEmpty()) {
        pointIndex = processQueue.dequeue();
        pointGrid.getPointPosition(pointIndex, pointX, pointY);

        if (pointX < xMin) xMin = pointX;
        if (pointX > xMax) xMax = pointX;
        if (pointY < yMin) yMin = pointY;
        if (pointY > yMax) yMax = pointY;

        enqueuePoint(pointX, pointY-1, pointGrid, processQueue);
        enqueuePoint(pointX, pointY+1, pointGrid, processQueue);
        enqueuePoint(pointX+1, pointY, pointGrid, processQueue);
        enqueuePoint(pointX-1, pointY, pointGrid, processQueue);

    }

    silhouetteIsValid = isSilhouette((xMax - xMin),
                                     (yMax - yMin),
                                     pointGrid.getHeight(),
                                     pointGrid.getWidth());
}

void silhouetteCount(string imageFileName){

    int nSilhouette=0;

    GImage imageFile(imageFileName);

    GBufferedImage image(0,0,imageFile.getWidth(), imageFile.getHeight());
    image.load(imageFileName);

    int imgHeight = image.getHeight();
    int imgWidth = image.getWidth();

    GWindow gw(imgWidth, imgHeight);
    gw.add(&imageFile);

    PointGrid pointGrid(imgHeight, imgWidth);

    loadPointGrid(&image, pointGrid);

    bool silhouetteIsValid = false;

    for(int x = 0; x < imgHeight; x++){
        for (int y = 0; y < imgWidth; y++){
            MyPoint* point = pointGrid.getPoint(x, y);
            if (point->color && !point->visited) {
                selectFindeArea(x,y, pointGrid, silhouetteIsValid);

                if (silhouetteIsValid)
                    nSilhouette++;
            }
        }
    }
    cout<<"in File: "<<imageFileName<<"; found "<< nSilhouette <<" silhouettes."<<endl;
    cout<<endl;
}

int main() {
    string imageFileName ="";
    Vector<string> testFileNameBase;
    testFileNameBase.push_back("s1_3.jpg");
    testFileNameBase.push_back("s2_11.jpg");
    testFileNameBase.push_back("s3_8.jpg");
    testFileNameBase.push_back("s4_6.jpg");
    testFileNameBase.push_back("s5_10.jpg");
    testFileNameBase.push_back("s6_8.jpg");

    cout<<"For the test mode program enter 1. For standard operation, enter 2."<<endl;
    cin>>imageFileName;
    if (imageFileName[0] == '1'){
        for (int i = 0; i < testFileNameBase.size(); i++){
            silhouetteCount(testFileNameBase[i]);
        }

    } else {
        cout<<"Enter the name of the source file: ";
        cin>>imageFileName;
        silhouetteCount(imageFileName);
    }

    return 0;
}
