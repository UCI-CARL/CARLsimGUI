

neurons = @(w) w .^ 2;

% connections for 
% 8 cardinals
% edges   % sides   % inner points
%connections8 = @(w) (4 * 3)  +   ((w - 2) * 4 * 5 ) + ( (w - 2) .^ 2 * 8);
 
% connections for 4 cardinals 
%connections4 = @(w) (4 * 2)  +   ((w - 2) * 4 * 3 ) + ( (w - 2) .^ 2 * 4); 

w = [13 32 64 128 256 512];

w2 = [13 32 64 128 256];  % [:-1]
%n = neurons(w);
%c4 = connections4(w);
%c8 = connections8(w);


figure(6, "position", [100, 100, 600,320]);

## setIntegrationMethod(RUNGE_KUTTA4, 10);
##
##  3: CPU,IZH,13,26,2
##  7: CPU,IZH,32,64,19
## 11: CPU,IZH,64,128,143
## 15: CPU,IZH,128,256,1092
## 19: CPU,IZH,256,512,9229
## 23: CPU,IZH,512,1024,77806
## 
##  4: CPU,LIF,13,26,1
##  8: CPU,LIF,32,64,9
## 12: CPU,LIF,64,128,65
## 16: CPU,LIF,128,256,583
## 20: CPU,LIF,256,512,4644
## 24: CPU,LIF,512,1024,45559
## 
##  1: GPU,IZH,13,26,5
##  5: GPU,IZH,32,64,13
##  9: GPU,IZH,64,128,34
## 13: GPU,IZH,128,256,114
## 17: GPU,IZH,256,512,938
## 21: GPU,IZH,512,1024,6079
##
##  2: GPU,LIF,13,26,4
##  6: GPU,LIF,32,64,13
## 10: GPU,LIF,64,128,31
## 14: GPU,LIF,128,256,96
## 18: GPU,LIF,256,512,819
## 22: GPU,LIF,512,1024,5271


% second chart
% LIF, IzhRS, CPU / GPU => 4   show break even 
% Integration method IZH Runge Cutta 10
CPU_IZH = [2, 19, 143, 1092, 9229, 77806]; 
CPU_LIF = [1,  9,  65,  583, 4644, 45559];  
GPU_IZH = [5,13,34,114,938,6079]; 
GPU_LIF = [4,13,31,96,819,5271];

% plot(w, CPU_IZH, w, CPU_LIF, w, GPU_IZH, w, GPU_LIF);

n = w.^2;
mt = w.*2;

%plot(w, mt ./ CPU_IZH , w, mt ./ CPU_LIF , w, mt ./ GPU_IZH, w , mt ./GPU_LIF );

%plot(n, GPU_LIF ./ GPU_IZH, n, CPU_LIF ./ CPU_IZH);

%plot(w, GPU_LIF ./ GPU_IZH, w, CPU_LIF ./ CPU_IZH);
%ylim([0.0, 1.05])

plot(w, CPU_IZH, w, CPU_LIF, w ,GPU_IZH, w, GPU_LIF); % w, mt);
ylim([0.0, 6500])
xlim([0, 550])

xticks (w)

%line([13,26, 512, 1024], "color", "r"); 
%line([13,512], [26, 1024]]); 


title("Izhikevich-RS / LIF Neuron");
%xlabel('Neurons');
xlabel('Edge length');
ylabel('Processing time in ms (Runge Kutta)');

%hlgd = legend("GPU: Izh.RS/LIF", "CPU: Izh.-RS/LIF");

hlgd = legend("CPU - Izhikevich", "CPU - LIF", "GPU - Izhikevich", "GPU - LIF"); % "Realtime");


%legend(hlgd, "location", "eastoutside")
%legend(hlgd, "location", "west");
legend(hlgd, "location", "southeast");

%semilogy(w, n, w, c8, w, c4)

print("-S600,320", "perf_wave.png");
