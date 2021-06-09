import math

class Ball:
    def __init__(self,depth,theta):
        self.depth = depth
        self.theta = theta
        '''converts depth,theta -> (x,y)'''
        #self.cartesian = self.createCartesian(depth,theta)
        #TODO remove, testing purpuses only
        self.cartesian = (depth,theta)
    
    def createCartesian(self, length, angle):
            '''Find the cartesian coordinates given the polar coordiantes'''
            if angle < math.pi / 2:
                return (math.sin(angle)*length, math.cos(angle)*length)
            elif angle < math.pi:
                angle = angle - math.pi / 2
                return (math.cos(angle)*length, -1 * math.sin(angle)*length)
            elif angle < 3 * math.pi / 2:
                angle = angle - math.pi
                print("Angle",angle)
                print(math.sin(angle)*length)
                return (-1 * math.sin(angle)*length, -1 * math.cos(angle)*length)
            angle = angle - 3 * math.pi / 2
            print("Angle2",angle)
            print(math.sin(angle)*length)
            return (-1 * math.cos(angle)*length, math.sin(angle)*length)

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
        dirs = [[1, 0], [0, 1], [-1, 0], [0, -1],[1,1],[1,-1],[-1,1],[-1,-1]]
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


g = Map(30, 15)
tmp = Ball(0,0)
g.add_ball_obstacle(tmp)
draw_grid(g)
