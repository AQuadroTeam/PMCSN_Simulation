echo "starting script"
make

N=20

echo "S - t_1_1 - t_1_2 - t_2_1 - t_2_2 - t_2_S_2 - t_wasted - total_path - s_t_1_1 - s_t_1_2 - s_t_2_1 - s_t_2_2 - s_t_2_S_2 - s_wasted - s_total - thr" >  thr_$N.data
echo "Doing N " $N

for ((S=1; S<=$N; S++));
do
  echo "Doing S " $S

  result=$(./simulation $N $S 1000 10136 64 1 0 | grep "Ended simulation" -A 12)
  part_time=$(echo "$result" | grep "Total Mean for path" -A 1 | grep "Total Mean for path" -v)
  thr=$(echo "$result" | grep "Effective throughput" -A 1 | grep "Effective throughput" -v)
  stds=$(echo "$result" | grep "Total Stds for path" -A 1 | grep "Total Stds for path" -v)
  echo $S - $part_time - $stds - $thr >> thr_$N.data

done

echo "ended"
