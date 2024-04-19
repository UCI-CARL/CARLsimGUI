

neurons = @(w) w .^ 2;

% connections for 
% 8 cardinals
% edges   % sides   % inner points
%connections8 = @(w) (4 * 3)  +   ((w - 2) * 4 * 5 ) + ( (w - 2) .^ 2 * 8);
 
% connections for 4 cardinals 
%connections4 = @(w) (4 * 2)  +   ((w - 2) * 4 * 3 ) + ( (w - 2) .^ 2 * 4); 

w = [1 2 5 10 20];

figure(6, "position", [100, 100, 600,320]);

##DEBUG
##
##4,1,47
##4,1,37
##4,2,43
##4,5,66
##4,10,111
##4,20,199
##
##13,1,67
##13,1,56
##13,2,64
##13,5,110
##13,10,134
##13,20,224
##
##RELEASE
##4,1,14
##4,1,3
##4,2,4
##4,5,6
##4,10,9
##4,20,14


% second chart
% LIF, IzhRS, CPU / GPU => 4   show break even 
% Integration method IZH Runge Cutta 10
DEB_4 = [37, 43, 66, 111, 199]; 
DEB_13 = [56, 54, 110, 134, 224];  
REL_4 = [3, 4, 6, 9, 14]; 
REL_13 = [25, 26, 32, 29, 36];

plot(w, DEB_4, w, DEB_13, w, REL_4, w, REL_13);

ylim([0.0, 250])
xlim([0, 22])

xticks (w)

title("Performance UpdateDelays");
xlabel('|Delta D|');
ylabel('Processing time in \mu sec.');

hlgd = legend("Edge Length 4 (debug version)", "Edge Length 13 (debug version)", "Edge Length 4 (release version)", "Edge Length 13 (release version)"); 

legend(hlgd, "location", "northwest");

print("-S600,320", "perf_update.png");
