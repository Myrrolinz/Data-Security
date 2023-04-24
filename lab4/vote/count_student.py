p=1000000007
#输入参与方 id
id=int(input("请输入参与方 id:"))
#Student_id 读取属于自己的秘密份额student_1_id.txt,student_2_id.txt,student_3_id.txt
data=[]
for i in range(1,4):
    with open(f'student_{i}_{id}.txt', "r") as f: #打开文本
        data.append(int(f.read())) #读取文本
#计算三个秘密份额的和
d=0
for i in range(0,3):
    d=(d+data[i])%p
#将求和后的秘密份额保存到文件 d_id.txt 内
with open(f'd_{id}.txt','w') as f:
    f.write(str(d))