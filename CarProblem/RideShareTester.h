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

class RideShareTester {
public:
	RideShareTester();

	void run_tests();

private:
	void run_json_test(const char* json_file, bool should_fail = false);
	void load_and_run_json(const char* filename);
	void load_json_file(const char* filename, json& json_obj);
	void process_request_json(json& json_obj, string* ret_name, int* start_x, int* start_y, int* end_x, int* end_y);
	void process_coordinate_json(json& json_obj, int* ret_x, int* ret_y);
	string get_passenger_list_str(vector<PassengerData*>& the_list, const char* if_empty_str);
};