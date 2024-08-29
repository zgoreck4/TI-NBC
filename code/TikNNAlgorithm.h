#ifndef TikNNAlgorithm_H
#define TikNNAlgorithm_H

#include <vector>
#include <cmath>
#include <chrono>
#include <bits/stdc++.h>

class DataSample;

#pragma once

class TikNNAlgorithm
{
public:
    TikNNAlgorithm(int k, std::vector<DataSample> data, std::chrono::microseconds::rep& sort_time, std::vector<double> ref_coords);
    ~TikNNAlgorithm();
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
    std::vector<double> ref_coords;
    std::vector<DistanceFrom> distances_to_ref;
    int no_nn;
    std::vector<DistanceFrom> distances;
    double eps;

    std::chrono::microseconds::rep* sort_time; 

    void calcDistances();
    double calcDistanceToRef(std::vector<double> coords1);
    double calcDistance(std::vector<double> coords1, std::vector<double> coords2);
    static bool compareDist(DistanceFrom dist1, DistanceFrom dist2)
    { return (dist1.dist < dist2.dist);};
    void choosekNN(int id_data_sample);
    void displayDist(std::vector<DistanceFrom> distaces);
    void tiKNN(int id_point);
    bool precedingPoint(int &id_point);
    bool followingPoint(int &id_point);
    void findFirstkCandidateForwBack(int &id_point, int &id_back, int &id_forw, bool &backwardSearch, bool &forwardSearch);
    void findFirstkCandidateBack(int &id_point, int &id_back, bool &backwardSearch);
    void findFirstkCandidateForw(int &id_point, int &id_forw, bool &forwardSearch);
    void verifykCandidateBack(int &id_point, int &id_back, bool &backwardSearch);
    void verifykCandidateForw(int &id_point, int &id_forw, bool &forwardSearch);
    double findMax(std::vector<DistanceFrom> vec);
    int calcNoMaxNeigh();

};

#endif