'''
A python script that can run our program automatically

Ideally, we should run 10 games for each map and each difficulty[MediumHard - VeryHard].
(Opponent race remains random).

Output format:
-----------------------------------------------
Map: 0
Difficulty: MediumHard
Win: 10
Lose: 0
-----------------------------------------------
Map: 0
Difficulty: Hard
Win: 10
Lose: 0
-----------------------------------------------
'''

import os
from time import time
times = 3
race = ['zerg', 'terran', 'protoss']
diff = ['Hard', 'HardVeryHard', 'VeryHard']


text_file = open("log.txt", "w")
text_file.write("---------------------------------------\n")
for rac in race:
    for d in diff:

        text_file.write("Map: " + "CactusValleyLE" + "\n")
        text_file.write("Difficulty: " + d + "\n")
        text_file.write("Race: " + rac + "\n")
        win = 0
        for t in range(times):
            
            command = "../build/bin/invincibot -c -a " + rac +" -d " + d +  " -m 1"
            print(command)

            rd = os.popen(command)
            s = rd.read()

            print(s)
            if('We' in s):
                win += 1
            print(win)

        text_file.write('win: ' + str(win) + ' \n')
        text_file.write("---------------------------------------\n")
text_file.close()
