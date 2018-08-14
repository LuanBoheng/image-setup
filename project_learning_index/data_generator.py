#generate data

import random

size = 10
data = []
number_generator = random.random

for i in range(size):
    data.append ([i,number_generator()])

data.sort()

print(data)


#write into file
import csv
file_name = "data.txt"
with open(file_name, 'wb') as f:
    writer = csv.writer(f)
    for key in range(len(data)):
        writer.writerows(data[key])
