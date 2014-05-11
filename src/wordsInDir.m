function wordsInDir(Directory, num_clusters)
name = textread([Directory 'ImageNames.txt'],'%s');
DCount = dlmread([Directory 'DCount.txt']);
W = dlmread([Directory strcat('Words_', int2str(num_clusters), '.txt')]);

x=1;
for i = 1:size(DCount,1)
	dlmwrite([Directory 'words/w_' char(name(i)) '.txt'], W(x:x+DCount(i)-1), 'precision',7);
	x = x+DCount(i);
end


