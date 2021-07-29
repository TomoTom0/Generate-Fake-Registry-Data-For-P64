// c++17
// g++ genFakeRegP64_min.cpp -lz -lssl -lcrypto -std=c++17 && ./a.out

#include <stdio.h>
#include <string.h>
#include <vector>
#include <openssl/md5.h>
#include <iostream>
#include <time.h>
#include <iomanip>
# include <map>
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

string checkInput(string defaultVal=""){
    string inputVal;
    //std::cin.ignore(INT_MAX, *"\n");
    std::getline(std::cin, inputVal);
    if (inputVal.length()==0) {
        if (defaultVal.length()>0) std::cout<<"default value: "<<defaultVal<<std::endl;
        return defaultVal;
    }
    else return inputVal;
}

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
    std::stringstream oss;
    oss<<std::uppercase;
    for (size_t ind=0; ind<32; ind++){
        const char s=hash_saved_char[ind];
        if (judgeHex(s)) oss<<s;
    }
    const string hash_saved=oss.str();
    return std::forward_as_tuple(* Info_star, hash_saved);
}

string obtainInput(const std::map<string, vector<string>>& input_map, const string& defaultVal=""){
    std::map<string, string> input_map_forFind;

    for (const auto& [key, vec] : input_map){
        for (const auto s: vec){
            input_map_forFind[s] = key;
        }
    }
    string inputVal;
    while(true){
        inputVal=checkInput(defaultVal);
        if (input_map_forFind.count(inputVal)!=0) break;
        else {
            std::cout<<"Invalid content. Please input again."<<std::endl;
        }
    }
    return input_map_forFind[inputVal];
}



vector<__uint8_t> obtainRegistryData(){
    std::cout<<"  Input Registry Data. After inputting, push Ctrl+D or Enter."<<std::endl;
    string input_line;
    vector<__uint8_t> registryData={};
    std::ostringstream oss;
    while(std::getline(std::cin, input_line)){
        if(std::cin.eof() || (registryData.size()>0 && input_line.length()==0)) break;
        const size_t userHexPos=input_line.find("\"user\"=hex:");
        if (userHexPos!=string::npos){
            oss.str("");
            registryData={};
            input_line=input_line.substr(userHexPos+1);
        }
        for (const auto s: input_line){
            if (s== * "\r" || s== *"\n"){
                continue;
            } else if (s==*"," || s==*"\\"){
                const string str_tmp=oss.str();
                if (str_tmp.length()>0){
                    __uint8_t data_tmp= (__uint8_t) (std::stoi(str_tmp, nullptr, 16)^0xAA);
                    registryData.push_back(data_tmp);
                }
                oss.str("");
            } else if (judgeHex(s)){
                oss<<s;
            } else {
                oss.str("");
            }
        }
    }
    for (auto s: registryData){
        std::cout<<int(s)<<",";
    }
    std::cout<<std::endl;
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
    string in_str="Aa";
    std::cout<< int(in_str[0])<<std::endl;;
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

    const time_t now = time(NULL);
    const uint32_t run_count_init = static_cast<uint32_t>((int32_t)-10000);
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
        std::cout<<"  Input MachineID (default: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA)"<<std::endl;
        std::ostringstream oss_id;
        while(true){
            const string inputVal=checkInput("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
            for (auto s: inputVal){
                if (judgeHex((s))) oss_id<<s;
            }
            MachineID_value=oss_id.str();
            if (MachineID_value.length()==32) break;
            else{
                std::cout<<"Invalid MachineID. Please input again."<<std::endl;
                continue;
            }
        }
    } else {
        vector<__uint8_t> registryData=obtainRegistryData();
        SupportInfo Info;
        std::tie(Info, std::ignore) = obtainInfo(registryData);
        if (strcmp(Info.MachineID, "")==0) return -1;
        MachineID_value=Info.MachineID;
    }
    std::cout<<"  Will you change the values for a fake registry data? (y/n, default: n)\n"<<\
    "\t  Code, Name, Email, RunCount"<<std::endl;

    SupportInfo m_SupportInfo={"code", "email", "name", "id", run_count_init, now, now, true};
    strcpy(m_SupportInfo.MachineID, MachineID_value.c_str());
    const std::map<string, vector<string>> input_map_yn={
        {"yes", {"y", "Y","yes", "Yes", "YES"}},
        {"no", {"n", "N", "no", "No", "NO"}}
    };
    const string selection_yn = obtainInput(input_map_yn, "n");

    if (selection_yn=="yes"){
        std::cout<<"  1/4, Code: the code as the evidence of donating (not important, default: code)"<<std::endl;
        strcpy(m_SupportInfo.Code, checkInput("code").c_str());
        std::cout<<"  2/4, Name: the name to identify you (not important, default: name)"<<std::endl;
        strcpy(m_SupportInfo.Name, checkInput("name").c_str());
        std::cout<<"  3/4, Email: the email address to identify you (mustn't be empty, default: email)"<<std::endl;
        strcpy(m_SupportInfo.Email, checkInput("email").c_str());
        std::cout<<"  4/4, RunCount: the redidual playing hours for the next support window (must be integer, default: -10000)"<<std::endl;
        string run_count_input;
        while(true){
            run_count_input = checkInput("-10000");
            bool isValid=true;
            for (auto s: run_count_input){
                const int c_p=int(s);
                if (s==*"-" && run_count_input.rfind(s)==0){
                    continue;
                } else if (48<= c_p && c_p<=57) {
                    continue;
                } else {
                    isValid=false;
                    break;
                }
            }
            if (isValid) break;
            std::cout<<"Your input cannot interpret integer. Please input again."<<std::endl;
        }
        const int run_count_tmp = -1 * abs(std::stoi(run_count_input));
        m_SupportInfo.RunCount = static_cast<uint32_t>((int32_t)run_count_tmp);
    }

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