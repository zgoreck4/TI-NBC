#ifndef CSVREADER_H
#define CSVREADER_H

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

class DataSample;

#pragma once

class CSVReader
{
	public:
		std::vector<DataSample> getData(const std::string& filename, std::chrono::microseconds::rep& load_file_time, std::chrono::microseconds::rep& normalize_time);
		std::vector<int> getClasses(const std::string& filename);

	private:
		std::string filename;

		std::vector<double> split(std::string target, std::string delim);
		std::vector<double> normalizeCoords(std::vector<double> coords);
		double calcLen(std::vector<double> coords);
};


#endif