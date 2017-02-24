
s = size(p_1_1,1);
x_ax = [1:1:s];
%%


figure(1)

xlabel('N=S')
ylabel('Probability')
grid on
plot(x_ax, p_1_1,'LineWidth',2)
hold on;
plot(x_ax, p_1_2,'LineWidth',2)
plot(x_ax, p_2_1,'LineWidth',2)
plot(x_ax, p_2_2,'LineWidth',2)
plot(x_ax, p_2_S_2,'LineWidth',2)
hold off
grid on
legend('p_1_1','p_1_2','p_2_1','p_2_2','p_2_S_2')

%%
figure(2)
plot(x_ax, t_1_1,'LineWidth',2)
hold on
plot(x_ax, t_1_2,'LineWidth',2)
plot(x_ax, t_2_1,'LineWidth',2)
plot(x_ax, t_2_2,'LineWidth',2)
plot(x_ax, t_2_S_2,'LineWidth',2)
plot(x_ax, t_wasted,'LineWidth',2)
plot(x_ax, total_path,'LineWidth',2)
grid on
xlabel('N=S')
ylabel('Seconds')
hold off;
legend('t_1_1','t_1_2','t_2_1','t_2_2','t_2_S_2','t_{wasted}','t_{total}')

%%

%%
s = size(p_1_1,1);
x_ax = [1:1:s];
figure(3)
hold on
grid on

plot(x_ax, t_1_1.*p_1_1,'LineWidth',2)
plot(x_ax, t_1_2.*p_1_2,'LineWidth',2)
plot(x_ax, t_2_1.*p_2_1,'LineWidth',2)
plot(x_ax, t_2_2.*p_2_2,'LineWidth',2)
plot(x_ax, t_2_S_2.*p_2_S_2,'LineWidth',2)
plot(x_ax, total_path,'LineWidth',2)
xlabel('S')
ylabel('Seconds')
title('times for N=5')
hold off
legend('t_1_1*p_1_1','t_1_2*p_1_2','t_2_1*p_2_1','t_2_2*p_2_2','t_2_S_2*p_2_S_2','t_{total}')
