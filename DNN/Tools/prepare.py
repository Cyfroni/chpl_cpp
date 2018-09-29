from os import listdir
from os.path import isfile

path = '../'

onlyfiles = [f for f in listdir(path) if isfile(path + f)]
for file in onlyfiles:
    with open(path + file, 'r+') as f:
        content = f.read()

        data = [line.split(',') for line in content.split('\n')]
        first_row = data[0]
        lines_amount = first_row[0]
        lines_length = first_row[1]

        if lines_amount == str(len(data) - 1) and lines_length == str(len(data[-1])) and len(first_row) == 2:
            continue
        print(file, ': correcting')

        lines_amount = len(data)
        lines_length = len(data[-1])
        f.seek(0, 0)
        f.write(str(lines_amount) + ',' + str(lines_length) + '\n')
        f.write(content)
