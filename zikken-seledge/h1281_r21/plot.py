import matplotlib as mp
import matplotlib.pyplot as plt
import sys

class History:
    def __init__(self, s, h, r, counts, values):
        self.s = s
        self.h = h
        self.r = r
        self.counts = counts
        self.values =values

def convert(his, xvalues):
    ans=[]
    for i in xvalues:
        ans.append(his.values[i][1])
    return ans

def convert2(his, xvalues):
    ans=[]
    for i in xvalues:

        tmp = his.values[i][0]
        tmp2=his.values[i][1]
        ans.append(tmp+tmp2/10)
    return ans

def inputf( path, name ):
    fname = path+"/"+name

    with open(fname, 'r') as file:
        
        #1行目
        file.readline()

        #2行目
        line=file.readline()
        parts=line.split()
        s=int(parts[1])
        h=int(parts[3])
        r=int(parts[5])

        #3行目
        line=file.readline()
        parts=line.split()
        counts=int(parts[1])

        results=[]
        for i in range(counts):
            line = file.readline()
            parts=line.split()
            diam=int(parts[0])
            haspl=float(parts[1])

            results.append((diam, haspl))

        return History(s, h, r, counts, results)

def func (s, h, r):

    name1="directed1.dat"
    name2="directed2.dat"
    name3="hillclimb.dat"

    path=f"s{s}_h{h}_r{r}"

    hill_his=inputf(path, name3)
    dir1_his=inputf(path, name1)
    dir2_his=inputf(path, name2)

    mincount = min([hill_his.counts, dir1_his.counts, dir2_his.counts])

    xvalues=[]
    # for i in range(0, 99):
    #     xvalues.append(int(mincount*i/100))
    # xvalues.append(mincount-1)

    # for i in range(0, int(mincount*0.15)):
    #     xvalues.append(i)

    xvalues=[ i for i in range(mincount)]

    fig = plt.figure()
    ax=fig.add_subplot(111, xlabel= "number of serches", ylabel="h-ASPL")

    # tmp = convert(hill_his, xvalues)
    # print(tmp)
    title=f"(s={s}, h={h}, r={r})"
    ax.set_title(title)
    ax.plot(xvalues, convert(hill_his, xvalues), marker="o", label="hillclimbing", markersize=3)
    ax.legend()
    ax.plot(xvalues, convert(dir1_his, xvalues), marker="x", label="directed1", markersize=3)
    ax.legend()
    ax.plot(xvalues, convert(dir2_his, xvalues), marker="o", label="directed2", markersize=3)
    ax.legend()

    picname=path+".png"
    fig.savefig(picname)
    print("saved " + picname)

s=100
h=1281
r=21
unit=5

for i in range(0, 6):
    tmps = i*unit+s
    func(tmps, h, r)