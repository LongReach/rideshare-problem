/**
 * @file passenger.h
 * @author Ryan McMahon (mcmahonryan@hotmail.com)
 * @brief Passenger classes and exception type for the ride-share simulation.
 */
#pragma once
#include <string>
#include "point.h"

namespace ride_share {

	using namespace std;

	/// @brief Exception thrown for invalid passenger state or out-of-range coordinates.
	class PassengerException {
	public:
		PassengerException() {
			info_ = "";
		}
		PassengerException(const char* info) {
			info_ = info;
		}
		PassengerException(const string& info) {
			info_ = info;
		}
		/// @brief Populates the exception for a coordinate that falls outside the city grid.
		void out_of_range(int x, int y, const char* name = nullptr);
		/// @brief Populates the exception for a passenger ID that cannot be found.
		void passenger_not_found(int id);
		/// @brief Populates the exception for a passenger name that cannot be found.
		void passenger_not_found(const char* name);
		string get_info() { return info_; }
	private:
		string info_;
	};

	/// @brief Lightweight record identifying a known passenger by name and ID.
	class PassengerData {
	public:
		PassengerData(const char* name, int id);
		const string& get_name() { return name_; }

	private:
		int id_;
		string name_;

		friend class Dispatcher;
	};

	/// @brief Represents a passenger in transit — waiting for pickup or riding in the car.
	class Passenger {
	public:
		Passenger(PassengerData* data);

		/// @brief Activates a new ride request for this passenger.
		/// @param start Pickup location.
		/// @param end Drop-off location.
		void activate(const Point& start, const Point& end);

		/// @brief Returns true if the passenger has been picked up.
		bool is_picked_up() const { return time_since_pickup_ != -1; }

		/// @brief Computes best-case pickup and journey times from the car's current position.
		/// @param car_pt Current car position.
		void compute_ideal_times(const Point& car_pt);

		/// @brief Returns the minimum time to fully serve this passenger starting from car_pt right now.
		/// @param car_pt Current car position.
		int compute_perfect_time(const Point& car_pt);

		/// @brief Advances the passenger state by one time step.
		/// @param car_pt Current car position.
		/// @return True if a state change (pickup or drop-off) occurred.
		bool update(const Point& car_pt);

		/// @brief Returns the passenger's current unhappiness score.
		float get_unhappiness_score();

		/// @brief Predicts unhappiness assuming the car heads to this passenger next.
		///
		/// Calculates the unhappiness at the future moment of drop-off, given that
		/// @p time_delta steps pass before the car begins serving this passenger.
		///
		/// @param car_pt Position the car will be at when it starts serving this passenger.
		/// @param time_delta Steps until the car begins heading toward this passenger.
		/// @return Predicted unhappiness at time of drop-off.
		float predict_unhappiness_score(const Point& car_pt, int time_delta = 0);

		/// @brief Returns the position the car must reach next for this passenger (pickup or drop-off).
		const Point& get_car_goal() const;

	private:
		/// @brief Core unhappiness formula given an elapsed time value.
		float do_unhappiness_calc(int time_elapsed);

		PassengerData* data_;
		Point start_;
		Point end_;
		int ideal_pickup_time_;
		int ideal_journey_time_;
		int time_elapsed_;
		int time_since_pickup_;
		bool dropped_off_;

		friend class Dispatcher;
	};

}  // namespace ride_share
