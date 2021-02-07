import random

grid_dims=(10, 10)

# Get Manhattan distance between two points, each is a tuple (X, Y)
def get_dist(pt1, pt2):
    return abs(pt2[0] - pt1[0]) + abs(pt2[1] - pt1[1])

# Represents a passenger, either one in the roster of known travelers, or an active one.
# An active passenger is at any stage of his/her journey, including awaiting pickup
class Passenger:

    passenger_roster = []
    active_passengers = []
    active_passengers_dict = {}
    total_trips_recorded = 0
    average_unhappiness = 0
    average_time_elapsed = 0

    def __init__(self, id):
        self.id = id
        self.start = (0, 0)
        self.end = (0, 0)
        self.ideal_pickup_time = None
        self.ideal_journey_time = None
        self.time_elapsed = 0
        self.time_since_pickup = None
        self.dropped_off = False
        self.system_unhappiness_score = 0

    # Call to begin the journey of active passenger
    def activate(self, start, end):
        self.start = start
        self.end = end
        self.ideal_pickup_time = None
        self.ideal_journey_time = None
        self.time_elapsed = 0
        self.time_since_pickup = None
        self.dropped_off = False

    def is_picked_up(self):
        return self.time_since_pickup is not None

    # Ideal time is the best-case scenario for the car to come to the passenger and deliver them to their destination.
    # It's computed at the moment the passenger first summons the car. It assumes that the car goes by the most
    # efficient route to the passenger, doesn't divert, and goes straight to the destination. The
    # unhappiness score is generated relative to this value.
    def compute_ideal_time(self, car_pt):
        self.ideal_pickup_time = get_dist(car_pt, self.start)
        self.ideal_journey_time = get_dist(self.start, self.end)

    # Perfect time is similar to ideal time, but at a given instant. If the passenger is already in the car,
    # perfect time is concerned only with the remaining journey. The value is returned.
    def compute_perfect_time(self, car_pt):
        perfect_time = 0
        if not self.is_picked_up():
            perfect_time = get_dist(car_pt, self.start) + get_dist(self.start, self.end)
        else:
            perfect_time = get_dist(car_pt, self.end)
        return perfect_time

    # car_pt is car's current location. We assume that time always advances by 1 unit
    def update(self, car_pt):
        event = False
        if self.is_picked_up():
            if car_pt == self.end:
                self.dropped_off = True
                event = True
        else:
            if car_pt == self.start:
                self.time_since_pickup = 0
                event = True
        self.time_elapsed = self.time_elapsed + 1
        if self.is_picked_up():
            self.time_since_pickup = self.time_since_pickup + 1
        return event

    def get_nearness_score(self, car_pt):
        nearness_dist = get_dist(car_pt, self.get_car_goal())
        nearness_score = (3.0 / nearness_dist) if nearness_dist > 0 else 3.0
        return nearness_score

    # We assume that passengers don't start to become unhappy until elapsed time has gone over ideal time by
    # 50% (the tolerance threshold). Before that moment, there will be a negative unhappiness score, which can
    # be thought of as happiness.
    def _do_unhappiness_calc(self, time_elapsed):
        tolerance_threshold = 1.5
        ideal_time = self.ideal_journey_time if self.is_picked_up() else self.ideal_pickup_time
        if ideal_time == 0: ideal_time = 1
        result = time_elapsed / ideal_time - tolerance_threshold
        return result
        #return 0.0 if result < 0.0 else result

    def get_unhappiness_score(self):
        time_elapsed = self.time_since_pickup if self.is_picked_up() else self.time_elapsed
        return self._do_unhappiness_calc(time_elapsed)

    # If time_delta time steps in the future, the car is at car_pt, return how unhappy the passenger will be
    # by the time the passenger is dropped off. This assumes that the car makes perfect time to the passenger
    # from car_pt
    def predict_unhappiness_score(self, car_pt, time_delta = 0):
        time_elapsed = self.time_since_pickup if self.is_picked_up() else self.time_elapsed
        return self._do_unhappiness_calc(time_elapsed + time_delta + self.compute_perfect_time(car_pt))

    def get_car_goal(self):
        return self.end if self.is_picked_up() else self.start

    @staticmethod
    def make_passenger(id):
        passenger = Passenger(id)
        Passenger.passenger_roster.append(passenger)

    @staticmethod
    def activate_passenger(start, end, car_pt, id):
        passenger = Passenger(id)
        passenger.activate(start, end)
        passenger.compute_ideal_time(car_pt)
        Passenger.active_passengers.append(passenger)
        Passenger.active_passengers_dict[id] = passenger

    @staticmethod
    def is_passenger_active(id):
        return Passenger.active_passengers_dict.get(id) is not None

    @staticmethod
    def get_total_unhappiness_score(car_pt, target_psgr):
        time_delta = get_dist(car_pt, target_psgr.get_car_goal())
        total_score = 0
        for p in Passenger.active_passengers:
            total_score = total_score + p.predict_unhappiness_score(target_psgr.get_car_goal(), time_delta)
        return total_score

    @staticmethod
    def update_all(car_pt):
        new_passenger_list = []
        event_occurred = False
        for p in Passenger.active_passengers:
            if p.update(car_pt): event_occurred = True
            if p.dropped_off:
                Passenger.active_passengers_dict.pop(p.id, None)
                Passenger.average_unhappiness = (Passenger.average_unhappiness * Passenger.total_trips_recorded +
                                                 p.get_unhappiness_score()) / (Passenger.total_trips_recorded + 1)
                Passenger.average_time_elapsed = (Passenger.average_time_elapsed * Passenger.total_trips_recorded +
                                                 p.time_elapsed) / (Passenger.total_trips_recorded + 1)
                Passenger.total_trips_recorded = Passenger.total_trips_recorded + 1
            else:
                new_passenger_list.append(p)
        Passenger.active_passengers = new_passenger_list
        return event_occurred

    @staticmethod
    def reorder(car_pt):
        for p in Passenger.active_passengers:
            system_unhappiness = Passenger.get_total_unhappiness_score(car_pt, p)
            p.system_unhappiness_score = system_unhappiness

        def _key_func(psgr):
            return psgr.system_unhappiness_score
        Passenger.active_passengers.sort(key=_key_func, reverse=False)

class Car:

    car = None

    def __init__(self):
        self.pos = (0, 0)
        self.movement_axis = 0 # 0 for x, 1 for y
        self.updated = False
        if Car.car is None: Car.car = self

    def update(self):
        if not self.updated:
            self.updated = True
            return
        goal = self.pos
        if len(Passenger.active_passengers) > 0:
            goal = Passenger.active_passengers[0].get_car_goal()

        delta = (goal[0] - self.pos[0], goal[1] - self.pos[1])
        movement_required = delta != (0, 0)
        while movement_required:
            if self.movement_axis == 0:
                if delta[0] != 0:
                    self.pos = (self.pos[0] + (1 if delta[0] > 0 else -1), self.pos[1])
                    movement_required = False
                else:
                    self.movement_axis = 1
            if self.movement_axis == 1:
                if delta[1] != 0:
                    self.pos = (self.pos[0], self.pos[1] + (1 if delta[1] > 0 else -1))
                    movement_required = False
                else:
                    self.movement_axis = 0

class System:

    def __int__(self):
        pass

    @staticmethod
    def print_grid(t):
        global grid_dims
        grid = []
        for y in range(grid_dims[1]):
            row = ["" for x in range(grid_dims[0])]
            grid.append(row)

        def _add_letter(pos, let):
            grid[pos[1]][pos[0]] = grid[pos[1]][pos[0]] + let
            if len(grid[pos[1]][pos[0]]) > 3:
                _str = grid[pos[1]][pos[0]]
                _str = _str[:3]
                grid[pos[1]][pos[0]] = _str

        for passenger in Passenger.active_passengers:
            start_let = chr(ord('A') + passenger.id)
            end_let = chr(ord('a') + passenger.id)
            start = passenger.start
            end = passenger.end
            if not passenger.is_picked_up():
                _add_letter(start, start_let)
            else:
                _add_letter(end, end_let)

        _add_letter(Car.car.pos, '*')

        print("Time step:", t)
        for y in range(grid_dims[1]):
            row_str = ""
            for x in range(grid_dims[0]):
                _str = grid[y][x]
                if len(_str) == 0:
                    _str = "."
                _str = "{:4}".format(_str)
                row_str = row_str + _str
            print(row_str)

        for passenger in Passenger.active_passengers:
            let = chr(ord('A') + passenger.id)
            print("Passenger {}: unhappiness prediction {:.2f}, system unhappiness {:.2f}".format(let, passenger.predict_unhappiness_score(Car.car.pos), passenger.system_unhappiness_score))

    @staticmethod
    def setup():
        car = Car()
        for i in range(10):
            Passenger.make_passenger(id)

    @staticmethod
    def activate_new_passengers():
        car = Car.car
        need_reorder = False
        for id in range(10):
            if Passenger.active_passengers_dict.get(id) is None:
                # This passenger has a one in twenty-five chance of wanting a ride
                r = random.randrange(25)
                if r == 0:
                    start_pos = (random.randrange(grid_dims[0]), random.randrange(grid_dims[1]))
                    end_pos = (random.randrange(grid_dims[0]), random.randrange(grid_dims[1]))
                    if start_pos != end_pos:
                        Passenger.activate_passenger(start_pos, end_pos, car.pos, id)
                        need_reorder = True
        if need_reorder:
            Passenger.reorder(Car.car.pos)

    @staticmethod
    def update():
        car = Car.car
        System.activate_new_passengers()
        event_occurred = Passenger.update_all(car.pos)
        if event_occurred:
            Passenger.reorder(car.pos)
        car.update()

System.setup()
t = 0
while(True):
    System.update()
    System.print_grid(t)
    t = t + 1
    x = input()
    if x == "exit":
        break

print("Average unhappiness: {:.3f}".format(Passenger.average_unhappiness))
print("Average trip time: {:.3f}".format(Passenger.average_time_elapsed))
