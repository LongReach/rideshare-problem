/**
 * @file point.cpp
 * @author Ryan McMahon (mcmahonryan@hotmail.com)
 */
#include <cmath>
#include <string>
#include "point.h"

namespace ride_share {

	using namespace std;

	Point Point::grid_dims_;

	Point::Point() {
		x_ = 0;
		y_ = 0;
	}

	Point::Point(int x, int y) {
		x_ = x;
		y_ = y;
	}

	Point::Point(const Point& other)
	{
		x_ = other.x_;
		y_ = other.y_;
	}

	string Point::get_string() const {
		string result = "(" + std::to_string(x_) + "," + std::to_string(y_) + ")";
		return result;
	}

	bool Point::operator==(const Point& other) const {
		return (x_ == other.x_ && y_ == other.y_);
	}

	void Point::operator=(const Point& other) {
		x_ = other.x_;
		y_ = other.y_;
	}

	Point Point::operator+(const Point& other) const {
		Point point(this->x_ + other.x_, this->y_ + other.y_);
		return point;
	}

	Point Point::operator-(const Point& other) const {
		Point point(this->x_ - other.x_, this->y_ - other.y_);
		return point;
	}

	void Point::set_grid_dims(int x, int y) {
		grid_dims_.set(x, y);
	}

	Point Point::get_grid_dims() {
		return grid_dims_;
	}

	int Point::get_dist(const Point& pt1, const Point& pt2) {
		Point delta = pt2 - pt1;
		return abs(delta.x()) + abs(delta.y());
	}

}  // namespace ride_share
