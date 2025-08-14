def mul():
    return [lambda x,i=i: i*x for i in range(3)]

print([m(100) for m in mul()])
