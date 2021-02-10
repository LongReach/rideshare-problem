/*
* Ryan McMahon
* 2/9/21
* mcmahonryan@hotmail.com
* Please read README.md
* Other code samples at: github.com/LongReach
*/

#pragma once
#include <string>
#include <vector>
#include <map>
#include "Point.h"
#include "Passenger.h"
#include "Car.h"

namespace RideShare {

	/*
	* This class manages:
	* - a roster of known passengers
	* - the car
	* - a list of all passangers currently in transit
	* 
	* Via its update function, it applies its scoring heuristic to determine which passenger to get/deposit next and moves
	* the car incrementally towards that goal. See the README.md file for a more high-level understanding of the heuristic.
	* 
	* Scores are only recalculated when something happens, when a passenger is picked up/dropped off, or a new passenger
	* requests a ride.
	*/
	class Dispatcher {
	public:
		Dispatcher();

		// The main update function. Returns lists of passengers picked up or dropped off on that step.
		void update(vector<PassengerData*>& ret_passengers_picked_up, vector<PassengerData*>& ret_passengers_dropped_off);
		// Returns true when all passengers have been dealt with and no new ones will request rides
		bool is_done();
		// Call to mark the last ride as having been requested.
		void set_last_request_made() { _last_request_made = true; }
		Point get_car_pos() { return _car._pos; }
		// Returns a list of passengers actually in the car
		void get_passengers_in_car(vector<PassengerData*>& ret_list);

		// Puts a new passenger-in-transit into tracking.
		void new_request(const char* name, int start_x, int start_y, int end_x, int end_y);

		void get_statistics(int* ret_num_trips, float* ret_avg_unhappiness, float* ret_avg_trip_time) {
			*ret_num_trips = _num_trips_completed;
			*ret_avg_unhappiness = _average_unhappiness;
			*ret_avg_trip_time = _average_trip_time;
		}

		bool is_passenger_active(const char* name);

	private:
		void make_passenger(const char* name);
		PassengerData* get_passenger_data(const char* name);
		PassengerData* get_passenger_data(int id);

		void activate_passenger(int id, const Point& start, const Point& end);
		Passenger* get_active_passenger(int id);

		// Returns the total systemic unhappiness that will result when the target passenger is dealt with
		float get_total_unhappiness_score(const Passenger& target_passenger);

		Car _car;

		vector<PassengerData*> _passenger_roster;
		map<string, PassengerData*> _passenger_name_map;
		vector<Passenger*> _active_passengers; // all passengers currently in transit
		map<int, Passenger*> _active_passenger_map; // for looking up passengers by ID
		Passenger* _next_passenger; // The passenger the car is currently dealing with, i.e. the one who produces the
									// lowest systemic unhappiness score

		bool _last_request_made;
		bool _new_request_made;

		int _num_trips_completed;
		float _average_unhappiness;
		float _average_trip_time;
	};

}