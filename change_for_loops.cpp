#include "basic_stuff.hpp"
#include "../pscpp/cpp_parser.hpp"

#include "algos.hpp"

using namespace std;

bool for_loop_matcher(const cpp_block& b)
{
}

void change_for_loops(string fname)
{
	cpp_block file_block(fname);
	std::vector<cpp_block> for_blocks;
	algos::flatten_if(file_block.begin(), file_block.end(),
		std::back_inserter(file_blocks), for_loop_matcher);
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
	change_for_loops(argv[i]);
    }
  catch (exception& e)
    {
      cerr << "An exception was caught.\n";
      cerr << e.what() <<"\n";
      return 1;
    }
  return 0;
}
