#ifndef _FUNCTIONS_HH_
#define _FUNCTIONS_HH_

#include <cmath>
#include <stdexcept>
#include <cstdarg>

namespace math {
	
	template<typename T, int nargs>
    inline T min(T x, ...) {
		va_list arguments;
        T val, best = x;
        va_start(arguments, x);
        for(int i=1; i<nargs; ++i) {
			val = va_arg(arguments, T);
            if(val < best) {
                best = val;
			}
		}
        va_end(arguments);
        return best;
	}
	
	//argmax
	template<typename T, int nargs>
	inline T max(T x, ...) {
		va_list arguments;
		T val, best = x;
		va_start(arguments, x);
		for(int i=1; i<nargs; ++i) {
			val = va_arg(arguments, T);
			if(val > best) {
				best = val;
			}
		}
		va_end(arguments);
		return best;
	}
	
	
	//argmin
	template<typename T, int nargs>
    inline T argmin(T x, ...) {
		va_list arguments;
        T val, best = x, result = 0;
        va_start(arguments, x);
        for(int i=1; i<nargs; ++i) {
			val = va_arg(arguments, T);
            if(val < best) {
				result = i;
				best = val;
			}
		}
        va_end(arguments);
        return result;
	}

	//argmax
	template<typename T, int nargs>
	inline T argmax(T x, ...) {
		va_list arguments;
		T val, best = x, result = 0;
		va_start(arguments, x);
		for(int i=1; i<nargs; ++i) {
			val = va_arg(arguments, T);
			if(val > best) {
				result = i;
				best = val;
			}
		}
		va_end(arguments);
		return result;
	}
	
	//adapter functions for STL
	template<typename T>
	struct plus : public std::unary_function<T, void> {
		plus(T val) : val(val) {}
		void operator()(T& other) {
			other += val; 
		}
		T val;
	};
	
	template<typename T>
	struct multiplies : std::unary_function<T, void> {
		multiplies(T val) : val(val) {}
		void operator()(T& other) {
			other *= val;
		}
		T val;
	};	
	
	const static double pi = 3.14159265358979323846;
	const static double ne = 2.71828182845904523536;

	template<typename T>
	inline  T sqr(const T x) {
		return x*x;
	}

	namespace lanczos {
		//lanczos approximation parameters
		const static double g = 7;
		const static double c[9] = {0.99999999999980993, 676.5203681218851, -1259.1392167224028,
    	 771.32342877765313, -176.61502916214059, 12.507343278686905,
    	 -0.13857109526572012, 9.9843695780195716e-6, 1.5056327351493116e-7};
  }

	inline double gamma(double z) {
		if(z < 0.5) {
			return (pi / (std::sin(pi * z) * gamma(1 - z)));
		} else {
			z -= 1;
			double a = lanczos::c[0];
			for(unsigned int i = 1; i < 9; ++i) {
				a += lanczos::c[i] / (z + i);
			}
			double zgh = z + lanczos::g + 0.5;
			return std::sqrt(2 * pi) * std::pow(zgh, z + 0.5) * std::exp(-zgh) * a;
		}
	}
		
	inline double lngamma(double z) {
		if(z < 0.5) {
			return std::log(pi) - std::log(std::sin(pi * z)) - lngamma(1 - z);
		} else {
			z -= 1;
			double zgh = z + lanczos::g + 0.5;
			double a = lanczos::c[0];
			for(unsigned int i = 1; i < 9; ++i) {
				a += lanczos::c[i] / (z + i);
			}
			return (z + 0.5) * std::log(zgh) - zgh + std::log(std::sqrt(2 * pi) * a); 
		}
	}

	inline double incomplete_gamma_series(double a, double x) {
		double res;		
		if(x < 0) {
			throw std::runtime_error("incomplete_gamma_series: x cannot be less than zero!");
		}	else if (x == 0) {
			res = 0;
		} else {
			double ap, del, sum;
			ap = a;
			del = sum = 1 / a;
			for(unsigned int n = 1; n < 100; ++n) {
				++ap;
				del *= x / ap;
				sum += del;
				if(std::fabs(del) < std::fabs(sum) * 3.0e-7) {
					return  sum * std::exp(-x + a * std::log(x) - lngamma(a));
				}
			}
		}
		return res;
	}

	inline double incomplete_gamma_cf(double a, double x) {
		double an, b, c, d, del, h;
		b = x + 1 - a;
		c = 1 / 1e-30;
		d = 1 / b;
		h = d;
		
		for(unsigned int i = 1; i < 100; ++i) {
			an = -i * (i - a);
			b += 2;
			d = an * d + b;
			if(std::fabs(d) < 1e-30) 
				d = 1e-30;
			c = b + an / c;
			if(std::fabs(c) < 1e-30) 
				c = 1e-30;
			d = 1 / d;
			del = d * c;
			h *= del;
			if(std::fabs(del - 1) < 3e-7) break;
		}
		return std::exp(-x + a * std::log(x) - lngamma(a)) * h;
	}

	inline double lower_incomplete_gamma(double a, double x) {
		if(x < 0 or a <= 0) {
			throw std::runtime_error("lower_incomplete_gamma: arguments must be greater than zero!");
		} else if (x < (a + 1)) {
			return incomplete_gamma_series(a, x);
		} else {
			return 1 - incomplete_gamma_cf(a, x);
		}
	}

	inline double upper_incomplete_gamma(double a, double x) {
		if(x < 0 or a <= 0) {
			throw std::runtime_error("upper_incomplete_gamma: arguments must be greater than zero!");
		} else if(x < (a + 1)) {
			return 1 - incomplete_gamma_series(a, x);
		} else {
			return incomplete_gamma_cf(a, x);
		}
	}

	inline double erf(double x) {
		return (x > 0) ? -lower_incomplete_gamma(0.5, sqr(x)) : lower_incomplete_gamma(0.5, sqr(x));
	}
	
	inline double erfc(double x) {
		return (x > 0) ? 1 + lower_incomplete_gamma(0.5, sqr(x)) : lower_incomplete_gamma(0.5, sqr(x));
	}

	inline double factorial(unsigned int x) {
		static unsigned int n = 4;
		static double m[32] = {1, 1, 2, 6, 24};
		if(x < 0) {
			throw std::runtime_error("factorial: cannot compute factorial less than zero!");		
		} else if(x > 32) {
			return std::exp(lngamma(x + 1));
		} else {
			unsigned int j;		
			while(n < x) {
				j = n++;
				m[n] = m[j] * n;
			}
			return m[x];
		}		
	}

	inline double lnfactorial(unsigned int x) {
		static double m[100];
		if(x < 0) {
			throw std::runtime_error("lnfactorial: cannot compute ln(factorial) less than zero!");
		} else if (x == 1) {
			return 0;
		} else if (x < 100) {
			return m[x] ? m[x] : (m[x] = lngamma(x + 1));
		} else {
			return lngamma(x + 1);
		}
	}

	inline double nchoosek(int n, int k) {
		if(n < k) throw std::runtime_error("nchoosek: n must be greater or equal to k!");
		return std::floor(0.5 + std::exp(lnfactorial(n) - lnfactorial(k) - lnfactorial(n - k)));
	}

	inline double beta(double z, double w) {
		return std::exp(lngamma(z) + lngamma(w) - lngamma(z + w));
	}

	inline double incomplete_beta_cf(double a, double b, double x) {
		int m;
		double frac, ab = a + b, am = a - 1.0, ap = a + 1.0, c, d, h, g; 
		c = 1.0;
		d = 1.0 - ab * x / ap;
		if(std::fabs(d) < 1e-30) d = 1e-30;		
		d = 1.0 / d;
		h = d;
		for(m = 1; m <= 100; ++m) {
			//even series
			frac = (m * (b - m) * x) / ((am + 2.0 * m) * (a + 2.0 * m));
			c = 1.0 + (frac / c);		
			d = 1.0 + (frac * d);
			if(std::fabs(c) < 1.0e-30) c = 1.0e-30;
			if(std::fabs(d) < 1.0e-30) d = 1.0e-30;		
			d = 1.0 / d;			
			h *= c * d;
			//odd series
			frac = -((a + m) * (ab + m) * x / (a + 2.0 * m) * (ap + 2.0 * m));
			c = 1.0 + (frac / c);
			d = 1.0 + (frac * d);
			if(std::fabs(c) < 1.0e-30) c = 1.0e-30;
			if(std::fabs(d) < 1.0e-30) d = 1.0e-30;	
			d = 1.0 / d;
			g = c * d;
			//product of the two
			h *= g;
			//are we done?
			if(std::fabs(g - 1.0) < 3.0e-7) {
				break;
			}
		}
		if(m > 100) {
			throw std::runtime_error("incomplete_beta_cf: either a or b too large of number of iterations too small!");
		} else {
			return h;
		}
	}

	inline double incomplete_beta(double a, double b, double x) {
		double f;
		if(x < 0.0 or x > 1.0) {
			throw std::runtime_error("incomplete_beta: x must be > 0 or < 1!");
		} else if(x == 0.0 or x == 1.0) {
			f = 0.0;
		} else {
			f = std::exp(lngamma(a + b) - lngamma(a) - lngamma(b) 
						+ (a * std::log(x)) + (b * std::log(1.0 - x)));
		}

		if(x < (a + 1.0) / (a + b + 2.0)) {
			return f * incomplete_beta_cf(a, b, x) / a;
		} else {
			return 1.0 - f * incomplete_beta_cf(b, a, 1.0 - x) / b;
		}
	}
	
	template<typename T>
	inline T floor0(const T& val) {
		T result = std::floor( std::fabs(val) );
		return (result < 0.0) ? -result : result;
	}
	
	template<typename T>
	inline T ceil0(const T& val) {
		T result = std::ceil( std::fabs(val) );
		return (result < 0.0) ? -result : result;
	}
	
	template<typename T>
	inline T round_halfup(const T& val) {
		return std::floor(val + 0.5);
	}
	
	template<typename T>
	inline T round_halfup0(const T& val) {
		T result = round_halfup( std::fabs(val) );
		return (result < 0.0) ? -result : result;
	}
	
	template<typename T>
	inline T round_halfdown(const T& val) {
		return std::ceil(val - 0.5);
	}
	
	template<typename T>
	inline T round_halfdown0(const T& val) {
		T result = round_halfdown( std::fabs(val) );
		return (result < 0.0) ? -result : result;
	}
	
	template<typename T, typename R, typename RNG>
	T round_random(const T& val, const R& mid, RNG& rng) {
		if(double(rng) < mid) {
			return round_halfup0(val);
		} else {
			return round_halfdown0(val);
		}
	}
}

#endif
