int test059(){int a; int b; int c; a = 3; b = 5; c = 0;while(a){while(b) {c += b;b-=1;if(b == 3) break;}b = 7;a-=1;if(a == 1) break;} return a*7+b*15+c*2;}
