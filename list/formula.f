收盘和下影线/R收盘近最高，下影线超过2.5%/R尾盘放量拉升至最高位置,去除涨停股。下影线超过2.5%/Rc1=C(1);c=C();o=O();x1=c1*1.1;x2=c1*1.2;
if (c>o)
    x=o;
else
    x=c;
((P()>=2.5 && (H()-c)/c<=0.003) || 
(x-L())/x>=0.03) && D(x1)!=c && D(x2)!=c;////下影线/R下影线大于3%/R下影线大于3%,收盘价高于开盘价。/Rc=C();o=O();
if (c>o)
    x=o;
else
    x=c;
(x-L())/x>=0.03 && c>o////公式2/R/R/RL(6,3)==L(6,10)&&H(6,5)==H(6,15)&&V()<150000000000 && C(6)>A(6,120)////涨停/R/R/Rc=C(1);d=C();x1=c*1.1;x2=c*1.2;
D(x1)==d || D(x2)==d;
////测试/R/R/Rc=C(1);
x1=c*110;
x2=c*120;
a1=x1+0.5;
a2=x2+0.5;
d=C()*100;
Math.round(x1)==Math.round(d) || Math.round(a1)==Math.round(d) ||
Math.round(x2)==Math.round(d) || Math.round(a2)==Math.round(d);////