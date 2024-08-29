#include "TiCoskNNAlgorithm.h"
#include "DataSample.h"

TiCoskNNAlgorithm::TiCoskNNAlgorithm(int k, std::vector<DataSample> data, std::chrono::microseconds::rep& sort_time, std::vector<double> ref_coords):
    k(k), data(data), sort_time(&sort_time), ref_coords(ref_coords){
}

TiCoskNNAlgorithm::~TiCoskNNAlgorithm()
{

}

std::vector<DataSample> TiCoskNNAlgorithm::calcKNN()
{
    distances_to_ref.clear();

    calcDistances();

    std::chrono::steady_clock::time_point begin_sort = std::chrono::steady_clock::now();
    sort(distances_to_ref.begin(), distances_to_ref.end(), compareDist);
    std::chrono::steady_clock::time_point end_sort = std::chrono::steady_clock::now();
    *sort_time = std::chrono::duration_cast<std::chrono::microseconds>(end_sort - begin_sort).count();

    for (int i = 0; i < distances_to_ref.size(); i++)
    {
        tiKNN(i);
    }
    return data;
}

void TiCoskNNAlgorithm::calcDistances()
{
    distances_to_ref.clear();
    for (int j=0; j<data.size(); j++)
    {
        double dist = calcDistanceToRef(data[j].coords);
        distances_to_ref.push_back(DistanceFrom(j, dist));
        // liczba obliczeń na punkcie
        data[j].no_calc++;
    }
}

double TiCoskNNAlgorithm::calcDistanceToRef(std::vector<double> coords1)
{
    if(coords1.size() != ref_coords.size())
		return -1;
    else
    {
		double dist = 0;
		for (int k = 0; k < coords1.size(); ++k)
			dist += (coords1[k] - ref_coords[k])*(coords1[k] - ref_coords[k]);
		return sqrt(dist);
    }
}

double TiCoskNNAlgorithm::calcDistance(std::vector<double> coords1, std::vector<double> coords2)
{
    if(coords1.size() != coords2.size())
		return -1;
    else
    {
		double dist = 0;
		for (int k = 0; k < coords1.size(); ++k)
			dist += coords1[k]*coords2[k];
		return dist;
    }
}

void TiCoskNNAlgorithm::displayDist(std::vector<DistanceFrom> distaces)
{
    for (int j = 0; j < distaces.size(); ++j)
    {
		std::cout<<distaces[j].id_data_sample<<"; "<<distaces[j].dist<<std::endl;  
    }
}

void TiCoskNNAlgorithm::tiKNN(int id_point)
{
    no_nn = 0;
    distances.clear();
    eps = 0;

    int id_back = id_point;
    int id_forw = id_point;
    bool backwardSearch = precedingPoint(id_back);
    bool forwardSearch = followingPoint(id_forw);
    findFirstkCandidateForwBack(id_point, id_back, id_forw, backwardSearch, forwardSearch);
    findFirstkCandidateBack(id_point, id_back, backwardSearch);
    findFirstkCandidateForw(id_point, id_forw, forwardSearch);
    eps_cos = findMin(distances);
    convert_eps_from_cos();

    int id_point_true = distances_to_ref[id_point].id_data_sample;
    data[id_point_true].maxEps = eps;

    verifykCandidateBack(id_point, id_back, backwardSearch);
    verifykCandidateForw(id_point, id_forw, forwardSearch);
     
    data[id_point_true].eps = eps;
    for (int j=0; j<distances.size(); j++)
    {
        int id_neigh = distances[j].id_data_sample;
        data[id_point_true].kNN.push_back(id_neigh);
        data[id_neigh].n_RkNN++;
    }
}

double TiCoskNNAlgorithm::findMin(std::vector<DistanceFrom> vec)
{
    auto it = std::max_element(vec.begin(),
                             vec.end(),
                             [](const DistanceFrom& a,const DistanceFrom& b) { return a.dist > b.dist; });                 
    if (it == vec.end()) throw "max_element called on emtpy vector";
    auto max = *it;
    return max.dist;
}

bool TiCoskNNAlgorithm::precedingPoint(int &id_back)
{
    if (id_back > 0)
    {
        id_back = id_back - 1;
        return true;
    }
    else    
        return false;
}

bool TiCoskNNAlgorithm::followingPoint(int &id_forw)
{
    if (id_forw < data.size()-1)
    {
        id_forw = id_forw + 1;
        return true;
    }
    else    
        return false;
}

void TiCoskNNAlgorithm::findFirstkCandidateForwBack(int &id_point, int &id_back, int &id_forw, bool &backwardSearch, bool &forwardSearch)
{
    while (backwardSearch && forwardSearch && no_nn < k)
    {
        if (distances_to_ref[id_point].dist - distances_to_ref[id_back].dist < distances_to_ref[id_forw].dist - distances_to_ref[id_point].dist)
        {
            int id_point_true = distances_to_ref[id_point].id_data_sample;
            int id_back_true = distances_to_ref[id_back].id_data_sample;
            double dist = calcDistance(data[id_point_true].coords, data[id_back_true].coords);
            data[id_point_true].no_calc++;
            data[id_back_true].no_calc++;
            no_nn++;
            distances.push_back(DistanceFrom(id_back_true, dist));
            backwardSearch = precedingPoint(id_back);
        }
        else
        {
            int id_point_true = distances_to_ref[id_point].id_data_sample;
            int id_forw_true = distances_to_ref[id_forw].id_data_sample;
            double dist = calcDistance(data[id_point_true].coords, data[id_forw_true].coords);
            data[id_point_true].no_calc++;
            data[id_forw_true].no_calc++;
            no_nn++;
            distances.push_back(DistanceFrom(id_forw_true, dist));
            forwardSearch = followingPoint(id_forw);
        }
    }
}

void TiCoskNNAlgorithm::findFirstkCandidateBack(int &id_point, int &id_back, bool &backwardSearch)
{
    while (backwardSearch && no_nn < k)
    {
        int id_point_true = distances_to_ref[id_point].id_data_sample;
        int id_back_true = distances_to_ref[id_back].id_data_sample;
        double dist = calcDistance(data[id_point_true].coords, data[id_back_true].coords);
        data[id_point_true].no_calc++;
        data[id_back_true].no_calc++;
        no_nn++;
        distances.push_back(DistanceFrom(id_back_true, dist));
        backwardSearch = precedingPoint(id_back);
    }
}

void TiCoskNNAlgorithm::findFirstkCandidateForw(int &id_point, int &id_forw, bool &forwardSearch)
{
    while (forwardSearch && no_nn < k)
    {
        int id_point_true = distances_to_ref[id_point].id_data_sample;
        int id_forw_true = distances_to_ref[id_forw].id_data_sample;
        double dist = calcDistance(data[id_point_true].coords, data[id_forw_true].coords);
        data[id_point_true].no_calc++;
        data[id_forw_true].no_calc++;
        no_nn++;
        distances.push_back(DistanceFrom(id_forw_true, dist));
        forwardSearch = followingPoint(id_forw);
    }
}

void TiCoskNNAlgorithm::verifykCandidateBack(int &id_point, int &id_back, bool &backwardSearch)
{

    while (backwardSearch && ((distances_to_ref[id_point].dist - distances_to_ref[id_back].dist) <= eps))
    {
        int id_point_true = distances_to_ref[id_point].id_data_sample;
        int id_back_true = distances_to_ref[id_back].id_data_sample;
        double dist = calcDistance(data[id_point_true].coords, data[id_back_true].coords);
        data[id_point_true].no_calc++;
        data[id_back_true].no_calc++;
        if (dist > eps_cos)
        {
            int no_max_neigh = calcNoMaxNeigh();
            if (distances.size() - no_max_neigh >= (k - 1))
            {
                distances.erase(
                    std::remove_if(distances.begin(), distances.end(), [this](const DistanceFrom& distFrom) {
                    return distFrom.dist == eps_cos;
                    }),
                    distances.end());
                distances.push_back(DistanceFrom(id_back_true, dist));
                eps_cos = findMin(distances);
                convert_eps_from_cos();
            }
            else
            {
                distances.push_back(DistanceFrom(id_back_true, dist));
            }
        }
        else if (dist == eps_cos)
        {
            distances.push_back(DistanceFrom(id_back_true, dist));
        }

        backwardSearch = precedingPoint(id_back);
    }
}

void TiCoskNNAlgorithm::verifykCandidateForw(int &id_point, int &id_forw, bool &forwardSearch)
{
    while (forwardSearch && ((distances_to_ref[id_forw].dist - distances_to_ref[id_point].dist) <= eps))
    {
        int id_point_true = distances_to_ref[id_point].id_data_sample;
        int id_forw_true = distances_to_ref[id_forw].id_data_sample;
        double dist = calcDistance(data[id_point_true].coords, data[id_forw_true].coords);
        data[id_point_true].no_calc++;
        data[id_forw_true].no_calc++;
        if (dist > eps_cos)
        {
            int no_max_neigh = calcNoMaxNeigh();
            if (distances.size() - no_max_neigh >= (k - 1))
            {
                distances.erase(
                    std::remove_if(distances.begin(), distances.end(), [this](const DistanceFrom& distFrom) {
                    return distFrom.dist == eps_cos;
                    }),
                    distances.end());
                distances.push_back(DistanceFrom(id_forw_true, dist));
                eps = findMin(distances);
                convert_eps_from_cos();
            }
            else
            {
                distances.push_back(DistanceFrom(id_forw_true, dist));
            }
        }
        else if (dist == eps_cos)
        {
            distances.push_back(DistanceFrom(id_forw_true, dist));
        }

        forwardSearch = followingPoint(id_forw);
    }
}

int TiCoskNNAlgorithm::calcNoMaxNeigh()
{
    int no_max_neigh = 0;
    for (int j=0; j<distances.size(); j++)
    {
        if (distances[j].dist == eps_cos)
            no_max_neigh++;
    }
    return no_max_neigh;
}

void TiCoskNNAlgorithm::writeResultkNN(const std::string& filename)
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

void TiCoskNNAlgorithm::convert_eps_to_cos()
{
    eps_cos = 1 - pow(eps, 2)/2;
}

void TiCoskNNAlgorithm::convert_eps_from_cos()
{
    eps = sqrt(2-2*eps_cos);
}