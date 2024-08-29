#ifndef NBCALGORITHM_H
#define NBCALGORITHM_H

#include <vector>
#include <list>
#include <cmath>
#include <bits/stdc++.h>
#include <algorithm>
#include <chrono>

class DataSample;

#pragma once

class NBCAlgorithm
{
    public:
        NBCAlgorithm(int k, std::vector<DataSample> data, std::chrono::microseconds::rep load_file_time, std::chrono::nanoseconds::rep normalize_time, std::chrono::microseconds::rep sort_time, std::chrono::microseconds::rep calcKNN_time, std::string data_filename,
            std::vector<int> classes);
        ~NBCAlgorithm();
        void clustering();
        void displayDist();
        std::vector<DataSample> getData();

        void writeResultkNN(const std::string& filename_kNN, const std::string& filename_OUT, const std::string& filename_STAT);
        void writeResultkNN(const std::string& filename);
        void writeResultOut(const std::string& filename);
        void writeResultStat(const std::string& filename);
        void calculateRandIndex();
        
    private:

        int k;
        std::vector<DataSample> data;
        std::set<int> seeds;
        std::vector<int> classes;
        
        std::string data_filename;
        std::chrono::microseconds::rep load_file_time;    
        std::chrono::nanoseconds::rep normalize_time;   
        std::chrono::microseconds::rep sort_time;    
        std::chrono::microseconds::rep calcKNN_time;
        std::chrono::microseconds::rep clustering_time; 
        std::chrono::microseconds::rep save_files_time; 
        std::chrono::microseconds::rep total_time_minus_load_save;
        std::chrono::microseconds::rep total_time; 
        int no_clusters;
        int no_outlier_p;
        int no_core_p;
        int no_border_p;
        double avg_no_calc_dist;
        int TP;
        int TN;
        int no_pairs;
        double RAND;

        void calcNDF(int id_data_sample);
        void createClusters();
        void addCorePoint(int i, int c_id);
        void displaySeedsList();
        int binomialTheorem(int n, int k=2);
        void clear_att();
};

#endif