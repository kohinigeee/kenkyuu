import matplotlib as mp
import matplotlib.pyplot as plt
import sys

def input (path):
    fname = path

    with open(fname, 'r') as file:
        
        #2行目
        line=file.readline()
        parts=line.split()
        s=int(parts[1])
        h=int(parts[3])
        r=int(parts[5])

        i = 40

        results=[]
        while(i <= 90):
            line=file.readline()
            parts=line.split()

            j=int(parts[0])
            haspl=float(parts[1])
            results.append(haspl)
            i += 5

        return results

def func(s, h, r):
    path=f"s{s}_h{h}_r{r}.txt"

    val=h/(s*r)

    results = input(path)
    x = []
    i = 40
    while( i <= 90 ):
        x.append(i/100)
        i+=5

        fig=plt.figure()

    ax=fig.add_subplot(111, xlabel="value of β", ylabel="h-ASPL")
    ax.set_title(f"(s={s}, h={h}, r={r}, p={val:.3f})")
    ax.plot(x, results, marker="o")
    # ax.legend()

    picname = f"{path}.png"
    fig.savefig(picname)
    print( f"sava {picname}")
    plt.close()

func(35, 32, 4)
func(42, 80, 6)
func(90, 432, 12)
func(300, 3800, 30)
func(350, 1204, 10)
func(1100, 1024, 5)
func(350, 1024, 10)