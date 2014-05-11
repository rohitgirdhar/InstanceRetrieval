#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <string>
#include <cstdlib>
#include <ctime>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;
using namespace cv;

typedef struct Tree{
    int centers[10][128];
    struct Tree* sub[10];
    int nodes[10];
    int n_centers;
    int n_sub;
    int depth;
}Tree;

typedef struct RImage{
    int index;
    double r_score;
    double m_score;
    vector< Point2f > corners;
}RImage;

Tree* ParseTree(ifstream &t_file, int d){
    if( !t_file.good() ){
        return NULL;
    }
    int centers, subs;
    t_file >> centers >> subs;
    Tree *t = (Tree*)malloc(sizeof(Tree)); 
    t->n_centers = centers;
    t->n_sub = subs;
    t->depth = d;
    for(int i = 0 ; i < centers ; i++ ){
        for(int j = 0 ; j < 128 ; j++ ){
            t_file >> t->centers[i][j]; 
        }
    }
    for(int i = 0 ; i < subs ; i++ ){
        t->sub[i] = ParseTree(t_file,d+1);
    }
    for(int i = subs ; i < 10 ; i++){
        t->sub[i] = NULL;
    }
    return t;
}

void GetPath(Tree* tree , int* center , int* path , int depth){
    if(tree == NULL){
        path[depth] = -1;
        return;
    }
    double minD = 100000;
    double tempD;
    int minC = 2;  
    for(int i = 0 ; i < tree->n_centers ; i++ ){
        tempD = 0;
        for(int j = 0 ; j < 128 ; j++ ){
            tempD += pow((double)(tree->centers[i][j] - center[j]),2.0);
        }
        tempD = sqrt(tempD);
        if(tempD < minD){
            minD = tempD;
            minC = i;
        }
    }
    path[depth] = minC+1;
    GetPath(tree->sub[minC], center, path, depth+1);
}

int GetWord( int *path ){
    int w = 0;
    for( int i = 0 ; i < 4 ; i++ ){
        w = (w*10) + path[i] - 1;
    }
    w += 1;
    return w;
}

int compare ( const void *p1 , const void *p2 ){
    double p1_c = (* (RImage *)p1).r_score;
    double p2_c = (* (RImage *)p2).r_score;
    return p1_c < p2_c ? 1 : -1; 
}

int compare2 ( const void *p1 , const void *p2 ){
    double p1_c = (* (RImage *)p1).m_score;
    double p2_c = (* (RImage *)p2).m_score;
    return p1_c < p2_c ? 1 : -1; 
}

int main(int argc, char *argv[]) {

    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help", "Show this help message")
        ("data-dir,d", po::value<string>(), "Output dir of the training process")
        ("num-matches,n", po::value<int>()->default_value(5), "Number of matches to show")
        ("first-match,f", po::value<int>()->default_value(1), "Index of the first match returned. NOTE: 1 indexed")
        ("query-img,q", po::value<string>(), "Query image path")
        ("resize,r", "Resize the query image to height = 360 for faster retrieval")
        ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return -1;
    }
    if (vm.count("data-dir") <= 0 || 
        vm.count("query-img") <= 0) {
        cerr << "data-dir, query-img is required argument" << endl;
        return -1;
    }
    string data_dir = vm["data-dir"].as<string>();
    int num_matches = vm["num-matches"].as<int>();
    int first_match = vm["first-match"].as<int>();
    string query_img_path = vm["query-img"].as<string>();
    
    ifstream InvertedIndexFile((data_dir + "InvertedIndex.txt").c_str(), ios::in);
    if (!InvertedIndexFile.is_open()) {
        cerr << "Error opening Inverted Index File" << endl;
        return -1;
    }

    map< int, map< int, int > > InvertedFile;
    int vword, vcount, vimage, vnum;
    while(InvertedIndexFile.good()){
        InvertedIndexFile >> vword >> vcount;
        for(int i = 0 ; i < vcount ; i++ ){
            InvertedIndexFile >> vimage >> vnum ;
            InvertedFile[vword][vimage] = vnum;
        }
    }
    InvertedIndexFile.close();
    ifstream t_file((data_dir + "HKMeans_10000.Tree").c_str(),ios::in);
    if (!t_file.is_open()) {
        cerr << "Error opening Tree file" << endl;
        return -1;
    }
    Tree* tree = ParseTree(t_file,0);
    t_file.close();

    
    ifstream imageListFile((data_dir + "ImageNames.txt").c_str());
    if (!imageListFile.is_open()) {
        cerr << "Error opening image list file" << endl;
        return -1;
    }
    int N = std::count(std::istreambuf_iterator<char>(imageListFile), 
            std::istreambuf_iterator<char>(), '\n');
    vector < string > ImageList;
    vector < string > Annotations;
    string temp, tempLine;
    
    string TF_filename = data_dir + "DCount.txt";
    vector < int > TF;
    int tempTF;
    ifstream TF_file;
    TF_file.open(TF_filename.c_str(), ios::in );
    if (!TF_file.is_open()) {
        cerr << "Error opening DCount file" << endl;
        return -1;
    }
    imageListFile.seekg(0);
    for(int i = 1 ; i <= N ; i++ ){
        imageListFile >> temp;
        getline(imageListFile, tempLine);
        ImageList.push_back( temp );
        Annotations.push_back( tempLine );
        TF_file >> tempTF;
        TF.push_back(tempTF);
    }

    SiftFeatureDetector detector;
    std::vector<KeyPoint> keypoints;

    SiftDescriptorExtractor extractor;
    Mat descriptors;

    Mat img_temp = imread(query_img_path, IMREAD_GRAYSCALE);
    if (!img_temp.data) {
        cerr << "Error reading query image" << endl;
        return -1;
    }
    int height = img_temp.rows;
    int width = img_temp.cols;
    Mat img;
    if (vm.count("resize") && (height > 500 || width > 500)) {
        int new_h = 360;
        int new_w = (new_h*width)/height;
        img.create(new_h,new_w,CV_8UC3);
        resize(img_temp,img,img.size(),0,0, INTER_LINEAR);
    } else {
        img = img_temp.clone();
    }

    detector.detect(img,keypoints);
    extractor.compute(img,keypoints,descriptors);


    map< int, int > QueryHist;
    vector < int > QWords;
    int des[128];
    for(int i = 0 ; i < descriptors.rows ; i++ ){
        for(int j = 0 ; j < 128 ; j++){
            des[j] = descriptors.at<float>(i,j);
        }
        int path[12];
        GetPath( tree, des, path, 0 );
        int hist = GetWord( path );
        if(QueryHist.count( hist ) > 0){
            QueryHist[ hist ] += 1;
        }
        else{
            QueryHist[ hist ] = 1;
        }
        QWords.push_back( hist );

    }

    double idf;
    RImage *ImagesRetrieved = new RImage[N + 1];
    for(int i = 1 ; i <= N ; i++ ) {
        ImagesRetrieved[ i ].index  = i;
        ImagesRetrieved[ i ].r_score  = 0;
        ImagesRetrieved[ i ].m_score  = 0;
    }

    int word, count;
    for( map< int, int >::iterator it = QueryHist.begin() ; it != QueryHist.end() ; ++it){
        word = (*it).first;
        count = (*it).second;
        if( InvertedFile[ word ].size() == 0 )
            continue;
        idf = log10( N/InvertedFile[word].size() );
        for( map< int , int >::iterator it2 = InvertedFile[word].begin() ; it2 != InvertedFile[word].end() ; ++it2 ){
            int terms = TF[ (*it2).first-1 ] > QWords.size() ? TF[ (*it2).first-1 ] : QWords.size() ;
            ImagesRetrieved[(*it2).first].r_score += ( ( ( count > (*it2).second ? (*it2).second : count ) * 1.0 )/terms)  * idf; 
        }
    }

    qsort( ImagesRetrieved + 1, N , sizeof(ImagesRetrieved[ 1 ]), compare );

    std::vector<KeyPoint> Rkeypoints;
    vector< int > RWords;
    vector < Mat > RMatches ;
    string WordKeyDir = data_dir + "wordkeys/wk_";
    for (int n = 1 ; n <= num_matches ; n++) {
        string RImgFilePath = WordKeyDir + ImageList[ ImagesRetrieved[n].index - 1] + ".txt";
        ifstream RImgFile( RImgFilePath.c_str(), ios::in );
        if (!RImgFile.is_open()) {
            cerr << "Error opening Wordkeys file (" << RImgFilePath << ")" << endl;
            return -1;
        }
        int countWords, Rword;
        Point2f RPoint;
        while( RImgFile.good() && !RImgFile.eof() ){
            RImgFile >> Rword >> RPoint.x >> RPoint.y;
            RWords.push_back( Rword );
            KeyPoint temp(RPoint,0);
            Rkeypoints.push_back(temp);
        }
        RImgFile.close();

        std::vector< DMatch > imatches;
        for(int i = 0 ; i < RWords.size() ; i++ ){
            for(int j = 0 ; j < QWords.size() ; j++ ){
                if( RWords[i] == QWords[j] ){
                    DMatch tempDMatch;
                    tempDMatch.queryIdx = i;
                    tempDMatch.trainIdx = j;
                    tempDMatch.distance = 0;
                    imatches.push_back(tempDMatch);
                }
            }
        }
        std::vector<Point2f> obj;
        std::vector<Point2f> scene;

        for( int i = 0; i < imatches.size(); i++ )
        {
            //-- Get the keypoints from the good matches
            obj.push_back( Rkeypoints[ imatches[i].queryIdx ].pt );
            scene.push_back( keypoints[ imatches[i].trainIdx ].pt ); 
        }

        std::vector<uchar> inliers(obj.size(),0);
        if(obj.size() == 0 ) continue;
        Mat F = findFundamentalMat( obj, scene, inliers , FM_RANSAC, 2, 0.99 );
        std::vector<cv::Point2f>::const_iterator itPts=obj.begin();
        std::vector<uchar>::const_iterator itIn= inliers.begin();
        std::vector<Point2f> obj_ransac;
        std::vector<Point2f> scene_ransac;

        int it = 0, total = 0;
        while (itPts!=obj.end()) {
            if (*itIn){
                obj_ransac.push_back( Rkeypoints[ imatches[ total ].queryIdx ].pt );
                scene_ransac.push_back( keypoints[ imatches[ total ].trainIdx ].pt );
                ++it;
            }
            ++itPts;
            ++itIn;
            total++;
        }
        ImagesRetrieved[ n ].m_score = 1.0 * it ; 
        Rkeypoints.clear();
        RWords.clear();
    }
    qsort( ImagesRetrieved + 1, N , sizeof(ImagesRetrieved[ 1 ]), compare2 );


    for (int i = 0; i < num_matches; i++) {
        int cur = i + first_match;
        if (ImagesRetrieved[cur].r_score != 0) {
            cout << ImageList[ImagesRetrieved[cur].index - 1]
                << endl;
        }
    }

    return 0;

}

