
s = size(thr,1);
x_ax = [1:1:s];
%%


figure(1)

xlabel('S')
ylabel('Throughput')
grid on
plot(x_ax, thr,'LineWidth',2)
hold off
grid on
legend('thr')

%%
figure(2)
hold on
%plot(x_ax, t_1_2,'LineWidth',2)
%plot(x_ax, t_2_1,'LineWidth',2)
%plot(x_ax, t_2_2,'LineWidth',2)
%plot(x_ax, t_2_S_2,'LineWidth',2)
plot(x_ax, t_wasted,'LineWidth',2)
plot(x_ax, total_path,'LineWidth',2)
plot(x_ax, thr,'LineWidth',2)
%axis([0 25 0 10])
grid on
xlabel('S')
yyaxis left
ylabel('sec')
yyaxis right
ylabel('op/sec')
hold off;
legend('t_{wasted}','t_{total}','throughput')
%legend('t_1_1','t_1_2','t_2_1','t_2_2','t_2_S_2','t_{wasted}','t_{total}','throughput')
%legend('throughput')

%%

%%

figure(3)
hold on
grid on

plot(x_ax, t_1_1.*p_1_1,'LineWidth',2)
plot(x_ax, t_1_2.*p_1_2,'LineWidth',2)
plot(x_ax, t_2_1.*p_2_1,'LineWidth',2)
plot(x_ax, t_2_2.*p_2_2,'LineWidth',2)
plot(x_ax, t_2_S_2.*p_2_S_2,'LineWidth',2)
plot(x_ax, total_path,'LineWidth',2)

xlabel('N=S')
ylabel('Seconds')
hold off
legend('t_1_1*p_1_1','t_1_2*p_1_2','t_2_1*p_2_1','t_2_2*p_2_2','t_2_S_2*p_2_S_2','t_{total}')
