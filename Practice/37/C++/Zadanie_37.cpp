#include <iostream>
#include <cmath> 
#include <string>
#include <fstream>
#include <vector>

using namespace std;


double sqr(double a);
bool equal(double a, double b, double e = 1E-10);

const double PI = 3.141592653589793;

enum Cords { Cartesian, Polar };

class Point {

private:
	static const double EPS;	// возможное отклонение
	double x, y, r, ang;

	void CartesianToPolar() {
		this->r = hypot(this->x, this->y);
		this->ang = atan2(this->y, this->x);
	}

	void PolarToCartesian() {
		this->x = this->r * cos(this->ang);
		this->y = this->r * sin(this->ang);
	}

public:
	Point(double a1 = 0, double a2 = 0, Cords coord_system = Cartesian) {
		if (coord_system == Cartesian) {	// Cartesian
			this->x = a1;
			set_y(a2);
		}
		else {								// Polar
			this->r = a1;
			set_phi(a2);
		}
	}

	bool operator == (Point& Other) { // const 
		return (abs(this->x - Other.get_x()) < EPS and
			abs(this->y - Other.get_y()) < EPS);
	}

	bool operator != (Point& Other) { // const 
		return not (abs(this->x - Other.get_x()) < EPS and
			abs(this->y - Other.get_y()) < EPS);
	}

	double get_x()   const { return x; }

	double get_y()   const { return y; }

	double get_r()   const { return r; }

	double get_phi() const { return ang; }

	void set_x(double x_) {
		this->x = x_;
		CartesianToPolar();
	}

	void set_y(double y_) {
		this->y = y_;
		CartesianToPolar();
	}

	void set_r(double r_) {
		this->r = r_;
		PolarToCartesian();
	}

	void set_phi(double phi) {
		this->ang = phi;
		PolarToCartesian();
	}
};
const double Point::EPS = pow(10, -10);

std::ostream& operator<< (ostream& out, Point& point) { //const 
	return out << "(" << point.get_x() << "," << point.get_y() << ")";
}

std::istream& operator>> (istream& istrm, Point& point) {
	string inp;
	istrm >> inp;
	std::size_t f1 = inp.find('(') + 1;
	std::size_t f2 = inp.find(',');
	std::size_t f3 = inp.find(')') - 1;

	if ((f1 == string::npos) || (f2 == string::npos) || (f3 == string::npos)) {
		return istrm; // ошибка формата строки ввода
	}

	point.set_x(stod(inp.substr(f1, f2 - f1)));
	point.set_y(stod(inp.substr(f2 + 1, f3 - f2)));

	return istrm;
}

// Задание 37 //////////////////////////////////////////////////////////////////

class Vector {

private:
	const double EPS = pow(10, -10);;	// возможное отклонение
	Point begin, end;
	void cords() {	this->cordX = this->end.get_x() - this->begin.get_x();
					this->cordY = this->end.get_y() - this->begin.get_y();}

public:
	Vector() :						begin(Point(0, 0)), end(Point(1, 0)) {this->cords();}
	Vector(Point end):				begin(Point(0, 0)), end(end)		 {this->cords();}
	Vector(Point begin, Point end): begin(begin),		end(end)		 {this->cords();}
	double cordX, cordY;

	bool operator == (const Vector& Other) {
		return (abs(this->cordX - Other.cordX) < EPS) &&
			   (abs(this->cordY - Other.cordY) < EPS);
	}
	Vector operator- () {
		Vector vect(this->begin, Point(this->begin.get_x()-cordX, this->begin.get_y() - cordY) );
		return vect;
	}
	Vector operator+ (const Vector& Other) {
		Point vect_end(this->end.get_x() + Other.cordX, this->end.get_y() + Other.cordY);
		Vector vect(this->begin, vect_end);
		return vect;
	}
	//Vector operator- (const Vector& Other) {
	//	return this + Other;
	//}
	
	Vector operator- (const Vector& Other) {
		Point vect_end(this->end.get_x() - Other.cordX, this->end.get_y() - Other.cordY);
		Vector vect(this->begin, vect_end);
		return vect;
	}

	Vector operator* (double Other) {
		Point vect_end(this->begin.get_x() + this->cordX * Other,
					   this->begin.get_y() + this->cordY * Other);
		Vector vect(this->begin, vect_end);
		return vect;
	}
	double length() { return hypot(this->cordX, this->cordY); }
	
	//double operator* (const Vector& Other) {
	//	double Other_len = Other.   .length();
	//	double this_ang = atan2(this->cordY, this->cordX);
	//	double Other_ang= atan2(Other.cordY, Other.cordX);
	//	return this->length() * Other.length() * cos(this_ang- Other_ang);
	//}
	
	double operator* (const Vector& Other) {
		double Other_len = hypot(Other.end.get_x() - Other.begin.get_x(), Other.end.get_y() - Other.begin.get_y());
		double scalar = (this->length()) * Other_len * cos(atan2((this->end.get_y() - this->begin.get_y()), (this->end.get_x() - this->begin.get_x())) - atan2((Other.end.get_y() - Other.begin.get_y()), (Other.end.get_x() - Other.begin.get_x())));
		return scalar;
	}
};

int main()
{
	Vector a(Point(1, 2)), b(Point(-2, 0), Point(-1, 2));

	if (a == b ) cout << "Equality test passed\n"; //&& b == a
	else cout << "Equality test failed\n";

	Vector na(Point(-1, -2)), ox(Point(1, 0)), nox(Point(-1, 0)), oy(Point(0, 1)), noy(Point(0, -1));

	if (a == -na && na == -a && -ox == nox && -oy == noy) cout << "Invert test passed\n";
	else cout << "Invert test failed\n";

	if (ox + oy + oy == a && -ox == -a + oy + oy) cout << "Summation test passed\n";
	else cout << "Summation test failed\n";

	if (-ox + oy == oy - ox && -oy + ox == ox - oy) cout << "Subtraction test passed\n";
	else cout << "Subtraction test failed\n";
	
	if (ox * 3 == ox + ox + ox &&
		oy * 3 == oy + oy + oy &&
		ox * (-3) == -ox - ox - ox &&
		oy * (-3) == -oy - oy - oy) cout << "Multiplication by number test passed\n";
	else cout << "Multiplication by number test failed\n";
	
	if (equal(ox.length(), 1) &&
		equal(oy.length(), 1) &&
		equal((ox * 3 + oy * 4).length(), 5)) cout << "Length test passed\n";
	else cout << "Length test failed\n";

	if (equal(ox * ox, sqr(ox.length())) &&
		equal(oy * oy, sqr(oy.length())) &&
		equal((ox * 3 + oy * 4) * (ox * 3 + oy * 4), sqr((ox * 3 + oy * 4).length()))) cout << "Multiplication by Vector test passed\n";
	else cout << "Multiplication by Vector test failed\n";
}



bool equal(double a, double b, double e) {
	if (-e < a - b && a - b < e) return true;
	else return false;
}

double sqr(double a) {
	return a * a;
}