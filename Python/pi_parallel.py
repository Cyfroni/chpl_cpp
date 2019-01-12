from multiprocessing import Pool

num_steps = 10 ** 7
step = 1.0 / num_steps
threads = 4


def fun(part):
    pi_sum = 0.0
    for i in range(part, num_steps, threads):
        x = (i + 0.5) * step
        pi_sum += 4 / (1 + x ** 2)
    return pi_sum


if __name__ == '__main__':
    with Pool(threads) as p:
        results = p.map(fun, range(threads))
    pi = sum(results) * step

    print(pi)
