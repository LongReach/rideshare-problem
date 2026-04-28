/**
 * @file car.cpp
 * @author Ryan McMahon (mcmahonryan@hotmail.com)
 */
#include "car.h"

namespace ride_share {

	Car::Car() {
		pos_.set(0, 0);
		move_axis_ = -1;
	}

	void Car::update(const Point* goal_pt) {
		if (move_axis_ == -1) {
			move_axis_ = 0;
			return;
		}

		if (goal_pt) {
			Point delta = (*goal_pt) - pos_;

			// The car continues along its most recent axis until there is a reason to change.
			bool movement_required = (delta.x() != 0 || delta.y() != 0);
			while (movement_required) {
				if (move_axis_ == 0) {
					if (delta.x() != 0) {
						pos_.set(pos_.x() + ((delta.x() > 0) ? 1 : -1), pos_.y());
						movement_required = false;
					}
					else {
						move_axis_ = 1;
					}
				}
				if (move_axis_ == 1) {
					if (delta.y() != 0) {
						pos_.set(pos_.x(), pos_.y() + ((delta.y() > 0) ? 1 : -1));
						movement_required = false;
					}
					else {
						move_axis_ = 0;
					}
				}
			}
		}
	}

}  // namespace ride_share
