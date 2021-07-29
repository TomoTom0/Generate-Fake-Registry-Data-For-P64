# Generate Fake Registry Data For P64

This program help you avoid the _annoying_ Support Window of Project 64.

- generate a fake registry data with support information
- read the information saved as the registry data

## Open the Registry Editor

You should open the registory editor; `Win+R` and open Registry Editor with `regedit`.

## Obtain the MachineID

If you _unfortunately_ have a open support window, the Machine is written on it.
If not, you should export the registry data of Project 64 because the program use it in order to obtain the MachineID.

## Compile

```bash
g++ genFakeRegP64.cpp -lz -lssl -lcrypto -std=c++17
```

The option `-lz` is for `zlib.h` and`-lssl -lcrypto` for `openssl/md5.h`.

## Execute

```bash
## for Linux, WSL / Mac
./a.out
## for Windows Powershell, cmd
# ./a.exe

## Select the Purpose

#>  What is the your purpose?: (1-2)
#>          1. Make a fake registry data for Project 64 to avoid the Support Window.
#>          2. Uncompress the registry data for Project 64 and obtain the saved information
1

## Input MachineID or Registry Data

#>  Which is your input content?: (1-2)
#>          1. Machine ID
#>          2. Registry Data
1
## for the case of 1. Machine ID
#
#>  Input MachineID
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

# 2
## for the case of 2. Regiistry Data
#
#> Input Registry Data. After inputting, push Ctrl+D.
#"user"=hex:d2,70,e1,64,e5,e3,cf,b2,af,ee,2b,7e,76,6e,66,36,7b,ca,8a,a0,4e,8f,\
#  4c,24,c,81,88,2b,89,ab,9a,b0,e8,a2,5a,4b,6c,55,55,8a,d0,44,c7,c,ee,ce,30,\
#  bb,80,45,c0,c0,43,46,48,cc,40,c6,cb,cc,43,cc,c3,42,40,42,cc,42,c6,c0,4c,4c,\
#  4e,42,42,c6,46,c0,c3,c6,c2,a6,aa,ab,4a,b6,db
#
## -----------After inputting, push Cctrl+D-----------

#>  Will you use the default values for a fake registry data? (y/n)
#>          Code, Name, Email, RunCount
y

#>-------------------
#>  The following is a new registry file content.
#>  Make new text file(`*.reg`) with it and import the file to the Registry Editor.
#>-------------------
#>Windows Registry Editor Version 5.00
#>
#>[HKEY_CURRENT_USER\SOFTWARE\Project64]
#>"user"=hex:d2,70,e1,64,e5,e3,cf,b2,af,ee,2b,7e,76,6e,66,36,7b,ca,8a,a0,4e,8f,\
#>  4c,24,c,81,88,2b,89,ab,9a,b0,e8,a2,5a,4b,6c,55,55,8a,d0,67,df,6c,ee,ce,30,\
#>  bb,80,c5,cc,c0,4a,46,40,48,46,c6,4e,4e,c6,c0,cc,c6,46,4e,ce,c0,4b,c8,c2,4a,\
#>  4c,c2,c4,c6,cb,ce,48,c2,40,ae,aa,ab,f7,b6,e2
```

## Import the Fake Registry Data

Import the text file containing the fake registry data to the Registry Editor, then the support window won't appear until you play Project 64 for 10000 hours.

## Paiza.io (Execute Online)

If you only want to generate a fake registry data, you can finish it even online with genFakeRegP64_min.cpp -> Paiza.io:  https://paiza.io/projects/PvvYNgzj-8Kb8qbpctxzdw

## Reference

- [project64](https://github.com/project64/project64)
- [zlib](https://github.com/madler/zlib)