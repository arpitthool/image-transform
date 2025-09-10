#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    string imagePath = "/home/arpit/Downloads/profile-pic.jpeg";
    string window = "Image Viewer";

    // Load the original color image
    Mat original = imread(imagePath, IMREAD_COLOR);
    if (original.empty()) {
        cout << "Error: Could not load image from " << imagePath << endl;
        return -1;
    }

    // Current working image starts as the original color image
    Mat currentImage = original.clone();

    cout << "Controls:" << endl;
    cout << "  'b' - Apply blur" << endl;
    cout << "  'g' - Apply grayscale" << endl;
    cout << "  'e' - Apply edge detection" << endl;
    cout << "  'w' - Increase brightness" << endl;
    cout << "  's' - Decrease brightness" << endl;
    cout << "  'd' - Increase contrast" << endl;
    cout << "  'a' - Decrease contrast" << endl;
    cout << "  'r' - Reset to original (color)" << endl;
    cout << "  'q' or ESC - Quit" << endl;

    while (true) {
        imshow(window, currentImage);

        char key = (char)waitKey(30);
        if (key == 'b' || key == 'B') {
            GaussianBlur(currentImage, currentImage, Size(15, 15), 0);
            cout << "Applied blur filter" << endl;
        }
        else if (key == 'g' || key == 'G') {
            if (currentImage.channels() == 3) {
                Mat gray;
                cvtColor(currentImage, gray, COLOR_BGR2GRAY);
                currentImage = gray;
                cout << "Applied grayscale filter" << endl;
            } else {
                cout << "Image is already grayscale" << endl;
            }
        }
        else if (key == 'e' || key == 'E') {
            Mat gray, edges;
            if (currentImage.channels() == 3) {
                cvtColor(currentImage, gray, COLOR_BGR2GRAY);
            } else {
                gray = currentImage;
            }
            Canny(gray, edges, 100, 200);
            currentImage = edges; // single-channel edges
            cout << "Applied edge detection" << endl;
        }
        else if (key == 'w' || key == 'W') {
            currentImage = currentImage + Scalar(30, 30, 30); // brighten
            cout << "Increased brightness" << endl;
        }
        else if (key == 's' || key == 'S') {
            currentImage = currentImage - Scalar(30, 30, 30); // darken
            cout << "Decreased brightness" << endl;
        }
        else if (key == 'd' || key == 'D') {
            currentImage.convertTo(currentImage, -1, 1.2, 0); // increase contrast
            cout << "Increased contrast" << endl;
        }
        else if (key == 'a' || key == 'A') {
            currentImage.convertTo(currentImage, -1, 0.8, 0); // decrease contrast
            cout << "Decreased contrast" << endl;
        }
        else if (key == 'r' || key == 'R') {
            currentImage = original.clone();
            cout << "Reset to original color image" << endl;
        }
        else if (key == 'q' || key == 27) {
            cout << "Exiting..." << endl;
            break;
        }
    }

    destroyWindow(window);
    return 0;
}
