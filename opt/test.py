import optuna
import subprocess

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
    x = trial.suggest_uniform("x", -10, 10)

    cmd = "./a.exe " + str(x)
    lis = remove_s(subprocess.check_output(cmd.split()))
    
    score = float(lis[0])
    return score

study = optuna.create_study()
study.optimize(objective, n_trials=100)

print(study.best_params)
print(study.best_value)