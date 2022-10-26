from collections import defaultdict
from fileinput import filename

word_idx_dict = defaultdict(set)
key = set()
keyset = open('/home/zws/Desktop/test/key_01.txt','w')
inverted = open('/home/zws/Desktop/test/inverted_01.txt','w')
with open('/home/zws/Desktop/test/ans_01',"r") as f:
    for line in f.readlines():
        line = line.split(" ")
        line[-1]=line[-1][:-1]
        idx = line[0]
        for i in range(1, len(line)):
            word_idx_dict[line[i]].add(idx)
            key.add(line[i])

for i in key:
    keyset.write(i+' ')
    inverted.write(i+' ')
    for j in word_idx_dict[i]:
        inverted.write(j+' ')
    inverted.write('\n')
keyset.close()
inverted.close()
print("address all new_txt to final!!!!\n")


   


