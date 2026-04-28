/**
 * @file dispatcher.h
 * @author Ryan McMahon (mcmahonryan@hotmail.com)
 * @brief Core routing dispatcher for the ride-share simulation.
 */
#pragma once
#include <string>
#include <vector>
#include <map>
#include "point.h"
#include "passenger.h"
#include "car.h"

namespace ride_share {

	/// @brief Manages the vehicle and all passengers, applying an unhappiness-minimizing
	///        heuristic to decide which passenger to serve next.
	///
	/// Scores are only recalculated on state changes: pickup, drop-off, or a new ride request.
	/// See README.md for a high-level description of the heuristic.
	class Dispatcher {
	public:
		Dispatcher();

		/// @brief Advances the simulation one time step.
		/// @param ret_passengers_picked_up Populated with passengers picked up this step.
		/// @param ret_passengers_dropped_off Populated with passengers dropped off this step.
		void update(vector<PassengerData*>& ret_passengers_picked_up, vector<PassengerData*>& ret_passengers_dropped_off);

		/// @brief Returns true when all passengers have been served and no new requests are coming.
		bool is_done();

		/// @brief Signals that the last ride request has already been submitted.
		void set_last_request_made() { last_request_made_ = true; }

		/// @brief Returns the car's current grid position.
		Point get_car_pos() { return car_.pos_; }

		/// @brief Populates @p ret_list with passengers currently riding in the car.
		void get_passengers_in_car(vector<PassengerData*>& ret_list);

		/// @brief Submits a new ride request.
		/// @param name Passenger name (used as the unique identifier).
		/// @param start_x Pickup x coordinate.
		/// @param start_y Pickup y coordinate.
		/// @param end_x Drop-off x coordinate.
		/// @param end_y Drop-off y coordinate.
		void new_request(const char* name, int start_x, int start_y, int end_x, int end_y);

		/// @brief Returns aggregate statistics for all completed trips.
		/// @param ret_num_trips Total trips completed.
		/// @param ret_avg_unhappiness Mean unhappiness score across all passengers.
		/// @param ret_avg_trip_time Mean total time from request to drop-off.
		void get_statistics(int* ret_num_trips, float* ret_avg_unhappiness, float* ret_avg_trip_time) {
			*ret_num_trips = num_trips_completed_;
			*ret_avg_unhappiness = (float)average_unhappiness_;
			*ret_avg_trip_time = (float)average_trip_time_;
		}

		/// @brief Returns true if the named passenger currently has an active ride.
		bool is_passenger_active(const char* name);

	private:
		void make_passenger(const char* name);
		PassengerData* get_passenger_data(const char* name);
		PassengerData* get_passenger_data(int id);

		void activate_passenger(int id, const Point& start, const Point& end);
		Passenger* get_active_passenger(int id);

		/// @brief Returns the predicted total systemic unhappiness if @p target_passenger is served next.
		float get_total_unhappiness_score(const Passenger& target_passenger);

		Car car_;

		vector<PassengerData*> passenger_roster_;
		map<string, PassengerData*> passenger_name_map_;
		vector<Passenger*> active_passengers_;
		map<int, Passenger*> active_passenger_map_;

		/// The passenger the car is currently heading toward (lowest predicted systemic unhappiness).
		Passenger* next_passenger_;

		bool last_request_made_;
		bool new_request_made_;

		int num_trips_completed_;
		double average_unhappiness_;
		double average_trip_time_;
	};

}  // namespace ride_share
