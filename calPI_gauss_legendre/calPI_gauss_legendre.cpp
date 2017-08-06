/*
	http://drele.nxthosts.net/
	Author: Jason Huang
	Date:   2016 / 04 / 21
*/

#include <iostream>
#include <mpirxx.h>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <future>

#define FILENAME "pi.txt"

using namespace std;

typedef chrono::system_clock::time_point t_point;

void calPI__GaussLegendre_algorithn(uint64_t digit);

int main()
{
	uint64_t _float_digit;
	cout << "Please enter the digits of pi to be calculated : ";
	cin >> _float_digit;
	calPI__GaussLegendre_algorithn(_float_digit);
	system("pause");
}

mpf_class _sqrt_an_bn(mpf_class _an, mpf_class _bn, uint64_t digit);
mpf_class _pow_an_an1(mpf_class _a, mpf_class _an, uint64_t digit);

void calPI__GaussLegendre_algorithn(uint64_t digit)
{
	ofstream out(FILENAME);

	t_point start_ = chrono::system_clock::now();	//	Start time

	mpf_class _an(1, digit * 3.4), _tn((1.0 / 4.0), digit * 3.4), _pn(1, digit * 3.4), sqrt_2(0, digit * 3.4);
				//	a{0} = 1		//	t{0} = 1 / 4				//	p{0} = 1

	mpf_sqrt(sqrt_2.get_mpf_t(), mpf_class(2).get_mpf_t());
	mpf_class _bn(1 / sqrt_2, digit * 3.4);
				//	b0 = 1 / ( (2) ^ 0.5 )

	for (int i = 0; i < 27; i++) {
		future<mpf_class> sqrt_an_bn = async(_sqrt_an_bn, _an, _bn, digit);
		//	( a{n} * b{n} ) ^ 0.5
		
		mpf_class _a((_an + _bn) / 2, digit * 3.4);
		//  a{n+1} = (an + bn) / 2		

		future<mpf_class> pow_an_an1 = async(_pow_an_an1, _a, _an, digit);
		//  ( a{n} - a{n+1} ) ^ 2 

		_an = _a; 
		_bn = sqrt_an_bn.get();				//	b{n+1} = ( a{n} * b{n} ) ^ 0.5
		_tn -= _pn * (pow_an_an1.get());	//  t{n+1} = t{n} - p{n} * ( a{n} - a{n+1} ) ^ 2
		_pn *= 2;							//  p{n+1} = 2 * p{n}
	}

	mpf_class _pow_an_bn(0, digit * 3.4);
	mpf_pow_ui(_pow_an_bn.get_mpf_t(), mpf_class(_an + _bn).get_mpf_t(), mpir_ui(2));
			  //  ( a{n} + b{n} ) ^ 2

	mpf_class pi(_pow_an_bn / (4 * _tn), digit * 3.4);
			  //  pi ~= ( ( a{n} + b{n} ) ^ 2 ) / ( 4 * t{n} )

	t_point end_ = chrono::system_clock::now();	//	Start time

	uint64_t calculate_time = chrono::duration_cast<std::chrono::seconds>(end_ - start_).count();

	/*  Save pi to file  */
	out << fixed << setprecision(digit) << pi << endl;
	out << endl << "Calculate Time: " << calculate_time << " seconds" << endl;
	out.close();

	/*  Screen print  */
	cout << endl << "Calculate Time: " << calculate_time << " seconds" << endl;
	cout << endl << "The calculate results have been saved in [ " << FILENAME << " ]." << endl;
}

mpf_class _sqrt_an_bn(mpf_class _an, mpf_class _bn, uint64_t digit)
{
	mpf_class sqrt_an_bn(0, digit * 3.4);
	mpf_sqrt(sqrt_an_bn.get_mpf_t(), mpf_class(_an * _bn).get_mpf_t());
	return sqrt_an_bn;
}

mpf_class _pow_an_an1(mpf_class _a, mpf_class _an, uint64_t digit)
{
	mpf_class pow_an_an1(0, digit * 3.4);
	mpf_pow_ui(pow_an_an1.get_mpf_t(), mpf_class(_an - _a).get_mpf_t(), mpir_ui(2));
	return pow_an_an1;
}