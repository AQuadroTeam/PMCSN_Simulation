echo "starting script from N=1 to 30"
make
echo "governor-t_1_1-t_1_2-t_2_1-t_2_2-t_2_S_2-t_wasted" >  governor_comparison.data
for N in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30;
do
  echo "$N of 30"
  echo -n " 0 - " >>  governor_comparison.data
  ./simulation $N $N 1000 10136 10 0 0 | grep "Ended simulation" -A 4 | grep "Total Mean for path" -A 2 | grep "Total Mean for path" -v >>  governor_comparison.data

  echo -n " 1 - " >>  governor_comparison.data
  ./simulation $N $N 1000 10136 10 1 0 | grep "Ended simulation" -A 4 | grep "Total Mean for path" -A 2 | grep "Total Mean for path" -v >>  governor_comparison.data

done
echo "ended"
