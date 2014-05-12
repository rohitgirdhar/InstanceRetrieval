function quantize(Directory,num_clusters)
load(fullfile(Directory, strcat('MatFiles/VocTree_', int2str(num_clusters), '.mat')));
W = uint32(zeros(1,size(A,2)));
for i = 1:size(A,1)
	W = (W*10) + A(i,:) - 1;
end
W = W+1;
dlmwrite(fullfile(Directory, strcat('Words_', int2str(num_clusters), '.txt')),W','precision',7);
