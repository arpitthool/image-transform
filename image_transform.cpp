#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    string imagePath = "/home/arpit/Downloads/profile-pic.jpeg";
    string window = "Grayscale Viewer";

    // Load the image
    Mat image = imread(imagePath);
    if (image.empty()) {
        cout << "Error: Could not load image from " << imagePath << endl;
        return -1;
    }

    // Convert to grayscale
    Mat grayImage;
    cvtColor(image, grayImage, COLOR_BGR2GRAY);

    // Show the grayscale image
    imshow(window, grayImage);

    // Wait until a key is pressed
    cout << "Press any key to exit..." << endl;
    waitKey(0);

    destroyWindow(window);
    return 0;
}