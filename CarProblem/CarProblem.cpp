// CarProblem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Point.h"
#include "Passenger.h"
#include "Dispatcher.h"
#include "RideShareTester.h"

using namespace std;
using namespace RideShare;

int main()
{
    Point::set_grid_dims(10, 10);
    Point point(2, 3);

#if 0
    PassengerData data("monkey", 0);
    Passenger passenger(&data);
    Point start(0, 0);
    Point end(9, 9);
    passenger.activate(start, end);
#endif

#if 0
    Dispatcher dispatcher;

    std::cout << "Launching Dispatcher!\n";

    // make some random trips
    dispatcher.new_request("George", 5, 0, 7, 3);
    dispatcher.new_request("Fido", 5, 1, 7, 4);
    dispatcher.new_request("Amy", 5, 5, 2, 8);
    dispatcher.new_request("Vader", 9, 9, 3, 3);
    dispatcher.set_last_request_made();

    int t = 0;
    while (!dispatcher.is_done()) {
        vector<PassengerData*> in_car;
        dispatcher.get_passengers_in_car(in_car);
        vector<PassengerData*> pickups;
        vector<PassengerData*> dropoffs;
        dispatcher.update(pickups, dropoffs);
        Point car_pos = dispatcher.get_car_pos();
        cout << "Time step: " << to_string(t) << ", car at: " << car_pos.get_string() << endl;

        cout << "Current passengers: " << get_passenger_list_str(in_car, "None") << endl;
        if (pickups.size() > 0) {
            cout << "Pickups: " << get_passenger_list_str(pickups, NULL) << endl;
        }
        if (dropoffs.size() > 0) {
            cout << "Dropoffs: " << get_passenger_list_str(dropoffs, NULL) << endl;
        }

        t++;
    }

    json j2 = {
      {"pi", 3.141},
      {"happy", true},
      {"name", "Niels"},
      {"nothing", nullptr},
      {"answer", {
        {"everything", 42}
      }},
      {"list", {1, 0, 2}},
      {"object", {
        {"currency", "USD"},
        {"value", 42.99}
      }}
    };
    cout << j2.dump() << endl;
#endif

    RideShareTester tester;
    tester.run_tests();
    exit(0);
}

