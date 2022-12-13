import optuna
import subprocess
import random

def remove_s( s ):
    tmp = s.decode()
    tmp = tmp.replace('\r', '')
    tmp = tmp.split()

    ans = []
    for i in tmp:
        if ( i != ""):
            ans.append(i)
    return ans

def objective(trial):
    seed = random.randint(1, 100000)
    N = trial.suggest_int("N", 100, 10000)
    minT = trial.suggest_int("minT", 1, 8000)

    minT_v = int(minT)
    maxT = trial.suggest_int("maxT", minT_v, 10000)

    # s = trial.suggest_int("s", 25, 50)

    fname = "optuna_annealing.exe"
    cmd = "./"+fname 
    cmd += " N " + str(N) 
    cmd += " minT " + str(minT)
    cmd += " maxT " + str(maxT)

    cmd += " s " + str(s)
    cmd += " h " + str(h)
    cmd += " r " + str(r)
    cmd += " seed " + str(seed)

    lis = remove_s(subprocess.check_output(cmd.split()))
    score = float(lis[0])
    return score

s = 31 
h = 32
r = 4
seed_i = 60

random.seed(seed_i)

study = optuna.create_study()
study.optimize(objective, n_trials=10000)


print(study.best_params)
print(f"s={s}, h={h}, r={r}, seed={seed_i}")
print(study.best_value)
