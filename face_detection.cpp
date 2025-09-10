#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    // VideoCapture is a class that captures video from a camera
    VideoCapture video(0);
    // we can also use a video file by passing the path to the video file
    // VideoCapture video("path/to/video.mp4");

    // check if the camera is opened
    if (!video.isOpened()) {
        cout << "Error: Could not open camera" << endl;
        return -1;
    }
    
    // declare the image
    Mat image;

    // we use the CascadeClassifier class to detect the faces. CascadeClassifier is a class that detects objects in an image 
    // using a cascade of classifiers.
    CascadeClassifier face_cascade;

    // load the face cascade classifier. The xml file is a pre-trained model that is used to detect faces. 
    // It contains a list of features that are used to detect faces.
    face_cascade.load("../haarcascade_frontalface_default.xml");

    // we initialize a vector of Rects to store the faces
    vector<Rect> faces;

    while (true) {
        // read the image from the video
        video.read(image);

        // detect the faces in the image. Here, 1.3 is the scale factor and 5 is the minNeighbors.
        // scaleFactor is the factor by which the image is scaled down at each level of the cascade.
        // minNeighbors is the minimum number of neighbors a candidate rectangle should have to be considered a face.
        face_cascade.detectMultiScale(image, faces, 1.3, 5);

        // draw the rectangle around the faces in the image
        for (Rect face : faces) {
            rectangle(image, face.tl(), face.br(), Scalar(0, 255, 0), 2);
        }

        // draw the rectangle at the top left corner of the image
        rectangle(image, Point(10, 0), Point(300, 80), Scalar(200, 50, 50), FILLED);

        // show the number of faces detected on the image
        string faces_detected =  to_string(faces.size()) + ( faces.size() == 1 ? " face detected" : " faces detected");
        putText(image, faces_detected , Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

        // press ESC or 'q' to quit
        putText(image, "(Press ESC or Q to quit)", Point(10, 60), FONT_HERSHEY_SIMPLEX , 0.7, Scalar(0, 255, 0), 2);

        // show the image
        imshow("Face Detection", image);

        
        // wait for 20ms and check if a key is pressed
        char key = waitKey(20);
        
        // Exit if 'q' is pressed or window is closed
        if (key == 'q' || key == 27) { // 27 is ESC key
            cout << "Exiting..." << endl;
            break;
        }
    }

    return 0;
}