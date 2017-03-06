#!/usr/bin/env python3
##########################################
## Kevin Turkington
## 3/5/17
## CS344
## py assignment
##########################################
import string
import random

# FileNames 
fileNames = [
    'G-Eazy',
    'TheWeekend',
    'Logic'
]

# NAME: GenRandom10Letters
# DESC: generates and returns a random lowercase string.
def GenRandom10Letters():
    RandoString = random.choice(string.ascii_lowercase)
    for i in range(10):
        RandoString += (random.choice(string.ascii_lowercase)) #gen random char and append.
    return RandoString

#####Begin Program#####
for i in range(3):#write file
    WritePtr = open(fileNames[i],'w+')
    # for additional newline replace with the following
    # WritePtr.write(GenRandom10Letters() + '\n')
    WritePtr.write(GenRandom10Letters())
    WritePtr.close()

for i in range(3):#read file
    ReadPtr = open(fileNames[i],'r+')
    print ReadPtr.read() #read all contents of file.
    ReadPtr.close()

#Gen random ints
x = random.randint(1,42)
y = random.randint(1,42)

#print and arithmatic
print(x)
print(y)
print(x * y)

#####End Program#######





