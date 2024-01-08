#include <iostream>
#include <fstream>
#include <list>
#include <dirent.h>
#include <windows.h>


using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

#include "histogram.hpp"

#include "text_size.hpp"
#include "icon_label.hpp"
#include "redresser_image.h"


void rect_cutting(int x_init, int y_init, int size, int inter_x, int inter_y, std::string imName, std::string numScripter, std::string numPage){

    //Import the lists of sizes and labels of all the rows of th page
    list<std::string> list_size = read_size(imName);
    list<std::string> list_label = reco_label(imName);

    for (std::string elem : list_label){
        std::cout<<elem<<std::endl;
    }
    for (std::string elem : list_size){
        std::cout<<elem<<std::endl;
    }

    // Load image
    Mat im = imread(imName);
    if(im.data == nullptr){
        cerr << "Image not found: "<< imName << endl;
        waitKey(0);
        //system("pause");
        exit(EXIT_FAILURE);
    }

    //initialisation des coordonnées
    int x = x_init+3;
    int y = y_init;
    int id = 0;

    //Parcourt les 7 lignes d'une page
    for (int i = 0; i < 7 ; i++){
        //Récupération du label
        std::string labelName = list_label.front();

        //Parcourt la ligne si son label est identifié
        if(labelName!="none"){
            //Parcourt les 5 cases de la ligne
            for (int j = 0; j < 5 ; j++){

            // Define the Region of Interest (ROI) using a rectangle
            //width et height : taille du retangle, x y  : point du coin haut gauche du rectangle ?
            Rect roiRect(x, y, size, size);  // Example: (x, y, width, height)

            // Extract the sub-image based on the defined ROI
            Mat roi = im(roiRect).clone();  // Use clone() to create a separate copy of the ROI

            //Récupération de la taille
            std::string size = list_size.front();

            // Save the extracted sub-image to the Result file
            // Saving Format of the image file : iconeID_scripterNumber_pageNumber_row_column.jpg
            imwrite(".//..//Result/"+labelName+"_"+numScripter+"_"+numPage+"_"+to_string((i+1))+"_"+ to_string(j+1)+".jpg", roi);

            //Création du fichier .txt de la forme iconeID_scripterNumber_pageNumber_row_column.txt
            //Ouverture du fichier en mode écriture (creation)
            std::ofstream fichierSortie(".//..//Result/"+labelName+"_"+numScripter+"_"+numPage+"_"+to_string((i+1))+"_"+ to_string(j+1)+".txt");

            //Ecriture dans le fichier des informations de l'imagette
            if (fichierSortie.is_open()) {
                fichierSortie<<"#Projet de Traitement Image et Vidéo 2023\n";
                fichierSortie<<"#Prune Lepvraud, Alexandre Mahjoub, Anouk Migliavacca et Pauline Roches\n";
                fichierSortie<<"label "+labelName+"\n";
                fichierSortie<<"form " + numScripter + numPage + "\n";
                fichierSortie<<"scripter " + numScripter + "\n";
                fichierSortie<<"page " + numPage + "\n";
                fichierSortie<<"row "+ to_string(i+1) +"\n";
                fichierSortie<<"column "+ to_string(j+1) +"\n";
                fichierSortie<<"size "+size+"\n";

                // Fermeture du fichier
                fichierSortie.close();
            }else {
                std::cerr << "Erreur : Impossible d'ouvrir le fichier." << std::endl;
            }

            //Incrémentation pour parcourir la ligne
            id++;
            x+=inter_x;
            }
        }
        //Supprime les premiers éléments des listes correspondantes aux tailles et types de chaque ligne pour pouvoir passer à la suivante
        list_label.pop_front();
        list_size.pop_front();

        //Incrémentation pour passer à la ligne suivante
        y+=inter_y;
        x=x_init;
    }
}

int main (void) {

    // Spécifier le chemin du dossier à parcourir
    const char* dossier = ".//..//Donnees";

    // Ouvre le dossier
    DIR* dir = opendir(dossier);

    const std::string outputDirectory = ".//..//images_droites";

    //Cette boucle redresse toutes les images de <dossier> et les met dans le dossier "image_droites"
    if (CreateDirectoryA(outputDirectory.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
        if (dir) {
            // Parcourt les fichiers dans le dossier
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string nom = entry->d_name;

                //nom different de . ou .. et pas le formulaire 22 (texte manuscrit)
                if(nom.size()>3 and nom.substr(3,2)!="22") {
                    redresser_image("./../Donnees/" + nom,nom);
                }
            }
            // Ferme le dossier
            closedir(dir);
        } else {
            std::cerr << "Erreur lors de l'ouverture du dossier." << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Failed to create output directory: " << outputDirectory << '\n';
        return 1;
    }


    // Spécifie le chemin du dossier à parcourir
    const char* dossier2 = ".//..//images_droites";

    // Ouvre le dossier
    DIR* dir2 = opendir(dossier2);

        if (dir2) {
            // Parcourir les fichiers dans le dossier
            struct dirent* entry;
            while ((entry = readdir(dir2)) != nullptr) {
                //extrait le nom du fichier actuel dans la variable nom
                std::string nom = entry->d_name;

                // Ignore les répertoires spéciaux "." et ".."
                if(nom.size()>3){
                    //Récupère le numéro de scripter
                    std::string scripterNumber = nom.substr(0,3);
                    //Récupère le numéro de page
                    std::string pageNumber = nom.substr(3,2);

                    //Récupère le path du fichier concerné
                    std::string imName = ".//..//images_droites/" + nom;

                    // Fonction de parcours de la page et de création des fichiers associés
                    rect_cutting(610+15,720+61,230,351-18,348-17, imName, scripterNumber, pageNumber);
                }
            }
            // Ferme le dossier
            closedir(dir);
        } else {
            std::cerr << "Erreur lors de l'ouverture du dossier2." << std::endl;
            return 1;
        }

	//termine le programme lorsqu'une touche est frappee
	waitKey(0);
	return EXIT_SUCCESS;
}
