//////////////////////////////////////////////////////////////////////////
// Option Images
// Projet, s�ance 1
// th�me : premier pas en OpenCV
// contenu : charge, affiche, r�duction, calcul et affichage d'histogramme
// version : 17.1128
//////////////////////////////////////////////////////////////////////////


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
    //(Coord x du coin gauche du carré initial, sa coord y, taille du carré,
    // interval horizontal entre deux coins gauche de carré, interval vertical, Image à découper)

    //crée un fichier de la forme <id> + <size> pour chaque ligne d'imagette dans icon_size
    list<std::string> list_size = read_size(imName);

    list<std::string> list_label = reco_label(imName);


    for (std::string elem : list_label){
        std::cout<<elem<<std::endl;
    }

    for (std::string elem : list_size){
        std::cout<<elem<<std::endl;
    }


    Mat im = imread(imName);
    if(im.data == nullptr){
        cerr << "Image not found: "<< imName << endl;
        waitKey(0);
        //system("pause");
        exit(EXIT_FAILURE);
    }

    int x = x_init+3;
    int y = y_init;
    int id = 0;
    for (int i = 0; i < 7 ; i++){

        //Récupération du label
        std::string labelName = list_label.front();
        //labelName.erase(labelName.size() - 4);
        if(labelName!="none"){
            for (int j = 0; j < 5 ; j++){

            // Define the Region of Interest (ROI) using a rectangle
            //width et height : taille du retangle, x y  : point du coin haut gauche du rectangle ?
            Rect roiRect(x, y, size, size);  // Example: (x, y, width, height)

            // Extract the sub-image based on the defined ROI
            Mat roi = im(roiRect).clone();  // Use clone() to create a separate copy of the ROI

            //Récupération de la taille
            std::string size = list_size.front();

            // Save the extracted sub-image to a file in cmake-build-debug (default),sinon pr�ciser le chemin
            // Saving Format of the image file : iconeID_scripterNumber_pageNumber_row_column.png
            imwrite(".//..//Result/"+labelName+"_"+numScripter+"_"+numPage+"_"+to_string((i+1))+"_"+ to_string(j+1)+".jpg", roi);


            //Création du fichier .txt
            // Ouverture du fichier en mode écriture (creation)
            std::ofstream fichierSortie(".//..//Result/"+labelName+"_"+numScripter+"_"+numPage+"_"+to_string((i+1))+"_"+ to_string(j+1)+".txt");
            if (fichierSortie.is_open()) {
               // std::cout<<numScripter+"_"+numPage+"_"+ to_string(i+1)+ to_string(j+1)<<std::endl;

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

            id++;

            //Affichage de l'extrait de l'image
            //imshow("ExtractedSquare"+to_string((i))+ to_string(j), im(roiRect).clone());

            x+=inter_x;

           // std::cout<<to_string(i)+ to_string(j)<<std::endl;
        }
        }
        //Supprime les premiers éléments des listes correspondantes aux tailles et types de chaque ligne pour pouvoir passer à la suivante
        list_label.pop_front();
        list_size.pop_front();

        y+=inter_y;
        x=x_init;
    }
}


int main (void) {

    // Spécifiez le chemin du dossier à parcourir
    //const char* dossier = ".//..//NicIcon//all-scans";
    //const char* dossier = ".//..//w000-scans";
    const char* dossier = ".//..//Donnees";

    // Ouvrir le dossier
    DIR* dir = opendir(dossier);

    const std::string outputDirectory = ".//..//images_droites";

    //Cette boucle redresse toutes les images de <dossier> et les met dans le dossier "image_droites"
    if (CreateDirectoryA(outputDirectory.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
        if (dir) {
            // Parcourir les fichiers dans le dossier
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string nom = entry->d_name;
                //nom different de . ou .. et pas le formulaire 22 (texte manuscrit)

                if(nom.size()>3 and nom.substr(3,2)!="22") {
                    //redresser_image(".//..//NicIcon//all-scans//" + nom,nom);
                    redresser_image("./../Donnees/" + nom,nom);
                    //redresser_image(".//..//w000-scans//" + nom,nom);
                }
            }
            // Fermer le dossier
            closedir(dir);
        } else {
            std::cerr << "Erreur lors de l'ouverture du dossier." << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Failed to create output directory: " << outputDirectory << '\n';
        return 1;
    }

    // Spécifiez le chemin du dossier à parcourir
    const char* dossier2 = ".//..//images_droites";

    // Ouvrir le dossier
    DIR* dir2 = opendir(dossier2);
   // std::fstream output_fstream;
   // const std::string outputDirectory2 = ".//..//Result";

   // if (CreateDirectoryA(outputDirectory2.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
        if (dir2) {
            // Parcourir les fichiers dans le dossier
            struct dirent* entry;
            while ((entry = readdir(dir2)) != nullptr) {

                //extrait le nom du fichier actuel dans la variable nom
                std::string nom = entry->d_name;

                // Assurez-vous de ne pas traiter les répertoires spéciaux "." et ".."
                if(nom.size()>3){
                    // Afficher le nom du fichier
                    //std::cout << nom << std::endl;

                    //Récupère le numéro de scripter
                    std::string scripterNumber = nom.substr(0,3);
                    //Récupère le numéro de page
                    std::string pageNumber = nom.substr(3,2);

                    //Récupère le path du fichier concerné
                    std::string imName = ".//..//images_droites/" + nom;

                    rect_cutting(610,720,240,351,348, imName, scripterNumber, pageNumber);



                    /*
                    //std::cout << "Nb page : " << nom.substr(3,2) << std::endl;
                    std::string filename = outputDirectory2+"//iconeID_"+scripterNumber+"_"+pageNumber+"_row_column";
                    //std::cout << filename << std::endl;
                    output_fstream.open(filename, std::ios_base::out);
                    if (!output_fstream.is_open()) {
                        std::cerr << "Failed to open " << filename << '\n';
                    } else {
                        output_fstream << "label\nform "
                                       //     +nom.substr(0,5)+
                                       //     "\n scripter "
                                       //      +scripterNumber+
                                       //      "\n page "
                                       //       +pageNumber+
                                       //       "row <rowNumber> \n  column <columnNumber> \n size <small/medium/large>"
                                       << std::endl;
                        std::cerr << "Done Writing!" << std::endl;
                        output_fstream.close();
                    }
                    */

                }
            }

            // Fermer le dossier
            closedir(dir);
        } else {
            std::cerr << "Erreur lors de l'ouverture du dossier2." << std::endl;
            return 1;
        }
  /*  } else {
        std::cerr << "Failed to create output directory2: " << outputDirectory2 << '\n';
        return 1;
    }*/

    //Découpe les imagette, crée les fichiers image correspondants et les fichiers textes avec les infos sur l'image dans
    // cmake-build-debug-mingw/Images et ImagesData
    //A l'origine : (608,720,255,350,350, im)

	//termine le programme lorsqu'une touche est frappee
	waitKey(0);
	return EXIT_SUCCESS;
}
