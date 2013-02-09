% in ms
close all; clear all;
len = 25;
overlap = 15;
ncoeffs = 13;
alpha = .95;
[wav fs] = wavread('example.wav');
wav = wav(1:16000);
feat = wav2feat(wav,1600,alpha,ncoeffs);