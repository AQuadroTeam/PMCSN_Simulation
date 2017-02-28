
s = size(t_wasted_first,1)
plot([1:1:s],t_wasted_first,'c','LineWidth',2)
hold on
plot([1:1:s],t_wasted_last,'m','LineWidth',2)
hold on
plot([1:1:s],total_path_first,'b','LineWidth',2)
hold on
plot([1:1:s],total_path_last,'r','LineWidth',2)
legend('t wasted bad governor','t wasted good governor', 't bad governor', 't good governor')
xlabel('N=S')
ylabel('seconds')
grid on
