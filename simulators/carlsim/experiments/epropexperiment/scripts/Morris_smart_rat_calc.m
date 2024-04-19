syms x y 
f(x,y) = 3*x.^2 + 5*y.^3;
fint = int(f, x)



f(2,3)

int(f, x, y)



syms x 

f(x) = 0.12*(x.*3)^4 - 0.5*(x.*5)^2 + 4;
x1 = linspace(-1,1);
y2 = double(f(x1));

plot(x1, y2)





syms x 

f(x) = 1/0.9258 * 1/4*(0.1*(x.*3)^4 - 0.35*(x.*5)^2 + 5);
x1 = linspace(-1,1);
y2 = double(f(x1));

plot(x1, y2)
ylim([0 1.5])

I_x = int(f, x)

double(I_x(1) - I_x(0))





syms x 

r = 6
f(x) = 1/0.9258 * 1/4*(0.1*(x.*3/r)^4 - 0.35*(x.*5/r)^2 + 5);
x1 = linspace(0,1*r);
y2 = double(f(x1));

plot(x1, y2)
ylim([0 1.5])

I_x = int(f, x)

double(I_x(r) - I_x(0))




syms x 

r = 6
f(x) = 1/0.8127 * 1/4*(0.1*(x.*3/r)^4 - 0.4*(x.*5/r)^2 + 5);
x1 = linspace(0,1*r);
y2 = double(f(x1));

plot(x1, y2)
ylim([0 1.75])

I_x = int(f, x)

double(I_x(r) - I_x(0))

4.93 / 6


syms x 
r = 6
f(x) = 1/4*(0.09*(x.*3/r)^4 - 0.325*(x.*5/r)^2 + 5);
x1 = linspace(0,1*r);
y2 = double(f(x1));
plot(x1, y2)
ylim([0 1.5])
I_x = int(f, x)
double(I_x(r) - I_x(0))


