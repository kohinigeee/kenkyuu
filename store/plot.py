import matplotlib as mp
import matplotlib.pyplot as plt
import sys

class Result_item:
    def __init__(self, avg_diam, avg_haspl, best):
        self.avg_diam = avg_diam
        self.avg_haspl = avg_haspl
        self.best = best

class Result:
    def __init__(self, st_s, rangev, h, r, seed, limt, exe, alpha, results):
        self.st_s = st_s
        self.range = rangev
        self.h = h
        self.r = r
        self.seed = seed
        self.limt = limt
        self.exe = exe
        self.alpha = alpha
        self.results = results
    


def input ( path, name ):
    fname = path+"/"+name

    with open(fname, 'r') as file:
        
        #1行目
        file.readline()

        #2行目
        line=file.readline()
        parts=line.split()
        st_s=int(parts[1])
        rangev=int(parts[3])
        h=int(parts[5])
        r=int(parts[7])

        #3行目
        line=file.readline()
        parts=line.split()
        seed=int(parts[1])
        limt=int(parts[3])
        exe=int(parts[5])
        alpha=float(parts[7])

        results=[]
        for i in range(rangev+1):
            file.readline() #空行
            file.readline() #[Result]

            line = file.readline()
            parts=line.split()
            avg_diam=float(parts[1])
            avg_haspl=float(parts[3])

            line=file.readline()
            parts=line.split()
            best=(int(parts[1]), float(parts[2]))

            results.append(Result_item(avg_diam, avg_haspl, best))

        return Result(st_s, rangev, h, r, seed, limt, exe, alpha, results)


h=80
r=6

# path=f"h{h}_r{r}"
# path="h32_r4_2"
path="h80_r6_2"

name_hill=f"hillclimb_{h}_{r}.dat"
res_hill = input(path, name_hill)

name_direct=f"directHillclimb_{h}_{r}.dat"
res_dir=input(path, name_direct)

# plt.legend(fontsize=16)
fig=plt.figure()
ax=fig.add_subplot(111, xlabel="number of swithes", ylabel="h-ASPL")

svalues=[ (i+res_hill.st_s) for i in range(res_hill.range+1)]

hill_values=[ (i.best[1]) for i in res_hill.results]
dir_values=[ (i.best[1]) for i in res_dir.results]

ax.plot(svalues, hill_values, marker="o", label="simple method")
ax.legend()
ax.plot(svalues, dir_values, marker="x", label="directed method")
ax.legend(fontsize=14)
ax.legend()

picname=path+".png"
fig.savefig(picname)
print("picture saved")