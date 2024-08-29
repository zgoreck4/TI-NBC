#include "DataSample.h"

DataSample::DataSample(int id, std::vector<double> coords):
    id(id), coords(coords), c_id(-2), n_RkNN(0), no_calc(0){
    // std::cout<<"Wywolano konstruktor DataSample"<<std::endl;
}

DataSample::~DataSample()
{
    // std::cout<<"Dekonstruktor DataSample"<<std::endl;
}

void DataSample::display()
{
    std::cout<<id<<"; ";
    for (int j = 0; j < coords.size(); j++)
    {
        std::cout << coords[j] << " ";
    }    
    std::cout << std::endl;
}

void DataSample::display_tab()
{
    std::cout<<id<<"; coords ";
    for (int j = 0; j < coords.size(); j++)
    {
        std::cout << coords[j] << " ";
    }    
    std::cout<<"; maxEps "<<maxEps;
    std::cout<<"; kNN ";
    displaykNN();
    std::cout<<"; n_RkNN "<<n_RkNN;
    std::cout<<"; NDF "<<NDF;
    std::cout<<"; CId "<<c_id;
    std::cout<<"; typ "<<point_type;
    std::cout << std::endl;
}

void DataSample::displaykNN()
{
    for (int l = 0; l < kNN.size(); l++)
    {
        std::cout << kNN[l] << " ";
    }  
}

void DataSample::addTokNN(int id_data_sample)
{
    kNN.push_back(id_data_sample);
}