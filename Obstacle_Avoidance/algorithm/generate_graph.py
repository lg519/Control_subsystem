all_nodes = []
for x in range(20):
    for y in range(10):
        all_nodes.append([x, y])

#print(*all_nodes, sep = "\n")

def neighbors(node):
    dirs = [[1, 0], [0, 1], [-1, 0], [0, -1],[1,1],[1,-1],[-1,1],[-1,-1]]
    result = []
    for dir in dirs:
        neighbor = [node[0] + dir[0], node[1] + dir[1]]
        if neighbor in all_nodes:
            result.append(neighbor)
    return result

tmp = neighbors([3,2])
print(*tmp, sep = "\n")