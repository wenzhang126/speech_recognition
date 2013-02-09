%% View energy levels
[x, fs] = wavread('recorded.wav');
winsecs = 10 / 1000;
winsize = round(winsecs * fs);
energy = zeros(1, length(x)-winsize);
energy2 = zeros(1, length(x)-winsize);

for i = 1:1:length(x)-winsize
    currWindow = x(i:i+winsize - 1);
    energy(i) = (sum(abs(currWindow)));
    energy2(i) = sum(currWindow .^ 2);
end

%plot(x); hold on;
plot(energy2); hold on;
plot(energy, 'r');

%% Two threshold adaptive endpointing

[x, fs] = wavread('recorded.wav');

noise = 0.02*randn(1, length(x))';
x = x + noise;
SNR = 20 * log10(sum(abs(x)) / sum(abs(noise)));

framesecs = 20 / 1000;
framesize = round(framesecs * fs);
hopsize = round(framesize / 2);
speech = zeros(length(x)-framesize, 1);
level = zeros(1, round(length(x) / hopsize) - 1);
energy = zeros(1, round(length(x) / hopsize) - 1);
background = zeros(1, round(length(x) / hopsize) - 12);
speechThresh = zeros(1, round(length(x) / hopsize) - 12);
silenceThresh = zeros(1, round(length(x) / hopsize) - 12);

ind = 1;

forget = 4;
avgEnergy = 0;
adjust = 0.0001;
alpha = 0.95;
lambda = 0.99;

for i = 1:hopsize:10*hopsize
    currFrame = x(i:i+framesize-1);
    currEnergy = (sum(abs(currFrame .^ 2)));
    energy(ind) = currEnergy;
    
    if(i == 1)
        level(ind) = currEnergy;
    else
        level(ind) = ((level(ind - 1) * forget) + currEnergy) / (forget + 1);
    end
    
    avgEnergy = avgEnergy + currEnergy;
    ind = ind + 1;
end

avgEnergy = avgEnergy / 10;
bg = avgEnergy;
lowThresh = 4 * bg;
highThresh = 40 * lowThresh;
elapsedspeech = 0;
elapsedsilence = 0;
elapsedspeechthresh = 250;
elapsedsilencethresh = 500;

for i = 11*hopsize:hopsize:length(x)-framesize
    
    currFrame = x(i:i+framesize-1);
    currEnergy = (sum(abs(currFrame .^ 2)));
    energy(ind) = currEnergy;
    
    level(ind) = ((level(ind - 1) * forget) + currEnergy) / (forget + 1);
    
    if(speech(i - 1) == 1)
        if((level(ind) - bg) < lowThresh)
            if(elapsedsilence < elapsedsilencethresh)
                speech(i:i+hopsize-1) = 1;
            else
                speech(i:i+hopsize-1) = 0;
                elapsedspeech = 0;
            end
            
            elapsedsilence = elapsedsilence + (framesecs * 1000)/2;
        else
            speech(i:i+hopsize-1) = 1;
            elapsedspeech = elapsedspeech + (framesecs * 1000)/2;
        end
    else
        if((level(ind) - bg) > highThresh)
            speech(i:i+hopsize-1) = 1;
            elapsedsilence = 0;
        else
            elapsedspeech = 0;
            elapsedsilence = elapsedsilence + (framesecs * 1000)/2;
        end
    end
    
    if(speech(i) == 1)
        bg = bg + (level(ind) - bg) * adjust;
        %bg = lambda*bg + (1-lambda)*(level(ind) - bg);
    else
        bg = alpha*bg + (1-alpha)*(level(ind) - bg);
    end
    
    lowThresh = 1.2 * bg;
    highThresh = 2 * lowThresh;
    
    background(ind - 10) = bg;
    speechThresh(ind - 10) = highThresh;
    silenceThresh(ind - 10) = lowThresh;
    
    ind = ind + 1;
end

%% Filtered 1st order derivative

deriv = [level(1) diff(level)];
delta = smooth(deriv, 50);
plot(delta);

%%
xplot = x / max(x);
levelplot = level / max(level);
levelaxis = 1:hopsize:length(x) - framesize + 1;

figure;
plot(x);
hold on;
plot(speech, 'r');
plot(levelaxis, level, 'g');
%axis([0 length(x)+1 -2 2]);
xlabel('n');
ylabel('s[n]');
title('Speech Signal and Endpoint Label');

figure();
plot(background);
hold on;
plot(silenceThresh, 'g');
plot(speechThresh, 'r');
