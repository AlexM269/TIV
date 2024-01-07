
#ifndef PROJET_OPENCV_CMAKE_ICON_LABEL_HPP
#define PROJET_OPENCV_CMAKE_ICON_LABEL_HPP


#include <iostream>
#include <list>

using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;


list<std::string> reco_label(const string file_img);

#endif
