// c++17
// g++ genFakeRegP64_min.cpp -lz -lssl -lcrypto -std=c++17 && ./a.out YOUR_MACHINE_ID

#include <stdio.h>
#include <string.h>
#include <vector>
#include <openssl/md5.h>
#include <iostream>
#include <time.h>
#include <iomanip>
#include "./zlib/zlib.h"
//#include <zlib.h>

using std::string;
using std::vector;

typedef struct
{
    char Code[300];
    char Email[300];
    char Name[300];
    char MachineID[300];
    uint32_t RunCount;
    time_t LastUpdated;
    time_t LastShown;
    bool Validated;
} SupportInfo;

// ##---------obtain-----------

bool judgeHex(const char c_in){
    const int c_p=int(c_in);
    return (48<= c_p && c_p<=57) ||  // 0-9
            (65<= c_p && c_p<=70) ||  // A-F
            (97<= c_p && c_p<=102); // a-f
}


const string obtainMd5(const unsigned char& hash_origin, const size_t& hash_size){
    unsigned char hash_char[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *) &hash_origin, hash_size, hash_char);
    std::ostringstream oss_tmp;
    for (const auto s: hash_char){
        oss_tmp<<std::hex<<std::setw(2)<<std::setfill(*"0")<<\
        std::uppercase<<int(s);
    }
    const string hash=oss_tmp.str();
    return hash;
}

int printInfo(SupportInfo Info){
    std::ostringstream oss;
    oss<<"Code: "<<Info.Code<<"\n"<<\
    "Email: "<<Info.Email<<"\n"<<\
    "Name: "<<Info.Name<<"\n"<<\
    "MachineId: "<<Info.MachineID<<"\n"<<\
    "RunCount: "<<int(Info.RunCount)<<"\n"<<\
    "LastUpdated: "<<Info.LastUpdated<<"\n"<<\
    "LastShown: "<<Info.LastShown<<"\n"<<\
    "Validaded: "<<Info.Validated<<"\n";
    std::cout<<"-------------------\n"<<\
    "  The saved support information: \n"<<\
    "-------------------\n"<<\
    oss.str()<<std::endl;

    return 0;
}

// ##---------compress-----------

int comp(string MachineID_value){

    const time_t now = time(NULL);
    const uint32_t run_count_init = static_cast<uint32_t>((int32_t)-10000);

    SupportInfo m_SupportInfo={"code", "email", "name", "id", run_count_init, now, now, true};
    strcpy(m_SupportInfo.MachineID, MachineID_value.c_str());

    printInfo(m_SupportInfo);

    const string hash=obtainMd5((const unsigned char &) m_SupportInfo, sizeof(m_SupportInfo));
    vector<__uint8_t> InData(sizeof(m_SupportInfo)+hash.length());
    vector<__uint8_t> OutData;

    memcpy(InData.data(), (const unsigned char *)&m_SupportInfo, sizeof(m_SupportInfo));
    memcpy(InData.data() + sizeof(m_SupportInfo), hash.data(), hash.length() );
    OutData.resize(InData.size());

    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;
    defstream.avail_in = (uInt)InData.size();
    defstream.next_in = (Bytef *)InData.data();
    defstream.avail_out = (uInt)OutData.size();
    defstream.next_out = (Bytef *)OutData.data();

    deflateInit(&defstream, Z_BEST_COMPRESSION);
    deflate(&defstream, Z_FINISH);
    deflateEnd(&defstream);

    OutData.resize(defstream.total_out);

    std::ostringstream oss_tmp;
    std::ostringstream oss;
    oss<<"Windows Registry Editor Version 5.00\n\n"<<\
    "[HKEY_CURRENT_USER\\SOFTWARE\\Project64]\n"<<\
    "\"user\"=hex:";
    int count=0;
    oss<<std::right<<std::setw(2)<<std::setfill(*"0")<<std::hex;
    for (const auto out : OutData){
        if (count>0){
            oss<<",";
        }
        if (count%25==22){
            oss<<"\\\n  ";
        }
        count+=1;
        oss<<(int(out)^0xAA);
    }
    const string RegistryFileContent=oss.str();
    std::cout<<"-------------------\n"<<\
    "  The following is a new registry file content.\n"<<\
    "  Make new text file(`*.reg`) with it and import the file to the Registry Editor.\n"<<\
    "-------------------"<<std::endl;
    std::cout<<RegistryFileContent<<std::endl;
    return 0;
}

// ##----------main------------

int main(int argc, char *argv[]){
    if (argc==2){
        comp(argv[1]);
    }

    return 0;
}