#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>

using namespace std;
using namespace cv;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
    rectangle(image, Point(10, image.rows - 280), Point(450, image.rows - 10), Scalar(0, 0, 0), -1);
    rectangle(image, Point(10, image.rows - 280), Point(450, image.rows - 10), Scalar(255, 255, 255), 2);

    // Draw control text
    vector<string> controls = {
        "Controls:",
        "g - Grayscale    b - Blur",
        "e - Edge Detection",
        "w - Brightness+  s - Brightness-",
        "d - Contrast+    a - Contrast-",
        "x - Rotate 3D X-axis  y - Rotate 3D Y-axis",
        "z - Rotate 3D Z-axis (30°)",
        "1 - Animate X  2 - Animate Y  3 - Animate Z",
        "v - Save Image   h - Hide/Show",
        "q - Quit"
    };

    int y_offset = image.rows - 260;
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

// Function to perform 3D rotation around specified axis
// axis: 'x', 'y', or 'z' for rotation around respective axis
// angle: rotation angle in degrees
// depth: depth factor for 3D effect (0.0 to 1.0, higher = more 3D effect)
Mat rotate3D(Mat image, char axis, double angle, double depth = 0.3) {
    int width = image.cols;
    int height = image.rows;
    
    // Convert angle to radians
    double angleRad = angle * M_PI / 180.0;
    
    // Define the four corners of the image in 3D space
    // We'll treat the image as a plane in 3D space
    vector<Point3f> srcPoints3D;
    srcPoints3D.push_back(Point3f(0, 0, 0));           // Top-left
    srcPoints3D.push_back(Point3f(width, 0, 0));       // Top-right
    srcPoints3D.push_back(Point3f(width, height, 0));  // Bottom-right
    srcPoints3D.push_back(Point3f(0, height, 0));      // Bottom-left
    
    // Create rotation matrix based on axis
    Mat rotationMatrix3D = Mat::eye(3, 3, CV_64F); 
    
    if (axis == 'x' || axis == 'X') {
        // Rotation around X-axis (pitch)
        rotationMatrix3D.at<double>(1, 1) = cos(angleRad);
        rotationMatrix3D.at<double>(1, 2) = -sin(angleRad);
        rotationMatrix3D.at<double>(2, 1) = sin(angleRad);
        rotationMatrix3D.at<double>(2, 2) = cos(angleRad);
    } else if (axis == 'y' || axis == 'Y') {
        // Rotation around Y-axis (yaw)
        rotationMatrix3D.at<double>(0, 0) = cos(angleRad);
        rotationMatrix3D.at<double>(0, 2) = sin(angleRad);
        rotationMatrix3D.at<double>(2, 0) = -sin(angleRad);
        rotationMatrix3D.at<double>(2, 2) = cos(angleRad);
    } else if (axis == 'z' || axis == 'Z') {
        // Rotation around Z-axis (roll) - same as 2D rotation
        rotationMatrix3D.at<double>(0, 0) = cos(angleRad);
        rotationMatrix3D.at<double>(0, 1) = -sin(angleRad);
        rotationMatrix3D.at<double>(1, 0) = sin(angleRad);
        rotationMatrix3D.at<double>(1, 1) = cos(angleRad);
    }
    
    // Apply rotation to 3D points
    vector<Point3f> dstPoints3D;
    for (const Point3f& pt : srcPoints3D) {
        Mat ptMat = (Mat_<double>(3, 1) << pt.x - width/2.0, pt.y - height/2.0, pt.z);
        Mat rotatedPt = rotationMatrix3D * ptMat;
        
        // Add depth effect for x and y rotations
        if (axis == 'x' || axis == 'X' || axis == 'y' || axis == 'Y') {
            rotatedPt.at<double>(2, 0) += depth * width * abs(sin(angleRad));
        }
        
        dstPoints3D.push_back(Point3f(
            rotatedPt.at<double>(0, 0) + width/2.0,
            rotatedPt.at<double>(1, 0) + height/2.0,
            rotatedPt.at<double>(2, 0)
        ));
    }
    
    // Project 3D points to 2D using perspective projection
    // Camera parameters for perspective projection
    double focalLength = width * 1.5; // Adjust for perspective effect
    vector<Point2f> srcPoints2D, dstPoints2D;
    
    srcPoints2D.push_back(Point2f(0, 0));
    srcPoints2D.push_back(Point2f(width, 0));
    srcPoints2D.push_back(Point2f(width, height));
    srcPoints2D.push_back(Point2f(0, height));
    
    for (const Point3f& pt3D : dstPoints3D) {
        // Perspective projection: x' = (x * f) / (z + f), y' = (y * f) / (z + f)
        double z = pt3D.z + focalLength;
        if (z > 0.1) { // Avoid division by zero
            double x2D = (pt3D.x * focalLength) / z;
            double y2D = (pt3D.y * focalLength) / z;
            dstPoints2D.push_back(Point2f(x2D, y2D));
        } else {
            dstPoints2D.push_back(Point2f(pt3D.x, pt3D.y));
        }
    }
    
    // Calculate homography matrix
    Mat homography = getPerspectiveTransform(srcPoints2D, dstPoints2D);
    
    // Calculate bounding box of transformed image
    vector<Point2f> corners;
    corners.push_back(Point2f(0, 0));
    corners.push_back(Point2f(width, 0));
    corners.push_back(Point2f(width, height));
    corners.push_back(Point2f(0, height));
    
    vector<Point2f> transformedCorners;
    perspectiveTransform(corners, transformedCorners, homography);
    
    // Find bounding rect
    Rect bbox = boundingRect(transformedCorners);
    
    // Adjust homography to fit in original image size
    Mat translation = (Mat_<double>(3, 3) << 
        1, 0, -bbox.x + (width - bbox.width) / 2.0,
        0, 1, -bbox.y + (height - bbox.height) / 2.0,
        0, 0, 1);
    
    Mat adjustedHomography = translation * homography;
    
    // Apply perspective transformation
    Mat result;
    warpPerspective(image, result, adjustedHomography, image.size(), 
                   INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
    
    return result;
}

// Function to animate 3D rotation
void animate3DRotation(Mat& currentImage, Mat originalImage, char axis, 
                       double startAngle, double endAngle, int frames = 60) {
    for (int i = 0; i <= frames; i++) {
        double progress = (double)i / frames;
        double currentAngle = startAngle + (endAngle - startAngle) * progress;
        
        currentImage = rotate3D(originalImage, axis, currentAngle);
        
        Mat displayImage = currentImage.clone();
        string opText = "3D Rotate " + string(1, toupper(axis)) + " " + 
                       to_string((int)currentAngle) + "°";
        showTextOnImage(displayImage, opText);
        
        imshow("Image Viewer", displayImage);
        waitKey(30);
    }
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
    Mat originalImage = image.clone();  // Keep original for 3D rotations
    string lastOperation = "";          // Track last operation
    bool showControls = true;           // Toggle for controls display
    double current3DAngleX = 0.0;       // Current 3D rotation angles
    double current3DAngleY = 0.0;
    double current3DAngleZ = 0.0;
    
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
            originalImage = image.clone();
            current3DAngleX = 0.0;
            current3DAngleY = 0.0;
            current3DAngleZ = 0.0;
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
        else if (key == 'x' || key == 'X') {
            // Rotate 3D around X-axis
            current3DAngleX += 30.0;
            // we use the original image (originalImage) as input to avoid cumulative distortion from repeated rotations.
            currentImage = rotate3D(originalImage, 'x', current3DAngleX);
            lastOperation = "3D Rotate X " + to_string((int)current3DAngleX) + "°";
            cout << "3D Rotated around X-axis by " << current3DAngleX << "°" << endl;
        }
        else if (key == 'y' || key == 'Y') {
            // Rotate 3D around Y-axis
            current3DAngleY += 30.0;
            // we use the original image (originalImage) as input to avoid cumulative distortion from repeated rotations.
            currentImage = rotate3D(originalImage, 'y', current3DAngleY);
            lastOperation = "3D Rotate Y " + to_string((int)current3DAngleY) + "°";
            cout << "3D Rotated around Y-axis by " << current3DAngleY << "°" << endl;
        }
        else if (key == 'z' || key == 'Z') {
            // Rotate 3D around Z-axis (30 degrees as requested)
            current3DAngleZ += 30.0;
            // we use the original image (originalImage) as input to avoid cumulative distortion from repeated rotations.
            currentImage = rotate3D(originalImage, 'z', current3DAngleZ);
            lastOperation = "3D Rotate Z " + to_string((int)current3DAngleZ) + "°";
            cout << "3D Rotated around Z-axis by " << current3DAngleZ << "°" << endl;
        }
        else if (key == '1') {
            // Animate 3D rotation around X-axis
            double startAngle = current3DAngleX;
            double endAngle = current3DAngleX + 360.0;
            // we use the original image (originalImage) as input to avoid cumulative distortion from repeated rotations.
            animate3DRotation(currentImage, originalImage, 'x', startAngle, endAngle);
            current3DAngleX = fmod(current3DAngleX + 360.0, 360.0);
            lastOperation = "Animated 3D Rotate X";
            cout << "Animated 3D rotation around X-axis" << endl;
        }
        else if (key == '2') {
            // Animate 3D rotation around Y-axis
            double startAngle = current3DAngleY;
            double endAngle = current3DAngleY + 360.0;
            // we use the original image (originalImage) as input to avoid cumulative distortion from repeated rotations.
            animate3DRotation(currentImage, originalImage, 'y', startAngle, endAngle);
            current3DAngleY = fmod(current3DAngleY + 360.0, 360.0);
            lastOperation = "Animated 3D Rotate Y";
            cout << "Animated 3D rotation around Y-axis" << endl;
        }
        else if (key == '3') {
            // Animate 3D rotation around Z-axis
            double startAngle = current3DAngleZ;
            double endAngle = current3DAngleZ + 360.0;
            // we use the original image (originalImage) as input to avoid cumulative distortion from repeated rotations.
            animate3DRotation(currentImage, originalImage, 'z', startAngle, endAngle);
            current3DAngleZ = fmod(current3DAngleZ + 360.0, 360.0);
            lastOperation = "Animated 3D Rotate Z";
            cout << "Animated 3D rotation around Z-axis" << endl;
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
