#include <fstream>
#include <string>
#include <regex>
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>

using namespace std;

/// If negative, we couldn't find an old style include guard, which is
/// a reasonable outcome, and not necessarily an exception.
/// Otherwise returns the line number at which the include guard is found.
/// An old school include guard is defined as:
///     #ifndef XXX
///     #define XXX
/// i.e. it's always over 2 lines, we return the line number (zero
/// indexed) of the first
int find_include_guard(ifstream& ifstrm)
{
	string line;
	unsigned int counter;
	smatch ifndef_match;
	regex ifndef_regex{"^#ifndef (.*)"};
	while (getline(ifstrm, line))
	{
		if(regex_search(line, ifndef_match,  ifndef_regex))
		{
			if (!getline(ifstrm, line))
				throw runtime_error("ifndef at eof");
			stringstream drs;
			drs << "^#define " << ifndef_match[0];
			regex define_regex{drs.str()};
			if (regex_search(line, define_regex))
				return counter;
			++counter;
		}
		++counter;
	}
	return -1;
}

/// Returns the number of lines parsed over until the closing endif is
/// found.  Throws an exception if none is found.
unsigned int find_closing_endif(ifstream& instrm)
{
	unsigned int counter{0};
	regex closing_regex{"^#endif"};
	std::string line;
	while (getline(instrm, line))
	{
		if(regex_search(line, closing_regex))
		{
			return counter;
		}
		++counter;
	}
	throw std::runtime_error("Did not find closing endif");
	return 0; // silence warnings.
}

void write_modified_file(const string& fname,
	unsigned int include_guard,
	unsigned int closing_endif)
{
	unsigned int counter{0};
	ofstream outfile(fname + ".tmp");
	ifstream infile (fname);
	string discard;
	string line;
	while (getline(infile, line))
	{
		if(counter==include_guard)
		{
			getline(infile, discard);
			getline(infile, discard);
			++counter;
		}else if(counter==closing_endif){
			getline(infile, discard);
		}
		else{
			outfile << line;
		}
		++counter;
	}
}

/// Change include guards in filename to "#pragma once
void change_include_guards(const string fname)
{
	ifstream infile(fname);
	int include_guard = find_include_guard(infile);
	if ( include_guard < 0 )
	{
		cerr << fname << "does not have an old-school include guard\n";
		return;
	}
	unsigned int closing_endif = find_closing_endif(infile);
	infile.close();
	std::string ofname = fname + ".tmp";
	write_modified_file(fname, include_guard, closing_endif);
	boost::filesystem::rename(fname, ofname);
}

int main (int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage is 'cig <filenames>\n'.";
		exit 1;
	}
	try
	{
		for (size_t i=0; i<argc; ++i)
			change_include_guards(argv[i]);
	}
	catch (exception&e)
	{
		cerr << "An exception was caught.\n";
		cerr << e.what() <<"\n";
		return 1;
	}
	return 0;
}
