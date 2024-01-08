#include "redresser_image.h"
#include <iostream>

using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <vector>


using namespace cv;

void redresser_image(const std::string& filename,const std::string& nom) {

    //Charge l'image à redresser
    cv::Mat image = cv::imread(filename);
    if (image.empty()) {
        std::cerr << "Erreur lors du chargement de l'image." << std::endl;
    }

    // Charge les modèles
    cv::Mat modele = cv::imread("../model_cross.png");
    if (modele.empty()) {
        std::cerr << "Erreur lors du chargement du modèle." << std::endl;
    }
    cv::Mat modele2 = cv::imread("../model_cross_2.png");
    if (modele.empty()) {
        std::cerr << "Erreur lors du chargement du modèle." << std::endl;
    }
    cv::Mat bord = cv::imread("../bord.png");
    if (modele.empty()) {
        std::cerr << "Erreur lors du chargement du modèle." << std::endl;
    }

    // Applique le matching de modèle
    cv::Mat resultat;
    cv::Mat resultat2;
    cv::matchTemplate(image, modele, resultat, cv::TM_CCOEFF_NORMED);
    cv::matchTemplate(image, bord, resultat2, cv::TM_CCOEFF_NORMED);

    // Définit un seuil pour filtrer les correspondances
    double seuil = 0.92;
    double seuil2 = 0.994;

    // Trouve les positions des correspondances supérieures au seuil
    std::vector<cv::Point> positions;
    while (true) {
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(resultat, &minVal, &maxVal, &minLoc, &maxLoc);

        if (positions.size()>1) {
            break;
        }

        // Si la correspondance est supérieure au seuil, ajoute la position et supprime la région détectée
        if (maxVal >= seuil) {
            positions.push_back(cv::Point(maxLoc.x + modele.cols / 2, maxLoc.y + modele.rows / 2));

            // Met à jour la région détectée avec une valeur inférieure au seuil
            cv::floodFill(resultat, maxLoc, cv::Scalar(0), nullptr, cv::Scalar(), cv::Scalar(), 8);
        }else{
        // Sort de la boucle si aucune correspondance supérieure au seuil n'est trouvée
            break;  
        }
    }
    cv::matchTemplate(image, modele2, resultat, cv::TM_CCOEFF_NORMED);

    if(positions.size()<1){
        positions.clear();
        while (true) {
            double minVal, maxVal;
            cv::Point minLoc, maxLoc;
            cv::minMaxLoc(resultat, &minVal, &maxVal, &minLoc, &maxLoc);

            if (positions.size()>1) {
                break;
            }
            // Si la correspondance est supérieure au seuil, ajoute la position et supprimer la région détectée
            if (maxVal >= seuil) {
                positions.push_back(cv::Point(maxLoc.x + modele.cols / 2, maxLoc.y + modele.rows / 2));

                // Mettre à jour la région détectée avec une valeur inférieure au seuil
                cv::floodFill(resultat, maxLoc, cv::Scalar(0), nullptr, cv::Scalar(), cv::Scalar(), 8);
            }else{
            // Sort de la boucle si aucune correspondance supérieure au seuil n'est trouvée
                break;  
            }
        }
    }

    int j=1;
    while (j) {
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(resultat2, &minVal, &maxVal, &minLoc, &maxLoc);
        if(positions.size()>2){
            break;
        }

        // Si la correspondance est supérieure au seuil, ajoute la position et supprime la région détectée
        if (maxVal >= seuil2) {
            positions.push_back(cv::Point(maxLoc.x + modele.cols / 2, maxLoc.y + modele.rows / 2));

            // Met à jour la région détectée avec une valeur inférieure au seuil
            cv::floodFill(resultat2, maxLoc, cv::Scalar(0), nullptr, cv::Scalar(), cv::Scalar(), 8);
        } else {
        // Sort de la boucle si aucune correspondance supérieure au seuil n'est trouvée
            break;  
        }
        j=0;
    }

    // Définit les points sources (centre des zones de matchTemplate)
    cv::Point2f pts1[3];
    int i=0;
    for (const auto& position : positions) {
        pts1[i]=position;
        i++;
    }

    if(pts1[0].x<2000){
        cv::Point2f temp = pts1[0];
        pts1[0]=pts1[1];
        pts1[1]=temp;
    }

    // Définit les points destinations en fonction de l'image de référence
    cv::Point2f pts2[3] = { cv::Point2f(2211, 469), cv::Point2f(258, 3230), cv::Point2f(1451, 632)};
    cv::Mat warpedImage;

    if(pts1[2]!=cv::Point2f(0, 0)){
        // Calcule la matrice de transformation affine
        cv::Mat M = cv::getAffineTransform(pts1, pts2);

        // Applique la transformation affine à l'image
        cv::warpAffine(image, warpedImage, M, image.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
    }else{
        warpedImage=image;
    }

    // Crée le chemin du nouveau fichier dans un dossier différent
    string outputPath = "../images_droites/" + nom;

    // Enregistre l'image redressée
    cv::imwrite(outputPath, warpedImage);

    // Attend que l'utilisateur appuie sur une touche
    cv::waitKey(0);

}
