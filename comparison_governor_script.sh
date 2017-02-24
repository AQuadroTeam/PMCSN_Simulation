echo "starting script from N=1 to 50"
make
echo "t_1_1_first - t_1_2_first - t_2_1_first - t_2_2_first - t_2_S_2_first - t_wasted_first - total_path_first" >  governor_comparison_first.data
echo "t_1_1_last - t_1_2_last - t_2_1_last - t_2_2_last - t_2_S_2_last - t_wasted_last - total_path_last" >  governor_comparison_last.data
for N in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50;
do
  echo "$N of 100"
    ./simulation $N $N 1000 10136 64 0 0 | grep "Ended simulation" -A 4 | grep "Total Mean for path" -A 2 | grep "Total Mean for path" -v >>  governor_comparison_first.data

done
for N in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50;
do
  echo "$N of 100"

  ./simulation $N $N 1000 10136 64 1 0 | grep "Ended simulation" -A 4 | grep "Total Mean for path" -A 2 | grep "Total Mean for path" -v >>  governor_comparison_last.data

done

echo "ended"
