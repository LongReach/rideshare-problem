/**
 * @file ride_share_tester.h
 * @author Ryan McMahon (mcmahonryan@hotmail.com)
 * @brief Test harness for the ride-share dispatcher.
 */
#pragma once
#include <string>
#include "nlohmann/json.hpp"
#include "passenger.h"
#include "dispatcher.h"

using namespace std;
using namespace ride_share;
using json = nlohmann::json;

/// @brief Exception thrown when a JSON ride-request file is malformed or semantically invalid.
class JSONException {
public:
	JSONException() {
		info_ = "";
	}
	JSONException(const char* info) {
		info_ = info;
	}
	JSONException(const string& info) {
		info_ = info;
	}
	string get_info() { return info_; }
private:
	string info_;
};

/// @brief Generates and runs JSON-based and randomized dispatcher tests.
class RideShareTester {
public:
	RideShareTester();

	/// @brief Runs the full test suite.
	void run_tests();

private:
	/// @brief Runs a single JSON-file test.
	/// @param json_file Filename within the data/ directory.
	/// @param should_fail If true, the test is expected to throw an exception.
	void run_json_test(const char* json_file, bool should_fail = false);

	/// @brief Loads and fully simulates the scenario described in @p filename.
	void load_and_run_json(const char* filename);

	/// @brief Loads a JSON file from the data/ directory into @p json_obj.
	void load_json_file(const char* filename, json& json_obj);

	/// @brief Parses a single ride-request JSON object into its component fields.
	void process_request_json(json& json_obj, string* ret_name, int* start_x, int* start_y, int* end_x, int* end_y);

	/// @brief Parses a JSON coordinate array [x, y] into integer components.
	void process_coordinate_json(json& json_obj, int* ret_x, int* ret_y);

	/// @brief Returns a comma-separated string of passenger names.
	/// @param the_list List of passengers.
	/// @param if_empty_str String to return when the list is empty; nullptr means return "".
	string get_passenger_list_str(vector<PassengerData*>& the_list, const char* if_empty_str);

	/// @brief Runs a simulation driven by randomly generated ride requests.
	/// @param test_name Label printed in output.
	/// @param city_size Width and height of the square city grid.
	/// @param request_odds Probability out of 100 that a passenger requests a ride each step.
	/// @param num_requests Total requests to generate before ending the simulation.
	/// @param verbose If true, prints per-step output.
	void run_random_test(const char* test_name, int city_size, int request_odds, int num_requests, bool verbose);
};
