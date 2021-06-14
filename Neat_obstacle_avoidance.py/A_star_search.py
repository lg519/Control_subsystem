import math
import random
############### CONVERSION #############
# def map_to_coor(coor):
#     (x,y)= coor
#     coor[0] += 150
#     coor[1] += 150
#     return coor

def coor_to_map(coor):
    (x,y)= coor
    coor[0] = math.floor(coor[0]) + 150
    coor[1] = math.floor(coor[1]) + 150
    return(coor)

############### BALL ##############
class Ball:
    def __init__(self,depth,theta):
        self.depth = depth
        self.theta = theta
        '''converts depth,theta -> (x,y)'''
        self.cartesian = coor_to_map (self.createCartesian(depth,theta))
    
    def createCartesian(self, length, angle):
            '''Find the cartesian coordinates given the polar coordiantes'''
            if angle < math.pi / 2:
                return [math.sin(angle)*length, math.cos(angle)*length]
            elif angle < math.pi:
                angle = angle - math.pi / 2
                return [math.cos(angle)*length, -1 * math.sin(angle)*length]
            elif angle < 3 * math.pi / 2:
                angle = angle - math.pi
                print("Angle",angle)
                print(math.sin(angle)*length)
                return [-1 * math.sin(angle)*length, -1 * math.cos(angle)*length]
            angle = angle - 3 * math.pi / 2
            print("Angle2",angle)
            print(math.sin(angle)*length)
            return [-1 * math.cos(angle)*length, math.sin(angle)*length]

############ MAP ############
class Map:
    def __init__(self, x_span, y_span):
        self.x_span = x_span
        self.y_span = y_span
        self.start = (x_span//2, y_span//2)
        self.obstacle = []
    
    def in_bounds(self, node):
        (x, y) = node
        return 0 <= x < self.x_span and 0 <= y < self.y_span

    def passable(self, node):
        return node not in self.obstacle
    
    def neighbors(self, node):
        (x, y) = node
        dirs = [[1, 0], [0, 1], [-1, 0], [0, -1]]
        result = []
        neighbors_list = []
        for dir in dirs:
            neighbors_list.append( (x + dir[0], y + dir[1]) )
        result = filter(self.in_bounds, neighbors_list)
        result = filter(self.passable, result)
        return result
    
    def add_ball_obstacle(self, ball):
        (x,y) = ball.cartesian
        for i in range(x-2,x+3):
            for j in range(y-2,y+3):
                if self.in_bounds((i,j)):
                    self.obstacle.append((i,j))
    
    def cost(self,node1, node2):
        #TODO modify this function and add variable costs
        return 1

############ DRAW MAP ###########
def draw_grid(graph, **style):
    print("___" * graph.x_span)
    for y in range(graph.y_span):
        for x in range(graph.x_span):
            print( draw_tile(graph, (x, y), style) , end='')
        print()
    print("~~~" * graph.x_span)

def draw_tile(graph, node, style):
    r = " . "
    if 'path' in style and node in style['path']:   r = " @ "
    if 'start' in style and node == style['start']: r = " S "
    if 'goal' in style and node == style['goal']:   r = " G "
    if node in graph.obstacle: r = " # "
    return r

############ PRIORITY QUEUE ###########
import heapq

class PriorityQueue:
    def __init__(self):
        self.elements = []
    
    def empty(self):
        return not self.elements
    
    def put(self, item, priority):
        heapq.heappush(self.elements, (priority, item))
    
    def get(self):
        return heapq.heappop(self.elements)[1]

############ BEST PATH ALGORITHM ############
def distance(node1 , node2) :
    (x1, y1) = node1
    (x2, y2) = node2
    return abs(x1 - x2) + abs(y1 - y2)

def reconstruct_path(came_from, start, goal):
    current = goal
    path = []
    while current != start:
        path.append(current)
        current = came_from[current]
    path.append(start)
    path.reverse()
    return path

def a_star_search(graph, start, goal):
    open_set = PriorityQueue()
    open_set.put(start, 0)
    came_from = {} #map containing best path from goal to start
    g_score = {} #map containing relative score of each node. Nodes with lowest score have priority
   
   
    came_from[start] = None
    g_score[start] = 0
    
    while not open_set.empty():
        current = open_set.get()
        
        if current == goal:
            return reconstruct_path(came_from, start, goal)
        
        for neighbor in graph.neighbors(current):
            tentative_g_score = g_score[current] + graph.cost(current,neighbor) 
            if neighbor not in g_score or tentative_g_score < g_score[neighbor]:
                g_score[neighbor] = tentative_g_score
                priority = tentative_g_score + distance(neighbor, goal)
                open_set.put(neighbor, priority)
                came_from[neighbor] = current



############# EXAMPLE ################

#construct map 300 * 300
g = Map(300,300)    

# add obstacles to map
for i in range(50):  
    g.add_ball_obstacle(Ball(random.randint(1,110),(math.pi/180)*random.randint(0,360)))

# set destination
(g_x,g_y) = coor_to_map([142,0])
goal = (g_x,g_y)


#came_from, cost_so_far = a_star_search(g, g.start, goal)

draw_grid(g, path = a_star_search(g, g.start, goal))