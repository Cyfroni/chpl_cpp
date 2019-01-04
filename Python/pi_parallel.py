from multiprocessing import Pool
import time

num_steps = 10000000
step = 1.0 / num_steps
threads = 4


def fun(i):
    x = (i + 0.5) * step
    return 4 / (1 + x ** 2)


if __name__ == '__main__':
    start = time.time()

    with Pool(threads) as p:
        results = p.map(fun, range(num_steps))
    pi = sum(results) * step

    end = time.time()
    print('time:\t', end - start)
    print('pi:\t\t', pi)
