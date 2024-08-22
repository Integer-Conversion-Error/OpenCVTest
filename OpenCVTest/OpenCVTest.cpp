#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//int main() {
//
//    string image_path = "/Users/togoo/Downloads/opencvtest.jpg";
//
//    Mat image = imread(image_path, IMREAD_COLOR);
//    resize(image, image, { 500,500 }, 0, 0, INTER_NEAREST);
//
//    imshow("Image", image);
//
//    waitKey(0);
//
//    return 0;
//}

int main() {


    /*if (argc != 2) {
        cout << "Usage: ./yellow_bw_contours <image_path>" << endl;
        return -1;
    }*/

    // Load the image
    Mat image = imread("opencvroadtest1.jpg");
    /*if (image.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }*/

    // Get the dimensions of the image
    int height = image.rows;
    int width = image.cols;

    // Crop the image to the bottom half
    Rect bottomHalf(0, height / 2, width, height / 2);
    image = image(bottomHalf);

    // Convert the cropped image to grayscale
    Mat grayImage;
    cvtColor(image, grayImage, COLOR_BGR2GRAY);

    // Apply Gaussian blur to the grayscale image
    Mat blurredImage;
    GaussianBlur(grayImage, blurredImage, Size(5, 5), 0);

    // Threshold the grayscale image to detect white areas
    Mat bwImage;
    threshold(grayImage, bwImage, 200, 255, THRESH_BINARY);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    erode(bwImage, bwImage, kernel);

    // Remove isolated white pixels (those with no adjacent white pixels)
    Mat kernel2 = getStructuringElement(MORPH_RECT, Size(4, 4));
    dilate(bwImage, bwImage, kernel2);

    

    // Find contours only on the white areas
    vector<vector<Point>> contours;
    findContours(bwImage, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    // Draw contours on the original image
    Mat contourImage = image.clone();
    drawContours(contourImage, contours, -1, Scalar(0, 0, 255), 2);

    // Create a mask for filtering contours
    Mat mask = Mat::zeros(bwImage.size(), CV_8UC1);

    // Filter out smaller contours and process only the larger ones
    for (size_t i = 0; i < contours.size(); i++) {
        double contourLength = arcLength(contours[i], true);
        if (contourLength > 100) {  // Filter threshold for contour length
            // Fit a line to the contour
            Vec4f line;
            fitLine(contours[i], line, DIST_L2, 0, 0.01, 0.01);

            // Calculate the line points
            double vx = line[0], vy = line[1];
            double x = line[2], y = line[3];
            Point point1, point2;

            // Extend the line to cover the whole image width
            point1.x = 0;
            point1.y = y - (x * vy / vx);
            point2.x = width;
            point2.y = y + ((width - x) * vy / vx);

            // Draw the original fitted line
            cv::line(contourImage, point1, point2, Scalar(0, 255, 0), 2);

            //// Draw parallel lines (e.g., offset by 20 pixels)
            //int offset = 20;
            //line(contourImage, point1 + Point(0, offset), point2 + Point(0, offset), Scalar(255, 0, 0), 2);
            //line(contourImage, point1 - Point(0, offset), point2 - Point(0, offset), Scalar(255, 0, 0), 2);
        }
    }


    // Display the results
    imshow("Original Image", image);

    imshow("Blurred Image", blurredImage);
    imshow("Dilated Image", bwImage);
    imshow("Filtered Contours", contourImage);
    // Wait until user exits the program
    waitKey(0);

    return 0;
}