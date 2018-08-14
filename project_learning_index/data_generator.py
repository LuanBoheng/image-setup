#generate data
import sys
import random
import numpy
from tqdm import tqdm

size = sys.argv[1]
size = int(size)
out_file = sys.argv[2]
data = []
#number_generator = random.random
#for key in tqdm(range(size)):
#   data.append (number_generator())
number_generator = numpy.random.normal
data = number_generator(0,0.1,size)
data.sort()
#print(data)
#write into file
f = open(out_file, "w")
sys.stdout = f
for key in tqdm(range(size)):
    print(data[key], sep=',')
