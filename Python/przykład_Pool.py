from multiprocessing import Pool


def fun(word):
    print('hello', word)


if __name__ == '__main__':
    args = [1, 2, 3, 4, 5, 6, 7, 8]
    with Pool(4) as p:
        p.map(fun, args)


