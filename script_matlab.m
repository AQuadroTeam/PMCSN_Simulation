exp = fitdist(response_time, 'exponential')
[a,b,c]=chi2gof(response_time, 'CDF',exp)
%% count number of jobs
batch_number = 64;
counter_jobs_per_batch = zeros(batch_number,1)
counter_jobs_per_batch_path = zeros(batch_number, 5)
data_cell_array = simN20S20batchtimetotal1000
for index = 1:numel(data_cell_array)-1
    
    element = data_cell_array(index,:,:);
    
    counter_jobs_per_batch(cell2mat(element(1))+1)=1 + counter_jobs_per_batch(cell2mat(element(1))+1);
    counter_jobs_per_batch_path(cell2mat(element(1))+1,cell2mat(element(2))+1) = 1 + counter_jobs_per_batch_path(cell2mat(element(1))+1,cell2mat(element(2))+1);
    
end
    
disp(counter_jobs_per_batch)
disp(counter_jobs_per_batch_path)

%% filter for batch, build new arrays -- old
response_per_path = [];



for index = 1:numel(data_cell_array)
    
    element = data_cell_array(index,:,:);
    
    if cell2mat(element(1)) == 0
        response_per_batch_1 = [response_per_batch_1;   cell2mat(element(3))];
    end
end

%% filter for batch, path, build new arrays
batch_to_consider = 2
response_1_1 = zeros(counter_jobs_per_batch_path(batch_to_consider,1),1);
response_1_2 = zeros(counter_jobs_per_batch_path(batch_to_consider,2),1);
response_2_1 = zeros(counter_jobs_per_batch_path(batch_to_consider,3),1);
response_2_2 = zeros(counter_jobs_per_batch_path(batch_to_consider,4),1);
response_2_S_2 = zeros(counter_jobs_per_batch_path(batch_to_consider,5),1);
response_total = zeros(counter_jobs_per_batch(batch_to_consider),1);


index_to_add_1_1 = 1;
index_to_add_1_2 = 1;
index_to_add_2_1 = 1;
index_to_add_2_2 = 1;
index_to_add_2_S_2 = 1;
index_to_add_total = 1;
for index = 1:numel(data_cell_array)
    
    element = data_cell_array(index,:,:);
    
    % 1_1
    if cell2mat(element(1)) == batch_to_consider-1 && cell2mat(element(2)) == 0
        response_1_1(index_to_add_1_1) = cell2mat(element(3));
        index_to_add_1_1 = index_to_add_1_1 + 1;
    end
    
    % 1_2
    if cell2mat(element(1)) == batch_to_consider-1 && cell2mat(element(2)) == 1
        response_1_2(index_to_add_1_2) = cell2mat(element(3));
        index_to_add_1_2 = index_to_add_1_2 + 1;
    end
    
    % 2_1
    if cell2mat(element(1)) == batch_to_consider-1 && cell2mat(element(2)) == 2
        response_2_1(index_to_add_2_1) = cell2mat(element(3));
        index_to_add_2_1 = index_to_add_2_1 + 1;
    end
    
    % 2_2
    if cell2mat(element(1)) == batch_to_consider-1 && cell2mat(element(2)) == 3
        response_2_2(index_to_add_2_2) = cell2mat(element(3));
        index_to_add_2_2 = index_to_add_2_2 + 1;
    end
    
    % 2_S_2
    if cell2mat(element(1)) == batch_to_consider-1 && cell2mat(element(2)) == 4
        response_2_S_2(index_to_add_2_S_2) = cell2mat(element(3));
        index_to_add_2_S_2 = index_to_add_2_S_2 + 1;
    end
    
    % total
    if cell2mat(element(1)) == batch_to_consider-1
        response_total(index_to_add_total) = cell2mat(element(3));
        index_to_add_total = index_to_add_total + 1;
    end
    
end

%% check if for is correct
disp('Start Checking')
if ~(index_to_add_1_1 -1 == counter_jobs_per_batch_path(batch_to_consider, 1))
    disp('Not Correct 1 1')
end

if ~(index_to_add_1_2 -1 == counter_jobs_per_batch_path(batch_to_consider, 2))
    disp('Not Correct 1 2')
end

if ~(index_to_add_2_1 -1 == counter_jobs_per_batch_path(batch_to_consider, 3))
    disp('Not Correct 2 1')
end

if ~(index_to_add_2_2 -1 == counter_jobs_per_batch_path(batch_to_consider, 4))
    disp('Not Correct 2 2')
end

if ~(index_to_add_2_S_2 -1 == counter_jobs_per_batch_path(batch_to_consider, 5))
    disp('Not Correct 2 S 2')
end
disp('End of Check')


%% check if response_1_1 is a good exp

if counter_jobs_per_batch_path(batch_to_consider, 1) > 0
    exp_1_1 = fitdist(response_1_1,'exponential')
end
if counter_jobs_per_batch_path(batch_to_consider, 2) > 0
    exp_1_2 = fitdist(response_1_2,'exponential') % if 0 creates problems
end
if counter_jobs_per_batch_path(batch_to_consider, 3) > 0
    exp_2_1 = fitdist(response_2_1,'Gamma')
    %histfit(response_2_1, 100, 'Gamma')
end
if counter_jobs_per_batch_path(batch_to_consider, 4) > 0
    exp_2_2 = fitdist(response_2_2,'exponential')
end
if counter_jobs_per_batch_path(batch_to_consider, 5) > 0
    exp_2_S_2 = fitdist(response_2_S_2,'Gamma')
end

if counter_jobs_per_batch(batch_to_consider) > 0
    exp_total = fitdist(response_total,'exponential')
end

%%
if counter_jobs_per_batch_path(batch_to_consider, 1) > 0
    [a_1_1,b_1_1,c_1_1]=chi2gof(response_1_1, 'CDF',exp_1_1,'Alpha',0.05)
end
if counter_jobs_per_batch_path(batch_to_consider, 2) > 0
    [a_1_2,b_1_2,c_1_2]=chi2gof(response_1_2, 'CDF',exp_1_2)
end
if counter_jobs_per_batch_path(batch_to_consider, 3) > 0
    [a_2_1,b_2_1,c_2_1]=chi2gof(response_2_1, 'CDF',exp_2_1)
end
if counter_jobs_per_batch_path(batch_to_consider, 4) > 0
    [a_2_2,b_2_2,c_2_2]=chi2gof(response_2_2, 'CDF',exp_2_2)
end
if counter_jobs_per_batch_path(batch_to_consider, 5) > 0
    [a_2_S_2,b_2_S_2,c_2_S_2]=chi2gof(response_2_S_2, 'CDF',exp_2_S_2)
end
if counter_jobs_per_batch(batch_to_consider) > 0
    [a_total,b_total,c_total]=chi2gof(response_total, 'CDF',exp_total)
end
% mmm, this response_1_1 must be very similar to an exponential, so there's
% a problem with my use of chi2gof

%%
if counter_jobs_per_batch_path(batch_to_consider, 1) > 0
    kstest2(response_1_1,exp_1_1)
end
if counter_jobs_per_batch_path(batch_to_consider, 2) > 0
    kstest2(response_1_2,exp_1_2)
end
if counter_jobs_per_batch_path(batch_to_consider, 3) > 0
    kstest2(response_2_1,exp_2_1)
end
if counter_jobs_per_batch_path(batch_to_consider, 4) > 0
    kstest2(response_2_2,exp_2_2)
end
if counter_jobs_per_batch_path(batch_to_consider, 5) > 0
    kstest2(response_2_S_2,exp_2_S_2)
end
if counter_jobs_per_batch(batch_to_consider) > 0
    kstest2(response_total,exp_total)
end
% mmm, this response_1_1 must be very similar to an exponential, so there's
% a problem with my use of chi2gof

%%
pd = makedist('Normal');
rng default;  % for reproducibility
x = random(pd,100,1);