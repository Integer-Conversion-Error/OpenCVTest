#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

//using namespace cv;
//using namespace std;

//int main() {
//
//    string image_path = "/Users/togoo/Downloads/opencvtest.jpg";
//
//    Mat image = imread(image_path, IMREAD_COLOR);
//    resize(image, image, { 500,500 }, 0, 0, cv::INTER_NEAREST);
//
//    imshow("Image", image);
//
//    waitKey(0);
//
//    return 0;
//}

int main() {


    /*if (argc != 2) {
        std::cout << "Usage: ./yellow_bw_contours <image_path>" << std::endl;
        return -1;
    }*/

    // Load the image
    cv::Mat image = cv::imread("/Users/togoo/Downloads/opencvtest.jpg");
    /*if (image.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }*/

    // Get the dimensions of the image
    int height = image.rows;
    int width = image.cols;

    // Crop the image to the bottom half
    cv::Rect bottomHalf(0, height / 2, width, height / 2);
    image = image(bottomHalf);

    // Convert the cropped image to grayscale
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    // Threshold the grayscale image to detect white areas
    cv::Mat bwImage;
    cv::threshold(grayImage, bwImage, 200, 255, cv::THRESH_BINARY);

    // Remove isolated white pixels (those with no adjacent white pixels)
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::erode(bwImage, bwImage, kernel);

    // Find contours only on the white areas
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(bwImage, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // Draw contours on the original image
    cv::Mat contourImage = image.clone();
    cv::drawContours(contourImage, contours, -1, cv::Scalar(0, 0, 255), 2);


    // Filter out smaller contours and process only the larger ones
    for (size_t i = 0; i < contours.size(); i++) {
        double contourLength = cv::arcLength(contours[i], true);
        if (contourLength > 100) {  // Filter threshold for contour length
            // Fit a line to the contour
            cv::Vec4f line;
            cv::fitLine(contours[i], line, cv::DIST_L2, 0, 0.01, 0.01);

            // Calculate the line points
            double vx = line[0], vy = line[1];
            double x = line[2], y = line[3];
            cv::Point point1, point2;

            // Extend the line to cover the whole image width
            point1.x = 0;
            point1.y = y - (x * vy / vx);
            point2.x = width;
            point2.y = y + ((width - x) * vy / vx);

            // Draw the original fitted line
            cv::line(contourImage, point1, point2, cv::Scalar(0, 255, 0), 2);

            //// Draw parallel lines (e.g., offset by 20 pixels)
            //int offset = 20;
            //cv::line(contourImage, point1 + cv::Point(0, offset), point2 + cv::Point(0, offset), cv::Scalar(255, 0, 0), 2);
            //cv::line(contourImage, point1 - cv::Point(0, offset), point2 - cv::Point(0, offset), cv::Scalar(255, 0, 0), 2);
        }
    }
    // Display the results
    cv::imshow("Original Image", image);
    cv::imshow("Black and White Image", bwImage);
    cv::imshow("Contours", contourImage);

    // Wait until user exits the program
    cv::waitKey(0);

    return 0;
}