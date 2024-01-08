
#include "text_size.hpp"
#include <algorithm>
#include <direct.h>
#include <list>


list<std::string> read_size(const string file_img){
    // Create temporary directory
    const char* cheminTmp = "../icon_sizes/tmp_extracted";
    mkdir(cheminTmp);

    // Load image
    Mat im = imread(file_img);
    if(im.data == nullptr){
        cerr << "Image not found: "<< file_img << endl;
        waitKey(0);
        //system("pause");
        exit(EXIT_FAILURE);
    }

    // Load references to compare
    Mat large = imread("../icon_sizes/references/large.png");
    Mat medium = imread("../icon_sizes/references/medium.png");
    Mat small = imread("../icon_sizes/references/small.png");

    //Create List of the results
    list <std::string> list_res;

    for(int i=0;i<7;i++){
        // Define the Region of Interest (ROI) using a rectangle
        // Width et height : retangle size, x y  : first left point
        Rect roiRect(235, 970+330*i, 230, 80);  // (x, y, width, height)

        // Extract the sub-image based on the defined ROI
        Mat roi = im(roiRect).clone();  // Use clone() to create a separate copy of the ROI
        int nb_line = i+1;
        // Save the extracted sub-image to a file in cmake-build-debug (default) or defined file
        string extracted_size = "../icon_sizes/tmp_extracted/size"+std::to_string(nb_line)+".png";
        imwrite(extracted_size, roi);

        Mat extracted_img = imread(extracted_size);

        // Convert all to NB for comparaison function
        cv::Mat extrNB, largeNB, medNB, smallNB;
        cv::cvtColor(extracted_img, extrNB, cv::COLOR_BGR2GRAY);
        cv::cvtColor(large, largeNB, cv::COLOR_BGR2GRAY);
        cv::cvtColor(medium, medNB, cv::COLOR_BGR2GRAY);
        cv::cvtColor(small, smallNB, cv::COLOR_BGR2GRAY);

        // Calcul the similarity between images and references based on shapes comparaison
        double simLarge = cv::matchShapes(extrNB, largeNB, cv::CONTOURS_MATCH_I1, 0);
        double simMed = cv::matchShapes(extrNB, medNB, cv::CONTOURS_MATCH_I1, 0);
        double simSmall = cv::matchShapes(extrNB, smallNB, cv::CONTOURS_MATCH_I1, 0);

        // Check best similarity (=min of the val)
        double min_sim = std::min(std::min(simLarge, simMed), simSmall);

        // Name path depending on the result
        if(min_sim>0.001){ // No good corresponding size found (pb on the image)
            imwrite("../icon_sizes/labelled_sizes/"+std::to_string(nb_line)+"none.png", roi);
            list_res.push_back("");
        }else {
            if (min_sim == simLarge) {
                imwrite("../icon_sizes/labelled_sizes/" + std::to_string(nb_line) + "large.png", roi);
                list_res.push_back( "large");
            }
            if (min_sim == simMed) {
                imwrite("../icon_sizes/labelled_sizes/" + std::to_string(nb_line) + "medium.png", roi);
                list_res.push_back( "medium");
            }
            if (min_sim == simSmall) {
                imwrite("../icon_sizes/labelled_sizes/" + std::to_string(nb_line) + "small.png", roi);
                list_res.push_back( "small");
            }
        }
    }

    // Delete temporary directory
    for(int i=1;i<8;i++){
        std::string chemin = "../icon_sizes/tmp_extracted/size" + std::to_string(i) + ".png";
        // Convert to char * for remove function
        const char* cheminChar = chemin.c_str();
        remove(cheminChar);
    }
    _rmdir("../icon_sizes/tmp_extracted");

    return list_res;

}
