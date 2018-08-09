import random

size = 10
data = []
number_generator = random.random

for i in range(10):
    data.append (number_generator())

data.sort()

print(data)

