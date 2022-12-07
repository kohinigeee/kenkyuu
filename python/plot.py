import matplotlib as mp
import matplotlib.pyplot as plt
import sys

class Results:
    def __init__(self, name):
        self.lists=dict()
        self.lists["avg_haspl"] = []
        self.lists["avg_diam"]=[]
        self.lists["min_haspl"]=[]
        self.lists["min_diam"]=[]
        self.name=name

    def getList( self, key ):
        return self.lists[key]
    
    def getName( self ):
        return self.name 
    
fname = sys.argv[1]
path = "results/"+fname+".txt"


with open(path, 'r') as file:
    print(f"oepn {fname}")

    params=dict()

    #s.h,rの読み込み
    line=file.readline()
    parts=line.split()
    s,h,r=int(parts[1]), int(parts[3]), int(parts[5])

    #paramsの読み込み
    line=file.readline()
    parts=line.split()
    for part in parts:
        tmp=part.split(":") 
        params[tmp[0]]=int(tmp[1])

    #n, rangeの読み込み
    line=file.readline()
    parts=line.split()
    n,srange=int(parts[3]), int(parts[5]) 

    #数値の名前の読み込み
    line=file.readline()
    line=file.readline()
    value_names=line.split()

    names=["random", "init", "mdst"]

    print(value_names)

    results=dict()
    for i in names:
        results[i]=Results(i)

    for i in range(0, srange+1):
        line_s=file.readline()

        for name_idx in range(len(names)):
            name=names[name_idx]
            line = file.readline()
            parts=line.split()

            for vname_idx in range(len(value_names)):
                vname=value_names[vname_idx]
                if ( vname == "min_diam"):
                    value=int(parts[vname_idx])
                else:
                    value=float(parts[vname_idx])
                results[name].getList(vname).append(value)

    print(s,h,r)        
    print(params)
    print(n, srange)
    print(value_names)
    
    pic_dir="pictures"
    x = []
    for i in range (s, s+srange+1):
        x.append(i)

    print(x) 
    for value_name in value_names:
        fig = plt.figure()
        ax = fig.add_subplot(111, xlabel="number of swithecs", ylabel = value_name)

        for method_name in names:
            y = results[method_name].getList(value_name)
            ax.plot(x, y, marker="o", label=method_name)
            ax.legend()
        
        picname=fname+"_"+value_name+".png"
        fig.savefig(pic_dir+"/"+picname)
        print(f"{value_name} : {picname}")
    