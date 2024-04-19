

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

##		const int nLength = 6;
##		int lengths[] = { 13, 32, 64, 128, 256, 512 };  //   
##    if (iModel == IZH) {
##      sim->setIntegrationMethod(RUNGE_KUTTA4, 10);
##    if (iModel == LIF) {
##	    sim->setIntegrationMethod(FORWARD_EULER, 2);     
       
##CPU,IZH,13,26,2
##CPU,IZH,32,64,20
##CPU,IZH,64,128,151
##CPU,IZH,128,256,1193
##CPU,IZH,256,512,9379

##CPU,LIF,13,26,0
##CPU,LIF,32,64,4
##CPU,LIF,64,128,20
##CPU,LIF,128,256,163
##CPU,LIF,256,512,1658
##CPU,LIF,512,1024,15214

##GPU,IZH,13,26,5
##GPU,IZH,32,64,14
##GPU,IZH,64,128,32
##GPU,IZH,128,256,108
##GPU,IZH,256,512,871
##GPU,IZH,512,1024,5731

##GPU,LIF,13,26,3
##GPU,LIF,32,64,9
##GPU,LIF,64,128,21
##GPU,LIF,128,256,73
##GPU,LIF,256,512,649
##GPU,LIF,512,1024,4546

% second chart
% LIF, IzhRS, CPU / GPU => 4   show break even 
% Integration method IZH Runge Cutta 10,  LIF ForwardEuler 2
CPU_IZH = [2, 19, 140, 1146, 9368, 9368];  % RUNGE_KUTTA4, 10   -- cut w
CPU_LIF = [0,  4,  20,  163, 1658, 15214];  % FORWARD_EULER, 2
%CPU_LIF = [1,  4,  33,  271, 2000];  % FORWARD_EULER, 4
GPU_IZH = [5,14,32,108,871,5731]; 
GPU_LIF = [3,9,21,73,649,4546];

% plot(w, CPU_IZH, w, CPU_LIF, w, GPU_IZH, w, GPU_LIF);

n = w.^2;
mt = w.*2;

%plot(w, mt ./ CPU_IZH , w, mt ./ CPU_LIF , w, mt ./ GPU_IZH, w , mt ./GPU_LIF );

%plot(n, GPU_LIF ./ GPU_IZH, n, CPU_LIF ./ CPU_IZH);

%plot(w, GPU_LIF ./ GPU_IZH, w, CPU_LIF ./ CPU_IZH);
%ylim([0.0, 1.05])

plot(w, GPU_LIF, w , GPU_IZH, w, CPU_LIF, w, CPU_IZH); %w, mt);
ylim([0.0, 6000])
xlim([0, 600])

xticks (w)

%line([13,26, 512, 1024], "color", "r"); 
%line([13,512], [26, 1024]]); 


title("Izhikevich-RS (Runge-Kutta) / LIF (Euler) ");
%xlabel('Neurons');
xlabel('Edge length');
ylabel('Processing time (ms)');

%hlgd = legend("GPU: Izh.RS/LIF", "CPU: Izh.-RS/LIF");

hlgd = legend("GPU - LIF", "GPU - Izhikevich", "CPU - LIF", "CPU - Izhikevich"); % "Realtime");


%legend(hlgd, "location", "eastoutside")
legend(hlgd, "location", "west");

%semilogy(w, n, w, c8, w, c4)

print("-S600,320", "perf_wave.png");
