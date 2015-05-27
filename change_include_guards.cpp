#include "filestuff.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

template <typename iterT>
bool has_matching_define( iterT line)
{
  regex subber_regex { "^#ifndef(.*)"};
  string defstr = regex_replace(*line, subber_regex, "#define");
  regex define_regex{defstr};
  ++line;
  if (regex_search(*line, define_regex))
    return true;
  return false;
}

/// return false if unable to remove include guard.
bool remove_include_guard(vector<string>& lines)
{
  const auto ifndef_pos =
    find_if(lines.begin(), lines.end(), [](string l) {
	return regex_match(l.begin(), l.end(), regex{"#ifndef (.*)"}); });
  if (ifndef_pos == lines.end())
    return false; 
  if (!has_matching_define(ifndef_pos))
    {
      auto temp = ifndef_pos;
      cerr << "warning:  found :" << *ifndef_pos << ", but following line is: "
	   << *(ifndef_pos+1)  << "which doesn't fit include guard pattern.\n";
      return false;
    }
  lines.erase(ifndef_pos, ifndef_pos+2);
  return true;
}

// return false if couldn't remove endif statement.
bool remove_endif(vector<string>& lines)
{
  auto closing_pos =
    find_if(lines.rbegin(), lines.rend(), [](string& line)
	    {return regex_match(line.begin(), line.end(), regex{"#endif.*"});} );
  if (closing_pos == lines.rend())
    {
      cerr << "Warning, found : " << *closing_pos << "but couldn't find a closing #endif.\n";
      return false;
    }
  lines.erase((--closing_pos.base()));
  return true;
}

/// Change include guards in filename to "#pragma once
void change_include_guards(const string fname)
{
  vector<string> lines = filestuff::read_file(fname);
  if (remove_include_guard(lines))
    if( remove_endif(lines) )
      filestuff::finalize_file(fname, lines);
}

int main (int argc, char** argv)
{
  if (argc < 2)
    {
      std::cerr << "Usage is 'cig <filenames>\n'.";
      exit(1);
    }
  try
    {
      for (size_t i=1; i<argc; ++i)
	change_include_guards(argv[i]);
    }
  catch (exception& e)
    {
      cerr << "An exception was caught.\n";
      cerr << e.what() <<"\n";
      return 1;
    }
  return 0;
}
