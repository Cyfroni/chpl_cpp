num_steps = 10 ** 9
step = 1.0 / num_steps

pi_sum = 0.0
for i in range(num_steps):
    x = (i + 0.5) * step
    pi_sum += 4 / (1 + x ** 2)
pi = pi_sum * step

print(pi)
