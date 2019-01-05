from multiprocessing import Process


def fun(word):
    print('hello', word)


if __name__ == '__main__':
    p = Process(target=fun,
                args=('world',))
    p.start()
    p.join()
