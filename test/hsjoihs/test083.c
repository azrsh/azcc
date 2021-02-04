int test083(){int a; int b; int c; int d; d=0; b = 5; c = 0;for(a = 3;a;d++){for(;b;++d) {c += b;b-=1;if(b == 3) break;}b = 7;a-=1;if(a == 1) break;} return a*7+b*15+c*2;}
