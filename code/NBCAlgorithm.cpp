#include "NBCAlgorithm.h"

NBCAlgorithm::NBCAlgorithm(int k, std::vector<DataSample> data, std::chrono::microseconds::rep load_file_time, std::chrono::nanoseconds::rep normalize_time, std::chrono::microseconds::rep sort_time, std::chrono::microseconds::rep calcKNN_time, std::string data_filename,
    std::vector<int> classes):
    k(k), data(data), load_file_time(load_file_time), normalize_time(normalize_time),sort_time(sort_time), calcKNN_time(calcKNN_time), data_filename(data_filename), classes(classes){
}

NBCAlgorithm::~NBCAlgorithm()
{

}

void NBCAlgorithm::clustering()
{
    std::chrono::steady_clock::time_point begin_total = std::chrono::steady_clock::now();
    clear_att();
    for (int i = 0; i < data.size(); i++)
    {
        calcNDF(i);
    }

    std::chrono::steady_clock::time_point begin_cluster = std::chrono::steady_clock::now();
    createClusters();
    std::chrono::steady_clock::time_point end_cluster = std::chrono::steady_clock::now();
    clustering_time = std::chrono::duration_cast<std::chrono::microseconds>(end_cluster - begin_cluster).count();

    calculateRandIndex();

    std::chrono::steady_clock::time_point end_total = std::chrono::steady_clock::now();
    total_time_minus_load_save = std::chrono::duration_cast<std::chrono::microseconds>(end_total - begin_total).count();
}

// void NBCAlgorithm::calcKNN(int id_data_sample)
// {
//     calcDistances(id_data_sample);

//     choosekNN(id_data_sample);
    
// }

// void NBCAlgorithm::calcDistances(int id_data_sample)
// {
//     distances.clear();
//     for (int j=0; j<data.size(); j++)
//     {
//         if(j != id_data_sample)
//         {
//             double dist = calcDistance(data[id_data_sample].coords, data[j].coords);
//             distances.push_back(DistanceFrom(j, dist));
//             // liczba obliczeń na punkcie
//             data[id_data_sample].no_calc++;
//             data[j].no_calc++;
//         }
//     }
// }

// double NBCAlgorithm::calcDistance(std::vector<double> coords1, std::vector<double> coords2)
// {
//     if(coords1.size() != coords2.size())
// 		return -1;
//     else
//     {
// 		double dist = 0;
// 		for (int k = 0; k < coords1.size(); ++k)
// 			dist += (coords1[k] - coords2[k])*(coords1[k] - coords2[k]);
// 		return sqrt(dist);
//     }
// }

// void NBCAlgorithm::displayDist()
// {
//     for (int j = 0; j < distances.size(); ++j)
//     {
// 		std::cout<<distances[j].id_data_sample<<"; "<<distances[j].dist<<std::endl;  
//     }
// }

// void NBCAlgorithm::choosekNN(int id_data_sample)
// {
//     std::chrono::steady_clock::time_point begin_sort = std::chrono::steady_clock::now();
//     sort(distances.begin(), distances.end(), compareDist);

//     data[id_data_sample].maxEps = distances[distances.size()-1].dist;

//     std::chrono::steady_clock::time_point end_sort = std::chrono::steady_clock::now();
//     sort_time += std::chrono::duration_cast<std::chrono::microseconds>(end_sort - begin_sort).count();

//     int id_data_sample_from;
//     for(int j=0; j<k; j++)
//     {
//         id_data_sample_from = distances[j].id_data_sample;
//         data[id_data_sample].addTokNN(id_data_sample_from);
//         data[id_data_sample_from].n_RkNN++;
//     }
//     data[id_data_sample].eps = distances[k-1].dist;
//     // dodanie dodatkowych sąsiadów, którzy mają odległość = odległości przyłączonego k-tego sąsiada
//     if (distances.size()>k)
//     {
//         double dist_next;
//         for(int l=0; l<distances.size(); l++)
//         {
//             dist_next = distances[k+l].dist;
//             double eps = data[id_data_sample].eps;
//             if (dist_next==eps)
//             {
//                 id_data_sample_from = distances[k+l].id_data_sample;
//                 data[id_data_sample].addTokNN(id_data_sample_from);
//                 data[id_data_sample_from].n_RkNN++;
//             }
//             else
//                 break;
//         }
//     }
// }

void NBCAlgorithm::calcNDF(int id_data_sample)
{
    if (data[id_data_sample].n_RkNN != 0)
        data[id_data_sample].NDF = float(data[id_data_sample].n_RkNN)/data[id_data_sample].kNN.size();
    else
        data[id_data_sample].NDF = 0;
}

void NBCAlgorithm::createClusters()
{
    int c_id = 0;
    seeds.clear();
    for (int i = 0; i < data.size(); i++)
    {
        //std::cout<<"Sprawdzamy punkt nr "<<i<<std::endl;
        if (data[i].c_id==-2) // -2 oznaczono punkty, które nie mają przydzielonej jeszcze żadnej grupy
        {
            //std::cout<<"Nie należy do żadnej grupy\n";
            if (data[i].NDF < 1)
            {
                data[i].c_id = -1;
                data[i].point_type = -1;
                no_outlier_p++;
            }
            else
            {
                addCorePoint(i, c_id);
                
                while(seeds.size() > 0)
                {
                    auto it = seeds.begin();
                    int indx = *it;
                    //std::cout<<"Dodajemy do grupy punkt z seeds nr: "<<indx<<std::endl;

                    if (data[indx].NDF < 1)
                    {
                        data[indx].c_id = c_id;
                        data[indx].point_type = 0;
                        no_border_p++;
                    }
                    else
                    {
                        addCorePoint(indx, c_id);
                    }
                    seeds.erase(indx);
                }
                c_id ++;
                //std::cout<<"!!!!!!!!!!! Koniec grupy, rozpoczynamy nową nr "<<c_id<<std::endl;
            }
        }
    }
    no_clusters = c_id;
    if (no_outlier_p)
        no_clusters++;
}

void NBCAlgorithm::addCorePoint(int i, int c_id)
{
    //std::cout<<"Punkt nr "<<i<<" jest rdzeniowy\n";
    no_core_p++;
    data[i].c_id = c_id;
    data[i].point_type = 1;
    for (int j = 0; j < data[i].kNN.size(); j++)
    {
        int idx_neighbor = data[i].kNN[j];
        if (data[idx_neighbor].c_id == -2)
        {
            seeds.insert(idx_neighbor);
        }
        else if (data[idx_neighbor].c_id == -1)
        {
            data[idx_neighbor].c_id = c_id;
            data[idx_neighbor].point_type = 0;
            no_border_p++;
            no_outlier_p--;
        }
    }
}

void NBCAlgorithm::displaySeedsList()
{
    for (int element : seeds) {
            std::cout << element << ", ";
        } 
}


std::vector<DataSample> NBCAlgorithm::getData()
{
    return data;    
}


void NBCAlgorithm::calculateRandIndex()
{
    TP = 0;
    TN = 0;
    for (int i = 0; i < classes.size() - 1; ++i)
    {
        for (int j = i + 1; j < classes.size(); ++j)
        {
            if (j != i)
            {
                if ((data[i].c_id == data[j].c_id) & (classes[i] == classes[j]))
                    TP ++;
                if ((data[i].c_id != data[j].c_id) & (classes[i] != classes[j]))
                    TN ++;
            }
        }
    }

    no_pairs = binomialTheorem(data.size());
    RAND = double(TP+TN) / no_pairs;

    RAND;
}

int NBCAlgorithm::binomialTheorem(int n, int k)
{
    if (k == 0 || k == n)
        return 1;
    return binomialTheorem(n - 1, k - 1) + binomialTheorem(n - 1, k);
}

void NBCAlgorithm::clear_att()
{  
    no_clusters=0;
    no_outlier_p=0;
    no_core_p=0;
    no_border_p=0;
    avg_no_calc_dist=0;
    TP=0;
    TN=0;
}

void NBCAlgorithm::writeResultkNN(const std::string& filename_kNN, const std::string& filename_OUT, const std::string& filename_STAT)
{
    std::chrono::steady_clock::time_point begin_saved = std::chrono::steady_clock::now();
    writeResultkNN(filename_kNN);
    writeResultOut(filename_OUT);
    std::chrono::steady_clock::time_point end_saved = std::chrono::steady_clock::now();
    save_files_time = std::chrono::duration_cast<std::chrono::microseconds>(end_saved - begin_saved).count();

    writeResultStat(filename_STAT);

}

void NBCAlgorithm::writeResultkNN(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << "id;";
        file << "eps;";
        file << "maxEps;";
        file << "NDF;";
        file << "|R-k+NN|;";
        file << "|k+NN|;";
        file << "k+NN;";
        file << std::endl;
        for (int i=0; i<data.size(); i++)
        {
            file << data[i].id << ";";
            file << data[i].eps << ";";
            file << data[i].maxEps << ";";
            file << data[i].NDF << ";";
            file << data[i].n_RkNN << ";";
            file << data[i].kNN.size() << ";";
            for (int j=0; j<data[i].kNN.size(); j++)
            {
                file << data[i].kNN[j] << ",";
            }
            file << std::endl;
        }
        file.close();
        std::cout << "Zapisano wyniki do pliku: " << filename << std::endl;
    }
    else
    {
        std::cout << "Error opening the file: " << filename << std::endl;
    }
}

void NBCAlgorithm::writeResultOut(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << "id;";
        for (int i=0; i<data[0].coords.size(); i++)
        {
            file << i+1<<" wspol.;";
        }
        file << "l. obliczen;";
        file << "typ punktu;";
        file << "CId;";
        file << std::endl;
        for (int i=0; i<data.size(); i++)
        {
            file << data[i].id << ";";
            for (int j=0; j<data[i].coords.size(); j++)
            {
                file << data[i].coords[j] << ";";
            }
            file << data[i].no_calc << ";";
            file << data[i].point_type << ";";
            file << data[i].c_id << ";";
            file << std::endl;
        }
        file.close();
        std::cout << "Zapisano wyniki do pliku: " << filename << std::endl;
    }
    else
    {
        std::cout << "Error opening the file: " << filename << std::endl;
    }
}


void NBCAlgorithm::writeResultStat(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        int no_all_calc_dist=0;
        for (int i=0; i<data.size(); i++)
        {
            no_all_calc_dist = no_all_calc_dist + data[i].no_calc;
        }
        avg_no_calc_dist = double(no_all_calc_dist)/data.size();

        total_time = total_time_minus_load_save + load_file_time + save_files_time;

        file << "nazwa_pliku_wejsciowego;";
        file << data_filename << std::endl;
        file << "liczba_wym_punktow;";
        file << data[0].coords.size() << std::endl;
        file << "l_punktow;";
        file << data.size() << std::endl;
        file << "k;";
        file << k << std::endl;
        file << "czas_wczytania_pliku_wej [us];";
        file << load_file_time << std::endl;
        file << "czas_normalizacji [us];";
        file << double(normalize_time)/1000 << std::endl;
        file << "calkowity_czas_sortowania [us];";
        file << sort_time << std::endl;
        file << "czas_obliczania_kNN [us];";
        file << calcKNN_time << std::endl;
        file << "czas_obliczania_RkNN [us];";
        file << calcKNN_time << std::endl;
        file << "czas_grupowania [us];";
        file << clustering_time << std::endl;
        file << "czas_zapisywania_plikow_out_kNN [us];";
        file << save_files_time << std::endl;
        file << "calkowity_czas_dzialania [us];";
        file << total_time << std::endl;
        file << "calkowity_czas_dzialania-czas_zapisywania_plikow [us];";
        file << total_time - save_files_time << std::endl;
        file << "l_odkrytych_grup;";
        file << no_clusters << std::endl; 
        file << "l_odkrytych_p_szumu;";
        file << no_outlier_p << std::endl;
        file << "l_odkrytych_p_rdzeniowych;";
        file << no_core_p << std::endl; 
        file << "l_odkrytych_p_brzegowych;";
        file << no_border_p << std::endl;
        file << "srednia_l_obl_odl;";
        file << avg_no_calc_dist << std::endl; 
        file << "|TP|;";
        file << TP << std::endl;
        file << "|TN|;";
        file << TN << std::endl;
        file << "l_par_punktow;";
        file << no_pairs << std::endl;
        file << "RAND;";
        file << RAND << std::endl;

        file.close();
        std::cout << "Zapisano wyniki do pliku: " << filename << std::endl;
    }
    else
    {
        std::cout << "Error opening the file: " << filename << std::endl;
    }
}
