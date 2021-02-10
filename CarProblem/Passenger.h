/*
* Ryan McMahon
* 2/9/21
* mcmahonryan@hotmail.com
* Please read README.md
* Other code samples at: github.com/LongReach
*/

#pragma once
#include <string>
#include "Point.h"

namespace RideShare {

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
		// When there's an attempt to use coordinate outside city space
		void out_of_range(int x, int y, const char* name = NULL);
		// When passenger with given ID can't be found
		void passenger_not_found(int id);
		// When passenger with given name can't be found
		void passenger_not_found(const char* name);
		string get_info() { return _info; }
	private:
		string _info;
	};

	/*
	* Represents a known passenger in the system, by their name and an ID
	*/
	class PassengerData {
	public:
		PassengerData(const char* name, int id);
		const string& get_name() { return _name; }

	private:
		int _id;
		string _name;

		friend class Dispatcher;
	};

	/*
	* Represents a passenger in transit, i.e. waiting for a ride or in the car.
	*/
	class Passenger {
	public:
		Passenger(PassengerData* data);

		// Call when passenger has requested a ride
		void activate(const Point& start, const Point& end);
		// Returns true if passenger has been picked up
		bool is_picked_up() const { return _time_since_pickup != -1; }

		// The ideal times are: the best-case time for the car to pick up the passenger
		// and to deliver the passenger to their destination, once picked up
		void compute_ideal_times(const Point& car_pt);
		// The perfect time is the time it will take to deliver the passenger to their
		// goal, if the car takes the most effecient route starting right now.
		int compute_perfect_time(const Point& car_pt);
		// The main update function
		bool update(const Point& car_pt);

		// Returns how unhappy the passenger is right now
		float get_unhappiness_score();
		// Returns how unhappy the passenger will be if 'time_delta' steps in the future,
		// the car is at 'car_pt' and immediately proceeds to fulfill the passenger's request.
		// The unhappiness is calculated for the future moment when the passenger is actually
		// dropped off.
		float predict_unhappiness_score(const Point& car_pt, int time_delta = 0);
		// Where the car needs to go for this passenger right now, whether to pick them up or
		// drop them off.
		const Point& get_car_goal() const;

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

		friend class Dispatcher;
	};

}