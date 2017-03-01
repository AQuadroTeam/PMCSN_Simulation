echo "starting script"
make

for N in 5 10 15 20 30 40;
do
  echo "N_good - S_good - p_1_1_good - p_1_2_good - p_2_1_good - p_2_2_good - p_2_S_2_good" >  p_simulated_good_gv_n_$N.data
  echo "N_bad - S_bad - p_1_1_bad - p_1_2_bad - p_2_1_bad - p_2_2_bad - p_2_S_2_bad" >  p_simulated_bad_gv_n_$N.data
  echo "Doing N " $N

  for ((S=1; S<=$N; S++));
  do
    echo "Doing S " $S

    result=$(./simulation $N $S 1000 10136 64 1 0 | grep "Ended simulation" -A 8)
    part_prob=$(echo "$result" | grep "Total P" -A 1 | grep "Total P" -v)
    echo $N - $S - $part_time - $part_prob >> p_simulated_good_gv_n_$N.data

    result=$(./simulation $N $S 1000 10136 64 0 0 | grep "Ended simulation" -A 8)
    part_prob=$(echo "$result" | grep "Total P" -A 1 | grep "Total P" -v)
    echo $N - $S - $part_time - $part_prob >> p_simulated_bad_gv_n_$N.data
  done
done
echo "ended"
