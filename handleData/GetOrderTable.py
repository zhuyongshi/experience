import re

num=[3137620]   #num数组代表每个文档里的行数，如图所示的左下角红框。
filename =['/home/zws/Desktop/wikiextractor/extra1/AA/wiki_00']
outdir =['/home/zws/Desktop/experience/test/ans_01']
for r in range(1):
    fx = filename[r]
    nx = num[r]
    ox = outdir[r]
    f = open(fx, "r")
    q = open(ox, 'w')
    i = 0
    id = ''
    temp = ''
    s1=''
    while(i < nx):
        s = f.readline()
        if(len(s)==0):
            i+=1
            continue
        if(s[0] == '\n'):
            i = i+1
            continue
        elif(s[0] == '<'):
            if s[1] == 'd':
                j = 9
                # 取文档id
                while(True):
                    if s[j] == '\"':
                        break
                    id = id+s[j]
                    j += 1
            elif s[1] == '/':
                k1 = s1.split()
                k2 = (" ".join(sorted(set(k1), key=k1.index)))
                q.write(id+' '+k2+'\n')
                id = ''
                s1 =''
        else:
            #处理文本的每一行
            temp = re.sub('[\W_]+',' ',s)
            temp1 = temp.split()
            z = 0
            temp2 = []
            while(z < len(temp1)) :
                if temp[z][0].isupper() :
                    t = list (temp1[z])
                    t[0] = t[0].lower()
                    tempz= "".join(t)
                    temp2.append(tempz)
                else:
                    temp2.append(temp1[z])
                z+=1

            s1 += (" ".join(temp2)+" ")
        # print(s)
        # print(s)
        # break
        i = i+1
    q.close()
    f.close()
print("address all new_txt to final!!!!\n")