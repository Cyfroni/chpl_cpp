import random

LINES_AMOUNT = 375  # 375

with open('wdbc_reformatted.data', 'r+') as f:
    content = f.read()

    data = content.splitlines()

    lines = {
        'M': [],
        'B': []
    }

    for line in data:
        lines[line[0]].append(line)

    m_length = len(lines['M'])
    b_length = len(lines['B'])

    m_learn = int(LINES_AMOUNT / 5)
    b_learn = int(LINES_AMOUNT - m_learn)

    print(m_learn)
    print(b_learn)

    # with open('../M.data', 'w+') as f_m:
    #     f_m.write('\n'.join(lines['M']))
    #
    # with open('../B.data', 'w+') as f_m:
    #     f_m.write('\n'.join(lines['B']))
    with open('../all.data', 'w+') as __f:
        __f.write('569,31,2\n')
        with open('../train.data', 'w+') as _f:
            _lines = lines['M'][:m_learn] + lines['B'][:b_learn]
            random.shuffle(_lines)
            _f.write(str(len(_lines)) + ',31,2\n' +
                     '\n'.join(_lines))
            __f.write('\n'.join(_lines) + '\n')

        with open('../test.data', 'w+') as _f:
            _lines = lines['M'][m_learn:] + lines['B'][b_learn:]
            random.shuffle(_lines)
            _f.write(str(len(_lines)) + ',31,2\n' +
                     '\n'.join(_lines))
            __f.write('\n'.join(_lines))
