function [p_1_1, p_1_2, p_2_1, p_2_2, p_2_S_2] = markov_analysis(N,S)

lambda_1 = 3.25;
lambda_2 = 6.25;
mu_1_cloudlet = 0.45;
mu_1_cloud = 0.25;
mu_2_cloudlet = 0.30;
mu_2_cloud = 0.22;
mu_setup = 1/0.8;
params = [lambda_1,lambda_2,mu_1_cloudlet,mu_2_cloudlet];



number_states = (N+1) * (S+1);
Flux = zeros(number_states, number_states);
P = zeros(number_states, number_states);


addpath('~/git/PMCSN_Simulation/');

% fill p
for c1 = 0:N
    for c2 = 0:S
        [f,up,down,right,left,busy1,busy2] = exit_flux(c1, c2, S, N,params);
        if f > 0
            for target = 1:number_states
                %down
                if target == index_of_state(c1+1, c2,S) && down == 1
                    Flux(index_of_state(c1, c2,S), target) = lambda_1;
                    P(index_of_state(c1, c2,S), target) = lambda_1/f;
                end
                %up
                if target == index_of_state(c1-1, c2,S) && up == 1
                    Flux(index_of_state(c1, c2,S), target) =c1*mu_1_cloudlet;
                    P(index_of_state(c1, c2,S), target) = c1*mu_1_cloudlet/f;
                end
                %right
                if target == index_of_state(c1, c2+1,S) && right == 1
                    
                    Flux(index_of_state(c1, c2,S), target) =lambda_2;
                    P(index_of_state(c1, c2,S), target) = lambda_2/f;
                end
                %left
                if target == index_of_state(c1, c2-1,S) && left == 1
                    Flux(index_of_state(c1, c2,S), target) =c2*mu_2_cloudlet;
                    P(index_of_state(c1, c2,S), target) = c2*mu_2_cloudlet/f;
                end

                %same
                if target == index_of_state(c1, c2,S) && busy2 == 1
                    if busy1 == 1
                        Flux(index_of_state(c1, c2,S), target) = lambda_2+lambda_1;
                        P(index_of_state(c1, c2,S), target) = (lambda_2+lambda_1)/f;
                    else
                        Flux(index_of_state(c1, c2,S), target) = lambda_2;
                        P(index_of_state(c1, c2,S), target) = lambda_2/f;
                    end
                end

                %preemptive
                if target == index_of_state(c1+1, c2-1,S) && busy2 == 1 && (c1+1<=S && c2-1>=0)
                    
                    Flux(index_of_state(c1, c2,S), target) = lambda_1;
                    P(index_of_state(c1, c2,S), target) = (lambda_1)/f;
                end
            end
            if f ~= sum(Flux(index_of_state(c1, c2,S),:))
                disp('Error flux')
                return
            end
        else
            % solo per somma 1
            %P(index_of_state(c1, c2,S), 2) = 1;
        end
    end
end
%disp(Flux)
%disp(P)

% just a check...
for row = 1:size(P,1)
    if (sum(P(row,:)) ~= 1)
        disp('Errore nella matrice se il numero sotto è diverso da 1 o 0')
        disp(row)
        disp(sum(P(row,:)))
    end
end


%% calculate in PI the stationary probs

[V , D ] = eig(P');
 
[foo , tp] = sort(diag(D));
 
PI = (V(: , tp(end))/sum(V(: , tp(end))))';
disp('stationary probs sum=')
disp(sum(PI))

%%
flux_2_immediately_discarded = 0;
flux_2_preemptive = 0;
flux_2_completed = 0;
for c1 = 0:N
    for c2 = 0:S
        if (c1+c2>=S)
            flux_2_immediately_discarded = PI(index_of_state(c1,c2,S)) + flux_2_immediately_discarded;
            if(c2>0)
                flux_2_preemptive = flux_2_preemptive + PI(index_of_state(c1,c2,S)) ;
            end
        end
        if (c2>0)
            flux_2_completed = flux_2_completed + PI(index_of_state(c1,c2,S))*c2;
        end
    end
end

D = flux_2_immediately_discarded;
A = PI(index_of_state(N,0,S));
B = 1 - A;
E = flux_2_preemptive*lambda_1/(lambda_2);
C = flux_2_completed*mu_2_cloudlet;

p_1_1 = B * lambda_1/(lambda_1+lambda_2);
p_1_2 = A * lambda_1/(lambda_1+lambda_2);
p_2_2 = D*lambda_2/(lambda_1+lambda_2);
p_2_S_2 = E*lambda_2/(lambda_1+lambda_2);
p_2_1 = 1 - p_2_2 - p_2_S_2 - p_1_1 - p_1_2;

%
disp('P calculated')
disp('p_1_1')
disp(p_1_1)
disp('p_1_2')
disp(p_1_2)
disp('p_2_1')
disp(p_2_1)
disp('p_2_2')
disp(p_2_2)
disp('p_2_S_2')
disp(p_2_S_2)

end