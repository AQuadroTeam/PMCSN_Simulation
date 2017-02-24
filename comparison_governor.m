importdata('~/git/PMCSN_Simulation/governor_comparison_first.data', '-','1')
importdata('~/git/PMCSN_Simulation/governor_comparison_last.data', '-','1')
%non funzionano
%%

plot([1:1:30],t_wasted_first,'c','LineWidth',2)
hold on
plot([1:1:30],t_wasted_last,'m','LineWidth',2)
hold on
plot([1:1:30],total_path_first,'b','LineWidth',2)
hold on
plot([1:1:30],total_path_last,'r','LineWidth',2)
legend('t wasted bad governor','t wasted good governor', 't bad governor', 't good governor')
xlabel('N=S')
ylabel('seconds')
grid on
