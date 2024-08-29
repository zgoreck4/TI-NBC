#ifndef DataSample_H
#define DataSample_H

#include <vector>
#include <iostream>

#pragma once

class DataSample
{
    public:
        int id;
        std::vector<double> coords;
        std::vector<int> kNN;
        int n_RkNN;
        float NDF;
        int point_type;
        int c_id;
        int no_calc;
        double eps;
        double maxEps;

        DataSample(int id, std::vector<double> coords);
        ~DataSample();
        void display();
        void display_tab();
        void displaykNN();
        void addTokNN(int id_data_sample);

};

#endif