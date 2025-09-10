#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    string imagePath = "/home/arpit/Downloads/profile-pic.jpeg";
    string window = "Image Viewer";

    // Load the image
    Mat image = imread(imagePath);
    if (image.empty()) {
        cout << "Error: Could not load image from " << imagePath << endl;
        return -1;
    }

    // Start with grayscale image
    Mat currentImage;
    cvtColor(image, currentImage, COLOR_BGR2GRAY);

    cout << "Controls:" << endl;
    cout << "  'b' - Apply blur" << endl;
    cout << "  'e' - Apply edge detection" << endl;
    cout << "  'q' or ESC - Quit" << endl;

    while (true) {
        imshow(window, currentImage);

        char key = waitKey(30);
        if (key == 'b' || key == 'B') {
            // Apply Gaussian blur
            GaussianBlur(currentImage, currentImage, Size(15, 15), 0);
            cout << "Applied blur filter" << endl;
        } 
        else if (key == 'e' || key == 'E') {
            // Apply Canny edge detection
            Mat edges;
            Canny(currentImage, edges, 100, 200);
            currentImage = edges;
            cout << "Applied edge detection" << endl;
        } 
        else if (key == 'q' || key == 27) {
            // Quit on 'q' or ESC
            cout << "Exiting..." << endl;
            break;
        }
    }

    destroyWindow(window);
    return 0;
}
