function [] = hikmeans( Directory, K, Nleaves)
	run('vlfeat/toolbox/vl_setup.m');
	descriptors = uint8(dlmread(fullfile(Directory, 'Descriptors.txt')));
	if( size(descriptors,2) > 128 )
		descriptors(:,129) = [];
	end
	save(fullfile(Directory, 'MatFiles/Descriptors.mat'), 'descriptors');
%	load([Directory 'MatFiles/' 'Descriptors.mat']);
	[treeStruct,A] = vl_hikmeans(descriptors',K,Nleaves);
	save(fullfile(Directory, strcat('MatFiles/VocTree_', int2str(Nleaves), '.mat')),'treeStruct','A');
end
