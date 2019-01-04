import time

num_steps = 10000000
step = 1.0 / num_steps

start = time.time()

pi_sum = 0.0
for i in range(num_steps):
    x = (i + 0.5) * step
    pi_sum += 4 / (1 + x ** 2)
pi = pi_sum * step

end = time.time()
print('time:\t', end - start)
print('pi:\t\t', pi)
