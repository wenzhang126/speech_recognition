function [ feat ] = wav2feat(wav,Fs,alpha,ncoeffs)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
tw = 25;
to = 15;
Nw = (tw/1000)*Fs;
No = (to/1000)*Fs;
nlogfilts = 40;
wav = preemphasis(wav,alpha);
[feat f t] = (spectrogram(wav,hamming(Nw),No));
feat = log(feat);
f = Fs*f/pi;
[filter ff] = log_filter(nlogfilts,f);
feat2 = zeros(nlogfilts,size(feat,2));
for i =1:size(feat,2)
    tmp = abs(f-ff(i));
    [v idx] = min(tmp); %index of closest value
    feat2(:,i) = filter*abs(feat(:,idx));
end

feat = dct(feat2);
feat = feat(1:ncoeffs,:);
mvfeat = mvnorm(feat);

sfeat = size(mvfeat);
velfeat = [mvfeat(:,2:end) zeros(sfeat(1),1)] -[zeros(sfeat(1),1) mvfeat(:,1:end-1)];
afeat = [velfeat(:,2:end) zeros(sfeat(1),1)] -[zeros(sfeat(1),1) velfeat(:,1:end-1)];
feat = [mvfeat;velfeat;afeat];
end

