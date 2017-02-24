echo "starting script from N=1 to 50"
make
echo "t_1_1 - t_1_2 - t_2_1 - t_2_2 - t_2_S_2 - t_wasted - total_path - p_1_1 - p_1_2 - p_2_1 - p_2_2 - p_2_S_2" >  response_time_per_s.data


for N in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50;
do
  echo "$N of 50"

  result=$(./simulation $N $N 1000 10136 64 1 0 | grep "Ended simulation" -A 6)
  part_time=$(echo "$result" | grep "Total Mean for path" -A 1 | grep "Total Mean for path" -v)
  part_prob=$(echo "$result" | grep "Total P" -A 2 | grep "Total P" -v)
  echo $part_time - $part_prob >> response_time_per_s.data

done

echo "ended"
