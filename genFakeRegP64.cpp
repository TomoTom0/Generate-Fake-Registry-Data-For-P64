// c++17
// g++ test.cpp -lz -lssl -lcrypto -std=c++17 && ./a.out

#include <stdio.h>
#include <string.h>
#include <vector>
#include <openssl/md5.h>
#include <iostream>
#include <time.h>
#include <iomanip>
#include <regex>
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

std::tuple<SupportInfo, string> obtainInfo(vector<__uint8_t> InData){
    vector<__uint8_t> OutData;
    OutData.resize(sizeof(SupportInfo)+100);
    uLongf DestLen = OutData.size();

    uncompress(OutData.data(), &DestLen, InData.data(), InData.size());
    OutData.resize(DestLen);
    SupportInfo * Info_star = (SupportInfo *) OutData.data();
    if (OutData.size() != sizeof(SupportInfo) + 32){
        std::cout<<"This is a invalid data. Please retry."<<std::endl;
        //SupportInfo Info_tmp={"", "", "", "", 0,0,0,false};
        //return std::forward_as_tuple(Info_tmp, "");
    }
    const char * hash_saved_char = (const char *)(OutData.data() + sizeof(SupportInfo));
    std::stringstream oss_tmp;
    string hash_saved_tmp=string(reinterpret_cast<const char*>(hash_saved_char));
    oss_tmp<<std::uppercase<<std::regex_replace(hash_saved_tmp, std::regex("[^(a-fA-F0-9)]*([a-fA-F0-9]+)[^(a-fA-F0-9)]*$"), "$1");
    const string hash_saved=oss_tmp.str();
    return std::forward_as_tuple(* Info_star, hash_saved);
}

string obtainInput(const std::map<string, vector<string>>& input_map, const string& defaultInput=""){
    std::map<string, string> input_map_forFind;

    for (const auto& [key, vec] : input_map){
        for (const auto s: vec){
            input_map_forFind[s] = key;
        }
    }
    string inputVal;
    while(true){
        std::cin>>inputVal;
        if (inputVal.length()==0 && input_map_forFind.count(defaultInput)!=0){
            inputVal=defaultInput;
            break;
        }
        else if (input_map_forFind.count(inputVal)!=0) break;
        else {
            std::cout<<"Invalid content. Please input again."<<std::endl;
        }
    }
    return input_map_forFind[inputVal];
}

vector<__uint8_t> obtainRegistryData(){
    std::cout<<"  Input Registry Data. After inputting, push Ctrl+D."<<std::endl;
    string input_line;
    vector<__uint8_t> registryData={};
    std::ostringstream oss;
    while(std::getline(std::cin, input_line)){
        if(std::cin.eof()) break;
        if (input_line.find("\"user\"=hex:")!=sring::npos){
            oss.str();
        }
        for (auto s: input_line){
            if (s!="\r" && s!= "\n"){
                oss<<s;
            }
        }
    }
    const string data_numbers=std::regex_replace(oss.str(), std::regex("[^(a-fA-F0-9)]*([a-fA-F0-9]+)[,\\\\s ]*"), "$1,");
    string res_data=data_numbers;
    while(true){
        const size_t com_place=res_data.find(",");
        if (com_place== string::npos) break;
        string data_str=res_data.substr(0,com_place);
        __uint8_t data_tmp= (__uint8_t) (std::stoi(data_str, nullptr, 16)^0xAA);
        registryData.push_back(data_tmp);
        res_data=res_data.substr(com_place+1);
    }

    return registryData;
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

// ##----------tmp--------------

int tmp(){
    string in_str;
    std::cout<<1<<std::endl;
    std::cin.ignore(INT_MAX, *"\n");
    while (std::getline(std::cin, in_str, )){
        std::cout<<in_str.length()<<std::endl;
    }
    std::cout<<2<<std::endl;
}

// ##---------uncompress-----------

int uncomp(){
    vector<__uint8_t> InData;
    // test data
    //InData = {120,218,75,206,79,73,101,24,5,68,129,212,220,196,204,156,209,96,32,10,228,37,230,142,166,43,34,129,35,1,48,26,66,8,240,225,198,255,255,32,122,223,110,198,68,100,154,17,42,239,100,100,96,100,230,236,234,232,104,96,96,224,226,104,96,106,97,226,104,96,105,96,230,108,234,232,228,102,230,100,102,1,0,249,79,28,21};
    InData=obtainRegistryData();
    SupportInfo Info;
    string hash_saved;
    std::tie(Info, hash_saved) = obtainInfo(InData);
    printInfo(Info);
    if (hash_saved.length()==0){
        return -1;
    }

    const string hash=obtainMd5((const unsigned char &) Info, sizeof(SupportInfo));
    if (strcmp(hash_saved.c_str(), hash.c_str())!=0){
        std::cout<<"\n  The saved md5 hash is wrong: \n"<<\
        hash_saved<<\
        "\n  The following is right: \n"<<\
        hash<<std::endl;
    } else {
        std::cout<<"\n  The saved md5 hash (correct): \n"<<\
        hash_saved<<std::endl;
    }

    return 0;
}

// ##---------compress-----------

int comp(){

    time_t now = time(NULL);
    uint32_t run_count_tmp = static_cast<uint32_t>((int32_t)-10000);
    std::cout<<"  Which is your input content?: (1-2, default: 1)\n"<<\
    "\t  1. Machine ID\n\t  2. Registry Data"<<std::endl;

    const std::map<string, vector<string>> input_map={
        {"MachineID", {"1"}},
        {"RegistryData", {"2"}}
    };
    string selection=obtainInput(input_map, "1");
    const bool isMachineID= selection == "MachineID";
    string MachineID_value;
    if (isMachineID){
        string inputVal;
        std::cout<<"  Input MachineID"<<std::endl;
        std::cin>>inputVal;
        MachineID_value=std::regex_replace(inputVal, std::regex(".*[^(a-fA-F0-9)]*([a-fA-F0-9]{32})[^(a-fA-F0-9)]*.*$"), "$1");
    } else {
        vector<__uint8_t> registryData=obtainRegistryData();
        SupportInfo Info;
        std::tie(Info, std::ignore) = obtainInfo(registryData);
        if (strcmp(Info.MachineID, "")==0) return -1;
        MachineID_value=Info.MachineID;
    }
    std::cout<<"  Will you use the default values for a fake registry data? (y/n, default: y)\n"<<\
    "\t  Code, Name, Email, RunCount"<<std::endl;
    const std::map<string, vector<string>> input_map_yn={
        {"yes", {"y", "Y","yes", "Yes", "YES"}},
        {"no", {"n", "N", "no", "No", "NO"}}
    };
    const string selection_yn = obtainInput(input_map_yn, "y");

    SupportInfo m_SupportInfo={"code", "email", "name", "id", run_count_tmp, now, now, true};
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

int main(){
    //std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout<<"  What is the your purpose?: (1-2)\n"<<\
    "\t  1. Make a fake registry data for Project 64 to avoid the Support Window.\n"<<\
    "\t  2. Uncompress the registry data for Project 64 and obtain the saved information\n";
    const std::map<string, vector<string>> input_map={
        {"comp",{"1"}},
        {"uncomp",{"2"}},
        {"tmp", {"3"}}};
    const string selection=obtainInput(input_map);
    if (selection=="comp"){
        comp();
    } else if (selection=="uncomp"){
        uncomp();
    } else if (selection=="tmp"){
        tmp();
    } 

    return 0;
}