

neurons = @(w) w .^ 2;

% connections for 
% 8 cardinals
% edges   % sides   % inner points
connections8 = @(w) (4 * 3)  +   ((w - 2) * 4 * 5 ) + ( (w - 2) .^ 2 * 8);
 
% connections for 4 cardinals 
connections4 = @(w) (4 * 2)  +   ((w - 2) * 4 * 3 ) + ( (w - 2) .^ 2 * 4); 

%w = [4 13 32 64 128 256 512 1024];

w = [13 32 64 128 256];
n = neurons(w);
c4 = connections4(w);
c8 = connections8(w);


figure(5, "position", [100, 100, 600,320]);


% second chart
% LIF, IzhRS, CPU / GPU => 4   show break even 
% Integration method IZH Runge Cutta 10,  LIF ForwardEuler 2
CPU_IZH = [1,19,140,1146, 1146];   % todo cut 
CPU_LIF = [0,3,20,162, 162, 666];  % estimated
GPU_IZH = [4,13,33,107,877]; 
GPU_LIF = [3,9,22,83,686];

plot(w, n, w, c4, w, c8, CPU_IZH, CPU_LIF, GPU_IZH, GPU_LIF  );

xticks (w)

title("Complexity and Performance");
xlabel('Edge Length');
ylabel('Neurons/ Synapses');

hlgd = legend("Neurons", "Synapses(C=4)", "Synapses(C=8)", "CPU_IZH", "CPU_LIF", "GPU_IZH", "GPU_LIF");



%legend(hlgd, "location", "eastoutside")
legend(hlgd, "left");
legend(hlgd, "location", "west");

%semilogy(w, n, w, c8, w, c4)

print("-S600,320", "complexity.png");
