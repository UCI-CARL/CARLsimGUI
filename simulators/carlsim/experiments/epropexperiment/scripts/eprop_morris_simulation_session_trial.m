# CARLsim GUI  13x13 Boone

# path = %path%;C:\Program Files\GNU Octave\Octave-6.3.0\mingw64\bin\octave-cli.exe
# working dir = current tain,loop_i, segment_i
# script can be copied into dir and evaluated for the specific context

#arg_list = argv ();
#if(rows(arg_list) > 1) 
# subdir = arg_list{1};
# printf("subdir %s\n", subdir); 
# cd(subdir);
#endif

#get context, current path: last defines segment, loop
wd = pwd();
dirs = strsplit(wd, "\\"); 
n = columns(dirs);
trial = strsplit(dirs{1,n},"_"){1,2};
session = strsplit(dirs{1,n-1},"_"){1,2};
simulation = strsplit(dirs{1,n-2},"_"){1,2};


# caution: %d reads int32 and implicit rounds when deviding
path = textread("path.txt", "%f");
path = transpose(path);

scale = 2

width = 320
height = 600

#
figure(11, "position", [100, 100, width*scale,height*scale]);

set(0, "defaulttextfontsize", 24 * scale)  % title
set(0, "defaultaxesfontsize", 12 * scale)  % axes labels

%set(0, "defaulttextfontname", "Courier")
%set(0, "defaultaxesfontname", "Courier")
%set(0, "defaultlinelinewidth", 10)

%visualize maze costs
subplot (2, 1, 1);
[nId e_i] = textread("..\\..\\..\\..\\maze.txt", "%d %f");
E_i = transpose(reshape(e_i, 13, 13));
imagesc(E_i);  % turns this matrix to an image
colorbar;  % adds a colorbar legend
line(mod(path, 13)+1, fix(path/13)+1, "linewidth", 4.0 * scale, "color", 'cyan');
axis("nolabel")

%visualize eligibitlly trace
subplot (2, 1, 2);
[nId e_i] = textread("eligibility.txt", "%d %f");
E_i = transpose(reshape(e_i, 13, 13));
imagesc(E_i);  % turns this matrix to an image
colorbar;  % adds a colorbar legend
line(mod(path, 13)+1, fix(path/13)+1, "linewidth", 4.0 * scale, "color", 'cyan');
axis("nolabel")

pngFile = sprintf("..\\..\\..\\png\\trial_%s_%s_%s.png", simulation, session, trial);
#print("-S320,600", pngFile);
print(sprintf("-S%d,%d", width*scale, height*scale), pngFile);






