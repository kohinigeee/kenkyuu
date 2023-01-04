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


path="h32_r4"
name="hillclimb_32_4.dat"
res = input(path, name)

print(f"st_s={res.st_s}, range={res.range}, h={res.h}, r={res.r}")
print(f"seed={res.seed}, limt={res.limt}, exe={res.exe}, alpha={res.alpha}")

for i in res.results:
    print()
    print(f"avg_diam={i.avg_diam}, avg_haspl={i.avg_haspl}")
    print(f"best= ({i.best[0]}, {i.best[1]})")