SIZES="32 1023 32767 33554431"

echo "----------------------------------------------------"
echo "Testing standard Blelloch prefix sum implementation."
echo "----------------------------------------------------"
for i in $SIZES
do
    echo "Array size : $i"
    ./prefix $i
    echo
done

echo "----------------------------------------------------"
echo "Testing reversed Blelloch prefix sum implementation."
echo "----------------------------------------------------"
for i in $SIZES
do
    echo "Array size : $i"
    ./prefix $i
    echo
done
