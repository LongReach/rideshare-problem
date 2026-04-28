/**
 * @file car.h
 * @author Ryan McMahon (mcmahonryan@hotmail.com)
 * @brief Vehicle class for the ride-share simulation.
 */
#pragma once
#include "point.h"

namespace ride_share {

	/// @brief Represents the single vehicle in the simulation.
	class Car {
	public:
		Car();

		/// @brief Advances the car one block toward the given goal, or stays put if no goal.
		/// @param goal_pt Target position; nullptr means the car does not move.
		void update(const Point* goal_pt = nullptr);

	private:
		Point pos_;
		int move_axis_;
		friend class Dispatcher;
	};

}  // namespace ride_share
