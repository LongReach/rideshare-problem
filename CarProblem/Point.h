#pragma once
#include <string>

namespace RideShare {

	using namespace std;

	/*
	* Represents a single point within the city space, or a delta between two points.
	*/
	class Point {
	public:
		Point();
		Point(int x, int y);
		Point(const Point& other);

		void set(int x, int y) { _x = x; _y = y; }
		int x() const { return _x; }
		int y() const { return _y; }

		string get_string() const;

		bool operator==(const Point& other) const;
		void operator=(const Point& other);
		Point operator+(const Point& other) const;
		Point operator-(const Point& other) const;

		static void set_grid_dims(int x, int y);
		static Point get_grid_dims();
		// Returns the Manhattan distance between two arbitrary points
		static int get_dist(const Point& pt1, const Point& pt2);

	private:
		int _x, _y;

		static Point _grid_dims;
	};

}