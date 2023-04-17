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

#构建多项式：x0 为常数项系数，T 为最高次项次数，p 为模数，fname 为多项式名
def get_polynomial(x0,T,p,fname):
    f=[]
    f.append(x0)
    for i in range(0,T):
        f.append(random.randrange(0,p))
    #输出多项式
    f_print=fname+'='+str(f[0])
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

if __name__ == '__main__':
    #设置模数 p
    p=1000000007
    print(f'模数 p：{p}')
    
    #输入门限(t,n)以及秘密值 s1,s2
    n=int(input("请输入参与者的个数 n:"))
    t=int(input("请输入需要几个人参与者才可以恢复秘密:"))
    s1=int(input("请输入秘密 s1:"))
    s2=int(input("请输入秘密 s2:"))
    
#秘密共享阶段：
    #为两个秘密 s1,s2 分别构造随机多项式 f1(x)和 f2(x)
    print('为秘密 s1,s2 构建随机多项式')
    f1=get_polynomial(s1,t-1,p,'f1')
    f2=get_polynomial(s2,t-1,p,'f2')
    
    #选择 n 个互不相同的随机值
    x=[]
    for i in range(0,n):
        temp=random.randrange(0,p)
        while temp in x:
            temp=random.randrange(0,p)
        x.append(temp)
        
    # 计算得到 shares=[xi,f1(xi),f2(xi)]，其中 f1(xi)为秘密 s1 的秘密份额，
    # f2(xi)为秘密 s2 的秘密份额
    shares=[]
    for i in range(0,n):
        shares.append([])
        shares[i].append(x[i])
        shares[i].append(count_polynomial(f1,x[i],p))
        shares[i].append(count_polynomial(f2,x[i],p))
    #输出秘密份额
    print('s1 秘密份额:')
    for i in range(0,n):
        print(f'({shares[i][0]},{shares[i][1]})')
    print('s2 秘密份额:')
    for i in range(0,n):
        print(f'({shares[i][0]},{shares[i][2]})')
    #将秘密分享给 n 个参与者，第 i 个参与者得到 shares[i]
    
#秘密重构阶段：
    #任意选取 t 个人
    Party=[]
    for i in range(0,t):
        temp=random.randint(0,n-1)
        while temp in Party:
            temp=random.randint(0,n-1)
        Party.append(temp)
    print('参与重构秘密的参与者:',Party)
    
    #t 个参与方分别将自己手中 s1 和 s2 的秘密份额相加，得到 s1+s2 的秘密份额
    shares_s1s2_x=[]
    shares_s1s2_y=[]
    print('s1+s2 的秘密份额为：')
    for i in range(0,t):
        shares_s1s2_x.append(x[Party[i]])
        shares_s1s2_y.append((shares[Party[i]][1]+shares[Party[i]][2])%p)
        print(f'({shares_s1s2_x[i]},{shares_s1s2_y[i]})')
    s1s2=restructure_polynomial(shares_s1s2_x,shares_s1s2_y,t,p)
    print(f'重构得到的秘密值为：{s1s2}')