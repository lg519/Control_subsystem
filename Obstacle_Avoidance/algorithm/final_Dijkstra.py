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

def dijkstra_search(graph, start, goal):
    queue = priority_queue()
    queue.push(start, 0)
    predecessor = {}
    costs = {}

    #intialize start node
    predecessor[start] = None
    costs[start] = 0
    
    while not queue.is_empty():
        current = queue.pop()
        
        if current == goal:
            break
        
        for next in graph.neighbors(current):
            new_cost = costs[current] + graph.cost(current, next)
            if next not in costs or new_cost < costs[next]:
                costs[next] = new_cost
                priority = new_cost
                queue.put(next, priority)
                predecessor[next] = current
    
    return predecessor, costs

def reconstruct_path(predecessor, start, goal):
    current = goal
    path = []
    while current != start:
        path.append(current)
        current = predecessor[current]
    path.append(start) 
    path.reverse() 
    return path