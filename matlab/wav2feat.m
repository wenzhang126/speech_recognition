function [ feat ] = wav2feat(wav,Fs,alpha,ncoeffs)
%converts wav input into mfcc, velocity, and acceleration features

%
tw = 25;
to = 15;
Nw = (tw/1000)*Fs;
No = (to/1000)*Fs;
nlogfilts = 40;

% preemphasize the audio
wav = preemphasis(wav,alpha);

% take the log magnitude of spectrogram
[feat f t] = spectrogram(wav,Nw,No);
feat = log10(abs(feat));
size(feat)

% plot log spectrogram
figure;subplot(3,1,1);title('log spectrum');surf(t,f,feat);axis tight;view(0,90);
f = Fs*f/pi;
size(f)


% compute and apply the log filterbank
[filter ff] = log_filter(nlogfilts,f);
size(ff)
feat2 = zeros(nlogfilts,numel(f));
for i =1:numel(f)
    tmp = abs(f-ff(i));
    [v idx] = min(tmp); %index of closest value
    feat2(:,i) = filter*abs(feat(:,idx));
end

subplot(3,1,2);title('log filterbank');surf(feat2);axis tight;view(0,90);

% take the dct to get the mfccs and mean variance normalize
feat = dct(feat2);
feat = feat(1:ncoeffs,:);
mvfeat = mvnorm(feat);
subplot(3,1,3);title('MFCC');surf(feat);axis tight;view(0,90);
% generate final feature matrix with velocity and acceleration
sfeat = size(mvfeat);
velfeat = [mvfeat(:,2:end) zeros(sfeat(1),1)] -[zeros(sfeat(1),1) mvfeat(:,1:end-1)];
afeat = [velfeat(:,2:end) zeros(sfeat(1),1)] -[zeros(sfeat(1),1) velfeat(:,1:end-1)];
feat = [mvfeat;velfeat;afeat];

end

