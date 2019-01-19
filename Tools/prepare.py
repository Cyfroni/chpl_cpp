from os import listdir
from os.path import isfile
import sys


path = '../'

onlyfiles = [f for f in listdir(path) if isfile(path + f) and f.endswith('.data')]
for file in onlyfiles:
    with open(path + file, 'r+') as f:
        content = f.read()

        data = [line.split(',') for line in content.splitlines()]
        first_row = data[0]
		
        lines = first_row[0].split(';')
        #print(lines_to_ignore)
        if len(lines) == 1:
            continue
        print(file + ': correcting')
        print('lines: ' + str(lines))
	
        category_column = int(lines[-1])
        del lines[-1]
        lines_to_ignore = [int(elem) for elem in lines if elem != '']
        lines_to_ignore.sort()
        lines_to_ignore.reverse()
        del data[0]
	
        categories = {line[category_column] for line in data}
        for col_number in lines_to_ignore:
            for row in data:
                del row[col_number]
            #print(data)
		
        lines_amount = len(data)
        lines_length = len(data[-1])
        f.seek(0, 0)
        f.truncate(0)
        f.write(str(lines_amount) + ',' + str(lines_length) + ',' + str(len(categories)) + '\n')
        data = [','.join(line) for line in data]
        f.write('\n'.join(data))
