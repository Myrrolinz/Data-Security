#include "examples.h"
/*该文件可以在SEAL/native/example目录下找到*/
#include <vector>
using namespace std;
using namespace seal;
#define N 3
//本例目的：给定x, y, z三个数的密文，让服务器计算x*y*z

int main(){
//初始化要计算的原始数据
vector<double> x, y, z;
	x = { 1.0, 2.0, 3.0 };
	y = { 2.0, 3.0, 4.0 };
	z = { 3.0, 4.0, 5.0 };

/**********************************
客户端的视角：生成参数、构建环境和生成密文
***********************************/
//（1）构建参数容器 parms
EncryptionParameters parms(scheme_type::ckks);
/*CKKS有三个重要参数：
1.poly_module_degree(多项式模数)
2.coeff_modulus（参数模数）
3.scale（规模）*/

size_t poly_modulus_degree = 8192;
parms.set_poly_modulus_degree(poly_modulus_degree);
parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 50,30,30,30,50 }));
//选用2^40进行编码
double scale = pow(2.0, 30);

//（2）用参数生成CKKS框架context 
SEALContext context(parms);

//（3）构建各模块
//首先构建keygenerator，生成公钥、私钥 
KeyGenerator keygen(context); 
auto secret_key = keygen.secret_key();
PublicKey public_key;
    keygen.create_public_key(public_key);

//构建编码器，加密模块、运算器和解密模块
//注意加密需要公钥pk；解密需要私钥sk；编码器需要scale
	Encryptor encryptor(context, public_key);
	Decryptor decryptor(context, secret_key);

	CKKSEncoder encoder(context);
//对向量x、y、z进行编码
	Plaintext xp, yp, zp;
	encoder.encode(x, scale, xp);
	encoder.encode(y, scale, yp);
	encoder.encode(z, scale, zp);
//对明文xp、yp、zp进行加密
	Ciphertext xc, yc, zc;
	encryptor.encrypt(xp, xc);
	encryptor.encrypt(yp, yc);
	encryptor.encrypt(zp, zc);
 

//至此，客户端将pk、CKKS参数发送给服务器，服务器开始运算
/**********************************
服务器的视角：生成重线性密钥、构建环境和执行密文计算
***********************************/
//生成重线性密钥和构建环境
SEALContext context_server(parms);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
	Evaluator evaluator(context_server);  


	Ciphertext temp1;
	Ciphertext result_c1;
//计算x*x，密文相乘，要进行relinearize和rescaling操作 
	evaluator.multiply(xc,xc,temp1);
	evaluator.relinearize_inplace(temp1, relin_keys);
	evaluator.rescale_to_next_inplace(temp1);

//在计算x*x*x之前，x没进行过rescaling操作，所以需要对x进行一次乘法和rescaling操作，目的是使得x*x和x在相同的层
	Plaintext wt;
	encoder.encode(1.0, scale, wt);
//此时，我们可以查看框架中不同数据的层级：
cout << "    + Modulus chain index for xc: "
<< context_server.get_context_data(xc.parms_id())->chain_index() << endl; 
cout << "    + Modulus chain index for temp(x*x): "
<< context_server.get_context_data(temp1.parms_id())->chain_index() << endl;
cout << "    + Modulus chain index for wt: "
<< context_server.get_context_data(wt.parms_id())->chain_index() << endl;

//执行乘法和rescaling操作：
	evaluator.multiply_plain_inplace(xc, wt);
	evaluator.rescale_to_next_inplace(xc);

//再次查看xc的层级，可以发现xc与temp1层级变得相同
cout << "    + Modulus chain index for zc after zc*wt and rescaling: "
<< context_server.get_context_data(xc.parms_id())->chain_index() << endl;

//最后执行temp（x*x）* xc（x*1.0）
	evaluator.multiply_inplace(temp1, xc);
	evaluator.relinearize_inplace(temp1,relin_keys);
	evaluator.rescale_to_next(temp1, result_c1);






	Ciphertext result_c2;
//在计算x*x*x+y*z之前，需要对y*Z进行一次乘法和rescaling操作，目的是使得x*x*x和y*z在相同的层
	Plaintext wt1;
	encoder.encode(1.0, scale, wt1);
//此时，我们可以查看框架中不同数据的层级：
cout << "    + Modulus chain index for yc: "
<< context_server.get_context_data(yc.parms_id())->chain_index() << endl; 
cout << "    + Modulus chain index for zc: "
<< context_server.get_context_data(zc.parms_id())->chain_index() << endl;
cout << "    + Modulus chain index for wt1: "
<< context_server.get_context_data(wt1.parms_id())->chain_index() << endl;

//执行乘法和rescaling操作：
	evaluator.multiply_plain_inplace(yc, wt1);
	evaluator.rescale_to_next_inplace(yc);

//执行乘法和rescaling操作：
	evaluator.multiply_plain_inplace(zc, wt1);
	evaluator.rescale_to_next_inplace(zc);
	


//最后执行(xc*1.0)*(yc*1.0)
//计算y*z，密文相乘，要进行relinearize和rescaling操作 
	evaluator.multiply(yc,zc,result_c2);
	evaluator.relinearize_inplace(result_c2, relin_keys);
	evaluator.rescale_to_next_inplace(result_c2);

//此时，我们可以查看框架中不同数据的层级：
cout << "    + Modulus chain index for result_c1: "
<< context_server.get_context_data(result_c1.parms_id())->chain_index() << endl; 
cout << "    + Modulus chain index for result_c2: "
<< context_server.get_context_data(result_c2.parms_id())->chain_index() << endl;

	Ciphertext result_c;
	
	evaluator.add(result_c1,result_c2,result_c);
	//evaluator.relinearize_inplace(result_c1, relin_keys);
	//evaluator.rescale_to_next(result_c1,result_c);



//计算完毕，服务器把结果发回客户端
/**********************************
客户端的视角：进行解密和解码
***********************************/
//客户端进行解密
	Plaintext result_p;
	decryptor.decrypt(result_c, result_p);
//注意要解码到一个向量上
	vector<double> result;
	encoder.decode(result_p, result);
	cout << "结果是：" << endl;
	print_vector(result,3,3);
return 0;
}

