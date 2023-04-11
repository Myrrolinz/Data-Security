#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>
#include <fstream>
#include "common.hpp"
using namespace libsnark;
using namespace std;
int main()
{
    //输入秘密值 x
    int x;
    cin>>x;
    //为私密输入提供具体数值
    int secret[4];
    secret[0]=x;
    secret[1]=x*x;
    secret[2]=x*x*x;
    secret[3]=x*x*x+x;
    //构造面包板
    protoboard<FieldT> pb=build_protoboard(secret);
    const r1cs_constraint_system<FieldT> constraint_system =
    pb.get_constraint_system();
    cout<<"公有输入："<<pb.primary_input()<<endl;
    cout<<"私密输入："<<pb.auxiliary_input()<<endl;
    //加载证明密钥
    fstream f_pk("pk.raw",ios_base::in);
    r1cs_gg_ppzksnark_proving_key<libff::default_ec_pp>pk;
    f_pk>>pk;
    f_pk.close();
    //生成证明
    const r1cs_gg_ppzksnark_proof<default_r1cs_gg_ppzksnark_pp> proof =
    r1cs_gg_ppzksnark_prover<default_r1cs_gg_ppzksnark_pp>(pk, pb.primary_input(),
    pb.auxiliary_input());
    //将生成的证明保存到 proof.raw 文件
    fstream pr("proof.raw",ios_base::out);
    pr<<proof;
    pr.close();
    return 0;
}