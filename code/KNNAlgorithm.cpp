#include "KNNAlgorithm.h"
#include "DataSample.h"

KNNAlgorithm::KNNAlgorithm(int k, std::vector<DataSample> data, std::chrono::microseconds::rep& sort_time):
    k(k), data(data), sort_time(&sort_time){
}

KNNAlgorithm::~KNNAlgorithm()
{

}

std::vector<DataSample> KNNAlgorithm::calcKNN()
{
    *sort_time = 0;
    for (int i = 0; i < data.size(); i++)
    {
        calcDistances(i);

        choosekNN(i);
    }
    return data;
}

void KNNAlgorithm::calcDistances(int id_data_sample)
{
    distances.clear();
    for (int j=0; j<data.size(); j++)
    {
        if(j != id_data_sample)
        {
            double dist = calcDistance(data[id_data_sample].coords, data[j].coords);
            distances.push_back(DistanceFrom(j, dist));
            // liczba obliczeń na punkcie
            data[id_data_sample].no_calc++;
            data[j].no_calc++;
        }
    }
}

double KNNAlgorithm::calcDistance(std::vector<double> coords1, std::vector<double> coords2)
{
    if(coords1.size() != coords2.size())
		return -1;
    else
    {
		double dist = 0;
		for (int k = 0; k < coords1.size(); ++k)
			dist += (coords1[k] - coords2[k])*(coords1[k] - coords2[k]);
		return sqrt(dist);
    }
}

void KNNAlgorithm::displayDist()
{
    for (int j = 0; j < distances.size(); ++j)
    {
		std::cout<<distances[j].id_data_sample<<"; "<<distances[j].dist<<std::endl;  
    }
}

void KNNAlgorithm::choosekNN(int id_data_sample)
{
    std::chrono::steady_clock::time_point begin_sort = std::chrono::steady_clock::now();
    sort(distances.begin(), distances.end(), compareDist);
    std::chrono::steady_clock::time_point end_sort = std::chrono::steady_clock::now();
    *sort_time += std::chrono::duration_cast<std::chrono::microseconds>(end_sort - begin_sort).count();

    data[id_data_sample].maxEps = distances[distances.size()-1].dist;

    int id_data_sample_from;
    for(int j=0; j<k; j++)
    {
        id_data_sample_from = distances[j].id_data_sample;
        data[id_data_sample].addTokNN(id_data_sample_from);
        data[id_data_sample_from].n_RkNN++;
    }
    data[id_data_sample].eps = distances[k-1].dist;
    // dodanie dodatkowych sąsiadów, którzy mają odległość = odległości przyłączonego k-tego sąsiada
    if (distances.size()>k)
    {
        double dist_next;
        for(int l=0; l<distances.size(); l++)
        {
            dist_next = distances[k+l].dist;
            double eps = data[id_data_sample].eps;
            if (dist_next==eps)
            {
                id_data_sample_from = distances[k+l].id_data_sample;
                data[id_data_sample].addTokNN(id_data_sample_from);
                data[id_data_sample_from].n_RkNN++;
            }
            else
                break;
        }
    }
}


void KNNAlgorithm::writeResultkNN(const std::string& filename)
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