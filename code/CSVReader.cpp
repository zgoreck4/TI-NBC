#include "CSVReader.h"
#include "DataSample.h"

std::vector<DataSample> CSVReader::getData(const std::string& filename, std::chrono::microseconds::rep& load_file_time, std::chrono::nanoseconds::rep& normalize_time) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    std::ifstream file(filename);
    std::vector<DataSample> data;
    std::string line = "";

    int id = 0;
    while (getline(file, line))
    {
        std::vector<double> coords;
        coords = this->split(line, ",");

        std::chrono::steady_clock::time_point begin_normalize = std::chrono::steady_clock::now();
        coords = normalizeCoords(coords);
        std::chrono::steady_clock::time_point end_normalize = std::chrono::steady_clock::now();
        normalize_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_normalize - begin_normalize).count();

        DataSample sample(id, coords);
        data.push_back(sample);
        id++;
    }
    file.close();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    load_file_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    return data;
}

std::vector<double> CSVReader::split(std::string target, std::string delim)
{
    std::vector<double> v;
    if (!target.empty()) {
        size_t start = 0;
        do {
            size_t x = target.find(delim, start);
            if (x == -1)
            {
                break;
            }
            std::string tmp = target.substr(start, x - start);
            v.push_back(stod(tmp));
            start += delim.size() + tmp.size();
        } while (true);
        v.push_back(stod(target.substr(start)));
    }
    return v;
};

std::vector<double> CSVReader::normalizeCoords(std::vector<double> coords)
{
    double len = calcLen(coords);
    for (int i=0; i<coords.size(); i++)
        coords[i] = coords[i]/len;
    return coords;
}

double CSVReader::calcLen(std::vector<double> coords)
{
    double len = 0;
    for (int i=0; i<coords.size(); i++)
        len += pow(coords[i], 2);
    return sqrt(len);
}

std::vector<int> CSVReader::getClasses(const std::string& filename)
{
    std::ifstream file(filename);
    std::vector<DataSample> data;
    std::string line = "";

    std::vector<int> classes;
    while (getline(file, line))
    {
        
        classes.push_back(std::stoi(line));
    }
    file.close();
    return classes;
}