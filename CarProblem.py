import random
import copy

grid_dims=(20, 20)

# Get Manhattan distance between two points
def get_dist(pt1, pt2):
    return abs(pt2[0] - pt1[0]) + abs(pt2[1] - pt1[1])

# Find the best route through a set of points
# Params:
#   start_point: an (X,Y) tuple
#   point_list: a list of (X,Y) tuples in no particular order
# Returns route as a list of tuples, including start point
def find_best_route(start_point, point_list):
    route, route_dist = _best_route(start_point, point_list, 100)
    return route

# Recursive workhorse function. Finds routes and subroutes
# Params:
#   start_point: an (X,Y) tuple
#   point_list: a list of (X,Y) tuples in no particular order
#   dist_remaining: the route must be no longer than this
# Returns route (including start pt), total distance, or returns None if no satisfactory route exists
def _best_route(start_pt, points, dist_remaining):
    # Deal with the trivial cases first
    if dist_remaining < 0:
        return None, 0
    if len(points) == 0:
        return [start_pt], 0
    elif len(points) == 1:
        dist = get_dist(start_pt, points[0])
        if dist <= dist_remaining:
            return [start_pt, points[0]], dist
        else:
            return None, 0

    # We want to consider the nearest available points first
    points.sort(key=lambda x: get_dist(x, start_pt))

    # Obviously, there are performance issues with all the sorting and copying, but this is a
    # prototype, so that doesn't matter.

    best_dist = dist_remaining
    best_route = None
    # At each recursion, we limit the number of branches to explore to the 3 closest neighbors
    # This is an arbitrary number
    max_neighbors = 3
    for idx in range(max_neighbors if max_neighbors < len(points) else len(points)):
        pt = points[idx]
        subpoints = copy.copy(points)
        subpoints.pop(idx)
        #print("point is {},{}. subpoints:".format(pt[0], pt[1]))
        #print_route(subpoints, True)
        first_leg_dist = get_dist(start_pt, pt)
        sub_route, sub_dist = _best_route(pt, subpoints, best_dist - first_leg_dist)
        if sub_route is not None:
            dist = first_leg_dist + sub_dist
            if dist < best_dist:
                best_dist = dist
                best_route = [start_pt] + sub_route
    return best_route, best_dist

# Prints out the route in a pretty-formatted way, showing the city grid
def print_route(route, simple=False):
    global grid_dims
    grid = []
    for y in range(grid_dims[1]):
        row = [-1 for x in range(grid_dims[0])]
        grid.append(row)

    total_dist = 0
    out_str = None
    for n,pt in enumerate(route):
        grid[pt[1]][pt[0]] = n
        pt_str = "({},{})".format(pt[0], pt[1])
        if out_str is None:
            out_str = pt_str
        else:
            out_str = out_str + "-" + pt_str
            total_dist = total_dist + get_dist(pt, route[n-1])
    print(out_str, total_dist)

    if not simple:
        for y in range(grid_dims[1]):
            row_str = ""
            for x in range(grid_dims[0]):
                let = '.'
                if grid[y][x] >= 0:
                    let = chr(ord('a') + grid[y][x])
                row_str = row_str + let + " "
            print(row_str)

point_list1 = [(1, 1), (7, 7), (4, 4), (6, 6), (3, 3), (2, 2), (5, 5)]
route = find_best_route((0, 0), point_list1)
print_route(route)

point_list2 = [(random.randrange(grid_dims[0]), random.randrange(grid_dims[1])) for i in range(15)]
route = find_best_route((0, 0), point_list2)
print_route(route)