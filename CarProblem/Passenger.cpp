/*
* Ryan McMahon
* 2/8/21
* mcmahonryan@hotmail.com
* Please read README.md
* Other code samples at: github.com/LongReach
*/

#include "Passenger.h"

namespace RideShare {

	void PassengerException::out_of_range(int x, int y, const char* name) {
		string name_str = "";
		if (name != NULL) {
			name_str = name;
			name_str = name_str + " ";
		}
		_info = "Point " + name_str + "(" + std::to_string(x) + ", " + std::to_string(y) + ") is invalid";
	}

	void PassengerException::passenger_not_found(int id) {
		_info = "Passenger with id " + std::to_string(id) + " not found";
	}

	void PassengerException::passenger_not_found(const char* name) {
		_info = "Passenger with name " + string(name) + " not found";
	}

	PassengerData::PassengerData(const char* name, int id) {
		_name = name;
		_id = id;
	}

	Passenger::Passenger(PassengerData* data) :
		_ideal_pickup_time(-1),
		_ideal_journey_time(-1),
		_time_elapsed(0),
		_time_since_pickup(-1),
		_dropped_off(false)
	{
		_data = data;
	}

	void Passenger::activate(const Point& start, const Point& end) {
		Point grid_dims = Point::get_grid_dims();
		PassengerException ex;
		if (start.x() < 0 || start.y() < 0 || start.x() >= grid_dims.x() || start.y() >= grid_dims.y()) {
			ex.out_of_range(start.x(), start.y(), "start");
			throw ex;
		}
		if (end.x() < 0 || end.y() < 0 || end.x() >= grid_dims.x() || end.y() >= grid_dims.y()) {
			ex.out_of_range(end.x(), end.y(), "end");
			throw ex;
		}
		_start = start;
		_end = end;
		_ideal_pickup_time = -1;
		_ideal_journey_time = -1;
		_time_elapsed = 0;
		_time_since_pickup = -1;
		_dropped_off = false;
	}

	void Passenger::compute_ideal_times(const Point& car_pt) {
		_ideal_pickup_time = Point::get_dist(car_pt, _start);
		_ideal_journey_time = Point::get_dist(_start, _end);
	}

	int Passenger::compute_perfect_time(const Point& car_pt) {
		int perfect_time = 0;
		if (is_picked_up()) {
			perfect_time = Point::get_dist(car_pt, _end);
		}
		else {
			perfect_time = Point::get_dist(car_pt, _start) + Point::get_dist(_start, _end);
		}
		return perfect_time;
	}

	bool Passenger::update(const Point& car_pt) {
		bool change_occurred = false;
		if (is_picked_up()) {
			if (car_pt == _end) {
				_dropped_off = true;
				change_occurred = true;
			}
		}
		else // not picked up
		{
			if (car_pt == _start) {
				_time_since_pickup = 0;
				change_occurred = true;
			}
		}
		_time_elapsed++;
		if (is_picked_up()) {
			_time_since_pickup++;
		}
		return change_occurred;
	}

	float Passenger::get_unhappiness_score() {
		int time_elapsed = is_picked_up() ? _time_since_pickup : _time_elapsed;
		return _do_unhappiness_calc(time_elapsed);
	}

	float Passenger::predict_unhappiness_score(const Point& car_pt, int time_delta) {
		int time_elapsed = is_picked_up() ? _time_since_pickup : _time_elapsed;
		return _do_unhappiness_calc(time_elapsed + time_delta + compute_perfect_time(car_pt));
	}

	const Point& Passenger::get_car_goal() const {
		return is_picked_up() ? _end : _start;
	}

	float Passenger::_do_unhappiness_calc(int time_elapsed) {
		// The bias factor assumes that passengers don't become unhappy until 50% more than
		// the ideal time has passed. Until then, passengers experience a negative unhappiness,
		// which is a positive happiness.
		float bias = 1.5;
		int ideal_time = is_picked_up() ? _ideal_journey_time : _ideal_pickup_time;
		if (ideal_time == 0) ideal_time = 1;
		float result = ((float)time_elapsed) / ((float)ideal_time) - bias;
		return result;
	}

}