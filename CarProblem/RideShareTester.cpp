/*
* Ryan McMahon
* 2/9/21
* mcmahonryan@hotmail.com
* Please read README.md
* Other code samples at: github.com/LongReach
*/

#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdlib.h>
#include <time.h>
#include "Dispatcher.h"
#include "RideShareTester.h"

using namespace RideShare;
using namespace std;
// Note: On Visual Studio, this only works if the language standard is ISO C++ 17
namespace fs = std::filesystem;

RideShareTester::RideShareTester() {

}

void RideShareTester::run_tests() {
	run_json_test("RideRequestsSimple.json");
	run_json_test("RideRequests1.json");
	run_json_test("RideRequestsDoubleRequest.json", true);
	run_json_test("RideRequestsBadCoordinate.json", true);
	run_json_test("RideRequestsDuplicateCoordinate.json", true);

	for (int bad_json_index = 1; bad_json_index <= 3; bad_json_index++) {
		string filename = "RideRequestsBadJSON" + to_string(bad_json_index) + ".json";
		run_json_test(filename.c_str(), true);
	}

	run_random_test("Random test 1", 15, 2, 500, false);
	run_random_test("Random test 2", 10, 5, 500, false);
	run_random_test("Random test 3", 10, 15, 500, false);
}

void RideShareTester::run_json_test(const char* json_file, bool should_fail) {
	bool failed = false;
	string info = "None";
	cout << endl << "Running test: " << string(json_file) << endl;
	cout << "----------------------------" << endl;
	try {
		load_and_run_json(json_file);
	}
	catch (PassengerException e) {
		info = e.get_info();
		failed = true;
	}
	catch (JSONException e) {
		info = e.get_info();
		failed = true;
	}
	if (should_fail != failed) {
		cout << "----------------------------" << endl;
		if (failed) {
			cout << "Test unexpectedly failed" << endl;
			cout << "Info: " << info << endl;
		}
		else {
			cout << "Test unexpectedly succeeded" << endl;
			cout << "Info: " << info << endl;
		}
	}
	else {
		cout << "----------------------------" << endl;
		string verb = should_fail ? "failed" : "succeeded";
		cout << "Test " << string(json_file) << " " << verb << " as expected." << endl;
		cout << "Info: " << info << endl;
	}
}

void RideShareTester::load_and_run_json(const char* json_file) {
	json j;
	load_json_file(json_file, j);

	Dispatcher dispatcher;
	int t = 0;
	json::iterator it = j.begin();
	// This is the main update loop for advancing through the scenario
	while (!dispatcher.is_done()) {

		// Are there requests for this time step to send? If so, extract from JSON
		if (it != j.end()) {
			if (!it->contains("requests")) {
				string info = "Missing requests object in JSON";
				JSONException ex(info);
				throw ex;
			}

			json& requests_json = *(it->find("requests"));
			// Iterate through all requests (if any)
			for (json::iterator request_it = requests_json.begin(); request_it != requests_json.end(); request_it++) {
				string name;
				int start_x, start_y, end_x, end_y;
				process_request_json(*request_it, &name, &start_x, &start_y, &end_x, &end_y);

				dispatcher.new_request(name.c_str(), start_x, start_y, end_x, end_y);
			}
			it++;
		}
		else {
			dispatcher.set_last_request_made();
		}

		// Update the simulation
		vector<PassengerData*> in_car;
		dispatcher.get_passengers_in_car(in_car);
		vector<PassengerData*> pickups;
		vector<PassengerData*> dropoffs;
		dispatcher.update(pickups, dropoffs);
		Point car_pos = dispatcher.get_car_pos();
		std::cout << "Time step: " << to_string(t) << ", car at: " << car_pos.get_string() << endl;

		std::cout << "Current passengers: " << get_passenger_list_str(in_car, "None") << endl;
		if (pickups.size() > 0) {
			std::cout << "Pickups: " << get_passenger_list_str(pickups, NULL) << endl;
		}
		if (dropoffs.size() > 0) {
			std::cout << "Dropoffs: " << get_passenger_list_str(dropoffs, NULL) << endl;
		}

		t++;
	}
	std::cout << endl << "JSON test complete for " << json_file << endl;
	int num_trips;
	float avg_unhappiness;
	float avg_trip_time;
	dispatcher.get_statistics(&num_trips, &avg_unhappiness, &avg_trip_time);
	std::cout << "Total trips: " << to_string(num_trips) << ", average unhappiness: " << to_string(avg_unhappiness) << ", average trip time: " << to_string(avg_trip_time) << endl;
}

void RideShareTester::load_json_file(const char* filename, json& json_obj) {
	// TODO: this might have to be written differently to also work on Linux
	fs::path current_path = fs::current_path();
	fs::path json_path(filename);
	fs::path full_path = current_path;
	full_path /= "data";
	full_path /= json_path;
	std::cout << "JSON path is: " << full_path << endl;
	ifstream filestream(full_path);
	filestream >> json_obj;
}

void RideShareTester::process_request_json(json& json_obj, string* ret_name, int* start_x, int* start_y, int* end_x, int* end_y)
{
	if (!json_obj.contains("name")) {
		string info = "Name missing from requests JSON object";
		JSONException ex(info);
		throw ex;
	}
	if (!json_obj.contains("start")) {
		string info = "Start missing from requests JSON object";
		JSONException ex(info);
		throw ex;
	}
	if (!json_obj.contains("end")) {
		string info = "End missing from requests JSON object";
		JSONException ex(info);
		throw ex;
	}
	json name_json = json_obj["name"];
	if (!name_json.is_string()) {
		string info = "Name is not a string";
		JSONException ex(info);
		throw ex;
	}
	*ret_name = name_json;

	json start_json = json_obj["start"];
	json end_json = json_obj["end"];
	int x[2];
	int y[2];
	json* coordinates_to_process[] = { &start_json, &end_json };
	for (int i = 0; i < 2; i++) {
		json& coord_json = *coordinates_to_process[i];
		process_coordinate_json(coord_json, &x[i], &y[i]);
	}
	*start_x = x[0];
	*start_y = y[0];
	*end_x = x[1];
	*end_y = y[1];
}

void RideShareTester::process_coordinate_json(json& json_obj, int* ret_x, int* ret_y) {
	if (!json_obj.is_array()) {
		string info = "Coordinate is not array";
		JSONException ex(info);
		throw ex;
	}
	int which_coord = 0;
	for (json::iterator it = json_obj.begin(); it != json_obj.end(); it++) {
		int val = 0;
		if (it->is_number()) {
			val = *it;
		}
		if (which_coord == 0) {
			*ret_x = val;
		}
		else {
			*ret_y = val;
		}
		which_coord++;
	}
}

string RideShareTester::get_passenger_list_str(vector<PassengerData*>& the_list, const char* if_empty_str) {
	string out_str = "";
	if (the_list.size() == 0 && if_empty_str) {
		out_str = if_empty_str;
		return out_str;
	}

	string comma_str = "";
	for (vector<PassengerData*>::iterator it = the_list.begin(); it != the_list.end(); it++) {
		out_str = out_str + comma_str + (*it)->get_name();
		comma_str = ", ";
	}
	return out_str;
}

void RideShareTester::run_random_test(const char* test_name, int city_size, int request_odds, int num_requests, bool verbose) {
	
	srand(time(NULL)); // initialize random number seed

	const char* names[] = { "Andy", "Betsy", "Charlie", "Danielle", "Emilio", "Francis", "George", "Heidi", "Igor", "Jamie" };
	int num_names = 10;
	
	Point::set_grid_dims(city_size, city_size);
	Dispatcher dispatcher;
	int t = 0;
	// This is the main update loop for advancing through the scenario
	while (!dispatcher.is_done()) {

		// Should we send any requests?
		if (num_requests > 0) {
			int n = rand() % num_names;
			if (!dispatcher.is_passenger_active(names[n])) {
				// This passenger can request a ride, so see if they should
				int dice_roll = rand() % 100;
				if (dice_roll < request_odds) {
					int start_x = rand() % city_size;
					int start_y = rand() % city_size;
					int end_x = rand() % city_size;
					int end_y = rand() % city_size;
					if (start_x != end_x || start_y != end_y) {
						dispatcher.new_request(names[n], start_x, start_y, end_x, end_y);
						num_requests--;
					}
				}
			}
		}
		else {
			dispatcher.set_last_request_made();
		}

		// Update the simulation
		vector<PassengerData*> in_car;
		dispatcher.get_passengers_in_car(in_car);
		vector<PassengerData*> pickups;
		vector<PassengerData*> dropoffs;
		dispatcher.update(pickups, dropoffs);
		Point car_pos = dispatcher.get_car_pos();
		if (verbose) {
			std::cout << "Time step: " << to_string(t) << ", car at: " << car_pos.get_string() << endl;

			std::cout << "Current passengers: " << get_passenger_list_str(in_car, "None") << endl;
			if (pickups.size() > 0) {
				std::cout << "Pickups: " << get_passenger_list_str(pickups, NULL) << endl;
			}
			if (dropoffs.size() > 0) {
				std::cout << "Dropoffs: " << get_passenger_list_str(dropoffs, NULL) << endl;
			}
		}

		t++;
	}
	std::cout << endl << "Random test " << string(test_name) << " complete." << endl;
	std::cout << "City size: (" << to_string(city_size) << "," << to_string(city_size) << "), request odds = " << to_string(request_odds) << "/100" << endl;
	int num_trips;
	float avg_unhappiness;
	float avg_trip_time;
	dispatcher.get_statistics(&num_trips, &avg_unhappiness, &avg_trip_time);
	std::cout << "Total trips: " << to_string(num_trips) << ", average unhappiness: " << to_string(avg_unhappiness) << ", average trip time: " << to_string(avg_trip_time) << endl;
}
