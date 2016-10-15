::Usage: lpasscrashman.bat Target_Name[8974/9x25] EBI_Binary_File Output_folderpath CRM_Buildpath CustomerprovidedOBJPath[Optional]
::Example lpasscrashman.bat 8974 C:\Dropbox\8k_dumps\lpass_crash\dram_0x80000000--0xbfffffff.lst C:\dropbox\123456 C:\YUG_M8960AAAAANAZL1047\lpass_proc C:\Dropbox\8k_dumps\lpass_proc\obj\qdsp6v4_ReleaseG

@echo off
if not "%1"=="" (
goto argumentspresent
) else (
echo Usage: adspcrashman.bat Target_Name[8974/9x25/8x10] EBI_Binary_File Output_folderpath CRM_Buildpath CustomerprovidedOBJPath[Optional]
goto endall
)
:argumentspresent

if not "%4"=="" (
goto insufficientarguments
) else (
echo Insufficient Arguments
echo Usage: adspcrashman.bat Target_Name[8974/9x25/8x10] EBI_Binary_File Output_folderpath CRM_Buildpath CustomerprovidedOBJPath[Optional]
goto endall
)
:insufficientarguments

if not "%5"=="" (
set buildd=%5
) else (
set buildd=%4
)  
:echo %buildd%

::if "%1"=="8660" (
::echo 8660 is not supported for now. Will be released shortly.
::goto endall
::)
cd > %3%\currentdir.txt

for /f "tokens=* delims= " %%v in (%3%\currentdir.txt) do (
set curdir=%%v%
::echo %curdir%
goto donedir
)
:donedir


::echo %curdir%

set targetid=%1%
set INDumpPath=%2%
set OUTDumpPath=%3%
set CRM_buildpath=%4%
set CustomerPath=%5%
set CurrDirectory=%curdir%


if "%5"=="SMMU" (
set smmu_flag=1
echo SMMU flag set
)

if "%6"=="SMMU" (
set smmu_flag=1
echo SMMU flag set
)


if "%smmu_flag%"=="1" (
set /p id=Please enter the VMLINUX path: 
echo %id%

For %%A in ("%INDumpPath%") do (
    Set INDumpPathOcMEM=%%~dpA
    Set Name=%%~nxA
)

cd SMMU_scripts

python ramparse.py  -a %INDumpPathOcMEM% -o %OUTDumpPath% -v %id% --print-iommu-pg-tables
echo SMMU Page table dump text files generated

cd ..



)


mkdir %OUTDumpPath%

dir %INDumpPath% /D /-C >%OUTDumpPath%\binarylength.txt

if exist %OUTDumpPath%\DumpTimeStamp.txt del %OUTDumpPath%\DumpTimeStamp.txt

perl Check_Ram_Size.pl %OUTDumpPath%/binarylength.txt
python Start_Addr1.py  %INDumpPath% %OUTDumpPath%\adspstartaddr.txt %OUTDumpPath%\dumpformant.txt %OUTDumpPath%\binarylength.txt



echo %curdir%

::echo Dumps related to target %targetid%
::echo CRM build path: %CRM_buildpath%

start C:\T32\t32mqdsp6.exe -c %CurrDirectory%\config_sim_usb.t32, "%CurrDirectory%\DSP_load_memorydump_crashman.cmm" %targetid% %INDumpPath% %OUTDumpPath% %CRM_buildpath% %CurrDirectory% %CustomerPath% %smmu_flag%

:endall


 
