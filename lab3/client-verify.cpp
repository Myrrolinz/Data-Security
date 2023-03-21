#include<libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>
#include<fstream>
#include"common.hpp"
using namespace libsnark;
using namespace std;
int main(){
    //构造面包板
    protoboard<FieldT>pb=build_protoboard(NULL);
    //加载验证密钥
    fstream f_vk("client_vk.raw",ios_base::in);
    r1cs_gg_ppzksnark_verification_key<libff::default_ec_pp>client_vk;
    f_vk>>client_vk;
    f_vk.close();
    //加载银行生成的证明
    fstream f_proof("bank_proof.raw",ios_base::in);
    r1cs_gg_ppzksnark_proof<libff::default_ec_pp>bank_proof;
    f_proof>>bank_proof;
    f_proof.close();
    //进行验证
    bool verified=r1cs_gg_ppzksnark_verifier_strong_IC<default_r1cs_gg_ppzksnark_pp>(client_vk,pb.primary_input(),bank_proof);
    cout<<"验证结果："<<verified<<endl;
    return 0;
}
