#!/bin/python3
import random





dataNum = 10000
baseNum = 1000


for i in range(1,dataNum):
    res = random.randint(1,10000)
    print(res - (res % 77))

