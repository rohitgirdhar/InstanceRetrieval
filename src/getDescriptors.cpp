#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace cv;
using namespace std;

#define IMG_NAMES_FNAME "ImageNames.txt"

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help", "produce help message")
        ("images-dir,i", po::value<string>(), "input directory path of images")
        ("sift-kpts,s", po::value<int>()->default_value(3000)->implicit_value(3000), 
         "Number of SIFT keypoints to find in image")
        ("output-dir,o", po::value<string>()->default_value("./"), "Output Directory")
        ("resize,r", "Resize the input image to height=360")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return -1;
    }

    int num_sift_kpts = vm["sift-kpts"].as<int>();
    if (vm.count("images-dir") <= 0) {
        cerr << "Required Option: images-dir" << endl;
        return -1;
    }
    string images_dir = vm["images-dir"].as<string>();
    string output_dir = vm["output-dir"].as<string>();
    if (*output_dir.rbegin() != '/') {
        output_dir += '/';
    }

    cout << "Using images dir: " << images_dir << endl;
    cout << "Using ouput dir: " << output_dir << endl;
    if (! fs::is_directory(output_dir)) {
        cout << "creating output directory" << endl;
        fs::create_directory(output_dir);
    }
    fs::create_directory(output_dir + "keypoints");
    fs::create_directory(output_dir + "descriptors");
    fs::create_directory(output_dir + "MatFiles");
    fs::create_directory(output_dir + "words");


    SIFT detector(num_sift_kpts);
    std::vector<KeyPoint> keypoints;

    SiftDescriptorExtractor extractor;
    Mat descriptors;

    ofstream descCountFile;
    descCountFile.open((output_dir + "DCount.txt").c_str(),ios::out);

    ofstream descriptorsFile;
    descriptorsFile.open((output_dir + "Descriptors.txt").c_str(),ios::out);
    ofstream desFile;

    ofstream keypointsFile;
    string keyF,DesF;

    Mat img,img_temp;
    string imageName;

    fs::recursive_directory_iterator rdi(images_dir);
    vector<string> img_names;
    while (rdi != fs::recursive_directory_iterator()) {
        img_names.push_back((*rdi).path().string());
        rdi++;
    }
    sort(img_names.begin(), img_names.end());

    fs::remove(output_dir + IMG_NAMES_FNAME);
    ofstream imageNamesFile;
    imageNamesFile.open((output_dir + IMG_NAMES_FNAME).c_str(), ios::app);
    for (int i = 0; i < img_names.size(); i++) {
        imageName = img_names[i];
        
        string img_basename = fs::basename(imageName);
        imageNamesFile << img_basename << endl;
        img_temp = imread(imageName, IMREAD_GRAYSCALE);
        int height = img_temp.rows;
        int width = img_temp.cols;

        if (vm.count("resize") && (height > 500 || width > 500 )) {
            int new_h = 360;
            int new_w = (new_h*width)/height;
            img.create(new_h,new_w,CV_8UC3);
            resize(img_temp,img,img.size(),0,0, INTER_LINEAR);
        } else {
            img = img_temp.clone();
        }
        detector.detect(img,keypoints);

        keyF = output_dir + "keypoints/K_" + img_basename + ".txt";
        DesF = output_dir + "descriptors/D_" + img_basename + ".txt";
        cout << keyF << endl;
        keypointsFile.open(keyF.c_str(),ios::out);
        for (int i = 0 ; i < keypoints.size() ; i++){
            keypointsFile << keypoints[i].pt.x << " "
                << keypoints[i].pt.y << " "
                << keypoints[i].size << " " 
                << keypoints[i].angle << endl;
        }
        keypointsFile.close();

        extractor.compute(img,keypoints,descriptors);
        descCountFile<<descriptors.rows<<endl;

        desFile.open( DesF.c_str(), ios::out );
        for(int i = 0 ; i < descriptors.rows ; i++){
            for(int j = 0 ; j < descriptors.cols ; j++){
                descriptorsFile << descriptors.at<float>(i,j) << " ";
                desFile << descriptors.at<float>(i,j) << " ";
            }
            descriptorsFile<<endl;
            desFile << endl;
        }
        desFile.close();
        keypoints.erase(keypoints.begin(), keypoints.end());
        descriptors.release();
    }
    imageNamesFile.close();
    descriptorsFile.close();
    descCountFile.close();
    return 0;
}
