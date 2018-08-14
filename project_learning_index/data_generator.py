#generate data
import sys
import random
import numpy

size = sys.argv[1]
size = int(size)
out_file = sys.argv[2]
data = []
#number_generator = random.random
number_generator = numpy.random.normal
for key in range(size):
    data.append (number_generator())
data.sort()
#print(data)
#write into file
f = open(out_file, "w")
sys.stdout = f
for key in range(size):
    print(data[key], sep=',')
