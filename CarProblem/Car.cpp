#include "Car.h"

namespace RideShare {

	Car::Car() {
		_pos.set(0, 0);
		_move_axis = -1;
	}

	void Car::update(const Point* goal_pt) {
		if (_move_axis == -1) {
			_move_axis = 0;
			return;
		}

		// There is a goal to move towards. If none, then car doesn't move
		if (goal_pt) {
			Point delta = (*goal_pt) - _pos;

			// The logic here is that the car will continue along its most recent movement
			// axis (x or y) until there is a reason to change it.
			bool movement_required = (delta.x() != 0 || delta.y() != 0);
			while (movement_required) {
				if (_move_axis == 0) { // y
					if (delta.x() != 0) {
						_pos.set(_pos.x() + ((delta.x() > 0) ? 1 : -1), _pos.y());
						movement_required = false;
					}
					else {
						_move_axis = 1;
					}
				}
				if (_move_axis == 1) { // y
					if (delta.y() != 0) {
						_pos.set(_pos.x(), _pos.y() + ((delta.y() > 0) ? 1 : -1));
						movement_required = false;
					}
					else {
						_move_axis = 0;
					}
				}
			} // end while
		} // end if (goal_pt)

	}

}