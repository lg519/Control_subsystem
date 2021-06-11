import heapq

class priority_queue:
    def __init__(self):
        self.elements = []
    
    def is_empty(self):
        return not self.elements
    
    def push(self, item, priority):
        heapq.heappush(self.elements, (priority, item))
    
    def pop(self):
        return heapq.heappop(self.elements)[1]

def best_path(graph, start, goal):
    queue = priority_queue()
    queue.push(start, 0)
    predecessor = {}
    costs = {}

    #intialize start node
    predecessor[start] = None
    costs[start] = 0
    
    while True:
        
        if queue.is_empty():
            break

        current = queue.pop()     
        
        if current == goal:
            break
        
        for next in graph.neighbors(current):
            current_cost = costs[current] + graph.cost(current, next)
            if next not in costs or current_cost < costs[next]:
                costs[next] = current_cost
                priority = current_cost
                queue.put(next, priority)
                predecessor[next] = current
    
    return predecessor, costs

def next_move(predecessor, start, goal):
    current = goal
    path = []
    while current != start:
        path.append(current)
        current = predecessor[current] 
    return path[-1]