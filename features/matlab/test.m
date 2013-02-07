% in ms
close all; clear all;
len = 25;
overlap = 15;
ncoeffs = 13;
alpha = .95;
[wav fs] = wavread('example.wav');
feat = wav2feat(wav,fs,alpha,ncoeffs);