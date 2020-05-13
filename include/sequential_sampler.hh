#ifndef _SEQUENTIAL_SAMPLER_H_
#define _SEQUENTIAL_SAMPLER_H_

#include <cmath>
#include <iostream>

#include "rng.hh"

namespace math {

class sequential_sampler {
public:
	sequential_sampler(long n, long N, math::random& rng)
	: n(n), N(N), rng(rng) {
		
		i = 0;
		threshold = 13 * n;
				
		vitter87_method_a_init = false;
		vitter87_method_d_init = false;
	
	}

	long sample() {
		if (n > 1 and threshold < N) {
			vitter87_method_d();
		} else if (n > 1) {
			vitter87_method_a();
		} else if (n == 1) {
			if(vitter87_method_a_init) {
				s = long(std::floor(double(N) * double(rng)));
			} else {
				s = long(std::floor(double(N) * v_prime));
			}
			i += s + 1;
		} else {
			return 0;
		}

		return i;
	}

private:
	void vitter87_method_a() {
		
		//init
		if (not vitter87_method_a_init) {
			top = N - n;
			
			vitter87_method_a_init = true;
		}


		v = double(rng);
		s = 0;
		quot = top / double(N);

		while (quot > v) {
			++s;
			--top;
			--N;
			quot *= top / double(N);
		}

		//
		i += s + 1;
		
		//
		--N;
		--n;
	}
	
	void vitter87_method_d() {

		if (not vitter87_method_d_init) {
			v_prime = std::exp(std::log(double(rng)) / double(n));
        		qu1 = N - n + 1;
														
			vitter87_method_d_init = true;
		}

		while(1) {
			
			while(1) {
				x = double(N) * ( 1.0 - v_prime);
				s = long(std::floor(x));
				if (s < qu1) break;
				v_prime = std::exp(std::log(double(rng)) / double(n));
			}

			u = double(rng);
			
			y1 = std::exp(std::log(u * double(N) / double(qu1)) / double(n - 1));
			v_prime = y1 * ((-x / double(N)) + 1.0) * double(qu1) / double(qu1 - s);
			if (v_prime <= 1.0) break;

			y2 = 1.0;
			top = double(N - 1);
			if (n-1 > s) {
				bottom = double(N - n);
				limit = N - s;
			} else {
				bottom = double(N - s - 1);
				limit = qu1;
			}
			for (long t = N - 1; t >= limit; --t) {
				y2 *= top / bottom;
				--top;
				--bottom;
			}
			if (double(N) / (double(N) - x) >= y1 * std::exp(std::log(y2) / double(n - 1))) {
				v_prime = std::exp(std::log(double(rng)) / double(n - 1));
				break;
			}

			v_prime = std::exp(std::log(double(rng)) / double(n));
		}

		//
		i += s + 1;

		//
		N = N - s - 1;
		--n;
		
		qu1 -= s;
		threshold -= 13;
	
	}

private:
	long n, N, i;

	math::random& rng;
	
	bool vitter87_method_a_init, vitter87_method_d_init;

	long s;
	double top, bottom;
	
	//
	double v, quot;

	//
	long qu1, limit, threshold;
	double x, u, v_prime, y1, y2;
};

}

#endif
