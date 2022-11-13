echo "start main.sh"

g++ test-back.cpp
./a.exe

dot -Tpng -o graph1.png graph_1.dot
dot -Tpng -o graph2.png graph_2.dot
dot -Tpng -o graph3.png graph_3.dot

echo -e "\ndone main.sh"