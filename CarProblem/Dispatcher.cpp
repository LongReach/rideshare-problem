/*
* Ryan McMahon
* 2/9/21
* mcmahonryan@hotmail.com
* Please read README.md
* Other code samples at: github.com/LongReach
*/

#include "Dispatcher.h"

namespace RideShare {

	Dispatcher::Dispatcher() {
		_last_request_made = false;
		_new_request_made = false;
		_next_passenger = NULL;
		_average_unhappiness = 0.0f;
		_average_trip_time = 0.0f;
		_num_trips_completed = 0;
	}

	void Dispatcher::update(vector<PassengerData*>& ret_passengers_picked_up, vector<PassengerData*>& ret_passengers_dropped_off) {
		
		// First, move the car. If there are an active passengers, it should always have a goal
		if (_next_passenger) {
			Point car_goal = _next_passenger->get_car_goal();
			_car.update(&car_goal);
		}
		else {
			_car.update(NULL);
		}

		// Update status of all passengers in transit. If any are dropped off, they are deleted.
		vector<Passenger*> new_passenger_list;
		bool change_occurred = false;
		for (vector<Passenger*>::iterator it = _active_passengers.begin(); it != _active_passengers.end(); it++) {
			Passenger* p = *it;
			bool status_change = p->update(_car._pos);
			if (status_change) { change_occurred = true; }

			if (status_change && p->is_picked_up() && !p->_dropped_off) {
				// Passenger was just picked up
				ret_passengers_picked_up.push_back(p->_data);
			}

			if (p->_dropped_off) {
				ret_passengers_dropped_off.push_back(p->_data);
				_active_passenger_map.erase(p->_data->_id);
				// Update global statistics
				_average_unhappiness = (_average_unhappiness * (float) _num_trips_completed + p->get_unhappiness_score()) / ((float) (_num_trips_completed + 1));
				_average_trip_time = (_average_trip_time * (float) _num_trips_completed + p->_time_elapsed) / ((float)(_num_trips_completed + 1));
				_num_trips_completed++;
				delete p;
			}
			else {
				new_passenger_list.push_back(p);
			}
		}
		_active_passengers = new_passenger_list;

		if (_new_request_made) {
			change_occurred = true;
			_new_request_made = false;
		}

		// If a change happened in the system (drop-off, pick-up, new ride request), recalculate a new immediate goal
		// fpr the car.
		if (change_occurred) {
			float lowest_systemic_score = 10000000.0f;
			Passenger* lowest_systemic_score_passenger = NULL;
			for (vector<Passenger*>::iterator it = _active_passengers.begin(); it != _active_passengers.end(); it++) {
				Passenger* p = *it;
				float systemic_unhappiness_score = get_total_unhappiness_score(*p);
				if (systemic_unhappiness_score < lowest_systemic_score) {
					lowest_systemic_score = systemic_unhappiness_score;
					lowest_systemic_score_passenger = p;
				}
			}
			_next_passenger = lowest_systemic_score_passenger;
		}
	}

	bool Dispatcher::is_done() {
		return (_last_request_made && _active_passengers.size() == 0);
	}

	void Dispatcher::get_passengers_in_car(vector<PassengerData*>& ret_list) {
		for (vector<Passenger*>::iterator it = _active_passengers.begin(); it != _active_passengers.end(); it++) {
			Passenger* p = *it;
			if (p->is_picked_up()) {
				ret_list.push_back(p->_data);
			}
		}
	}

	void Dispatcher::new_request(const char* name, int start_x, int start_y, int end_x, int end_y) {
		PassengerData* data = get_passenger_data(name);
		if (!data) {
			make_passenger(name);
			data = get_passenger_data(name);
		}
		Point start(start_x, start_y);
		Point end(end_x, end_y);
		activate_passenger(data->_id, start, end);
	}

	bool Dispatcher::is_passenger_active(const char* name) {
		PassengerData* data = get_passenger_data(name);
		if (data) {
			if (_active_passenger_map.find(data->_id) != _active_passenger_map.end()) {
				return true;
			}
		}
		return false;
	}

	void Dispatcher::make_passenger(const char* name) {
		int new_id = _passenger_roster.size();
		PassengerData* data = new PassengerData(name, new_id);
		_passenger_roster.push_back(data);
		_passenger_name_map[name] = data;
	}

	PassengerData* Dispatcher::get_passenger_data(const char* name) {
		map<string, PassengerData*>::iterator it = _passenger_name_map.find(name);
		if (it != _passenger_name_map.end()) {
			return it->second;
		}
		return NULL;
	}

	PassengerData* Dispatcher::get_passenger_data(int id) {
		if (id < 0 || id >= (int)_passenger_roster.size()) {
			return NULL;
		}
		return _passenger_roster[id];
	}

	void Dispatcher::activate_passenger(int id, const Point& start, const Point& end) {
		PassengerData* data = get_passenger_data(id);
		if (!data) {
			PassengerException e;
			e.passenger_not_found(id);
			throw e;
		}
		if (start == end) {
			string info = "Start position " + start.get_string() + " same as end position " + end.get_string();
			PassengerException e(info);
			throw e;
		}
		if (get_active_passenger(id)) {
			string info = "Active passenger " + std::to_string(id) + " already exists";
			PassengerException e(info);
			throw e;
		}
		Passenger* passenger = new Passenger(data);
		passenger->activate(start, end);
		passenger->compute_ideal_times(_car._pos);
		_active_passengers.push_back(passenger);
		_active_passenger_map[data->_id] = passenger;
		_new_request_made = true;
	}

	Passenger* Dispatcher::get_active_passenger(int id) {
		map<int, Passenger*>::iterator it = _active_passenger_map.find(id);
		if (it != _active_passenger_map.end()) {
			return it->second;
		}
		return NULL;
	}

	float Dispatcher::get_total_unhappiness_score(const Passenger& target_passenger) {
		int time_delta = Point::get_dist(_car._pos, target_passenger.get_car_goal());
		float total_score = 0.0f;
		for (vector<Passenger*>::iterator it = _active_passengers.begin(); it != _active_passengers.end(); it++) {
			Passenger* p = *it;
			total_score += p->predict_unhappiness_score(target_passenger.get_car_goal(), time_delta);
		}
		return total_score;
	}

}