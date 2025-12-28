import multiprocessing


def calcs(x):
    i=0
    for i in range(x):
        i = i + i
    return i

if __name__ == '__main__':
    with multiprocessing.Pool() as _pool:
        print(_pool.map(calcs, range(100000)))

