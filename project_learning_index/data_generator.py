#generate data

import random

size = 1000000
data = []
number_generator = random.random

for key in range(size):
    data.append (number_generator())

data.sort()

print(data)


#write into file
import sys

f = open("data.csv","w")
sys.stdout = f

for key in range(size):
    print(key, data[key], sep=',')
