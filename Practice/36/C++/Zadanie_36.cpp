#include <iostream>
#include <cmath> 
#include <string>
#include <fstream>
#include <vector>

using namespace std;

const double PI = 3.141592653589793;

enum Cords {Cartesian,	Polar};

class Point {

private:
	static const double EPS;	// возможное отклонение
	double x, y, r, ang;
	
	void CartesianToPolar() {
		this->r   = hypot(this->x, this->y);
		this->ang = atan2(this->y, this->x);
	}
	
	void PolarToCartesian() {
		this->x = this->r * cos(this->ang);
		this->y = this->r * sin(this->ang);
	}

public:
	Point(double a1 = 0, double a2 = 0, Cords coord_system = Cartesian){
		if (coord_system == Cartesian) {	// Cartesian
			this->x = a1; 
			set_y(a2);
		}else{								// Polar
			this->r = a1; 
			set_phi(a2);
		}
	}

	bool operator == (Point& Other) { // const 
		return (abs(this->x - Other.get_x()) < EPS   and 
				abs(this->y - Other.get_y()) < EPS);
	}
	
	bool operator != (Point& Other) { // const 
		return not (abs(this->x - Other.get_x()) < EPS and
			        abs(this->y - Other.get_y()) < EPS);
	}
	
	double get_x()   const {return x;  }
	
	double get_y()   const {return y;  }
	
	double get_r()   const {return r;  }
	
	double get_phi() const {return ang;}

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

std::istream& operator>> (istream& istrm, Point& point){
	string inp;
	istrm >> inp;
	std::size_t f1 = inp.find('(') + 1;
	std::size_t f2 = inp.find(',');
	std::size_t f3 = inp.find(')') - 1;

	if ((f1 == string::npos) || (f2 == string::npos) || (f3 == string::npos)) {
		return istrm; // ошибка формата строки ввода
	}

	point.set_x( stod(inp.substr(f1    , f2 - f1)) );
	point.set_y( stod(inp.substr(f2 + 1, f3 - f2)) );

	return istrm;
}


int main() {
	std::vector<Point> original;
	std::ifstream fin("data.txt");
	if (!fin.is_open()) {
		std::cout << "Can't open file" << std::endl;
		return 1;
	}
	else {
		while (!fin.eof()) {
			Point p;
			fin >> p;
			fin.ignore(2); // Точки разделены двумя символами ", "
			original.push_back(p);
		}
		fin.close();
	}

	std::vector<Point> simulacrum(original);
	for (auto& p : simulacrum) {
		std::cout << p;
		p.set_x(p.get_x() + 10);
		p.set_phi(p.get_phi() + 180 * PI / 180);
		p.set_y(-p.get_y());
		p.set_x(-p.get_x() - 10);
		std::cout << p << std::endl;
	}

	if (std::equal(original.begin(), original.end(), simulacrum.begin()))
		std::cout << "\nIt works!\n";
	else
		std::cout << "\nIt not works!\n";
}