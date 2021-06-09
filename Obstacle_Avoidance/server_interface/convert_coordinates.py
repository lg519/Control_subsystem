from Obstacle_Avoidance.server_interface.map import Map, draw_grid

import math

balls = [Ball(15,math.pi),Ball(13,0)]

g = Map(300,300)

for ball in balls:
    g.add_ball_obstacle(ball)

draw_grid(g)