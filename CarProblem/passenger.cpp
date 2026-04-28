/**
 * @file passenger.cpp
 * @author Ryan McMahon (mcmahonryan@hotmail.com)
 */
#include "passenger.h"

namespace ride_share {

	void PassengerException::out_of_range(int x, int y, const char* name) {
		string name_str = "";
		if (name != nullptr) {
			name_str = name;
			name_str = name_str + " ";
		}
		info_ = "Point " + name_str + "(" + std::to_string(x) + ", " + std::to_string(y) + ") is invalid";
	}

	void PassengerException::passenger_not_found(int id) {
		info_ = "Passenger with id " + std::to_string(id) + " not found";
	}

	void PassengerException::passenger_not_found(const char* name) {
		info_ = "Passenger with name " + string(name) + " not found";
	}

	PassengerData::PassengerData(const char* name, int id) {
		name_ = name;
		id_ = id;
	}

	Passenger::Passenger(PassengerData* data) :
		ideal_pickup_time_(-1),
		ideal_journey_time_(-1),
		time_elapsed_(0),
		time_since_pickup_(-1),
		dropped_off_(false)
	{
		data_ = data;
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
		start_ = start;
		end_ = end;
		ideal_pickup_time_ = -1;
		ideal_journey_time_ = -1;
		time_elapsed_ = 0;
		time_since_pickup_ = -1;
		dropped_off_ = false;
	}

	void Passenger::compute_ideal_times(const Point& car_pt) {
		ideal_pickup_time_ = Point::get_dist(car_pt, start_);
		ideal_journey_time_ = Point::get_dist(start_, end_);
	}

	int Passenger::compute_perfect_time(const Point& car_pt) {
		int perfect_time = 0;
		if (is_picked_up()) {
			perfect_time = Point::get_dist(car_pt, end_);
		}
		else {
			perfect_time = Point::get_dist(car_pt, start_) + Point::get_dist(start_, end_);
		}
		return perfect_time;
	}

	bool Passenger::update(const Point& car_pt) {
		bool change_occurred = false;
		if (is_picked_up()) {
			if (car_pt == end_) {
				dropped_off_ = true;
				change_occurred = true;
			}
		}
		else
		{
			if (car_pt == start_) {
				time_since_pickup_ = 0;
				change_occurred = true;
			}
		}
		time_elapsed_++;
		if (is_picked_up()) {
			time_since_pickup_++;
		}
		return change_occurred;
	}

	float Passenger::get_unhappiness_score() {
		int time_elapsed = is_picked_up() ? time_since_pickup_ : time_elapsed_;
		return do_unhappiness_calc(time_elapsed);
	}

	float Passenger::predict_unhappiness_score(const Point& car_pt, int time_delta) {
		int time_elapsed = is_picked_up() ? time_since_pickup_ : time_elapsed_;
		return do_unhappiness_calc(time_elapsed + time_delta + compute_perfect_time(car_pt));
	}

	const Point& Passenger::get_car_goal() const {
		return is_picked_up() ? end_ : start_;
	}

	float Passenger::do_unhappiness_calc(int time_elapsed) {
		// Passengers don't become unhappy until 50% over ideal time (bias = 1.5).
		// Below that threshold, unhappiness is negative (i.e. the passenger is happy).
		float bias = 1.5;
		int ideal_time = is_picked_up() ? ideal_journey_time_ : ideal_pickup_time_;
		if (ideal_time == 0) ideal_time = 1;
		float result = ((float)time_elapsed) / ((float)ideal_time) - bias;
		return result;
	}

}  // namespace ride_share
