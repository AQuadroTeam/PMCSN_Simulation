addpath('~/git/PMCSN_Simulation');

p_1_1_markov = [];
p_1_2_markov = [];
p_2_1_markov = [];
p_2_2_markov = [];
p_2_S_2_markov = [];

N_test = N_bad(1)
for S_now = 1:N_test
    [p_1_1_now, p_1_2_now, p_2_1_now, p_2_2_now, p_2_S_2_now] = markov_analysis(N_test,S_now);
    p_1_1_markov = [p_1_1_markov p_1_1_now];
    p_1_2_markov = [p_1_2_markov p_1_2_now];
    p_2_1_markov = [p_2_1_markov p_2_1_now];
    p_2_2_markov = [p_2_2_markov p_2_2_now];
    p_2_S_2_markov = [p_2_S_2_markov p_2_S_2_now];
end

%%

plot(S_bad, p_1_1_bad)
axis([1,S_bad(1)+1,0,0.5])
hold on
plot(S_bad, p_1_1_good)
plot(S_bad, p_1_1_markov')
legend('bad','good','markov')
title('p path 1 1, N = 10')
xlabel('S')

%%
hold off
plot(S_bad, p_1_2_bad)
axis([1,S_bad(1)+1,0,0.000000000000002])
hold on
plot(S_bad, p_1_2_good)
plot(S_bad, p_1_2_markov')
legend('bad','good','markov')
title('p path 1 2, N = 10')
%%
plot(S_bad, p_2_S_2_bad)

hold on
plot(S_bad, p_2_S_2_good)
plot(S_bad, p_2_S_2_markov')
legend('bad','good','markov')
title('p path 2 S 2,N=10')
%%

plot(S_bad, p_2_2_bad)
hold on
plot(S_bad, p_2_2_good)
plot(S_bad, p_2_2_markov')
legend('bad','good','markov')
title('p path 2 2,N=10')
%%
plot(S_bad, p_2_1_bad)
hold on
plot(S_bad, p_2_1_good)
plot(S_bad, p_2_1_markov')
legend('bad','good','markov')
title('p path 2 1,N=10')