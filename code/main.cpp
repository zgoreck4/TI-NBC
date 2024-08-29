#include <iostream>
#include <vector>
#include <unordered_map>
#include <math.h>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>

#include <chrono>
#include <ctime> 


#include "DataSample.cpp"
#include "CSVReader.cpp"
#include "NBCAlgorithm.cpp"
#include "KNNAlgorithm.cpp"
#include "TikNNAlgorithm.cpp"
#include "TiCoskNNAlgorithm.cpp"

int main()
{
    int no_dataset = 2;
    int k = 2;
    std::string path_output = "output/";
    std::string mode_kind[3] = {"noTi", "Ti", "TiCos"};
    std::string mode = mode_kind[0];

    std::string datasets[5][2] = {{"own_dataset.csv", "own_dataset_classes.csv"}, {"wyklad.csv", "wyklad_classes.csv"}, {"iris.csv", "iris_classes.csv"}, {"glass.csv", "glass_classes.csv"}, {"WineQT.csv", "WineQT_classes.csv"}};
    std::string dataset_name = datasets[no_dataset][0];
    std::string dataset_calsses = datasets[no_dataset][1];
    const std::string path = "data/";
    const std::string path_classes = "data/"+dataset_calsses;

    CSVReader reader;
    std::chrono::microseconds::rep load_file_time;    
    std::chrono::nanoseconds::rep normalize_time;    
    std::vector<DataSample> data = reader.getData(path+dataset_name, load_file_time, normalize_time);
    std::cout<<"Wczytano dane\n";
    std::vector<int> classes = reader.getClasses(path_classes);

    std::chrono::microseconds::rep sort_time;
    std::chrono::microseconds::rep calcKNN_time;
    std::string filename;

    if (mode=="noTi")
    {
        KNNAlgorithm kNN_alg = KNNAlgorithm(k, data, sort_time);
        std::chrono::steady_clock::time_point begin_kNN = std::chrono::steady_clock::now();
        data = kNN_alg.calcKNN();
        std::chrono::steady_clock::time_point end_kNN = std::chrono::steady_clock::now();
        calcKNN_time = std::chrono::duration_cast<std::chrono::microseconds>(end_kNN - begin_kNN).count();

        filename = "NBC_k"+std::to_string(k)+dataset_name;
    }
    else if (mode=="Ti")
    {
        // współrzędne punktu referencyjnego
        int dim = data[0].coords.size();
        std::vector<double> ref_coords(dim, 0);
        ref_coords[0] = 1;

        TikNNAlgorithm tikNN_alg = TikNNAlgorithm(k, data, sort_time, ref_coords);
        std::chrono::steady_clock::time_point begin_kNN = std::chrono::steady_clock::now();
        data = tikNN_alg.calcKNN();
        std::chrono::steady_clock::time_point end_kNN = std::chrono::steady_clock::now();
        calcKNN_time = std::chrono::duration_cast<std::chrono::microseconds>(end_kNN - begin_kNN).count();

        filename = "TiNBC_k"+std::to_string(k)+dataset_name;
    }
    else if (mode=="TiCos")
    {
        // współrzędne punktu referencyjnego
        int dim = data[0].coords.size();
        std::vector<double> ref_coords(dim, 0);
        ref_coords[0] = 1;

        TiCoskNNAlgorithm tiCoskNN_alg = TiCoskNNAlgorithm(k, data, sort_time, ref_coords);
        std::chrono::steady_clock::time_point begin_kNN = std::chrono::steady_clock::now();
        data = tiCoskNN_alg.calcKNN();
        std::chrono::steady_clock::time_point end_kNN = std::chrono::steady_clock::now();
        calcKNN_time = std::chrono::duration_cast<std::chrono::microseconds>(end_kNN - begin_kNN).count();

        filename = "TiCosNBC_k"+std::to_string(k)+dataset_name;
    }

    NBCAlgorithm NBC_alg = NBCAlgorithm(k, data, load_file_time, normalize_time, sort_time, calcKNN_time, dataset_name, classes);
    NBC_alg.clustering();

    std::string filename_kNN = path_output+"k+NN_"+filename;
    std::string filename_out = path_output+"OUT_"+filename;
    std::string filename_stat = path_output+"STAT_"+filename;
    NBC_alg.writeResultkNN(filename_kNN, filename_out, filename_stat);

    }
	