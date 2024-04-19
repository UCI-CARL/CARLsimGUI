# CARLsim GUI  13x13 Boone


[loop loss activity] = textread("training.txt", "%d %f %f");

scale = 2

width = 320
height = 600

#
figure(6, "position", [100, 100, width*scale,height*scale]);

set(0, "defaulttextfontsize", 24 * scale)  % title
set(0, "defaultaxesfontsize", 12 * scale)  % axes labels


%visualize maze costs
subplot (2, 1, 1);
plot(loop, loss, "-", "linewidth", 2.0 * scale);
title "A. Loss"
ylabel "Loss"
xlabel "Training Loop"

subplot (2, 1, 2);
plot(loop, activity, "-", "linewidth", 2.0 * scale);
title "B. Neural Activity"
ylabel "Activity"
xlabel "Training Loop"

pngFile = sprintf("png\\training.png");
#Draft
#print("-S640,320", pngFile);
print(sprintf("-S%d,%d", width*scale, height*scale), pngFile);

#Camera ready
#print("-S6400,3200", pngFile);






