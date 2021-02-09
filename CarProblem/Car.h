#pragma once
#include "Point.h"

namespace RideShare {

	class Car {
	public:
		Car();

		void update(const Point* goal_pt = NULL);

	private:

		Point _pos;
		int _move_axis;
		friend class Dispatcher;
	};

}