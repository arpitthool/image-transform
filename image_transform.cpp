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
    Mat original;
    original = imread(imagePath, IMREAD_COLOR);
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
    cout << "  'r' - Reset to original (color)" << endl;
    cout << "  'q' or ESC - Quit" << endl;

    while (true) {
        imshow(window, currentImage);

        char key = (char)waitKey(30);
        if (key == 'b' || key == 'B') {
            // Apply Gaussian blur (works on color or grayscale)
            GaussianBlur(currentImage, currentImage, Size(15, 15), 0);
            cout << "Applied blur filter" << endl;
        }
        if (key == 'g' || key == 'G') {
            // Apply Gaussian blur (works on color or grayscale)
            Mat gray;
            cvtColor(currentImage, gray, COLOR_BGR2GRAY);
            currentImage = gray;
            cout << "Applied gray filter" << endl;
        }
        else if (key == 'e' || key == 'E') {
            // Apply Canny edge detection (compute on grayscale)
            Mat gray, edges;
            if (currentImage.channels() == 3) {
                cvtColor(currentImage, gray, COLOR_BGR2GRAY);
            } else {
                gray = currentImage;
            }
            Canny(gray, edges, 100, 200);
            currentImage = edges; // keep edges as single-channel image
            cout << "Applied edge detection" << endl;
        }
        else if (key == 'r' || key == 'R') {
            // Reset to the original color image
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
