import collections

class Queue:
    def __init__(self):
        self.elements = collections.deque()
    
    def empty(self):
        return not self.elements
    
    def put(self, x):
        self.elements.append(x)
    
    def get(self):
        return self.elements.popleft()

class Map:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.walls = []
    
    def in_bounds(self, node):
        (x, y) = node
        return 0 <= x < self.width and 0 <= y < self.height

    def passable(self, node):
        return node not in self.walls
    
    def neighbors(self, node):
        (x, y) = node
        dirs = [[1, 0], [0, 1], [-1, 0], [0, -1],[1,1],[1,-1],[-1,1],[-1,-1]]
        result = []
        neighbors_list = []
        for dir in dirs:
            neighbors_list.append( (x + dir[0], y + dir[1]) )
        result = filter(self.in_bounds, neighbors_list)
        result = filter(self.passable, result)
        return result


# utility functions for dealing with square grids
def from_id_width(node, width):
    return (node % width, node // width)

def draw_tile(graph, node, style):
    r = " . "
    if 'number' in style and node in style['number']: r = " %-2d" % style['number'][node]
    if 'point_to' in style and style['point_to'].get(node, None) is not None:
        (x1, y1) = node
        (x2, y2) = style['point_to'][node]
        if x2 == x1 + 1: r = " > "
        if x2 == x1 - 1: r = " < "
        if y2 == y1 + 1: r = " v "
        if y2 == y1 - 1: r = " ^ "
    if 'path' in style and node in style['path']:   r = " @ "
    if 'start' in style and node == style['start']: r = " A "
    if 'goal' in style and node == style['goal']:   r = " Z "
    if node in graph.walls: r = " # "
    return r

def draw_grid(graph, **style):
    print("___" * graph.width)
    for y in range(graph.height):
        for x in range(graph.width):
            print("%s" % draw_tile(graph, (x, y), style), end="")
        print()
    print("~~~" * graph.width)


DIAGRAM1_WALLS = [from_id_width(node, width=30) for node in [21,22,51,52,81,82,93,94,111,112,123,124,133,134,141,142,153,154,163,164,171,172,173,174,175,183,184,193,194,201,202,203,204,205,213,214,223,224,243,244,253,254,273,274,283,284,303,304,313,314,333,334,343,344,373,374,403,404,433,434]]


def breadth_first_search(graph, start):
    frontier = Queue()
    frontier.put(start)
    came_from = {}
    came_from[start] = None
    
    while not frontier.empty():
        current = frontier.get()
        for next in graph.neighbors(current):
            if next not in came_from:
                frontier.put(next)
                came_from[next] = current
    
    return came_from

g = Map(30, 15)
g.walls = DIAGRAM1_WALLS

start = (8, 7)
parents = breadth_first_search(g, start)
draw_grid(g, point_to=parents, start=start)