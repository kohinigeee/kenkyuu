echo exe main.sh
g++ main.cpp
echo "[execute]" g++ main.cpp
./a.exe
dot -Tpng -o before_swing.png before_swing.dot
dot -Tpng -o after_swing.png after_swing.dot
echo "[execute]" dot -Tpng -o after_swing.png after_swing.dot