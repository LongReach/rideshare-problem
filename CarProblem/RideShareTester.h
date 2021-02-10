/*
* Ryan McMahon
* 2/9/21
* mcmahonryan@hotmail.com
* Please read README.md
* Other code samples at: github.com/LongReach
*/

#pragma once
#include <string>
#include "nlohmann/json.hpp"
#include "Passenger.h"
#include "Dispatcher.h"

using namespace std;
using namespace RideShare;
using json = nlohmann::json;

class JSONException {
public:
	JSONException() {
		_info = "";
	}
	JSONException(const char* info) {
		_info = info;
	}
	JSONException(const string& info) {
		_info = info;
	}
	string get_info() { return _info; }
private:
	string _info;
};

/*
* This class is responsible for generating and running tests. Exceptions may be thrown if bad data is encountered.
*/
class RideShareTester {
public:
	RideShareTester();

	// Runs all the tests
	void run_tests();

private:
	// Runs a test based on the named JSON file. If should_fail is true, the test should trigger an exception.
	void run_json_test(const char* json_file, bool should_fail = false);
	// Runs the simulation using all the requests in the specified JSON. It lives in the /data folder.
	void load_and_run_json(const char* filename);
	// Loads a JSON file and returns a json object.
	void load_json_file(const char* filename, json& json_obj);
	// Given a JSON object specifying an individual request, process and run it.
	void process_request_json(json& json_obj, string* ret_name, int* start_x, int* start_y, int* end_x, int* end_y);
	// Given a JSON object specifying a coordinate, extract it.
	void process_coordinate_json(json& json_obj, int* ret_x, int* ret_y);
	// Converts a list of passengers into a string containing a list of names. 'if_empty_str' contains string to show
	// if passenger list is empty.
	string get_passenger_list_str(vector<PassengerData*>& the_list, const char* if_empty_str);

	// Runs a test based on a series of random requests. 
	// Params:
	//   city_size: dimensions of the city
	//   request_odds: chances, out of 100, that a passenger will make a request on a given time step.
	//   num_requests: total requests to make before simulation ends
	//   verbose: print the information for each time step
	void run_random_test(const char *test_name, int city_size, int request_odds, int num_requests, bool verbose);
};