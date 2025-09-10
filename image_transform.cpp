#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace cv;

// Show the last operation text on the image
void showTextOnImage(Mat& image, const string& text) {
    if (!text.empty()) {
        putText(image,
                "Last operation: " + text,
                Point(10, 30),
                FONT_HERSHEY_SIMPLEX,
                1.0,
                Scalar(0, 255, 0),
                2);
    }
}

// Function to draw controls overlay on image
void drawControlsOverlay(Mat& image) {
    // Draw semi-transparent background for controls
    rectangle(image, Point(10, image.rows - 200), Point(400, image.rows - 10), Scalar(0, 0, 0), -1);
    rectangle(image, Point(10, image.rows - 200), Point(400, image.rows - 10), Scalar(255, 255, 255), 2);
    
    // Draw control text
    vector<string> controls = {
        "Controls:",
        "g - Grayscale    b - Blur",
        "e - Edge Detection",
        "w - Brightness+  s - Brightness-",
        "d - Contrast+    a - Contrast-",
        "r - Reset        h - Hide/Show", // TODO: implement hide
        "q - Quit"
    };
    
    int y_offset = image.rows - 180;
    for (const string& control : controls) {
        putText(image, control, Point(20, y_offset), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 1);
        y_offset += 25;
    }
}

int main() {
    string imagePath = "/home/arpit/Downloads/profile-pic.jpeg";
    string window = "Image Viewer";

    // Load the original color image
    Mat original = imread(imagePath, IMREAD_COLOR);
    if (original.empty()) {
        cout << "Error: Could not load image from " << imagePath << endl;
        return -1;
    }

    Mat currentImage = original.clone();
    string lastOperation = "";

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
        Mat displayImage = currentImage.clone();

        // Overlays
        showTextOnImage(displayImage, lastOperation);
        drawControlsOverlay(displayImage); // always visible in this commit

        imshow(window, displayImage);

        char key = (char)waitKey(30);
        if (key == 'b' || key == 'B') {
            GaussianBlur(currentImage, currentImage, Size(15, 15), 0);
            lastOperation = "Blur";
            cout << "Applied blur filter" << endl;
        }
        else if (key == 'g' || key == 'G') {
            if (currentImage.channels() == 3) {
                cvtColor(currentImage, currentImage, COLOR_BGR2GRAY);
                lastOperation = "Grayscale";
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
            currentImage = edges;
            lastOperation = "Edge Detection";
            cout << "Applied edge detection" << endl;
        }
        else if (key == 'w' || key == 'W') {
            currentImage = currentImage + Scalar(30, 30, 30);
            lastOperation = "Brightness +";
            cout << "Increased brightness" << endl;
        }
        else if (key == 's' || key == 'S') {
            currentImage = currentImage - Scalar(30, 30, 30);
            lastOperation = "Brightness -";
            cout << "Decreased brightness" << endl;
        }
        else if (key == 'd' || key == 'D') {
            currentImage.convertTo(currentImage, -1, 1.2, 0);
            lastOperation = "Contrast +";
            cout << "Increased contrast" << endl;
        }
        else if (key == 'a' || key == 'A') {
            currentImage.convertTo(currentImage, -1, 0.8, 0);
            lastOperation = "Contrast -";
            cout << "Decreased contrast" << endl;
        }
        else if (key == 'r' || key == 'R') {
            currentImage = original.clone();
            lastOperation = "Reset";
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
