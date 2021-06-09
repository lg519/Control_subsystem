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
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.start = (width//2, height//2)
        self.obstacle = []
    
    def in_bounds(self, node):
        (x, y) = node
        return 0 <= x < self.width and 0 <= y < self.height

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

def draw_grid(graph, **style):
    print("___" * graph.width)
    for y in range(graph.height):
        for x in range(graph.width):
            print( draw_tile(graph, (x, y), style) , end='')
        print()
    print("~~~" * graph.width)

def draw_tile(graph, node, style):
    r = " . "
    if 'path' in style and node in style['path']:   r = " @ "
    if 'start' in style and node == style['start']: r = " S "
    if 'goal' in style and node == style['goal']:   r = " G "
    if node in graph.obstacle: r = " # "
    return r

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

def dijkstra_search(graph, start, goal):
    frontier = PriorityQueue()
    frontier.put(start, 0)
    came_from = {}
    cost_so_far = {}
    came_from[start] = None
    cost_so_far[start] = 0
    
    while not frontier.empty():
        current = frontier.get()
        
        if current == goal:
            break
        
        for next in graph.neighbors(current):
            new_cost = cost_so_far[current] + 1  
            if next not in cost_so_far or new_cost < cost_so_far[next]:
                cost_so_far[next] = new_cost
                priority = new_cost
                frontier.put(next, priority)
                came_from[next] = current
    
    return came_from, cost_so_far

def reconstruct_path(came_from, start, goal):
    current = goal
    path = []
    while current != start:
        path.append(current)
        current = came_from[current]
    path.append(start) # optional
    path.reverse() # optional
    return path
    



#########################################################
balls = [Ball(15,math.pi),Ball(13,0),Ball(45,math.pi/2)]

for i in range(50):
    balls.append(Ball(random.randint(1,110),(math.pi/180)*random.randint(0,360)))

g = Map(300,300)

(g_x,g_y) = coor_to_map([142,0])
goal = (g_x,g_y)

for ball in balls:
    g.add_ball_obstacle(ball)


came_from, cost_so_far = dijkstra_search(g, g.start, goal)

draw_grid(g, path=reconstruct_path(came_from, start=g.start, goal=goal))