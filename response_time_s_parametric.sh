echo "starting script"
make

for N in 5 10 15 20 30 40;
do
  echo "t_1_1 - t_1_2 - t_2_1 - t_2_2 - t_2_S_2 - t_wasted - total_path - p_1_1 - p_1_2 - p_2_1 - p_2_2 - p_2_S_2" >  response_time_n_$N.data
  echo "Doing N " $N

  for ((S=1; S<=$N; S++));
  do
    echo "Doing S " $S

    result=$(./simulation $N $S 1000 10136 64 1 0 | grep "Ended simulation" -A 8)
    part_time=$(echo "$result" | grep "Total Mean for path" -A 1 | grep "Total Mean for path" -v)
    part_prob=$(echo "$result" | grep "Total P" -A 2 | grep "Total P" -v)
    echo $part_time - $part_prob >> response_time_n_$N.data
  done
done
echo "ended"
