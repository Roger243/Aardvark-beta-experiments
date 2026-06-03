# winAgent AI for Windows 11

winAgent AI is a local, offline Windows 11 C++17 console agent that trains and runs a logistic-regression model. It replaces the kernel-driver scaffold because AI inference and training should run in user mode, where memory, files, and model state can be handled safely.

No kernel driver, privilege escalation, remote-control channel, stealth persistence, or process-kill logic is included. The agent reads local CSV files, trains a model, saves weights, writes predictions, and can generate the logistic cost curve from the provided formula.

## Features

- Batch gradient logistic regression with `eta`, `n_iter`, and `random_state`.
- Sigmoid activation with clipping for numerical stability.
- Logistic costs for `y=1` and `y=0` for plotting `J(w)` against `phi(z)`.
- Local model save/load with plain-text weights.
- `--agent-once` mode for a simple autonomous local cycle: train, save, predict, and write results.
- Visual Studio 2022 project tuned for Windows 11 x64 and a portable CMake build for CI checks.

## Commands

```cmd
winagent-ai.exe --cost-curve costs.csv
winagent-ai.exe --train train.csv --model model.txt --eta 0.05 --n-iter 100 --header
winagent-ai.exe --predict features.csv --model model.txt --output predictions.csv --header
winagent-ai.exe --agent-once train.csv --predict features.csv --model model.txt --output predictions.csv --header
```

Training CSV files use the last column as the binary label by default. Feature CSV files contain numeric features only. Prediction output contains `label,probability`.

## Build on Windows 11

Visual Studio 2022:

```cmd
msbuild winagent-ai.vcxproj /p:Configuration=Release /p:Platform=x64
```

CMake:

```cmd
cmake -S . -B build-msvc -G "Visual Studio 17 2022" -A x64
cmake --build build-msvc --config Release
```

## Example data

`train.csv`:

```csv
x1,x2,label
0,0,0
0,1,0
1,0,0
1,1,1
```

`features.csv`:

```csv
x1,x2
0,0
1,1
```
