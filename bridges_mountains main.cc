#include "Bridges.h"
#include "ColorGrid.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <cmath>
#include "tile.h"
#include <queue>
#include <climits>
#include <unordered_set>
using namespace std;
using namespace bridges;

// This program implements the mountain paths assignment in C++. See the README.md for a
// detailed description

// function prototypes
ColorGrid *getImage (int *img_arr, int imgWidth, int imgHeight, int maxVal, const string &filename);
int *readData (int *imgWidth, int *imgHeight, int *maxVal, string file_name);
void findPath(int *img_arr, int imgWidth, int imgHeight, int startRow, int maxVal);


int xyToAddress(int x, int y, int imgWidth) {
    return x + (y * imgWidth);
}

struct Pixel : public Tile {
    double distFromEnd = -1; //straightDistance
    int elevation = -1;

    Pixel(int x, int y, int from_x, int from_y, double distance, int* elev_data, int imgWidth) : Tile(x, y, from_x, from_y, distance) {
        distFromEnd = 0;
        elevation = elev_data[xyToAddress(x, y, imgWidth)];
    }
    Pixel(int x, int y, int from_x, int from_y, double distance, int* elev_data, int imgWidth, Pixel& endPix ) : Tile(x, y, from_x, from_y, distance) {
        distFromEnd = distance + straightDistance(endPix);
        elevation = elev_data[xyToAddress(x, y, imgWidth)];
    }

    bool operator==(const Pixel& rhs) const {
        return (x == rhs.x && y == rhs.y);
    }
    bool operator<(const Pixel& rhs) const {
        return (distance + distFromEnd < rhs.distance + rhs.distFromEnd);
    }

    double straightDistance(Pixel& pixel2) const {
        return sqrt(pow(1840. * (pixel2.x - x), 2.) + pow(1840. * (pixel2.y - y), 2.));
    }

};

struct Pixel_Comp {
    bool operator()(const Pixel& lhs, const Pixel& rhs) {
        return (lhs.distance + lhs.distFromEnd > rhs.distance + rhs.distFromEnd);
    }
};

namespace std {
    template <> struct hash<Pixel> {
        size_t operator()(const Pixel& p) const {
            return 1000 * (size_t)p.x + (size_t)p.y;
        }
    };
}

void betterFindPath(int *elev_data, int imgWidth, int imgHeight, int startRow, int startCol, int endRow, int endCol, int maxVal);


int main(int argc, char **argv) {
    const int bridges_project_id = 9001;
    const string bridges_username = "hi";
    const string bridges_api_key  = "text here";

    //Delete this line once you get registered on BRIDGES
    static_assert(false,"You must register on BRIDGES and get your own username and API key.");

    // bridges object initialization
    Bridges bridges(bridges_project_id, bridges_username, bridges_api_key);

    // read input elevation data
    int width, height, maxVal;
    int *elev_data;

    // parse command line arguments
    // argument 2 is input file
    string filename = "./san_joaquin.dat";
    if (argc >= 2) filename = argv[1]; //This is how you read a command line parameter
    //Elev_data is a pointer to a chunk of memory of size width * height
    //Warning: You are now the owner of a block of memory but this isn't
    // documented anywhere. Look up owning pointers in the GSL --WRK
    elev_data = readData(&width, &height, &maxVal, filename);

    // set title
    //YOU: Change this to "Mountain Paths - Dijkstra's"
    bridges.setTitle("Mountain Paths "s + filename + " - A* Algorithm");


    //WRK - Added this section
    //Get starting row and column - the greedy algorithm only needs starting row,
    // but you will use the two locations for doing Dijkstra's
    int startRow = 0, startCol = 0, endRow = 0, endCol = 0;
    cout << "Please enter starting row and col:\n"; //Only starting row is used for greedy mode
    cin >> startRow >> startCol;
    cout << "Please enter ending row and col:\n"; //Neither of these is used in greedy mode
    cin >> endRow >> endCol;

    //YOU: Replace the greedy algorithm with Dijkstra's or A* between the starting and ending points
    //YOU: Look at how findPath does it's think in regards to converting between a 2D location and a 1D array, as well as how it marks the final path by changing the elevation to -1 along the path
    // find path by applying a greedy algorithm from the left side of the screen to the right
    betterFindPath (elev_data, width, height, startRow, startCol, endRow, endCol, maxVal);

    //cerr << "Test: betterFindPath function ended." << endl; //TEST

    // get the path written into a color grid for visualization
    //YOU: Change the path color to red DONE
    ColorGrid *cg = getImage(elev_data, width, height, maxVal, filename);

    // visualize
    bridges.setDataStructure(cg);
    bridges.visualize();

    delete cg; //WRK Fix memory leak in BRIDGES
    delete[] elev_data; //WRK Ditto
}

int *readData(int *width, int *height, int *maxVal, string file_name) {

    ifstream infile(file_name);

    // read header
    int max = 0, w, h, val;
    infile >> w >> h;

    *width = w; *height = h;

    // allocate space for the input data
    // we will use a 1D array
    int size = w*h;
    int *elev_data = new int[size];
    assert(elev_data); //WRK

    // read the elevation data
    for (int k = 0; k < size; k++) {
        infile >>  val;

        // update the max val
        max = (val > max) ? val : max;

        // store  in elevation array
        elev_data[k] = val;
    }
    // record the max value
    *maxVal = max;

    return elev_data;
}

// takes in the processed elevation data and returns a color grid for
// visualization using BRIDGES
ColorGrid *getImage(int *elev_data, int width, int height, int maxVal, const string &filename)) {
    // create color grid
    ColorGrid *cg = new ColorGrid(height, width);

    float pixel_val, maxBlue = 0.25, maxGreen = 0.5, maxYellow = 0.75;
    int n = 0, gray, blue, green, LYellow;

    if(filename == "./san_joaquin.dat" || filename == "./korea.dat") maxBlue = 0.00009;

    // load the elevation data
    for (int j = 0; j < height; j++)
    for (int k = 0; k < width; k++) {
        pixel_val = (float) elev_data[n++];
        if (pixel_val == INT_MIN) {
            // this is the path drawn in purple(255, 0, 255) across the image Red(255, 8, 0)
            cg->set(j, k, Color(255, 8, 0));
            continue;
        }
        if (pixel_val < 0) {
            pixel_val = 0.0;
        }
        float scale = pixel_val/maxVal;
        if(scale < 0) scale = 0.0;
        if(scale > 1) scale = 1.0;
        //within 0-0.24 blue
        if(scale < maxBlue){
            blue = 128 + (int) (scale/maxBlue * 127);
            cg->set(j, k, Color(0, 0, blue));
        }
        //0.25-0.5 green
        else if(scale < maxGreen){
            green =10 + (int) ((scale - maxBlue)/(maxGreen - maxBlue) * 100);
            int finalGreen = green -30;
            if (finalGreen < 0) finalGreen = 0;
            if (finalGreen > 255) finalGreen = 255;
            cg->set(j, k, Color(finalGreen, green, 25));
        }
        //0.50-0.75 light yellow
        else if(scale < maxYellow){
            LYellow = (int) ((scale - maxGreen)/(maxYellow - maxGreen) * 255);
            int BYellow = LYellow;
            if(BYellow < 0) BYellow = 0;
            if(BYellow < 50) BYellow = BYellow+60;
            if(BYellow < 90) BYellow = BYellow+30;
            if(BYellow < 255 && BYellow > 190) BYellow = BYellow-25;
            if(BYellow > 255) BYellow = 255;
            cg->set(j, k, Color(BYellow,BYellow, 13));//other like
        }
        //0.75-1 white

        else {
            // scale value to be within 0-255, for r,g,b range
            gray = (int) (pixel_val*255./maxVal);
            cg->set(j, k, Color(gray, gray, gray));
        }
    }
    return cg;
}

// determines the least effort path through the mountain starting a point on
// the left edge of the image
void findPath(int *elev_data, int imgWidth, int imgHeight, int startRow, int maxVal) {

    if (startRow < 0 || startRow >= imgHeight) {
        cout << "Invalid starting row.\n"; //WRK Let them know
        return;
    }

    int col = 0, row = startRow;
    int pix_addr = startRow*imgWidth + col;

    while (col < imgWidth-2) {
        // hold neighbors in array
        int neighb[3] = {0, 0, 0};

        // compute 2D address of pixel from 1D address
        row = pix_addr/imgWidth;
        col = pix_addr - row*imgWidth;
        int top_right = pix_addr - imgWidth + 1;
        int right = pix_addr + 1;
        int bottom_right = pix_addr + imgWidth + 1;
        int cnt;

        // color the pixel, its in the path
        elev_data[pix_addr] = -1;

        // deal with boundary cases first
        if (row == 0) {
            // must  go right or down
            cnt = 2;
            neighb[0] = right; neighb[1] = bottom_right;
        }
        else if (row == imgHeight-1){
            // must go right or up
            cnt = 2;
            neighb[0] = right; neighb[1] = top_right;
        }
        else {
            // general case
            // check the 3 right neighbors, make greedy decision
            cnt = 3;
            neighb[0] = top_right; neighb[1] = right; neighb[2] = bottom_right;
        }

        // compute differences with right neighbors
        int min = maxVal;
        int diff[3];
        for (int k = 0; k < cnt; k++)  {
            int d = elev_data[pix_addr] - elev_data[neighb[k]];
            diff[k] = (d < 0) ? -d : d;
        }
        // choose pixe with the smallest difference
        switch (cnt) {
            case 2:
                pix_addr =  (diff[0] <= diff[1]) ? neighb[0] : neighb[1];
                break;
            case 3:
                if (diff[0] < diff[1] && diff[0] < diff[2])
                    pix_addr = neighb[0];
                else
                    if (diff[1] < diff[0] && diff[1] < diff[2])
                        pix_addr = neighb[1];
                else
                    if (diff[2] < diff[0] && diff[2] < diff[1])
                        pix_addr = neighb[2];
                else if ((diff[0] == diff[1]) || (diff[1] == diff[2]))
                        pix_addr = neighb[1];
                else {
                    double val = drand48();
                    pix_addr = (val < 0.5) ? neighb[0] : neighb[2];
            }
        }
        col++;
    }
}


void betterFindPath(int *elev_data, int imgWidth, int imgHeight, int startY, int startX, int endY, int endX, int maxVal) {
    if (startX < 0 || startX >= imgWidth || startY < 0 || startY >= imgHeight || endX < 0 || endX >= imgWidth || endY < 0 || endY >= imgHeight) {
        cout << "Invalid start or end position.\n";
        return;
    }

    priority_queue<Pixel, vector<Pixel>, Pixel_Comp> pixelQueue;
    unordered_set<Pixel> processedList;
    Pixel endPix(endX, endY, -1, -1, INT_MAX, elev_data, imgWidth);
    Pixel startPix(startX, startY, -1, -1, 0, elev_data, imgWidth, endPix);

    //cerr << "Test: Distance to end is " << startPix.distFromEnd << endl; //TEST

    pixelQueue.push(startPix);

    while (pixelQueue.size() > 0 && pixelQueue.top() < endPix) {

        cerr << "Test: Queue size is " << pixelQueue.size() << endl; //TODO: Delete this if it's annoying

        if (pixelQueue.top() == endPix) {
            //cerr << "Test: endPix found." << endl; //TEST
            processedList.insert(pixelQueue.top());  //Unsure if this works; Stops pathfinding when end point is at the top of the queue
            break;
        }

        if (processedList.find(pixelQueue.top()) != processedList.end()) {
            //cerr << "Test: Pixel already processed." << endl; //TEST
            pixelQueue.pop();
            continue;
        }

        int currX = pixelQueue.top().x;
        int currY = pixelQueue.top().y;

        //cerr << "Center pix: " << currX << ' ' << currY << ' ' << pixelQueue.top().from_x << ' ' << pixelQueue.top().from_y << endl; //TEST

        for (int i = -1; i <= 1; i++) {                       //Iterate through 8 surrounding tiles
            if (currX + i < 0 || currX + i >= imgWidth) {
                continue;
            }
            for (int j = -1; j <= 1; j++) {
                int distFactor = 2601;
                if (currY + j < 0 || currY + j >= imgHeight) {
                    continue;
                }
                if (i == 0 && j == 0) {
                    continue;
                }
                if (i == 0 || j == 0) {
                    distFactor = 1840;
                }
                //cerr << "Test: i = " << i << " and j = " << j << endl; //TEST
                //cerr << "Test: Current Pixel is " << currX + i << ' ' << currY + j << endl; //TEST
                if (processedList.find(Pixel(currX + i, currY + j, -1, -1, -1, elev_data, imgWidth)) == processedList.end()) {
                    Pixel tempPix(currX + i, currY + j, currX, currY, -1, elev_data, imgWidth, endPix);
                    if (tempPix.elevation < 0) {
                        continue;                //Avoids negative elevation, like water
                    }
                    tempPix.distance = pixelQueue.top().distance + (abs(pixelQueue.top().elevation - tempPix.elevation) * 20) + distFactor;
                    if (tempPix < endPix) {          //Ensure this could be a shorter path (possibly useless?)
                        //cerr << "Pushing: "; //TEST
                        pixelQueue.push(tempPix);
                        //cerr << "Done." << endl; //TEST
                    }
                }
            }
        }

        //cerr << "Popping: "; //TEST
        processedList.insert(pixelQueue.top());
        pixelQueue.pop();
        //cerr << "Done." << endl; //TEST
    }

    //cerr << "Test: Pixels processed." << endl; //TEST

    if(processedList.find(endPix) == processedList.end()) {
        cout << "End not found, I guess." << endl;
        return;
    }

    Pixel pathPos = *processedList.find(endPix);
    cout << "Distance of path is " << pathPos.distance << "m" << endl;
    while (pathPos.from_x >= 0) {
        elev_data[xyToAddress(pathPos.x, pathPos.y, imgWidth)] = INT_MIN;
        pathPos = *processedList.find(Pixel(pathPos.from_x, pathPos.from_y, -1, -1, -1, elev_data, imgWidth));
    }
    elev_data[xyToAddress(pathPos.x, pathPos.y, imgWidth)] = INT_MIN; //Color starting pixel
}
