function parseTree(Directory, num_clusters)
load([Directory strcat('MatFiles/VocTree_', int2str(num_clusters), '.mat')]);
recurse(treeStruct,0,Directory,num_clusters);
