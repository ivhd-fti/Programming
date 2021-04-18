
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cmath>   // #include <math.h>
#include <float.h> //#include <string> //#include <vector>


using namespace std;

class Rational {
	int num, den;

public:
	double double_;
	Rational() {
		num = 0;
		den = 1;
		double_ = num/ double(den);
	}

	Rational(int numerator_, int denominator_) {
		num = numerator_; den = denominator_;
		if (den < 0) {den = -den; num = -num;}

		if (!den && !num) {double_ = NAN;}
		else if (!den) {
			double_ = INFINITY; if (num < 0) {double_ = -double_;}
		}else {	double_ = num / double(den);	}
	}
	////////////////////////////////////////////////////


	bool operator == (const Rational& Other) {return (this->double_ == Other.double_);}

	Rational operator+ (const Rational& Other) {
		if (isinf(this->double_) && isinf(Other.double_)) {
			if (isnan(this->double_ + Other.double_)) {return Rational(0, 0);}
			
			if (this->double_ + Other.double_ > 0) {return Rational(1, 0);}
			return Rational(-1, 0);
		}
		return Rational(this->num * Other.den + Other.num * this->den, 
			this->den * Other.den);
	}

	Rational operator- (const Rational& Other) {
		if (isnan(this->double_) || isnan(Other.double_))	return Rational(0, 0);
		
		if (isinf(this->double_) && isinf(Other.double_)) {
			if (isnan(this->double_ - Other.double_))	return Rational(0, 0);

			if (this->double_ - Other.double_ > 0)		return Rational(1, 0);
														return Rational(-1, 0);
		}
		
		return Rational(this->num * Other.den - Other.num * this->den,
						this->den * Other.den);
	}

	Rational operator* (const Rational& Other) {
		return Rational(this->num * Other.num, this->den * Other.den);
	}

	Rational operator/ (const Rational& Other) {
		if (isinf(this->double_) && !isinf(Other.double_) && Other.double_ < 0) {
			return (this->double_ > 0) ? Rational(-1, 0) : Rational(1, 0);
		}
		return Rational(this->num * Other.den, this->den * Other.num);
	}

	//cout << (Rational(22, 0) / Rational(-22, 9) == Rational(-22, 0)) << "\n";
	//cout << (Rational(-22, 0) / Rational(-22, 9) == Rational(22, 0)) << "\n";


	///////////////////////////////////////////////

	static int gcd(int int1, int int2) { return abs((!int2) ? int1 : gcd(int2, int1 % int2)); }
	operator double() {return double_;}
	operator bool()   {return bool(this->double_);}
	bool isNaN()		{return (!num && !den);}
	int numerator()		{return num;}
	int denominator()	{return den;}
};


//bool equal(double a, double b, double e = 1E-10)
bool equal(double a, double b, double e = 1E-10) {
	if (-e < a - b && a - b < e) return true;
	else return false;
}


int main()
{
	if (Rational::gcd(91, 65) == 13 &&
		Rational::gcd(10, 3) == 1 &&
		Rational::gcd(-10, 3) == 1 &&
		Rational::gcd(10, -3) == 1 &&
		Rational::gcd(-10, -3) == 1 &&
		Rational::gcd(30, 10) == 10 &&
		Rational::gcd(10, 30) == 10 &&
		Rational::gcd(0, 10) == 10 &&
		Rational::gcd(10, 0) == 10) std::cout << "gcd test passed\n";
	else std::cout << "gcd test failed\n";

	if (!Rational(22, 0).isNaN() &&
		!Rational(22, 9).isNaN() &&
		!Rational(0, 9).isNaN() &&
		!Rational(-22, 9).isNaN() &&
		!Rational(-22, 0).isNaN() &&
		Rational(0, 0).isNaN()
		) std::cout << "isNaN test passed\n";
	else std::cout << "isNaN test failed\n";

	if (Rational(22, 0) == Rational(22, 0) &&
		Rational(22, 0) == Rational(9, 0) &&
		!(Rational(22, 0) == Rational(22, 9)) &&
		!(Rational(22, 0) == Rational(-22, 9)) &&
		!(Rational(22, 0) == Rational(-22, 0)) &&
		!(Rational(22, 0) == Rational(0, 9)) &&
		!(Rational(22, 0) == Rational(0, 0)) &&

		Rational(22, 9) == Rational(22, 9) &&
		Rational(22, 9) == Rational(-22, -9) &&
		Rational(22, 9) == Rational(110, 45) &&
		Rational(22, 9) == Rational(-110, -45) &&
		!(Rational(22, 9) == Rational(-22, 9)) &&
		!(Rational(22, 9) == Rational(22, -9)) &&
		!(Rational(22, 9) == Rational(9, 22)) &&
		!(Rational(22, 9) == Rational(22, 0)) &&
		!(Rational(22, 9) == Rational(-22, 0)) &&
		!(Rational(22, 9) == Rational(0, 9)) &&
		!(Rational(22, 9) == Rational(0, 0)) &&

		Rational(0, 1) == Rational(0, 1) &&
		Rational(0, 1) == Rational(0, 9) &&
		Rational(0, 1) == Rational(0, -9) &&
		!(Rational(0, 1) == Rational(22, 9)) &&
		!(Rational(0, 1) == Rational(-22, 9)) &&
		!(Rational(0, 1) == Rational(22, 0)) &&
		!(Rational(0, 1) == Rational(-22, 0)) &&
		!(Rational(0, 1) == Rational(0, 0)) &&

		Rational(-22, 9) == Rational(-22, 9) &&
		Rational(-22, 9) == Rational(22, -9) &&
		Rational(-22, 9) == Rational(-110, 45) &&
		Rational(-22, 9) == Rational(110, -45) &&
		!(Rational(-22, 9) == Rational(-22, -9)) &&
		!(Rational(-22, 9) == Rational(22, 9)) &&
		!(Rational(-22, 9) == Rational(9, -22)) &&
		!(Rational(-22, 9) == Rational(22, 0)) &&
		!(Rational(-22, 9) == Rational(-22, 0)) &&
		!(Rational(-22, 9) == Rational(0, 9)) &&
		!(Rational(-22, 9) == Rational(0, 0)) &&

		Rational(-22, 0) == Rational(-22, 0) &&
		Rational(-22, 0) == Rational(-9, 0) &&
		!(Rational(-22, 0) == Rational(22, 9)) &&
		!(Rational(-22, 0) == Rational(-22, 9)) &&
		!(Rational(-22, 0) == Rational(22, 0)) &&
		!(Rational(-22, 0) == Rational(0, 9)) &&
		!(Rational(-22, 0) == Rational(0, 0)) &&

		!(Rational(0, 0) == Rational(0, 0))
		) std::cout << "Equality test passed\n";
	else std::cout << "Equality test failed\n";

	if (Rational(22, 0) + Rational(22, 0) == Rational(22, 0) &&
		Rational(22, 9) + Rational(22, 0) == Rational(22, 0) &&
		Rational(0, 9) + Rational(22, 0) == Rational(22, 0) &&
		Rational(-22, 9) + Rational(22, 0) == Rational(22, 0) &&
		(Rational(-22, 0) + Rational(22, 0)).isNaN() &&

		Rational(22, 0) + Rational(22, 9) == Rational(22, 0) &&
		Rational(22, 9) + Rational(22, 9) == Rational(44, 9) &&
		Rational(0, 9) + Rational(22, 9) == Rational(22, 9) &&
		Rational(-22, 9) + Rational(22, 9) == Rational(0, 9) &&
		Rational(-22, 0) + Rational(22, 9) == Rational(-22, 0) &&

		Rational(22, 0) + Rational(0, 1) == Rational(22, 0) &&
		Rational(22, 9) + Rational(0, 1) == Rational(22, 9) &&
		Rational(0, 9) + Rational(0, 1) == Rational(0, 9) &&
		Rational(-22, 9) + Rational(0, 1) == Rational(-22, 9) &&
		Rational(-22, 0) + Rational(0, 1) == Rational(-22, 0) &&

		Rational(22, 0) + Rational(-22, 9) == Rational(22, 0) &&
		Rational(22, 9) + Rational(-22, 9) == Rational(0, 9) &&
		Rational(0, 9) + Rational(-22, 9) == Rational(-22, 9) &&
		Rational(-22, 9) + Rational(-22, 9) == Rational(-44, 9) &&
		Rational(-22, 0) + Rational(-22, 9) == Rational(-22, 0) &&

		(Rational(22, 0) + Rational(-22, 0)).isNaN() &&
		Rational(22, 9) + Rational(-22, 0) == Rational(-22, 0) &&
		Rational(0, 9) + Rational(-22, 0) == Rational(-22, 0) &&
		Rational(-22, 9) + Rational(-22, 0) == Rational(-22, 0) &&
		Rational(-22, 0) + Rational(-22, 0) == Rational(-22, 0) &&

		(Rational(22, 0) + Rational(0, 0)).isNaN() &&
		(Rational(22, 9) + Rational(0, 0)).isNaN() &&
		(Rational(0, 9) + Rational(0, 0)).isNaN() &&
		(Rational(-22, 9) + Rational(0, 0)).isNaN() &&
		(Rational(-22, 0) + Rational(0, 0)).isNaN()
		) std::cout << "Summation test passed\n";
	else std::cout << "Summation test failed\n";

	if ((Rational(22, 0) - Rational(22, 0)).isNaN() &&
		Rational(22, 9) - Rational(22, 0) == Rational(-22, 0) &&
		Rational(0, 9) - Rational(22, 0) == Rational(-22, 0) &&
		Rational(-22, 9) - Rational(22, 0) == Rational(-22, 0) &&
		Rational(-22, 0) - Rational(22, 0) == Rational(-22, 0) &&

		Rational(22, 0) - Rational(22, 9) == Rational(22, 0) &&
		Rational(22, 9) - Rational(22, 9) == Rational(0, 9) &&
		Rational(0, 9) - Rational(22, 9) == Rational(-22, 9) &&
		Rational(-22, 9) - Rational(22, 9) == Rational(-44, 9) &&
		Rational(-22, 0) - Rational(22, 9) == Rational(-22, 0) &&

		Rational(22, 0) - Rational(0, 1) == Rational(22, 0) &&
		Rational(22, 9) - Rational(0, 1) == Rational(22, 9) &&
		Rational(0, 9) - Rational(0, 1) == Rational(0, 9) &&
		Rational(-22, 9) - Rational(0, 1) == Rational(-22, 9) &&
		Rational(-22, 0) - Rational(0, 1) == Rational(-22, 0) &&

		Rational(22, 0) - Rational(-22, 9) == Rational(22, 0) &&
		Rational(22, 9) - Rational(-22, 9) == Rational(44, 9) &&
		Rational(0, 9) - Rational(-22, 9) == Rational(22, 9) &&
		Rational(-22, 9) - Rational(-22, 9) == Rational(0, 9) &&
		Rational(-22, 0) - Rational(-22, 9) == Rational(-22, 0) &&

		Rational(22, 0) - Rational(-22, 0) == Rational(22, 0) &&
		Rational(22, 9) - Rational(-22, 0) == Rational(22, 0) &&
		Rational(0, 9) - Rational(-22, 0) == Rational(22, 0) &&
		Rational(-22, 9) - Rational(-22, 0) == Rational(22, 0) &&
		(Rational(-22, 0) - Rational(-22, 0)).isNaN() &&

		(Rational(22, 0) - Rational(0, 0)).isNaN() &&
		(Rational(22, 9) - Rational(0, 0)).isNaN() &&
		(Rational(0, 9) - Rational(0, 0)).isNaN() &&
		(Rational(-22, 9) - Rational(0, 0)).isNaN() &&
		(Rational(-22, 0) - Rational(0, 0)).isNaN()
		) std::cout << "Subtraction test passed\n";
	else std::cout << "Subtraction test failed\n";

	if (Rational(22, 0) * Rational(22, 0) == Rational(22, 0) &&
		Rational(22, 9) * Rational(22, 0) == Rational(22, 0) &&
		(Rational(0, 9) * Rational(22, 0)).isNaN() &&
		Rational(-22, 9) * Rational(22, 0) == Rational(-22, 0) &&
		Rational(-22, 0) * Rational(22, 0) == Rational(-22, 0) &&

		Rational(22, 0) * Rational(22, 9) == Rational(22, 0) &&
		Rational(22, 9) * Rational(22, 9) == Rational(22 * 22, 9 * 9) &&
		Rational(0, 9) * Rational(22, 9) == Rational(0, 9) &&
		Rational(-22, 9) * Rational(22, 9) == Rational(-22 * 22, 9 * 9) &&
		Rational(-22, 0) * Rational(22, 9) == Rational(-22, 0) &&

		(Rational(22, 0) * Rational(0, 1)).isNaN() &&
		Rational(22, 9) * Rational(0, 1) == Rational(0, 9) &&
		Rational(0, 9) * Rational(0, 1) == Rational(0, 9) &&
		Rational(-22, 9) * Rational(0, 1) == Rational(0, 9) &&
		(Rational(-22, 0) * Rational(0, 1)).isNaN() &&

		Rational(22, 0) * Rational(-22, 9) == Rational(-22, 0) &&
		Rational(22, 9) * Rational(-22, 9) == Rational(-22 * 22, 9 * 9) &&
		Rational(0, 9) * Rational(-22, 9) == Rational(0, 9) &&
		Rational(-22, 9) * Rational(-22, 9) == Rational(22 * 22, 9 * 9) &&
		Rational(-22, 0) * Rational(-22, 9) == Rational(22, 0) &&

		Rational(22, 0) * Rational(-22, 0) == Rational(-22, 0) &&
		Rational(22, 9) * Rational(-22, 0) == Rational(-22, 0) &&
		(Rational(0, 9) * Rational(-22, 0)).isNaN() &&
		Rational(-22, 9) * Rational(-22, 0) == Rational(22, 0) &&
		Rational(-22, 0) * Rational(-22, 0) == Rational(22, 0) &&

		(Rational(22, 0) * Rational(0, 0)).isNaN() &&
		(Rational(22, 9) * Rational(0, 0)).isNaN() &&
		(Rational(0, 9) * Rational(0, 0)).isNaN() &&
		(Rational(-22, 9) * Rational(0, 0)).isNaN() &&
		(Rational(-22, 0) * Rational(0, 0)).isNaN()
		) std::cout << "Multiplication test passed\n";
	else std::cout << "Multiplication test failed\n";

	if ((Rational(22, 0) / Rational(22, 0)).isNaN() &&
		Rational(22, 9) / Rational(22, 0) == Rational(0, 9) &&
		Rational(0, 9) / Rational(22, 0) == Rational(0, 9) &&
		Rational(-22, 9) / Rational(22, 0) == Rational(0, 9) &&
		(Rational(-22, 0) / Rational(22, 0)).isNaN() &&

		Rational(22, 0) / Rational(22, 9) == Rational(22, 0) &&
		Rational(22, 9) / Rational(22, 9) == Rational(9, 9) &&
		Rational(0, 9) / Rational(22, 9) == Rational(0, 9) &&
		Rational(-22, 9) / Rational(22, 9) == Rational(-9, 9) &&
		Rational(-22, 0) / Rational(22, 9) == Rational(-22, 0) &&

		Rational(22, 0) / Rational(0, 1) == Rational(22, 0) &&
		Rational(22, 9) / Rational(0, 1) == Rational(22, 0) &&
		(Rational(0, 9) / Rational(0, 1)).isNaN() &&
		Rational(-22, 9) / Rational(0, 1) == Rational(-22, 0) &&
		Rational(-22, 0) / Rational(0, 1) == Rational(-22, 0) &&

		Rational(22, 0) / Rational(-22, 9) == Rational(-22, 0) &&
		Rational(22, 9) / Rational(-22, 9) == Rational(-9, 9) &&
		Rational(0, 9) / Rational(-22, 9) == Rational(0, 9) &&
		Rational(-22, 9) / Rational(-22, 9) == Rational(9, 9) &&
		Rational(-22, 0) / Rational(-22, 9) == Rational(22, 0) &&

		(Rational(22, 0) / Rational(-22, 0)).isNaN() &&
		Rational(22, 9) / Rational(-22, 0) == Rational(0, 9) &&
		Rational(0, 9) / Rational(-22, 0) == Rational(0, 9) &&
		Rational(-22, 9) / Rational(-22, 0) == Rational(0, 9) &&
		(Rational(-22, 0) / Rational(-22, 0)).isNaN() &&

		(Rational(22, 0) / Rational(0, 0)).isNaN() &&
		(Rational(22, 9) / Rational(0, 0)).isNaN() &&
		(Rational(0, 9) / Rational(0, 0)).isNaN() &&
		(Rational(-22, 9) / Rational(0, 0)).isNaN() &&
		(Rational(-22, 0) / Rational(0, 0)).isNaN()
		) std::cout << "Division test passed\n";
	else std::cout << "Division test failed\n";

	if (equal(Rational(-22, -9), 22 / 9.0) &&
		equal(Rational(-9, -9), 1) &&
		equal(Rational(-6, -9), 6 / 9.0) &&
		equal(Rational(0, -9), 0) &&
		equal(Rational(6, -9), -6 / 9.0) &&
		equal(Rational(9, -9), -1) &&
		equal(Rational(22, -9), -22 / 9.0) &&
		std::isinf((double)Rational(-9, 0)) &&
		std::isnan((double)Rational(0, 0)) &&
		std::isinf((double)Rational(9, 0)) &&
		equal(Rational(-22, 9), -22 / 9.0) &&
		equal(Rational(-9, 9), -1) &&
		equal(Rational(-6, 9), -6 / 9.0) &&
		equal(Rational(0, 9), 0) &&
		equal(Rational(6, 9), 6 / 9.0) &&
		equal(Rational(9, 9), 1) &&
		equal(Rational(22, 9), 22 / 9.0)) std::cout << "Conversion to double test passed\n";
	else std::cout << "Conversion to double test failed\n";

	if (Rational(-22, -9) &&
		Rational(-9, -9) &&
		Rational(-6, -9) &&
		!Rational(0, -9) &&
		Rational(6, -9) &&
		Rational(9, -9) &&
		Rational(22, -9) &&
		Rational(-9, 0) &&
		Rational(0, 0) &&
		Rational(9, 0) &&
		Rational(-22, 9) &&
		Rational(-9, 9) &&
		Rational(-6, 9) &&
		!Rational(0, 9) &&
		Rational(6, 9) &&
		Rational(9, 9) &&
		Rational(22, 9)) std::cout << "Conversion to bool test passed\n";
	else std::cout << "Conversion to bool test failed\n";

}











/////////////////////////////////////////////////////////////////////////////




int main2()
{
    double a = INFINITY;
    double b = -a;

    //double c = NAN;
    double c = nan("1");
    cout << a << ","<< b << "," << a+b << a+a << b+b<<"\n";
    printf("isnan(NAN)         = %d\n", isnan(NAN));
    printf("isnan(INFINITY)    = %d\n", isnan(INFINITY));
    printf("isnan(0.0)         = %d\n", isnan(0.0));
    printf("isnan(DBL_MIN/2.0) = %d\n", isnan(DBL_MIN / 2.0));
    //printf("isnan(0.0 / 0.0)   = %d\n", isnan(0.0 / 0.0));
    printf("isnan(Inf - Inf)   = %d\n", isnan(INFINITY - INFINITY));
    std::cout << "Hello World!\n";
	return 0;
}

