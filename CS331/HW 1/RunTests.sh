#!/bin/bash
#800080
#use -i flag to pause and see results in terminal

#BFS
echo "BFS Tests"
python umagatl_CaM.py testStart1.txt testGoal1.txt bfs output.txt
python umagatl_CaM.py testStart2.txt testGoal2.txt bfs output.txt
python umagatl_CaM.py testStart3.txt testGoal3.txt bfs output.txt


#DFS
echo "DFS Tests"
python umagatl_CaM.py testStart1.txt testGoal1.txt dfs output.txt
python umagatl_CaM.py testStart2.txt testGoal2.txt dfs output.txt
python umagatl_CaM.py testStart3.txt testGoal3.txt dfs output.txt

#IDDFS
echo "IDDFS Tests"
python umagatl_CaM.py testStart1.txt testGoal1.txt iddfs output.txt
python umagatl_CaM.py testStart2.txt testGoal2.txt iddfs output.txt
python umagatl_CaM.py testStart3.txt testGoal3.txt iddfs output.txt

#A*
echo "A* Tests"
python umagatl_CaM.py testStart1.txt testGoal1.txt astar output.txt
python umagatl_CaM.py testStart2.txt testGoal2.txt astar output.txt
python umagatl_CaM.py testStart3.txt testGoal3.txt astar output.txt