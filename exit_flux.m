function [f,up,down,right,left,busy1,busy2] = exit_flux(c1, c2, S, N,params)
    if c1>N || c2>S  || (c1+c2>S && c2~=0)
        f = -1;
        %disp('OUT OF BOUND!!!')
        %disp([c1,c2,S,N])
        up = 0;
        down = 0;
        right = 0;
        left = 0;
        busy1=0;
        busy2=0;
        return;
    end
    
    up = 1;
    down = 1;
    right = 1;
    left = 1;
    busy1 = 0;
    busy2 = 0;
    
    if c1==0
        up = 0;
    end
    if c2==0
        left = 0;
        if c1>=S
            right = 0;
            busy2 = 1;
        end
    end  
    if c1 == N
        down = 0;
        right = 0;
        left =0 ;
        busy1 = 1;
    end
    % sulla diagonale
    if c1+c2==S && c2 ~=0
        down = 0;
        right=0;
        busy2 = 1;
    end  

    
    f = up*up_flux(c1,c2,S,N,params)+left*left_flux(c1,c2,S,N,params)+ discard_2_flux(c1,c2,S,N,params);
end

function s = up_flux(c1,c2,S,N,params)
    s = c1*params(3);
    if s<=0 || c1 < 0 || c2 < 0
    %|| (c1 >= c2 && c2 ~= 0) || (c2==0 && c1 >= N) || c2 >= S
        s = 0;
    end
end

function s = down_flux(c1,c2,S,N,params)
    s = params(1);
    if s<=0 || c1 < 0 || c2 < 0
        s = 0;
    end
end

function s = right_flux(c1,c2,S,N,params)
    s = params(2);
end

function s = left_flux(c1,c2,S,N,params)
    s = c2*params(4);
end

function s = discard_1_flux(c1,c2,S,N,params)
    s = params(1);
end


function s = discard_2_flux(c1,c2,S,N,params)
    s = params(2) + params(1);
end