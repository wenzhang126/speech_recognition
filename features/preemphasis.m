function [ wav] = preemphasis( wav,alpha )
    wav(2:end,:) = wav(2:end,:) - alpha*wav(1:end-1,:);
end
