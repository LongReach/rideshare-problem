#include <cmath>
#include <string>
#include "Point.h"

namespace RideShare {

	using namespace std;

	Point Point::_grid_dims;

	Point::Point() {
		_x = 0;
		_y = 0;
	}

	Point::Point(int x, int y) {
		_x = x;
		_y = y;
	}

	Point::Point(const Point& other)
	{
		_x = other._x;
		_y = other._y;
	}

	string Point::get_string() const {
		string result = "(" + std::to_string(_x) + "," + std::to_string(_y) + ")";
		return result;
	}

	bool Point::operator==(const Point& other) const {
		return (_x == other._x && _y == other._y);
	}

	void Point::operator=(const Point& other) {
		_x = other._x;
		_y = other._y;
	}

	Point Point::operator+(const Point& other) const {
		Point point(this->_x + other._x, this->_y + other._y);
		return point;
	}

	Point Point::operator-(const Point& other) const {
		Point point(this->_x - other._x, this->_y - other._y);
		return point;
	}

	void Point::set_grid_dims(int x, int y) {
		_grid_dims.set(x, y);
	}

	Point Point::get_grid_dims() {
		return _grid_dims;
	}


	int Point::get_dist(const Point& pt1, const Point& pt2) {
		Point delta = pt2 - pt1;
		return abs(delta.x()) + abs(delta.y());
	}

}