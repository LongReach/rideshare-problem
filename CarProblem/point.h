/**
 * @file point.h
 * @author Ryan McMahon (mcmahonryan@hotmail.com)
 * @brief 2D grid coordinate for the city simulation.
 */
#pragma once
#include <string>

namespace ride_share {

	using namespace std;

	/// @brief Represents a single point within the city grid, or a delta between two points.
	class Point {
	public:
		Point();
		Point(int x, int y);
		Point(const Point& other);

		void set(int x, int y) { x_ = x; y_ = y; }
		int x() const { return x_; }
		int y() const { return y_; }

		/// @brief Returns a human-readable "(x,y)" string.
		string get_string() const;

		bool operator==(const Point& other) const;
		void operator=(const Point& other);
		Point operator+(const Point& other) const;
		Point operator-(const Point& other) const;

		/// @brief Sets the global city grid dimensions.
		static void set_grid_dims(int x, int y);

		/// @brief Returns the global city grid dimensions.
		static Point get_grid_dims();

		/// @brief Returns the Manhattan distance between two arbitrary points.
		static int get_dist(const Point& pt1, const Point& pt2);

	private:
		int x_, y_;
		static Point grid_dims_;
	};

}  // namespace ride_share
