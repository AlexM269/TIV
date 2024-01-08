#include "icon_label.hpp"
#include <algorithm>
#include <direct.h>
#include <vector>
#include <list>

list<std::string> reco_label(const string file_img){
    // Create temporary directory
    const char* cheminTmp = "../icon_label/tmp_extracted";
    mkdir(cheminTmp);

    // Load image
    Mat im = imread(file_img);
    if(im.data == nullptr){
        cerr << "Image not found: "<< file_img << endl;
        waitKey(0);
        //system("pause");
        exit(EXIT_FAILURE);
    }

    //Création de la liste des résultats
    list <std::string> list_res;

    // Load references to compare
    // Base path
    const std::string cheminDossier = "../icon_label/references/";
    // File name of images
    const std::vector<std::string> nomsFichiers = {
            "accident.png", "bomb.png", "car.png", "casualty.png", "electricity.png",
            "fire.png", "fire_brigade.png", "flood.png", "gas.png", "injury.png",
            "paramedics.png", "person.png", "police.png", "road_block.png"
    };
    std::vector<cv::Mat> references;
    for (const auto& nomFichier : nomsFichiers) {
        // Complete path
        const std::string cheminComplet = cheminDossier + nomFichier;
        // Load image
        cv::Mat image = cv::imread(cheminComplet);
        // Convert to NB for comparaison (later)
        cv::Mat imgNB;
        cv::cvtColor(image, imgNB, cv::COLOR_BGR2GRAY);
        cv::Mat dilr;
        cv::erode(imgNB, dilr, cv::Mat(), cv::Point(-1, -1), 3);
        cv::Mat dilr2;
        cv::dilate(dilr, dilr2, cv::Mat(), cv::Point(-1, -1), 2);
        cv::Mat seuilImage = dilr2.clone();
        cv::MatIterator_<uchar> it, end;
        cv::GaussianBlur(imgNB, imgNB, cv::Size(7, 7), 0);
        for (it = seuilImage.begin<uchar>(), end = seuilImage.end<uchar>(); it != end; ++it) {
            *it = (*it >= 200) ? 255 : 0;
        }
        imwrite("../icon_label/labelled_draw/ref.png", seuilImage);
        references.push_back(seuilImage);
    }

    for(int i=0;i<7;i++){
        // Define the Region of Interest (ROI) using a rectangle
        // Width et height : rectangle size, x y  : first left point
        Rect roiRect(285, 825+329*i, 150, 150);  // (x, y, width, height)

        // Extract the sub-image based on the defined ROI
        Mat roi = im(roiRect).clone();  // Use clone() to create a separate copy of the ROI
        int nb_line = i+1;
        // Save the extracted sub-image to a file in cmake-build-debug (default) or path
        string extracted_lab = "../icon_label/tmp_extracted/label"+std::to_string(nb_line)+".png";
        imwrite(extracted_lab, roi);

        Mat extracted_img = imread(extracted_lab);
        // Convert to NB for comparaison
        // Erode, dilate & apply a Gaussian blur on the image
        cv::Mat extrNB;
        cv::cvtColor(extracted_img, extrNB, cv::COLOR_BGR2GRAY);
        cv::Mat dil;
        cv::erode(extrNB, dil, cv::Mat(), cv::Point(-1, -1), 3);
        cv::Mat dilr2;
        cv::dilate(dil, dilr2, cv::Mat(), cv::Point(-1, -1), 2);
        cv::Mat seuilImage = dilr2.clone();
        cv::GaussianBlur(seuilImage, seuilImage, cv::Size(7, 7), 0);

        cv::MatIterator_<uchar> it, end;
        for (it = seuilImage.begin<uchar>(), end = seuilImage.end<uchar>(); it != end; ++it) {
            *it = (*it >= 200) ? 255 : 0;
        }

        // Calculate the similarity between images and references based on shapes comparaison
        double simLab = 10000.0;
        string res ;
        for(int j=1;j<15;j++){
            double newSim = cv::matchShapes(seuilImage, references[j-1], cv::CONTOURS_MATCH_I1, 0);
            if(simLab>newSim){
                simLab = newSim; // Take min value (best correspondance)
                res = nomsFichiers[j-1].substr(0,nomsFichiers[j-1].length() - 4);
            }
        }
        if(simLab>0.001){ // No good correspondance found
            res = "none";
        }
        //Add the result of the comparaison to be used
        list_res.push_back(res);
    }

    // Delete temporary directory
    for(int i=1;i<8;i++){
        std::string chemin = "../icon_label/tmp_extracted/label" + std::to_string(i) + ".png";
        // Convert to char * for remove function
        const char* cheminChar = chemin.c_str();
        remove(cheminChar);
    }
    _rmdir("../icon_label/tmp_extracted");

    return list_res;
}