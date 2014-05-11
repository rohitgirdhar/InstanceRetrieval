function [treeStruct,count] = recurse(treeStruct,count,Directory,num_clusters)
 if( isempty(treeStruct) )
   return;
 end
 treeFile = [Directory strcat('HKMeans_', int2str(num_clusters), '.Tree')];
 treeStruct.ind = count;
 count = count + 1;
 subs = [];
 fid = fopen( treeFile, 'a' );
 fprintf( fid,'%d %d\n',size(treeStruct.centers,2), size(treeStruct.sub,2) );
 fclose(fid);
 dlmwrite( treeFile,treeStruct.centers','-append','delimiter',' ');
 for i = 1:size(treeStruct.sub,2)
   [t, count] = recurse(treeStruct.sub(i),count,Directory,num_clusters);
   subs = [subs t];
 end
 treeStruct.sub = subs;
end

