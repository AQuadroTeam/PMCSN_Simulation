function a= index_of_state(c1,c2,S)
    if c2 > S
        a = -1;
    
    else
        a = c2 + (S+1)*(c1) +1;
    end
end