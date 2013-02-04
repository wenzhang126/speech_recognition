function [ mvfeat ] = mvnorm( feat )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
sfeat = size(feat);
m = sum(feat,2)/sfeat(2);
mfeat = feat-repmat(m,1,sfeat(2));
sd = sqrt(sum(mfeat.^2,2)/sfeat(2));
mvfeat = mfeat./repmat(sd,1,sfeat(2));
end

