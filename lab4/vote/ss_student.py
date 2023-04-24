import ss_function as ss_f

#设置模数 p
p=1000000007
print(f'模数 p：{p}')
#输入参与方 id 以及秘密 s
id=int(input("请输入参与方 id:"))
s=int(input(f'请输入 student_{id}的投票值 s:'))

#秘密份额为(share_x,share_y)
shares_x=[1,2,3]
shares_y=[]
#计算多项式及秘密份额(t=2,n=3)
print(f'Student_{id}的投票值的多项式及秘密份额：')
f=ss_f.get_polynomial(s,1,p,str(id))
temp=[]
for j in range(0,3):
    temp.append(ss_f.count_polynomial(f,shares_x[j],p))
    print(f'({shares_x[j]},{temp[j]})')
    shares_y.append(temp[j])
#Student_id 将自己的投票值的秘密份额分享给两外两个学生
#将三份秘密份额分别保存到 student_id_1.txt,student_id_2.txt,student_id_3.txt
#Student_i 获得 Student_id_i.txt
for i in range(1,4):
    with open(f'student_{id}_{i}.txt','w') as f:
        f.write(str(shares_y[i-1]))