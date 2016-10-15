#===============================================================================
#
# AVS Cosim configuration for simulation
#
# GENERAL DESCRIPTION
#    AVS Cosim configuration for simulation
#     
#
# Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/build/cosim_config.py#18 $
#  $DateTime: 2013/12/23 10:31:42 $
#  $Author: coresvc $
#  $Change: 5011360 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 09/21/12   kk      AVS Cosim configuration
#
#===============================================================================

import re
import string
import os
import sys
def generate_cosim_config(env):
    ADSP_AFE_COSIM = "adsptest/Audio/drivers/afe/AFECosim8660/scfAudioIfCosim"
    print "----"+env.get('PLATFORM')
    if env.get('PLATFORM') in ["Windows_NT","windows","win32","cygwin"]:
        if env.get('CHECKSIMBOOT') == True:
        
            PLATFORM_DIR="cygwin"    
            COSIM_EXT="w32"
            LIB_EXT="dll"
        else:
		# <-- HACK! --> Force Linux path always. PW cannot build in Linux. So have to use
        #windows builds to test in Linux. Clean-up when Linux builds work in PW.
            PLATFORM_DIR="lnx64"    
            COSIM_EXT="lnx64"
            LIB_EXT="so"
    elif env.get('PLATFORM') == 'linux':
        PLATFORM_DIR="lnx64"    
        COSIM_EXT="lnx64"
        LIB_EXT="so"
    else:
        print "Un supported platform"
        sys.exit(0)
    ADSP_AFE_COSIM_WITH_EXT=ADSP_AFE_COSIM+"_"+COSIM_EXT+"."+LIB_EXT



    
    DMAIOFILE = "afe/drivers/cosims/8974/dma_io.cfg"
    AVTIOFILE = "afe/drivers/cosims/8974/avtimerCosim.cfg"
    BTFMIOFILE = "afe/drivers/cosims/8974/BTFM.cfg"
    DMCOSIMFILE = "afe/drivers/cosims/8974/scfdm_cosim_"+COSIM_EXT+"."+LIB_EXT
    DMACFGFILE = "afe/drivers/cosims/8974/scfAudioIfCosim_"+COSIM_EXT+"."+LIB_EXT
    MIDICOSIMFILE = "afe/drivers/cosims/8974/scfmidiCosim_"+COSIM_EXT+"."+LIB_EXT
    AVTCOSIMFILE = "afe/drivers/cosims/8974/scfavtimerCosim_"+COSIM_EXT+"."+LIB_EXT
    RSCOSIMFILE = "afe/drivers/cosims/8974/resamplerCosim_"+COSIM_EXT+"."+LIB_EXT
    VFRCOSIMFILE = "afe/drivers/cosims/8974/vfrGenerator_"+COSIM_EXT+"."+LIB_EXT
    VIDCOSIMFILE = "afe/drivers/cosims/8974/vid_hw_workaround_cosim_"+COSIM_EXT+"."+LIB_EXT
    BTFMCOSIMFILE = "afe/drivers/cosims/8974/scfbtfmCosim_"+COSIM_EXT+"."+LIB_EXT
    DALCHIPINFOCOSIMFILE = "afe/drivers/cosims/common/dalchip_Cosim_"+COSIM_EXT+"."+LIB_EXT

    BTFMCOSIMPARAMS = " -ctl_reg 0xFB206000\
                    -l2vicbaseaddr 0xfe290000\
                    -BT_SPKR_IntNum 10\
                    -BT_MIC_IntNum 17\
                    -MailBox_IntNum 15\
                    -FM_IntNum 16\
                    -Cfg_FileName "+BTFMIOFILE+"\
                    -lpass_cc_baseaddr 0xfe000000"
    DMCOSIMPARAMS =  " 0xFE04A000--0xFE04A014-10\
                     -l2vicbaseaddr 0xfe290000\
                     -l2vicintrnum 23"
    DMACFGPARAMS = " -aifstartaddr 0xFE0C0000\
                   -aifendaddr 0x0xFE0DFFF0\
                   -aifresetaddr 0xFE0DFFF4\
                   -buslatency 30\
                   -l2vicbaseaddr 0xfe290000\
                   -l2vicintrnum 19\
                   -aifcfg "+DMAIOFILE
    MIDICFGPARAMS = " -midistartaddr 0xFE070000\
                    -midiendaddr 0xFE07FFFF\
                    -buslatency 30\
                    -l2vicbaseaddr 0xfe290000\
                    -l2vicintrnum 22"
    AVTCOSIMPARAMS = " -l2vicbaseaddr 0xfe290000\
                     -l2vicintrnum 50\
                     -avtstartaddr 0xFE053000\
                     -avtendaddr 0xFE0533FF\
                     -buslatency 30\
                     -avtimercfg "+AVTIOFILE
    RSCFGPARAMS = " -rsstartaddr 0xFE058000\
                  -rsendaddr 0xFE05BFFF\
                  -buslatency 30\
                  -l2vicbaseaddr 0xfe290000\
                  -l2vicintrnum 53"
    
    DALCHIPINFOPARAMS = " "+env.get('BUILD_ASIC')
    
    if env.get('CHIPSET') == "msm8974" or env.get('CHIPSET') == "msm8x26" or env.get('CHIPSET') == "msm8x10":
		VFR1CFGPARAMS = " -trigger_lpaif TRUE\
                      -init_delay_us 0\
                   -vfr_period_us 20000\
                  -lpaif_baseaddr 0xFE0C0000\
                  -l2vicbaseaddr 0xfe290000\
                  -l2vicintrnum 36\
                  -lpass_cc_baseaddr 0x0"
    else:
		VFR1CFGPARAMS = " -trigger_lpaif TRUE\
                  -init_delay_us 0\
                  -vfr_period_us 20000\
                  -lpaif_baseaddr 0xFE0C0000\
                  -l2vicbaseaddr 0xfe290000\
                  -l2vicintrnum 51\
                  -lpass_cc_baseaddr 0x0"

    VFR2CFGPARAMS = " -trigger_lpaif FALSE\
                  -init_delay_us 10000\
                  -vfr_period_us 20000\
                  -lpaif_baseaddr 0x0\
                  -l2vicbaseaddr 0xfe290000\
                  -l2vicintrnum 40\
                  -lpass_cc_baseaddr 0x0"

	# Hardcoding the tools version for the qtimer cosim to 5.0.13
    if env.get('Q6VERSION')=="v5":
        if env.get('PLATFORM') in ["Windows_NT","windows","win32","cygwin"]:
            if env.get('CHECKSIMBOOT') == True:
                
                TIMERCFGFILE = "C:/Qualcomm/HEXAGON_Tools/5.0.13/qc/lib/iss/qtimer."+LIB_EXT
                L2VICCFGFILE = "C:/Qualcomm/HEXAGON_Tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/l2vic."+LIB_EXT
                TIMERCFGPARAMS = " --csr_base=0xfe280000 --irq_p=2,3 --freq=19200000 --cnttid=17"
                L2VICCFGPARAMS = " 32 0xfe290000"
            else:
                # <-- HACK! --> Force Linux path always. PW cannot build in Linux. So have to use
                #windows builds to test in Linux. Clean-up when Linux builds work in PW.
                TIMERCFGFILE = "/pkg/qct/software/hexagon/releases/tools/5.0.13/qc/lib/iss/qtimer."+LIB_EXT
                L2VICCFGFILE = "/pkg/qct/software/hexagon/releases/tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/l2vic."+LIB_EXT
                TIMERCFGPARAMS = " --csr_base=0xfe280000 --irq_p=2,3 --freq=19200000 --cnttid=17"
                L2VICCFGPARAMS = " 32 0xfe290000"
        elif env.get('PLATFORM') == 'linux':	
            TIMERCFGFILE = "/pkg/qct/software/hexagon/releases/tools/5.0.13/qc/lib/iss/qtimer."+LIB_EXT
            L2VICCFGFILE = "/pkg/qct/software/hexagon/releases/tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/l2vic."+LIB_EXT
            TIMERCFGPARAMS = " --csr_base=0xfe280000 --irq_p=2,3 --freq=19200000 --cnttid=17"
            L2VICCFGPARAMS = " 32 0xfe290000"
        else:
            print "Unsupported flavor(supports only on windows and Linux)"
            sys.exit(0)
    else:
        TIMERCFGFILE = "/pkg/qct/software/hexagon/releases/tools/5.0.13/qc/lib/iss/qtimer."+LIB_EXT
        L2VICCFGFILE = "/pkg/qct/software/hexagon/releases/tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/l2vic."+LIB_EXT
        TIMERCFGPARAMS = " --csr_base=0xfe280000 --irq_p=2,3 --freq=19200000 --cnttid=17"
        L2VICCFGPARAMS = " 32 0xfe290000"               

            
    Q6SS_OBJ_FILE_PATH = "../"
    Q6SS_OBJ_FILE_TEMP = Q6SS_OBJ_FILE_PATH+"q6ss1.cfg"
    Q6SS_OBJ_FILE = Q6SS_OBJ_FILE_PATH+"q6ss.cfg"
    ADSP_TEST_COSIMOPTS_FILE = "../adsp_test_cosimopts.cfg"
    ADSP_TEST_INTERNAL_BT_FM_COSIMOPTS_FILE = "../adsp_test_internal_bt_fm_cosimopts.cfg"
    DMAIOFILE_PATH="../"+DMAIOFILE
    BTFMIOFILE_PATH="../"+BTFMIOFILE

    if env.get('PLATFORM') in ["Windows_NT","windows","win32","cygwin"]:
        # <-- HACK! --> Force Linux path always. PW cannot build in Linux. So have to use
        #windows builds to test in Linux. Clean-up when Linux builds work in PW.
        if env.get('CHECKSIMBOOT') == True:    
            TIMERCFGFILE = TIMERCFGFILE.replace("/","\\")
            DMACFGFILE = DMACFGFILE.replace("/","\\")
            MIDICOSIMFILE = MIDICOSIMFILE.replace("/","\\")
            DMCOSIMFILE = DMCOSIMFILE.replace("/","\\")
            L2VICCFGFILE = L2VICCFGFILE.replace("/","\\")
            AVTCOSIMFILE = AVTCOSIMFILE.replace("/","\\")
            VIDCOSIMFILE = VIDCOSIMFILE.replace("/","\\")
            RSCOSIMFILE = RSCOSIMFILE.replace("/","\\")
            VFRCOSIMFILE = VFRCOSIMFILE.replace("/","\\")
            BTFMCOSIMFILE = BTFMCOSIMFILE.replace("/","\\")
            Q6SS_OBJ_FILE = Q6SS_OBJ_FILE.replace("/","\\")
            Q6SS_OBJ_FILE_PATH = Q6SS_OBJ_FILE_PATH.replace("/","\\")
            ADSP_TEST_COSIMOPTS_FILE = ADSP_TEST_COSIMOPTS_FILE.replace("/","\\")
            ADSP_TEST_INTERNAL_BT_FM_COSIMOPTS_FILE = ADSP_TEST_INTERNAL_BT_FM_COSIMOPTS_FILE.replace("/","\\")
            DMAIOFILE_PATH = DMAIOFILE_PATH.replace("/","\\")
            BTFMIOFILE_PATH = BTFMIOFILE_PATH.replace("/","\\")
         

                
    
    # generate q6ss.cfg
    if env.get('PLATFORM') in ["Windows_NT","windows","win32","cygwin"] :
        if env.get('CHECKSIMBOOT') == True:
            fo_q6ss = open(Q6SS_OBJ_FILE,'wb')
            fo_q6ss.write(TIMERCFGFILE+TIMERCFGPARAMS+"\n")
            fo_q6ss.write(L2VICCFGFILE+L2VICCFGPARAMS+"\n")
            fo_q6ss.close()
        else: 
            fo_q6ss = open(Q6SS_OBJ_FILE,'wb')
            fo_q6ss.write(TIMERCFGFILE+TIMERCFGPARAMS+"\n")
            fo_q6ss.write(L2VICCFGFILE+L2VICCFGPARAMS+"\n")
            fo_q6ss.write(VIDCOSIMFILE+"\n")
            fo_q6ss.write(MIDICOSIMFILE+MIDICFGPARAMS+"\n")
            fo_q6ss.write(DMACFGFILE+DMACFGPARAMS+"\n")
            fo_q6ss.write(DMCOSIMFILE+DMCOSIMPARAMS+"\n")
            fo_q6ss.write(AVTCOSIMFILE+AVTCOSIMPARAMS+"\n")
            fo_q6ss.write(RSCOSIMFILE+RSCFGPARAMS+"\n")
            fo_q6ss.write(VFRCOSIMFILE+VFR1CFGPARAMS+"\n")
            fo_q6ss.write(VFRCOSIMFILE+VFR2CFGPARAMS+"\n")
            fo_q6ss.write(BTFMCOSIMFILE+BTFMCOSIMPARAMS+"\n")
            fo_q6ss.write(DALCHIPINFOCOSIMFILE+DALCHIPINFOPARAMS+"\n")
            fo_q6ss.close()
    elif env.get('PLATFORM') == 'linux':
        fo_q6ss = open(Q6SS_OBJ_FILE,'wb')
        fo_q6ss.write(TIMERCFGFILE+TIMERCFGPARAMS+"\n")
        fo_q6ss.write(L2VICCFGFILE+L2VICCFGPARAMS+"\n")
        fo_q6ss.write(VIDCOSIMFILE+"\n")
        fo_q6ss.write(MIDICOSIMFILE+MIDICFGPARAMS+"\n")
        fo_q6ss.write(DMACFGFILE+DMACFGPARAMS+"\n")
        fo_q6ss.write(DMCOSIMFILE+DMCOSIMPARAMS+"\n")
        fo_q6ss.write(AVTCOSIMFILE+AVTCOSIMPARAMS+"\n")
        fo_q6ss.write(RSCOSIMFILE+RSCFGPARAMS+"\n")
        fo_q6ss.write(VFRCOSIMFILE+VFR1CFGPARAMS+"\n")
        fo_q6ss.write(VFRCOSIMFILE+VFR2CFGPARAMS+"\n")
        fo_q6ss.write(BTFMCOSIMFILE+BTFMCOSIMPARAMS+"\n")
        fo_q6ss.write(DALCHIPINFOCOSIMFILE+DALCHIPINFOPARAMS+"\n")
        fo_q6ss.close()
    else:
        print "Unsupported flavor"
        sys.exit(0)
     
    

    """
        forg_q6ss = open(Q6SS_OBJ_FILE,'w')
        with open(Q6SS_OBJ_FILE_TEMP,'r') as f:
            for line in f:
                line_mod = re.sub('\r', '', line)
                forg_q6ss.write(line_mod)

        forg_q6ss.close()
        
    """


    #generate adsp_test_cosimopts.cfg
    fo_opts = open(ADSP_TEST_COSIMOPTS_FILE,'wb')
    fo_opts.write(DMACFGFILE+DMACFGPARAMS+"\n")
    fo_opts.close()

    #generate adsp_test_internal_bt_fm_cosimopts.cfg
    fo_btfm_opts = open(ADSP_TEST_INTERNAL_BT_FM_COSIMOPTS_FILE,'wb')
    fo_btfm_opts.write(BTFMCOSIMFILE+BTFMCOSIMPARAMS+"\n")
    fo_btfm_opts.close()




    #Making DMA IO File Writeable, for cosim reset option...
    os.chmod(DMAIOFILE_PATH,0777)

    #Making INTERNAL BT FM File Writeable, for cosim reset option...
    os.chmod(BTFMIOFILE_PATH,0777)

    return




