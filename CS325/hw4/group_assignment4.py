from pulp import *
import math
import csv
import plotly.plotly as py
from plotly.graph_objs import *

inputFile = open("Corvallis_data.csv", 'r')
solutions, solutionsLinear = [], []
avgTemps, daysPassed, bestFitTemps = [], [], [] #used for days and for average temps

lineCount = 0
#Location to read information from
for line in inputFile:
    if(lineCount != 0):
        line = line.split(';')
        avgTemps.append(float(str(line[7])))
        daysPassed.append(int(line[8].replace('\n', '')))
    lineCount+=1
DATA_SET = len(daysPassed)

#Initialize Problem
lprob = LpProblem('Best Fit Line Problem', LpMinimize)

#set decision variables
X = LpVariable('Z', 0)
x0 = LpVariable('x0', 0)
x1 = LpVariable('x1', 0)
x2 = LpVariable('x2', 0)
x3 = LpVariable('x3', 0)
x4 = LpVariable('x4', 0)
x5 = LpVariable('x5', 0)

#separate parts of main equation
linear = 0
seasonal = 0
solar = 0

#What we want to minimize for our LpProblem
lprob += X

#get max deviation
for i in range(0, DATA_SET):
	#linear part
    linear = (x0 + x1 * daysPassed[i])
	#seasonal part
    seasonal = (x2 * math.cos((2*math.pi*daysPassed[i])/(365.25)) + (x3 * math.sin((2*math.pi*daysPassed[i])/(365.25))))
    #solar part
    solar = (x4 * math.cos((2*math.pi * daysPassed[i])/(364.25*10.7)) + (x5 * math.sin((2*math.pi*daysPassed[i])/(364.25 * 10.7))))
	#main equation
    lprob += X >= avgTemps[i] - (linear + seasonal + solar)
    lprob += -1 * X <= avgTemps[i] - (linear + seasonal + solar)
	
#solve LP
status = lprob.solve()

for i in range(0, DATA_SET):
	#linear part
    linear = (value(x0) + value(x1) * daysPassed[i])
	#seasonal part
    seasonal = (value(x2) * math.cos((2*math.pi*daysPassed[i])/(365.25)) + (value(x3) * math.sin((2*math.pi*daysPassed[i])/(365.25))))
	#solar part
    solar = (value(x4) * math.cos((2*math.pi * daysPassed[i])/(364.25*10.7)) + (value(x5) * math.sin((2*math.pi*daysPassed[i])/(364.25 * 10.7))))
    tD = linear + seasonal + solar
    solutions.append(tD)
    solutionsLinear.append(linear)

#print out values
print("Status: " + LpStatus[lprob.status])
for v in lprob.variables():
    print(v.name + " = " + str(v.varValue))
	

#plot data
avgTemp = Scatter(
	x = daysPassed,
	y = avgTemps,
	name = "Data points (Avgerage Temperatures)")
	
bestFitLine = Scatter(
	x = daysPassed,
	y = solutions,
	name = "Best-Fit Line")

trendLine = Scatter(
	x = daysPassed,
	y = solutionsLinear,
	name = "Trend Line")
	
data = Data([avgTemp, bestFitLine, trendLine])

#layout
layout = Layout(
	title = "Average Temperature Vs. Days Passed",
	xaxis = XAxis(
		title = "Days passed"),
	yaxis = YAxis(
		title = "Average Temperature (1/10 of Degrees Celsius)"))

#Graph it
fig = Figure(data = data, layout = layout)
unique_url = py.plot(fig, filename = 'Average Temperatures Vs. Days Passed')
