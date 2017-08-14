# Lance Umagat (Umagatl)
# CS 331 Assignment 1
# Cannibals and Missionaries

import sys
import collections
import heapq

# Globals
totalCreated  = 0
totalExpanded = 0
depth         = 0

# actions a state may take in form of [missionary, cannibal]
possible  = [[1,0],[2,0], [0,1], [1,1], [0,2]]
supported = ["bfs", "dfs", "iddfs", "astar"]

# priority queue referenced from
# https://www.safaribooksonline.com/library/view/python-cookbook-3rd/9781449357337/ch01s05.html
class PriorityQueue:
	def __init__(self):
		self._queue = []
		self._index = 0
		
	def __len__(self):
		return len(self._queue)
	
	def push(self, item, priority):
		heapq.heappush(self._queue, (priority, self._index, item))
		self._index += 1
	
	def pop(self):
		return heapq.heappop(self._queue)[-1]
		
class Node():
	def __init__(self, leftBank, rightBank, depth, cost, parent, action):
		# Increment global counter
		global totalCreated
		totalCreated += 1
		
		# Set puzzle banks
		self.leftBank  = leftBank
		self.rightBank = rightBank
		self.state     = tuple(self.leftBank + self.rightBank)
		
		# Set attributes
		self.depth  = depth
		self.cost   = cost
		self.parent = parent
		self.action = action

class Result():
	def __init__(self, startBank, endBank, action, boatEndBank):
		startBank[0] = startBank[0] - action[0]
		endBank[0]   = endBank[0] + action[0]
		startBank[1] = startBank[1] - action[1]
		endBank[1]   = endBank[1] + action[1]
		if boatEndBank == "left":
			self.rightBank = startBank
			self.leftBank = endBank
			self.rightBank[2] = 0
			self.leftBank[2] = 1
		else:
			self.rightBank = endBank
			self.leftBank = startBank
			self.rightBank[2] = 1
			self.leftBank[2] = 0
		self.action = action
		
def main():
	# Get and set the command line arguments
	initFileState = sys.argv[1]
	goalFileState = sys.argv[2]
	mode          = sys.argv[3]
	output        = sys.argv[4]
	
	# Set File IO
	initStateData = getFileState(initFileState)
	goalStateData = getFileState(goalFileState)
	
	# Create states
	initState = createNode(initStateData)
	goalState = createNode(goalStateData)
	
	# Execute based on mode
	if mode in supported:
		if mode == "bfs":
			fringe      = collections.deque()
			resultState = breathFirstSearch(fringe, initState, goalState)
		if mode == "dfs":
			fringe      = collections.deque()
			resultState = depthFirstSearch(fringe, initState, goalState)
		if mode == "iddfs":
			fringe      = collections.deque()
			resultState = iterativeDeepeningDFS(fringe, initState, goalState)
		if mode == "astar":
			fringe      = PriorityQueue()
			resultState = aStarSearch(fringe, initState, goalState)
	else:
		sys.exit("Mode not supported!")
	
	printToUser(resultState)
	
	outputFile = open(output, 'w')
	outputFile.write('\n')
	outputFile.write(str(findSolution(resultState)))
	outputFile.write('\n')
	outputFile.close()
	
	
	
def getFileState(file):
	with open(file) as theFile:
		data = theFile.readlines()
	return data
	
def createNode(data):
	return Node(map(int, data[0].strip('\n').split(',')), map(int, data[1].strip('\n').split(',')), 0, 0, None, None)

def breathFirstSearch(fringe, initState, goalState):
	global totalCreated, totalExpanded, depth
	closedList = {}
	fringe.append(initState)
	
	while True:
		if len(fringe) == 0:
			sys.exit("No Solution found")
		
		# Remove the fringe
		current = fringe.popleft()
		
		# Check if next if goal state
		if (current.leftBank == goalState.leftBank) and (current.rightBank == goalState.rightBank):
			return current
			
		# Check if closed list and expand if needed
		if current.state in closedList and current.depth >= closedList[current.state]:
			continue
		else:
			closedList[current.state] = current.depth
			map(fringe.append, expandNode(current))
			totalExpanded += 1
			
def depthFirstSearch(fringe, initState, goalState):
	global totalCreated, totalExpanded, depth
	closedList ={}
	fringe.append(initState)
	
	while True:
		if len(fringe) == 0:
			sys.exit("No Solution Path Found")
			
		# Remove from fringe
		current = fringe.pop()
		
		# Check if we're in the goal state
		if (current.leftBank == goalState.leftBank) and (current.rightBank == goalState.rightBank):
			return current
		# Check if in closed list and expand when necessary
		if current.state in closedList and current.depth >= closedList[current.state]:
			continue
		else:
			if current.depth > 400:
				continue
			closedList[current.state] = current.depth
			map(fringe.append, expandNode(current))
			totalExpanded += 1
			
def iterativeDeepeningDFS(fringe, initState, goalState):
	global totalCreated, totalExpanded, depth
	closedList = {}
	fringe.append(initState)
	
	while True: 
		if len(fringe) == 0:
			if depth > 400:
				sys.exit("Depth Limit Reached!")
			
			closedList = {}
			depth += 1
			totalCreated = 0;
			fringe.append(initState)
			continue
			
		#remove from fringe
		current = fringe.pop()
		
		# Check if next if goal state
		if (current.leftBank == goalState.leftBank) and (current.rightBank == goalState.rightBank):
			return current
			
		# Check if closed list and expand if needed
		if current.state in closedList and current.depth >= closedList[current.state]:
			continue
		else:
			closedList[current.state] = current.depth
			map(fringe.append, expandNodeIDDFS(current))
			totalExpanded += 1
			
def aStarSearch(fringe, initState, goalState):
	global totalCreated, totalExpanded, depth
	closedList = {}
	fringe.push(initState, initState.cost)
	
	while True:
		if len(fringe) == 0:
			sys.exit("No Solution found")
		
		# Remove the fringe
		current = fringe.pop()
		
		# Check if next if goal state
		if (current.leftBank == goalState.leftBank) and (current.rightBank == goalState.rightBank):
			return current
			
		# Check if closed list and expand if needed
		if current.state in closedList and current.depth >= closedList[current.state]:
			continue
		else:
			closedList[current.state] = current.depth
			map(lambda i : fringe.push(i, i.cost + aStarHeuristic(i, goalState)),expandNode(current))
			totalExpanded += 1
	
def expandNode(node):
	global possible
	successorNodes = []
	
	validAction = filter(lambda i: checkAction(i, node), possible)
	expandedNodes = map(lambda j: executeAction(j, node), validAction)
	
	for succ in expandedNodes:
		updateNode = Node(succ.leftBank, succ.rightBank, node.depth + 1, node.depth + 1, node, succ.action)
		successorNodes.append(updateNode)
	return successorNodes
	
def expandNodeIDDFS(node):
	global possible
	successorNodes = []
	
	if node.depth == depth:
		expandedNodes = []
	else:
		validAction = filter(lambda i: checkAction(i, node), possible)
		expandedNodes = map(lambda j: executeAction(j, node), validAction)
	
	for succ in expandedNodes:
		updateNode = Node(succ.leftBank, succ.rightBank, node.depth + 1, node.depth + 1, node, succ.action)
		successorNodes.append(updateNode)
	return successorNodes
	
def checkAction(action, node):

	# Check where the boat is
	if node.leftBank[2] == 1:
		startBank = list(node.leftBank)
		endBank   = list(node.rightBank)
	else:
		startBank = list(node.rightBank)
		endBank   = list(node.leftBank)
	
	# Perform and check result
	startBank[0] = startBank[0] - action[0]
	endBank[0]   = endBank[0] + action[0]
	startBank[1] = startBank[1] - action[1]
	endBank[1]   = endBank[1] + action[1]
	
	# Compare missionary to cannibal
	if(startBank[0] < 0) or (startBank[1] < 0) or (endBank[0] < 0) or (endBank[1] < 0):
		return False
	elif((startBank[0] == 0) or (startBank[0] >= startBank[1])) and ((startBank[0] == 0) or (startBank[0] >= startBank[1])):
		return True
	else:
		return False

def executeAction(action, node):
	if node.leftBank[2] == 1:
		result = Result(list(node.leftBank), list(node.rightBank), action, "right")
	else:
		result = Result(list(node.rightBank), list(node.leftBank), action, "left")
	return result
	
def aStarHeuristic(current, goalState):
	if goalState.leftBank[2] == 1:
		heuristic = (current.rightBank[0] + current.rightBank[1]) / 2
	else:
		heuristic = (current.leftBank[0] + current.leftBank[1]) / 2
	return heuristic
	
def printToUser(resultState):
	print "Total expanded Nodes: {0}".format(totalExpanded)
	print "Solution Path Length: {0}".format(len(findSolution(resultState)))
	print findSolution(resultState)
	
# http://stackoverflow.com/questions/8922060/how-to-trace-the-path-in-a-breadth-first-search
def findSolution(node):
	path    = []
	current = node
	
	while True:
		try:
			if current.parent != None:
				path.append(current.action)
		except:
			break
		current = current.parent
	return path

main()
