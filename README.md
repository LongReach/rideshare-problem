# The Car Problem

## Process

I decided to write my initial prototypes in Python, because it's easier to hack stuff together in that language than in C++.

## First Pass

`CarProblem.py`

I first considered the problem as a version of the Traveling Salesman Problem. I generated a random list of points (within the grid space), then searched for the most efficient path through them. This was a process of recursively exploring a search tree, but constraining the search to reduce the number of branches generated at each node (i.e. only the closest unvisited points are considered as a next step). I'd also abort the recursion if the distance traveled had surpassed the best route found so far.

#### Sample Output

The letters indicate the order in which points are visited.
```
(0,0)-(0,3)-(6,5)-(2,10)-(1,11)-(0,14)-(3,19)-(4,14)-(7,14)-(11,16)-(14,15)-(15,12)-(10,10)-(12,5)-(12,4)-(19,3) 80
a . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . .
b . . . . . . . . . . . . . . . . . . p
. . . . . . . . . . . . o . . . . . . .
. . . . . . c . . . . . n . . . . . . .
. . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . .
. . d . . . . . . . m . . . . . . . . .
. e . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . l . . . .
. . . . . . . . . . . . . . . . . . . .
f . . . h . . i . . . . . . . . . . . .
. . . . . . . . . . . . . . k . . . . .
. . . . . . . . . . . j . . . . . . . .
. . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . .
. . . g . . . . . . . . . . . . . . . .

(venv) D:\CodingProjects\Python\CarProblem>
```

#### Disadvantages

This isn't the fastest solution when there are a LOT of points to cover, although I'm sure performance in C++ would have been satisfactory. Also, I'd made no distinction between pick-up and drop-off points, and anticipated that that requirement would make the recursive function more complicated.

Then there's the issue that, realistically, passengers would expect the car to try to prioritize earlier requests over later ones. An efficient route planner doesn't take that into account. It's easy to imagine a situation where someone on a long trip keeps being diverted as local requests pop up.

Finally, this felt like too old-school of an algorithm, effective for a rigid set of requirements, but not very flexible or adaptable.

## Second Pass

`CarProblem2.py`

I thought about the problem in terms of maximizing passenger happiness (or, really, minimizing *unhappiness*). A passenger who is waiting for a ride has an expectation of being picked up within a certain amount of time after they summon the car. If that threshold is surpassed, the passenger grows more and more unhappy.

Similarly, once a passenger boards the car, they have a hope that the journey will take a certain amount of time, and they grow unhappier the more time they spend in the car beyond that amount. Fortunately, passengers who anticipated a longer trip are more tolerant of delay (in absolute terms) than passengers who expected a shorter trip. Naturally, a person going across town to the airport will add more cushion time to their plans than a person going to a nearby bar.

The car chooses its next passenger to pick up or drop off by predicting how much unhappiness a given choice will bring to ALL of the active passengers (including those waiting for a ride). The goal is to minimize overall unhappiness within the system. Because of the predictive factor, looking forward to when and where the car will be after arriving at a certain destination, the planning system is discouraged from wandering too far away from nearby pick-up or drop-off points, especially those with a higher level of urgency.

On each step, the Python program prints out a map of the city. It also prints out statistics on the systemic unhappiness that will be caused by selecting each active passenger as the next choice. The one with the lowest systemic happiness score is the best choice.

The program also keeps statistics about the average passenger unhappiness and journey time. I tried different ideas, seeking to minimize these over the course of hundreds of time steps. I also noticed that if I hold down the "Enter" key, the flipbook animation that results shows the car making what generally look like sensible moves. It reminded me, pleasingly, of the [last ASCII-based action game](https://www.youtube.com/watch?v=sj8SjoMIeOU) I'd played.

#### Sample Output

Here, the car (a "*") has just arrived where Passenger D is. It would bring about the least systemic unhappiness to pick up passenger D, followed by Passenger E and B, then dropping off Passenger I.

Dropping off A is the least good choice right now

```
Time step: 155
.   .   .   .   .   .   .   .   a   .
.   .   .   .   .   .   h   .   .   .
J   .   .   .   .   .   .   .   .   .
.   .   .   .   .   .   .   .   .   .
.   .   .   .   .   .   .   .   .   .
.   E   .   .   .   .   .   .   .   .
.   .   D*  .   .   .   .   .   .   .
.   .   .   .   .   .   .   .   .   .
i   .   .   .   .   .   .   .   .   .
.   B   .   .   .   .   .   .   .   .
Passenger D: unhappiness prediction 5.00, system unhappiness 41.72
Passenger E: unhappiness prediction 3.67, system unhappiness 47.00
Passenger B: unhappiness prediction 4.33, system unhappiness 49.30
Passenger I: unhappiness prediction 14.80, system unhappiness 49.60
Passenger J: unhappiness prediction 3.83, system unhappiness 60.02
Passenger H: unhappiness prediction 2.25, system unhappiness 71.02
Passenger A: unhappiness prediction 7.83, system unhappiness 81.87
```

## Improvements

There are countless ways to tweak this system.

One idea is to add a factor that makes idealized passengers more patient (less quick to become unhappy) at times when there's more demand for the car.

Another idea is to give passengers a convenience score, so the car is encouraged to go to nearby points even when the happiness of that particular passenger isn't in immediate danger.