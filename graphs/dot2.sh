name=$1
kaisuu=$2

for i in {1..20}
do
    if test $i -gt $kaisuu; then
        break
    fi
    echo $name$i
    fname=$name$i
    dot -Tpng -o $fname.png $fname.dot
done