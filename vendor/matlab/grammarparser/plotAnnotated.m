function [  ] = plotAnnotated( val,ann )
%PLOTANNOTATED Summary of this function goes here
%   Detailed explanation goes here
unique_ann=unique(ann);
colors=['r'; 'g' ; 'k' ; 'b';];

len=length(val);
current=1;
figure;
hold on;
%To prepare Legen
for i=1:length(unique_ann)
        plot([0],[0],colors(i));
end
for i=1:len
    if(ann(current)~=ann(i))
        %if(ann(current)==4)
        %       colors(unique_ann==4)
            stem(current:i,val(current:i),colors(unique_ann==ann(current)))
        %end
        current=i;
    end
    
     

end
stem(current:len,val(current:len),colors(unique_ann==ann(current)))

