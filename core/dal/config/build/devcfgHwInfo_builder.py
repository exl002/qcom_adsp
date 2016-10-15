#===============================================================================
#
# Device Config Builders
#
# GENERAL DESCRIPTION
#    Contains builder(s) defenitions
#
# Copyright (c) 2011,2012 by Qualcomm Technologies Incorporated.
# All Rights Reserved.
# Qualcomm Confidential and Proprietary
#
#-------------------------------------------------------------------------------
#
#  $prop: //source/qcom/qct/core/pkg/trustzone/dev/abhianan.DEVCFG_STRUCT_SUPPORT/trustzone_images/core/bsp/build/scripts/devcfg_builder.py#2 $
#  $DateTime: 2013/07/10 18:33:19 $
#  $Author: coresvc $
#  $Change: 4074731 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 09/25/12   aa      Added Support to parse structure template expressed in xml files
# 11/14/11   jay     Device Configuration builder implementation
#===============================================================================

import os
import subprocess
import string
import re
from SCons.Script import *

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
DEVCFG_HW_INFO_DESCRIPTOR = 'DEVCFG_HW_INFO_COLLECTOR'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('devcfgHwInfo_builder')

def generate(env):

    rootenv = env.get('IMAGE_ENV')
    
    #Add Builder to generate structure C prop in env
    devcfg_prop_auto_generate(env)
    #Add Builder to generate ASM code to generate section create rules.
    devcfg_section_auto_generate(env)
    
    return
    
#===============================================================================
# DEV Config prop build rules
#===============================================================================
def devcfg_prop_auto_generate(env):
    
    rootenv = env.get('IMAGE_ENV')
    
    # load builder into enviroment
    devcfg_prop_act = env.GetBuilderAction(devcfg_create_prop_info_builder)
    devcfg_prop_bld = env.Builder(action = devcfg_prop_act)
    env.Append(BUILDERS = {'DevcfgPropInfoBuilder' : devcfg_prop_bld})
    
    return

#===============================================================================
# DEV Config Section ASM build rules
#===============================================================================
def devcfg_section_auto_generate(env):
    
    rootenv = env.get('IMAGE_ENV')
    
    # load builder into enviroment
    devcfg_sec_act = env.GetBuilderAction(devcfg_create_section_info_builder)
    devcfg_sec_bld = env.Builder(action = devcfg_sec_act)
    env.Append(BUILDERS = {'DevcfgSectionInfoBuilder' : devcfg_sec_bld})
    
    return

#===============================================================================
# Read the XML file to parse struc_template into a buffer for python xml parser
#===============================================================================    
def devcfg_parse_chip_info_header_file(ddi_file):
    buffer = ""
    # open the xml file for reading structure template
    input_ddi_file = open(ddi_file, 'r')
    ChipInfoDict = {}
    while(1):
       line = input_ddi_file.readline()
       line = line.strip()
       if(re.search( r'DALCHIPINFO_ID', line)):    
         tuple =  re.findall(r'[\w]+', str(line))
         #ChipInfoDict.setdefault(str(tuple[0]), [  ]).append(str(tuple[1]))
         ChipInfoDict.update({str(tuple[0]) : str(tuple[1])})          
       if(re.search( r'DalChipInfoIdType', line)):
         break
    
    # Close the file
    input_ddi_file.close()
    return ChipInfoDict

#===============================================================================
# Read the XML file to parse struc_template into a buffer for python xml parser
#===============================================================================    
def devcfg_parse_platform_info_header_file(ddi_file):
    buffer = ""
    # open the xml file for reading structure template
    input_ddi_file = open(ddi_file, 'r')
    PlatformInfoDict = {}
    while(1):
       line = input_ddi_file.readline()
       line = line.strip()
       if(re.search( r'DALPLATFORMINFO_TYPE', line)):      
         tuple =  re.findall(r'[\w]+', str(line))
         #PlatformInfoDict.setdefault(str(tuple[0]), [  ]).append(str(tuple[1]))
         PlatformInfoDict.update({str(tuple[0]) : str(tuple[1])})         
       if(re.search( r'DalPlatformInfoPlatformType', line)):
         break
    
    # Close the file
    input_ddi_file.close()
    return PlatformInfoDict

#===============================================================================
# Read the XML file to parse struc_template into a buffer for python xml parser
#===============================================================================    
def devcfg_create_seg_c_file(prop_info_file, ResultantHWInfoDict, TCSRChipInfoDDIDict, tcsr_soc_hw_ver_addr):
   
   #open the c file to write
   SEG_CFILE = open(prop_info_file, 'w')
   SEG_CFILE.write("#include \"DALSysInt.h\" \n\n")
   SEG_CFILE.write("#define DEVCFG_TCSR_SOC_HW_ADDR\t" + tcsr_soc_hw_ver_addr + "\n\n")
   #SEG_CFILE.write("uint32 dwSocLength = " + str(len(ResultantHWInfoDict))+";\n")
   NumofPlatformInSoc = {}
   # first pass to print all platform structures
   NullPlatDict = {}
   for element in  ResultantHWInfoDict.keys() :
      PlatformInfoDict = (ResultantHWInfoDict[element])['PLATFORM']
      for platform_type in  PlatformInfoDict.keys() :
         SEG_CFILE.write("extern unsigned int " + str(PlatformInfoDict[platform_type]) + ";\n")
   SEG_CFILE.write("\n")  
   for element in  ResultantHWInfoDict.keys() :
      PlatformInfoDict = (ResultantHWInfoDict[element])['PLATFORM']
      platform_size = len(PlatformInfoDict)
      NumofPlatformInSoc.update({element : platform_size})
      if(platform_size > 0) :
         SEG_CFILE.write("DEVCFG_PLATFORM_SEG_INFO devcfg_target_platform_info_" + 
                          str(element) +"[" + str(platform_size) + "] =\n{\n" )      
         for platform_type in  PlatformInfoDict.keys() :
            SEG_CFILE.write("\t{ " + str(platform_type) + ", (DALProps *)&" + 
                            str(PlatformInfoDict[platform_type]) + "},\n")
         SEG_CFILE.write("};\n")
         del(ResultantHWInfoDict[element])['PLATFORM']
      else:
         del(ResultantHWInfoDict[element])['PLATFORM']
         NullPlatDict[element] = 'NULL'         
      
   # second pass to print all chipinfo and final structure
   for element in  ResultantHWInfoDict.keys() :
      ChipInfoDict = ResultantHWInfoDict[element]
      for chip_type in  ChipInfoDict.keys() :
         SEG_CFILE.write("extern unsigned int " + str(ChipInfoDict[chip_type]) + ";\n")
   SEG_CFILE.write("\n")
   
   SEG_CFILE.write("DEVCFG_TARGET_INFO devcfg_target_soc_info" + "[ ] =\n{\n" )
   for element in ResultantHWInfoDict.keys() :
      ChipInfoDict = ResultantHWInfoDict[element]
      for chip_type in ChipInfoDict.keys() :
         if TCSRChipInfoDDIDict.has_key(chip_type):
            tcsr_soc_id = TCSRChipInfoDDIDict[chip_type]
         else :
            tcsr_soc_id = ['0xFFFFFFFF']
         for id in tcsr_soc_id :
            if NullPlatDict.has_key(element):
               SEG_CFILE.write("\t{ " + str(id) + ", (DALProps *)&" + str(ChipInfoDict[chip_type]) + 
                            ", " + NullPlatDict[element] + ", " + 
                            str(NumofPlatformInSoc[element])+"},\n")
            else:
               SEG_CFILE.write("\t{ " + str(id) + ", (DALProps *)&" + str(ChipInfoDict[chip_type]) + 
                            ", " + "devcfg_target_platform_info_" + element + ", " + 
                            str(NumofPlatformInSoc[element])+"},\n")
   SEG_CFILE.write("\t{0, NULL, NULL, 0}\n")   
   SEG_CFILE.write("};\n")                 
   SEG_CFILE.close()
   return

   
#===============================================================================
# Read the XML file to parse struc_template into a buffer for python xml parser
#===============================================================================    
def devcfg_find_platform_for_soc(chipid_tag, LookupHWInfoDict ):
   TempPlatformInfoDic = {} 
   for element in  LookupHWInfoDict.keys() :
      if 'DALPLATFORMINFO' in element:
         for platform in LookupHWInfoDict[element]:
            if chipid_tag in platform : #symbols
               TempPlatformInfoDic.update({element : platform})
               LookupHWInfoDict[element].remove(platform)
               if(len(LookupHWInfoDict[element]) == 0):
                  del LookupHWInfoDict[element]
   
   return (TempPlatformInfoDic, LookupHWInfoDict)
 
#===============================================================================
# Read the XML file to parse struc_template into a buffer for python xml parser
#===============================================================================    
def devcfg_create_hw_info(ResultantHWInfoDict, LookupHWInfoDict ):
   for element in  LookupHWInfoDict.keys() :
      if 'DALCHIPINFO' in element :
         #chipid_tag =  re.findall(r'[\d|\w]+', element)
         chipid_tag =  element.split('_')[1]
         TempHWInfoDic = {} 
         TempHWInfoDic.update({element : LookupHWInfoDict[element]})
         del(LookupHWInfoDict[element])
         TempPlatformInfoDic, LookupHWInfoDict = devcfg_find_platform_for_soc(chipid_tag, LookupHWInfoDict) 
         TempHWInfoDic.update({'PLATFORM' : TempPlatformInfoDic})
         ResultantHWInfoDict.update({chipid_tag : TempHWInfoDic})
   
   # platform without soc
   if(len(LookupHWInfoDict) > 0):
      for element in  LookupHWInfoDict.keys() :
         TempHWInfoDic = {'0xDEADDEAD' : 'NULL'} 
         #chipid_tag =  re.findall(r'[\d|\w]+', LookupHWInfoDict[element])
         chipid_tag =  LookupHWInfoDict[element].split('_')
         TempPlatformInfoDic, LookupHWInfoDict = devcfg_find_platform_for_soc(chipid_tag[0], LookupHWInfoDict) 
         TempHWInfoDic.update({'PLATFORM' : TempPlatformInfoDic})
         ResultantHWInfoDict.update({chipid_tag[0] : TempHWInfoDic})
         
   return ResultantHWInfoDict
   
#===============================================================================
# Method to contruct the seginfo dictionary. It should follow the same
# convention for names as in devcfg_lcs_builder.py. This can not be 
# retrieved for devcfg_lcs_builder.py as that builder is for devcfg
# image and not the primary image, which is the case here.
#===============================================================================
def create_seginfo_dict(env):
   devcfg_xml_tag_list = env.GetDevcfgRelevantTags([], {})
   env_segment_info = {}
   for elem in devcfg_xml_tag_list:
      # __8026_PLATFORM_CDP_devcfg_data_addr_base__
      segment_base_addr = '__' + elem.replace('xml', 'devcfg_data_addr_base__')
      env_segment_info[elem] = segment_base_addr
      
   return env_segment_info      
   
#===============================================================================
# Builder def to generate a prop file.
#===============================================================================     
def devcfg_create_prop_info_builder(target, source, env):
   #DDIChipInfo & DDIPlatfromInfo file as source and 
   #Prop Info file to be generated as target for builder.
   chip_info_file = str(source[0])
   platform_info_file = str(source[1])
   prop_info_file = str(target[0])
   
   #parse the ddi files to get chip and platform informations.
   DDIChipInfoDict = devcfg_parse_chip_info_header_file(chip_info_file)
   DDIPlatformInfoDict = devcfg_parse_platform_info_header_file(platform_info_file)
   #import pdb;pdb.set_trace() 
   rootenv = env.get('IMAGE_ENV')
   
   # Create the seginfo addr dictionary
   HWInfoDict = create_seginfo_dict(rootenv)
   
   ChipInfoDict = {}
   PlatformInfoDict = {}
   DDIMapHWInfoDict = {}
   TCSRChipInfoDDIDict = {}
   ResultantHWInfoDict = {}
   
   # Read DEVCFG_SOC_DICT env variable, defined in tbc_core_devcfg.py
   soc_dicts = env.get('DEVCFG_SOC_DICT')
   tcsr_soc_hw_ver_addr = env.get('DEVCFG_TCSR_SOC_HW_ADDR')
   
   # Segrerate the ChipInfo and Platform Info in dedicated dictionary
   for element in  HWInfoDict.keys() :
      if ('PLATFORM' in element) or ('platform' in element):
         PlatformInfoDict.update({element : HWInfoDict[element]})
      else:
         ChipInfoDict.update({element : HWInfoDict[element]})
   
   # Map the DDI Chip Type Enum to symbol info in ChipInfoDict   
   for element in  ChipInfoDict.keys() :
      ddi_chip_element = 'DALCHIPINFO_' + element.split('_')[0]
      DDIMapHWInfoDict.update({ddi_chip_element : ChipInfoDict[element]})

            
   # Map the DDI Chip Type Enum to soc info as defined in tcsr_soc_hw_info register(ipcat),
   # passed from target levl image_cfg.xml file   
   for element in  soc_dicts.keys() :
      ddi_chip_element = 'DALCHIPINFO_' + element.split('_')[0]
      TCSRChipInfoDDIDict.update({ddi_chip_element : soc_dicts[element]})
   
   # Map the DDI Platform Type Enum to symbol info in PlatformInfoDict   
   for element in  PlatformInfoDict.keys() :
      platform = element.split('_')
      if 'MTP' in platform :
         platform_string = platform[2] + '_' + platform[3]
      else :
         platform_string = platform[2]
      for ddi_platform_element in DDIPlatformInfoDict.keys():
         if platform_string in ddi_platform_element :
            #DDIMapHWInfoDict.update({ddi_platform_element : PlatformInfoDict[element]})
            DDIMapHWInfoDict.setdefault(ddi_platform_element,[]).append(PlatformInfoDict[element])
            break;

   ResultantHWInfoDict = devcfg_create_hw_info(ResultantHWInfoDict, DDIMapHWInfoDict) 
   devcfg_create_seg_c_file(prop_info_file, ResultantHWInfoDict, TCSRChipInfoDDIDict, tcsr_soc_hw_ver_addr)
   
   return 

#===============================================================================
# Builder def to generate section asm files.
#===============================================================================  
def devcfg_create_section_info_builder(target, source, env):
   rootenv = env.get('IMAGE_ENV')
   used_xml_tags = rootenv.get('DEVCFG_ENV_USED_XML_TAGS')
   #all_tags = env.get('DEVCFG_XML_TAGS')
   #import pdb; pdb.set_trace()
   for pad_file in target:
       pad_file_name = env.RealPath(str(pad_file))
       basename = os.path.basename(pad_file_name)
       basename = basename.split(".")[0]
       tag = basename.replace('devcfg_pad_','')
       xml_tag = tag + '_xml'
       #open the ASM file to write
       SEG_ASMFILE = open(pad_file_name, 'w')
       if xml_tag in used_xml_tags:
           SEG_ASMFILE.write("#include <customer.h> \n")
           SEG_ASMFILE.write("/**\n\t@note: This is being marked as write only because the DATA image\n")
           SEG_ASMFILE.write("\tshould not be technically allowed.\n")
           SEG_ASMFILE.write("*/ \n\n")
           line = ".section ."+tag+ "_DEVCFG_DATA,\"aw\", @progbits\n"
           SEG_ASMFILE.write(line)
           SEG_ASMFILE.write(".align 8\n")
           line = ".globl __"+tag+"_devcfg_data_addr_base__\n"
           SEG_ASMFILE.write(line)
           line = ".type  __"+tag+"_devcfg_data_addr_base__, @function\n\n"
           SEG_ASMFILE.write(line)
           line = "__"+tag+"_devcfg_data_addr_base__:\n"
           SEG_ASMFILE.write(line)
           SEG_ASMFILE.write("\t.word 0\n")
           line = "\t.size __"+tag+"_devcfg_data_addr_base__, .-__"+tag+"_devcfg_data_addr_base__\n"
           SEG_ASMFILE.write(line)
           if('PLATFORM' in xml_tag) :
               SEG_ASMFILE.write("\t.space (DEVCFG_PLATFORM_DATA_SEG_SIZE-0x4), 0\n")
           else:
               SEG_ASMFILE.write("\t.space (DEVCFG_DATA_SEG_SIZE-0x4), 0\n")
           SEG_ASMFILE.write("\n")                 
       SEG_ASMFILE.close()
   return
