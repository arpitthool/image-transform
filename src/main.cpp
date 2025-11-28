#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <ctime>

using namespace std;
using namespace cv;

// Function to display text on image for a specified duration
void showTextOnImage(Mat& image, const string text) {
    if (!text.empty()) {
        putText(image, "Last operation: " + text, Point(10, 30),
                FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
    }
}

// Function to draw controls overlay on image
void drawControlsOverlay(Mat& image) {
    // Draw semi-transparent background for controls
    rectangle(image, Point(10, image.rows - 220), Point(400, image.rows - 10), Scalar(0, 0, 0), -1);
    rectangle(image, Point(10, image.rows - 220), Point(400, image.rows - 10), Scalar(255, 255, 255), 2);

    // Draw control text
    vector<string> controls = {
        "Controls:",
        "g - Grayscale    b - Blur",
        "e - Edge Detection",
        "w - Brightness+  s - Brightness-",
        "d - Contrast+    a - Contrast-",
        "t - Rotate Clockwise (10°) r - Reset",
        "v - Save Image   h - Hide/Show",
        "q - Quit"
    };

    int y_offset = image.rows - 200;
    for (const string& control : controls) {
        putText(image, control, Point(20, y_offset), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 1);
        y_offset += 25;
    }
}

// Function to convert image to grayscale
Mat convertToGrayscale(Mat image) {
    if (image.channels() == 3) {
        Mat gray;
        cvtColor(image, gray, COLOR_BGR2GRAY);
        cout << "Applied grayscale filter" << endl;
        return gray;
    } else {
        cout << "Image is already grayscale" << endl;
        return image;
    }
}

Mat convertToGaussianFilter(Mat image) {
    Mat blur;
    GaussianBlur(image, blur, Size(15, 15), 0);
    cout << "Applied blur filter" << endl;
    return blur;
}

// Function to rotate image by custom angle
Mat rotateImage(Mat image, double angle) {
    Point2f center(image.cols / 2.0, image.rows / 2.0);
    Mat rotationMatrix = getRotationMatrix2D(center, angle, 1.0);
    
    // Calculate bounding box
    // Rect2f bbox = RotatedRect(Point2f(), image.size(), angle).boundingRect2f();
    /**
    . Original image center
    Original center X: image.cols / 2.0
    Original center Y: image.rows / 2.0
    2. New bounding box center
    New center X: bbox.width / 2.0
    New center Y: bbox.height / 2.0
    3. Offset needed
    X offset: bbox.width / 2.0 - image.cols / 2.0
    Y offset: bbox.height / 2.0 - image.rows / 2.0
    */
    // rotationMatrix.at<double>(0, 2) += bbox.width / 2.0 - image.cols / 2.0;
    // rotationMatrix.at<double>(1, 2) += bbox.height / 2.0 - image.rows / 2.0;
    
    Mat rotated;
    warpAffine(image, rotated, rotationMatrix, image.size());
    return rotated;
}

int main() {
    string imagePath = "../selfie.png";
    string window = "Image Viewer";

    Mat image = imread(imagePath);
    if (image.empty()) {
        cout << "Error: Could not load image from " << imagePath << endl;
        return -1;
    }

    Mat currentImage = image.clone();   // Keep original image
    string lastOperation = "";          // Track last operation
    bool showControls = true;           // Toggle for controls display
    
    // Extract file extension from original image path
    string originalExtension = "";
    size_t dotPos = imagePath.find_last_of(".");
    if (dotPos != string::npos) {
        originalExtension = imagePath.substr(dotPos);
    } else {
        originalExtension = ".png"; // Default fallback
    }

    while (true) {
        Mat displayImage = currentImage.clone();

        // Show operation text on image
        showTextOnImage(displayImage, lastOperation);

        // Draw controls overlay if enabled
        if (showControls) {
            drawControlsOverlay(displayImage);
        }

        imshow(window, displayImage);

        // IMPORTANT: read as int and mask to ASCII range
        int key = waitKey(30) & 0xFF;

        if (key == 'g' || key == 'G') {
            // Convert to grayscale
            currentImage = convertToGrayscale(currentImage);
            lastOperation = "Grayscale";
        }
        else if (key == 'b' || key == 'B') {
            // Apply blur
            convertToGaussianFilter(currentImage);
            lastOperation = "Blur";
        }
        else if (key == 'e' || key == 'E') {
            // Apply edge detection
            Mat gray;
            if (currentImage.channels() == 3) {
                cvtColor(currentImage, gray, COLOR_BGR2GRAY);
            } else {
                gray = currentImage.clone();
            }
            Canny(gray, currentImage, 100, 200);
            lastOperation = "Edge Detection";
            cout << "Applied edge detection" << endl;
        }
        else if (key == 'r' || key == 'R') {
            // Reset to original
            currentImage = image.clone();
            lastOperation = "Reset";
            cout << "Reset to original image" << endl;
        }
        else if (key == 'h' || key == 'H') {
            // Toggle controls display
            showControls = !showControls;
            cout << "Controls " << (showControls ? "shown" : "hidden") << endl;
        }
        else if (key == 'w' || key == 'W') {
            // Increase brightness
            currentImage = currentImage + Scalar(30, 30, 30);
            lastOperation = "Brightness +";
            cout << "Increased brightness" << endl;
        }
        else if (key == 's' || key == 'S') {
            // Decrease brightness
            currentImage = currentImage - Scalar(30, 30, 30);
            lastOperation = "Brightness -";
            cout << "Decreased brightness" << endl;
        }
        else if (key == 'd' || key == 'D') {
            // Increase contrast
            currentImage.convertTo(currentImage, -1, 1.2, 0);
            lastOperation = "Contrast +";
            cout << "Increased contrast" << endl;
        }
        else if (key == 'a' || key == 'A') {
            // Decrease contrast
            currentImage.convertTo(currentImage, -1, 0.8, 0);
            lastOperation = "Contrast -";
            cout << "Decreased contrast" << endl;
        }
        else if (key == 't' || key == 'T') {
            // Rotate by custom angle
            double angle = 10;
            currentImage = rotateImage(currentImage, angle);
            lastOperation = "Rotate " + to_string((int)angle) + "°";
            cout << "Rotated image by " << angle << "°" << endl;
        }
        else if (key == 'v' || key == 'V') {
            // Save current image with original extension
            string filename = "image_transform_" + to_string(time(nullptr)) + originalExtension;
            bool success = imwrite(filename, currentImage);
            if (success) {
                lastOperation = "Image saved in build folder: " + filename;
                cout << "Image saved as: " << filename << endl;
            } else {
                lastOperation = "Save failed";
                cout << "Failed to save image!" << endl;
            }
        }
        else if (key == 'q' || key == 27) { // 'q' or ESC to quit
            cout << "Exiting..." << endl;
            break;
        }
        // if key == -1 (no key), loop continues
    }

    destroyWindow(window);
    return 0;
}
