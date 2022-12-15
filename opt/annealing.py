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

    params_trial = dict()
    for key, value in params.items():
        if ( type(value) != type(()) ):
            params_trial[key] = value 
        else:
            params_trial[key] = trial.suggest_int(key, int(value[0]), int(value[1]))

    minT_v = int(params_trial["minT"])
    params_trial["maxT"] = trial.suggest_int("maxT", minT_v+100, int(params_trial["maxT"]))

    fname = "optuna_annealing.exe"
    cmd = "./"+fname 

    for key, value in params_trial.items():
        cmd += f" {key} " + str(value) 

    cmd += " seed " + str(seed)

    lis = remove_s(subprocess.check_output(cmd.split()))
    score = float(lis[0])
    return score

cname = "optuna_annealing"
cmd = f"g++ {cname}.cpp -o {cname}.exe"

subprocess.call(cmd.split())

param_names = ["s", "h", "r", "N", "minT", "maxT"]
params = dict()

params["s"] = 32
params["h"] = 32
params["r"] = 4
params["N"] = 8000

#固定パラメータではないものはタプルで範囲を指定
#(maxTだけは例外、上限値のみを指定)
minT_max = 8000
params["minT"] = (1, minT_max)
params["maxT"] = minT_max+2000

seed_i = 1
random.seed(seed_i)

study = optuna.create_study()
study.optimize(objective, n_trials=5000)


print(study.best_params)
print("[Fixed paramaters]")
for key, value in params.items():
    if ( key == "maxT" ):
        continue
    if ( type(value) != type(()) ):
        print(f"{key}={value}, ", end="")
print(f"seed={seed_i}")
print(study.best_value)
