#include <fstream>

#include "rng.hh"
#include "functions.hh"
#include "sequential_sampler.hh"
#include "options.hh"

int main(int argc, const char* argv[]) {
	
	int n = 1, N = -1, s = -1;
	
	std::ios_base::sync_with_stdio(false);

	std::cin.exceptions( std::ios::badbit | std::ios::eofbit | std::ios::failbit );

	misc::options::parser opts("random-lines-pairs", "output paired random lines", "");
	opts.add_store_option('n', "num", "number of lines to return", n, "1", true);	
	opts.add_store_option('N', "max", "Total lines in the file", N, "4294967295", true);
	opts.add_store_option('s', "seed", "seed for random number generator", s); 
	opts.parse(argv, argv + argc);

	if (n >= N) {
		std::cerr << "ERROR: The number of lines to return must be less than the total lines in the file!" << std::endl;
		return 1;
	}

	if(N%2>0) {
		std::cerr << "ERROR: THe number of total lines must be EVEN" << std::endl;
		return 1;
	}

	math::random rng(s);
	math::sequential_sampler samp(n, N/2, rng);

	size_t seekline, currline = 0;		
	std::string line1, line2;

	for (int i = 0; i < n; ++i) {

		seekline = samp.sample() * 2;

		while (currline < seekline) {
			
			try {
				
				std::getline(std::cin, line1);
				++currline;
				
				std::getline(std::cin, line2);
				++currline;

			} catch (std::ifstream::failure e) {

				std::cerr << "ERROR: Prematurely reached the end of the file stream! -- check if the total lines is set correctly" << std::endl;

				return 1;

			}

		}

		std::cout << line1 << std::endl;
		std::cout << line2 << std::endl;
	}
	
	return 0;
}
