#!/bin/py

f = open("x")#read
f_4 = open("x_4", "w")#write
f__4 = open("x__4", "w")#write

for line in f.readlines():
	lines = line.strip().split("\t")
	uid = int((lines[0].strip().split(":"))[1])
	#print uid
	last = (uid % 100) / 10
	#print str(last) + "\t" + lines[1]
	if last == 4:
		f_4.write(str(last) + "\t" + lines[1] + "\n")
	else:
		f__4.write(str(last) + "\t" + lines[1] + "\n")
	

f.close()
f_4.close()
f__4.close()
