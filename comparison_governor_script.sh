echo "starting script from N=1 to 30" > governor_comparison.data
make >> governor_comparison.data
for N in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30;
do
  echo " First" >>  governor_comparison.data
  ./simulation $N $N 1000 10136 10 0 0 | grep "Ended simulation" -A 7 >>  governor_comparison.data
  echo " Last" >>  governor_comparison.data
  ./simulation $N $N 1000 10136 10 1 0 | grep "Ended simulation" -A 7 >> governor_comparison.data
done
echo "ended" >> governor_comparison.data
