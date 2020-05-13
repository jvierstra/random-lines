#ifndef _RNG_HH_
#define _RNG_HH_

#include <ctime>
#include <climits>

/* Implementation of the TWISTER algorithm */

namespace math {
	class random {
	public:
		random(unsigned long seed = -1) { 
			
			if(seed == -1) {

				static int diff = 0;

				time_t t = time(NULL);
				clock_t c = clock();
			
				int h1 = 0;
				unsigned char* p1 = (unsigned char*) &t;
			
				for(size_t i = 0; i < sizeof(t); ++i) {
					h1 *= UCHAR_MAX + 2U;
					h1 += p1[i];
				}
				int h2 = 0;
				unsigned char* p2 = (unsigned char*) &c;
				for(size_t j = 0; j < sizeof(c); ++j) {
					h2 *= UCHAR_MAX + 2U;
					h2 += p2[j];
				}

				init((h1 + diff++) ^ h2);
				
			} else {

				init(seed);
			}
		}
		
		/*
		random() {	

			static int diff = 0;

			time_t t = time(NULL);
			clock_t c = clock();
			
			int h1 = 0;
			unsigned char* p1 = (unsigned char*) &t;
			for(size_t i = 0; i < sizeof(t); ++i) {
				h1 *= UCHAR_MAX + 2U;
				h1 += p1[i];
			}
			int h2 = 0;
			unsigned char* p2 = (unsigned char*) &c;
			for(size_t j = 0; j < sizeof(c); ++j) {
				h2 *= UCHAR_MAX + 2U;
				h2 += p2[j];
			}
			
			unsigned long seed = (h1 + diff++) ^ h2;
			
			init(seed);
		}
		*/

		operator int() {
			return sample();
		}
		
		operator unsigned long() {
			return sample();
		}
		
		operator long() {
			return long(sample() >> 1);
		}

		operator double() {
			return sample() * (1.0 / 4294967295.0);
		}	
		
	protected:
		void init(unsigned long seed) {
			mt[0] = seed & 0xffffffffUL;
			for(index = 1; index < 624; ++index) {
				mt[index] = (1812433253UL * (mt[index-1] ^ (mt[index-1] >> 30)) + index);
				mt[index] &= 0xffffffffUL;
			}
		}

		void generate() {			
			unsigned long x;
			static unsigned long mag01[2]={0x0UL, MATRIX_A};

			int i;	
			for(i = 0; i < N - M; ++i) {
				x = (mt[i] & UPPER_MASK) | (mt[i + 1] & LOWER_MASK);
				mt[i] = mt[i + M] ^ (x >> 1) ^ mag01[x & 0x1UL];
			}
			for(; i < N - 1; ++i) {
				x = (mt[i] & UPPER_MASK) | (mt[i + 1] & LOWER_MASK);
				mt[i] = mt[i + M - N] ^ (x >> 1) ^ mag01[x & 0x1UL];
			}
			x = (mt[N - 1] & UPPER_MASK) | (mt[i + 1] & LOWER_MASK);
			mt[N - 1] = mt[M - 1] ^ (x >> 1) ^ mag01[x & 0x1UL];
	
			index = 0;
		}	

		unsigned long sample() {
			if(index >= N) {
				 generate();
			}

			unsigned long x = mt[index++];
			x ^= (x >> 11);
			x ^= (x << 7) & 0x9d2c5680UL;
			x ^= (x << 15) & 0xefc60000UL;
			x ^= (x >> 18);
			return x;
		}

		static const int N = 624;
		static const int M = 397;

		static const unsigned long MATRIX_A   = 0x9908b0dfUL;
		static const unsigned long UPPER_MASK = 0x80000000UL;
		static const unsigned long LOWER_MASK = 0x7fffffffUL;
	
		unsigned long mt[624];
		int index;
	};

}

#endif

