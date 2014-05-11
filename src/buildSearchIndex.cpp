#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cstdio>
#include <fstream>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

int main(int argc, char *argv[]) {
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help", "produce this help message")
        ("output-dir,o", po::value<string>(), "output directory used")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return -1;
    }
    if (vm.count("output-dir") <= 0) {
        cerr << "Output dir is required argument" << endl;
        return -1;
    }
    string Directory = vm["output-dir"].as<string>();
    if (*Directory.rbegin() != '/') {
        Directory += '/';
    }

    int N;
    vector< string > ImageNames;
    string tempName;

    ifstream imagesFile((Directory + "ImageNames.txt").c_str(), ifstream::in);
    N = std::count(std::istreambuf_iterator<char>(imagesFile), 
            std::istreambuf_iterator<char>(), '\n');
    imagesFile.close();

    cout << "Counted the # of images as " << N << endl;

    string iNamesN = Directory + "ImageNames.txt";
    ifstream iNames(iNamesN.c_str(),ifstream::in);
    if (iNames.is_open()) {
        for (int i = 1 ; i <= N ; i++) {
            iNames >> tempName;
            ImageNames.push_back(tempName);
        }
        iNames.close();
    } else {
        cout << "error opening file ImageNames.txt" << endl;
    }

    string WordsDir = Directory + "words/w_";
    map < int , map< int, int > > Index;
    double temp;
    int nW;
    int vword;
    double vx, vy, va, vb, vc;
    ifstream iWords;
    for( int i = 1 ; i <= N ; i++ ){
        string fName = WordsDir +  ImageNames[i-1] + ".txt";
        cout << fName <<endl;
        iWords.open( fName.c_str() );
        if( iWords.is_open() && iWords.good() ){
            while( !iWords.eof() && iWords.good() ){
                iWords >> vword;
                if( Index[vword].count(i) > 0 ){
                    Index[vword][i] += 1;
                }
                else{
                    Index[vword][i] = 1;
                }
            }	    
            iWords.close();
        }
        else{
            cout << "error opening file " << endl;
        }
    }

    string indexFileN = Directory + "InvertedIndex.txt";
    FILE *fp;
    fp = fopen(indexFileN.c_str(),"wb");
    for( map< int, map < int, int > >::iterator it1 = Index.begin() ; it1 != Index.end() ; ++it1 ){
        vword = it1->first;
        fprintf( fp, "%d %lu\n",vword, Index[vword].size() );
        for( map< int , int >::iterator it2 = Index[vword].begin() ; it2 != Index[vword].end() ; ++it2 ){
            fprintf( fp, "%d %d\n",it2->first, it2->second );
        }
    }
    fclose(fp);
}
