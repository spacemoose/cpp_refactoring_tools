#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <boost/filesystem.hpp>

namespace filestuff {

void finalize_file(std::string fname, std::vector<std::string> lines)
{
    std::string temp_fname = fname + ".tmp";
    std::ofstream outfile(fname);
    outfile << "#pragma once\n";
    for (auto line : lines)
	{
		outfile << line << "\n";
	}
    boost::filesystem::rename( temp_fname,fname);
}


std::vector<std::string> read_file(const std::string& fname)
{
    std::fstream infile(fname);
    std::vector<std::string> lines;
    std::string line;
    while (getline(infile, line))
		lines.push_back(line);
    return lines;
}

}
