#pragma once
#include <string>
#include "Point.h"

using namespace std;

class PassengerException {
public:
	PassengerException() {
		_info = "";
	}
	PassengerException(const char* info) {
		_info = info;
	}
	PassengerException(const string& info) {
		_info = info;
	}
	void out_of_range(int x, int y, const char* name = NULL);
	string get_info() { return _info; }
private:
	string _info;
};

class PassengerData {
public:
	PassengerData(const char *name, int id);

private:
	int _id;
	string _name;
};

class Passenger {
public:
	Passenger(PassengerData *data);

	void activate(const Point& start, const Point& end);
	bool is_picked_up() { return _time_elapsed != -1; }

	void compute_ideal_times(const Point& car_pt);
	int compute_perfect_time(const Point& car_pt);
	bool update(const Point& car_pt);

	float get_unhappiness_score();
	float predict_unhappiness_score(const Point& car_pt, int time_delta = 0);
	const Point& get_car_goal();

private:

	float _do_unhappiness_calc(int time_elapsed);

	PassengerData* _data;
	Point _start;
	Point _end;
	int _ideal_pickup_time;
	int _ideal_journey_time;
	int _time_elapsed;
	int _time_since_pickup;
	bool _dropped_off;
	int _system_unhappiness_score;
};