import random

#快速幂计算 a^b%p
def quickpower(a,b,p):
    a=a%p
    ans=1
    while b!=0:
        if b&1:
            ans=(ans*a)%p
        b>>=1
        a=(a*a)%p
    return ans

#构建多项式：x0 为常数项系数，T 为最高次项次数，p 为模数,fname 为多项式名
def get_polynomial(x0,T,p,fname):
    f=[]
    f.append(x0)
    for i in range(0,T):
        f.append(random.randrange(0,p))
    #输出多项式
    f_print='f'+fname+'='+str(f[0])
    for i in range(1,T+1):
        f_print+='+'+str(f[i])+'x^'+str(i)
    print(f_print)
    return f

#计算多项式值
def count_polynomial(f,x,p):
    ans=f[0]
    for i in range(1,len(f)):
        ans=(ans+f[i]*quickpower(x,i,p))%p
    return ans

#重构函数 f 并返回 f(0)
def restructure_polynomial(x,fx,t,p):
    ans=0
    #利用多项式插值法计算出 x=0 时多项式的值
    for i in range(0,t):
        fx[i]=fx[i]%p
        fxi=1
        #在模 p 下，(a/b)%p=(a*c)%p，其中 c 为 b 在模 p 下的逆元，c=b^(p-2)%p
        for j in range(0,t):
            if j !=i:
                fxi=(-1*fxi*x[j]*quickpower(x[i]-x[j],p-2,p))%p
        fxi=(fxi*fx[i])%p
        ans=(ans+fxi)%p
    return ans
