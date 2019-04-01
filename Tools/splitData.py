import random

with open('wdbc_reformatted.data', 'r+') as f:
    content = f.read()

    data = content.splitlines()

    lines = {
        'M': [],
        'B': []
    }

    for line in data:
        lines[line[0]].append(line)

    print(len(lines['M']))
    print(len(lines['B']))

    # with open('../M.data', 'w+') as f_m:
    #     f_m.write('\n'.join(lines['M']))
    #
    # with open('../B.data', 'w+') as f_m:
    #     f_m.write('\n'.join(lines['B']))

    with open('../train.data', 'w+') as _f:
        _lines = lines['M'][:75] + lines['B'][:300]
        random.shuffle(_lines)
        _f.write(str(len(_lines)) + ',31,2\n' +
                 '\n'.join(_lines))

    with open('../test.data', 'w+') as _f:
        _lines = lines['M'][75:] + lines['B'][300:]
        random.shuffle(_lines)
        _f.write(str(len(_lines)) + ',31,2\n' +
                 '\n'.join(_lines))
