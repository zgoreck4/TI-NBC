#ifndef KNNALGORITHM_H
#define KNNALGORITHM_H

#include <vector>
#include <cmath>
#include <chrono>
#include <bits/stdc++.h>

class DataSample;

#pragma once

class KNNAlgorithm
{
public:
    KNNAlgorithm(int k, std::vector<DataSample> data, std::chrono::microseconds::rep& sort_time);
    ~KNNAlgorithm();
    std::vector<DataSample> calcKNN();

    void writeResultkNN(const std::string& filename);

private:
    struct DistanceFrom
    {
        DistanceFrom(int id, double dist):
            id_data_sample(id), dist(dist) {}
        int id_data_sample;
        double dist;
    };
    
    int k;
    std::vector<DataSample> data;
    std::vector<DistanceFrom> distances;

    std::chrono::microseconds::rep* sort_time; 

    void calcDistances(int id_data_sample);
    double calcDistance(std::vector<double> coords1, std::vector<double> coords2);
    static bool compareDist(DistanceFrom dist1, DistanceFrom dist2)
    { return (dist1.dist < dist2.dist);};
    void choosekNN(int id_data_sample);
    void displayDist();

};

#endif