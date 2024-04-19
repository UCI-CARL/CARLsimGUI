# CARLsim GUI  13x13 Morris

# The 'boxplot' function belongs to the statistics package from Octave Forge 
# which you have installed but not loaded.  To load the package, run
# 'pkg load statistics' from the Octave prompt.


[simulation session loss activity length] = textread("trails.txt", "%d %d %f %f %f");  % trials.txt

#
figure(6, "position", [100, 100, 800, 250]);

sessions = 8
simulations = 6

data = reshape (loss, sessions, simulations);
data = transpose(data);
subplot (1, 2, 1);
boxplot(data);
title "A. Loss"
ylabel "Loss"
xlabel "Session"
xticks (1:8)


##data = reshape (activity, sessions, simulations);
##transpose(data);
##subplot (1, 2, 2);
##boxplot(data);
##title "B. Neural Activity"
##ylabel "Activity"
##xlabel "Session"
##xticks (1:8)

##subplot (1, 2, 2);
##plot((1:8), activity(1:8), "-");
##title "B. Neural Activity"
##ylabel "Activity"
##xlabel "Session"


data = reshape (activity, sessions, simulations);
transpose(data);
subplot (1, 2, 2);
plot(data, "-");
title "B. Neural Activity"
ylabel "Activity"
xlabel "Session"
xticks (1:8)
xlim([0,9])
legend("simulation 1", "simulation 2", "simulation 3", "simulation 4", "simulation 5", "simulation 6");


pngFile = sprintf("png\\trials.png");
#Draft
print("-S800,250", pngFile);
#Camera ready
#print("-S6400,3200", pngFile);






