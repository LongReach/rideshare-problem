/**
 * @file dispatcher.cpp
 * @author Ryan McMahon (mcmahonryan@hotmail.com)
 */
#include "dispatcher.h"

namespace ride_share {

	Dispatcher::Dispatcher() {
		last_request_made_ = false;
		new_request_made_ = false;
		next_passenger_ = nullptr;
		average_unhappiness_ = 0.0;
		average_trip_time_ = 0.0;
		num_trips_completed_ = 0;
	}

	void Dispatcher::update(vector<PassengerData*>& ret_passengers_picked_up, vector<PassengerData*>& ret_passengers_dropped_off) {

		// Move the car. If there are active passengers, it should always have a goal.
		if (next_passenger_) {
			Point car_goal = next_passenger_->get_car_goal();
			car_.update(&car_goal);
		}
		else {
			car_.update(nullptr);
		}

		// Update all passengers in transit. Drop-offs are deleted.
		vector<Passenger*> new_passenger_list;
		bool change_occurred = false;
		for (vector<Passenger*>::iterator it = active_passengers_.begin(); it != active_passengers_.end(); it++) {
			Passenger* p = *it;
			bool status_change = p->update(car_.pos_);
			if (status_change) { change_occurred = true; }

			if (status_change && p->is_picked_up() && !p->dropped_off_) {
				ret_passengers_picked_up.push_back(p->data_);
			}

			if (p->dropped_off_) {
				ret_passengers_dropped_off.push_back(p->data_);
				active_passenger_map_.erase(p->data_->id_);
				average_unhappiness_ = (average_unhappiness_ * (float)num_trips_completed_ + p->get_unhappiness_score()) / ((float)(num_trips_completed_ + 1));
				average_trip_time_ = (average_trip_time_ * (float)num_trips_completed_ + p->time_elapsed_) / ((float)(num_trips_completed_ + 1));
				num_trips_completed_++;
				delete p;
			}
			else {
				new_passenger_list.push_back(p);
			}
		}
		active_passengers_ = new_passenger_list;

		if (new_request_made_) {
			change_occurred = true;
			new_request_made_ = false;
		}

		// If a state change occurred, recalculate which passenger to serve next.
		if (change_occurred) {
			float lowest_systemic_score = 10000000.0f;
			Passenger* lowest_systemic_score_passenger = nullptr;
			for (vector<Passenger*>::iterator it = active_passengers_.begin(); it != active_passengers_.end(); it++) {
				Passenger* p = *it;
				float systemic_unhappiness_score = get_total_unhappiness_score(*p);
				if (systemic_unhappiness_score < lowest_systemic_score) {
					lowest_systemic_score = systemic_unhappiness_score;
					lowest_systemic_score_passenger = p;
				}
			}
			next_passenger_ = lowest_systemic_score_passenger;
		}
	}

	bool Dispatcher::is_done() {
		return (last_request_made_ && active_passengers_.size() == 0);
	}

	void Dispatcher::get_passengers_in_car(vector<PassengerData*>& ret_list) {
		for (vector<Passenger*>::iterator it = active_passengers_.begin(); it != active_passengers_.end(); it++) {
			Passenger* p = *it;
			if (p->is_picked_up()) {
				ret_list.push_back(p->data_);
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
		activate_passenger(data->id_, start, end);
	}

	bool Dispatcher::is_passenger_active(const char* name) {
		PassengerData* data = get_passenger_data(name);
		if (data) {
			if (active_passenger_map_.find(data->id_) != active_passenger_map_.end()) {
				return true;
			}
		}
		return false;
	}

	void Dispatcher::make_passenger(const char* name) {
		if (get_passenger_data(name)) {
			return;
		}
		int new_id = passenger_roster_.size();
		PassengerData* data = new PassengerData(name, new_id);
		passenger_roster_.push_back(data);
		passenger_name_map_[name] = data;
	}

	PassengerData* Dispatcher::get_passenger_data(const char* name) {
		map<string, PassengerData*>::iterator it = passenger_name_map_.find(name);
		if (it != passenger_name_map_.end()) {
			return it->second;
		}
		return nullptr;
	}

	PassengerData* Dispatcher::get_passenger_data(int id) {
		if (id < 0 || id >= (int)passenger_roster_.size()) {
			return nullptr;
		}
		return passenger_roster_[id];
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
		passenger->compute_ideal_times(car_.pos_);
		active_passengers_.push_back(passenger);
		active_passenger_map_[data->id_] = passenger;
		new_request_made_ = true;
	}

	Passenger* Dispatcher::get_active_passenger(int id) {
		map<int, Passenger*>::iterator it = active_passenger_map_.find(id);
		if (it != active_passenger_map_.end()) {
			return it->second;
		}
		return nullptr;
	}

	float Dispatcher::get_total_unhappiness_score(const Passenger& target_passenger) {
		int time_delta = Point::get_dist(car_.pos_, target_passenger.get_car_goal());
		float total_score = 0.0f;
		for (vector<Passenger*>::iterator it = active_passengers_.begin(); it != active_passengers_.end(); it++) {
			Passenger* p = *it;
			total_score += p->predict_unhappiness_score(target_passenger.get_car_goal(), time_delta);
		}
		return total_score;
	}

}  // namespace ride_share
