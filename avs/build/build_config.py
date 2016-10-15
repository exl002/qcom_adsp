#===============================================================================
#
# AVS build configuration
#
# GENERAL DESCRIPTION
#    AVS build configuration for different chipsets & AVSTest must be specified here. 
#     
#    set in the AVS image scons script file.
#       1) A normal library with symbols - no need specify it
#       2) A stub library  USES_<LIB NAME IN CAPITAL CASE>_STUB
#       3) A stripped library USES_<LIB NAME IN CAPITAL CASE>_STRIP
#
# Copyright (c) 2009-2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/build/build_config.py#52 $
#  $DateTime: 2014/02/25 12:23:02 $
#  $Author: coresvc $
#  $Change: 5340958 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 09/21/12   kk      Build configuration
# 04/18/13   nr      Stub out ultrasound stream manager for 8x10 and 9x25
#
#===============================================================================


def chipset_specific_build_config(env):
    env.Replace(USES_DTS =False)  
    env.Replace(USES_MIDI =False)  

    if env.get('AVS_TEST') == True:
        env.Append(CFLAGS='-DADSP_TEST_ENTRY -DSIM -DSIM_DEFINED')
        env.Replace(USES_DTS =True)
        env.Replace(USES_MIDI =True)        
        import re
        temp=env.get('CC_DFLAGS')
        yy = re.sub('-DASSERT=ASSERT_FATAL','',temp)
        env.Replace(CC_DFLAGS=yy)
        
        #Stubbing out DTS for SIM builds
        if env.get('USES_DTS') == True:
            env.Replace(USES_DTSMODULE_STUB = True) 
        #Stubbing out srs:ss3d
        env.Replace(USES_SRSSS3DMODULE_STUB = True)
        #Stubbing out dts:hpx
        env.Replace(USES_DTS_HPX_MODULE_STUB = True)


        env.Replace(USES_DOLBYPLUS_ENCODER_STUB = True) 

    else:

        if env.get('CHIPSET') == "msm8974" or env.get('CHIPSET') == "msm8x26" or env.get('CHIPSET') == "msm8x10" or env.get('CHIPSET') == "apq8084" or env.get('CHIPSET') == "msm8x62" or env.get('CHIPSET') == "msm8962" :
            env.Replace(USES_DTS =True)
            env.Replace(USES_MIDI =True)
            # NOTE: Please ensure that every newly added library should be properly stripped
            # for those symbols which are proprietary.

            # Currently DTS HPX will be enabled on 8974 CHIPSET after approval.
            # if env.get('CHIPSET') != "msm8974" or env.get('BUILD_ASIC') == "8x26":
            env.Replace(USES_DTS_HPX_MODULE_STUB = True)

            if env.get('COSIM_PROFILING_TEST') != True: # disabling library stripping for cosim based testfwk used for profiling.
                #Add AVS Voice Library Types for this image
                env.Replace(USES_QUARTET_STRIP = True)
                env.Replace(USES_SAMPLE_SLIP_STRIP = True)
                env.Replace(USES_RVE_STRIP = True)
                #env.Replace(USES_RSBY2_STRIP = True)
                #env.Replace(USES_IIR_FILTER_STRIP = True)
                #env.Replace(USES_FLUENCE_VOICEPLUS_STRIP = True)
                #env.Replace(USES_FIR_FILTER_STRIP = True)
                env.Replace(USES_ECHO_CANCELLER_STRIP = True)
                env.Replace(USES_DTMF_DETECT_STRIP = True)
                #env.Replace(USES_CTM_STRIP = True)
                #env.Replace(USES_CROSS_FADE_STRIP = True)
                #env.Replace(USES_CNG_STRIP = True)
                env.Replace(USES_AVC_TX_STRIP = True)
                env.Replace(USES_AVC_RX_STRIP = True)
                #env.Replace(USES_VFE_FILTERS_STRIP = True)
                #env.Replace(USES_TTY_STRIP = True)
                env.Replace(USES_TIME_WARPING_STRIP = True)
                #env.Replace(USES_VOCODER_AMRWB_STRIP = True)
                #env.Replace(USES_VOCODER_AMRNB_STRIP = True)
                #env.Replace(USES_VOCODER_FR_STRIP = True)
                #env.Replace(USES_VOCODER_FOURGV_STRIP = True)
                #env.Replace(USES_VOCODER_EVRC_STRIP = True)
                #env.Replace(USES_VOCODER_EFR_STRIP = True)
                env.Replace(USES_VOCODER_EAMR_STRIP = True)
                env.Replace(USES_SLOW_TALK_STRIP = True)
                #env.Replace(USES_VOCODER_CAPI_STRIP = True)
                #env.Replace(USES_VOLUME_CONTROL_STRIP = True)
                #env.Replace(USES_VOICE_RESAMPLER_STRIP = True)
                #env.Replace(USES_VOCODER_V13K_STRIP = True)
                #env.Replace(USES_VOCODER_HR_STRIP = True)
                #env.Replace(USES_VOCODER_G729AB_STRIP = True)
                #env.Replace(USES_VOCODER_G711_STRIP = True)
                #env.Replace(USES_WIDE_VOICE_STRIP = True)

                # Add AVS Afe Library Types for this image
                #env.Replace(USES_ADAP_ANC_STRIP =True)
                #env.Replace(USES_FFSPKRPROT_STRIP=True)
                #env.Replace(USES_DTMFGENERATION_STRIP=True)
                #env.Replace(USES_REMOVECLICKLIB_STRIP=True)
                #env.Replace(USES_AFERIVADRIVER_STRIP=True)
                #env.Replace(USES_AFESLIMBUSDRIVER_STRIP=True)

                #Add AVS Audio Library Types for this image
                env.Replace(USES_AACDECODERMODULE_STRIP=True)
                #env.Replace(USES_ETSIBITBUFFLIB_STRIP=True)
                #env.Replace(USES_ETSIFFRLIB_STRIP=True)
                #env.Replace(USES_ETSIOPLIB_STRIP=True)
                env.Replace(USES_ADPCMDECODERMODULE_STRIP=True)
                env.Replace(USES_ADPCMENCODERMODULE_STRIP=True)
                #env.Replace(USES_AMRBITBUFFERLIB_STRIP=True)
                #env.Replace(USES_AMRCORELIB_STRIP=True)
                #env.Replace(USES_AMRMATHLIB_STRIP=True)
                #env.Replace(USES_ETSIAMROPLIB_STRIP=True)
                #env.Replace(USES_ETSIAMRWBPLUSDECMODULE_STRIP=True)
                env.Replace(USES_MP3DECODERMODULE_STRIP=True)
                #env.Replace(USES_SBCENCODERMODULE_STRIP=True)
                #env.Replace(USES_WMAPRODECODERMODULE_STRIP=True)
                env.Replace(USES_WMASTDDECODERMODULE_STRIP=True)
                env.Replace(USES_WMASTDV8ENCODERMODULE_STUB=True)
                env.Replace(USES_DOLBYPLUS_ENCODER_STUB = True)
                env.Replace(USES_SRSSS3DMODULE_STUB = True)

            if env.get('USES_DTS') == True:
                env.Replace(USES_DTSMODULE_STUB = True)

            if env.get('USES_MIDI') == True:
                env.Replace(USES_MIDICOREMODULE_STRIP=True)

            if env.get('CHIPSET') == "msm8x10":
                if env.get('DINO512MB'):
                    print "Dino 512MB Memory Feature Set is Selected"
                    env.Replace(USES_ULTRASOUND_STREAM_MGR_STUB=True)
                    env.Replace(USES_AFESLIMBUSDRIVER_STUB = True)
                    env.Replace(USES_ADAP_ANC_STUB = True)
                    env.Replace(USES_VOICE_WAKEUP_STUB = True)
                    env.Replace(USES_FBSPKRPROT_STUB = True)
                    env.Replace(USES_AFESWMAD_STUB = True)
                    env.Replace(USES_LISTEN_FEATURE_EXTRACTION_STUB = True)
                    env.Replace(USES_LISTEN_SIGNAL_PROCESSING_STUB = True)
                    #Add AVS Audio Library Types for this image
                    env.Replace(USES_SRSTRUMEDIAMODULE_STUB=True)
                    env.Replace(USES_HPMP2DECODERMODULE_STUB=True)
                    env.Replace(USES_AC3PACKETIZERMODULE_STUB=True)
                    env.Replace(USES_SPECTRUMANALYZERMODULE_STUB=True)
                    env.Replace(USES_ETSIAMRWBPLUSDECMODULE_STUB=True)
                    env.Replace(USES_FLACDECODERMODULE_STUB=True)
                    env.Replace(USES_HPMP3DECODERMODULE_STUB=True)
                    env.Replace(USES_ETSIEAACPLUSENCODERMODULE_STUB=True)
                    env.Replace(USES_VORBISDECODERMODULE_STUB = True)
                    env.Replace(USES_PASSTHRUFORMATTERMODULE_STUB=True)
                    env.Replace(USES_DTSHDPACKETIZERMODULE_STUB=True)
                    env.Replace(USES_DOLBYMOBILEMODULE_STUB = True)
                    env.Replace(USES_DOLBYPULSEDECODERMODULE_STRIP = True)
                    env.Replace(USES_GENERICDEPACKETIZERMODULE_STUB=True)
                    env.Replace(USES_MP3ENCODERMODULE_STUB=True)
                    env.Replace(USES_SBCENCODERMODULE_STUB=True)
                    env.Replace(USES_EAC3PACKETIZERMODULE_STUB=True)
                    env.Replace(USES_WMAPRODECODERMODULE_STUB=True)
                    env.Replace(USES_DOLBYSURROUNDMODULE_STUB=True)
                    env.Replace(USES_QENSEMBLEMODULE_STUB=True)
                    env.Replace(USES_CQCONCERTPLUSLIB_STUB=True)
                    env.Replace(USES_REVERBMODULE_STUB=True)
                    env.Replace(USES_BASSBOOSTMODULE_STUB=True)
                    env.Replace(USES_PBEMODULE_STUB=True)
                    env.Replace(USES_VIRTUALIZERMODULE_STUB=True)
                    env.Replace(USES_DS1LICENSEHANDLER_STUB=True)
                    env.Replace(USES_RS_DRIVER_STUB = True)
                    env.Replace(USES_DML_HAL_STUB = True)
                    env.Replace(USES_AFESPDIFDRIVER_STUB = True)
                    env.Replace(USES_FLUENCE_VOICEPLUS_STRIP = True)
                    env.Replace(USES_WIDE_VOICE_STUB = True)
                    #env.Replace(USES_QUARTET_STUB = True)
                    env.Replace(USES_AFECODECDRIVER_STUB = True)
                    #env.Replace(USES_SLOW_TALK_STUB = True)
                    env.Replace(USES_DOLBYPLUS_ENCODER_STUB = True) 
                    env.Replace(USES_VOCODER_G729AB_STUB = True)
                    env.Replace(USES_AFEHDMIINPUTDRIVER_STUB = True)
                    env.Replace(USES_AFEDMAMANAGER_STUB = True)
                    env.Replace(USES_SRSSS3DMODULE_STUB = True)
                elif env.get('DINO256MB'):
                    print "Dino 256MB Memory Feature Set is Selected"
                    env.Replace(USES_ULTRASOUND_STREAM_MGR_STUB=True)
                    env.Replace(USES_AFESLIMBUSDRIVER_STUB = True)
                    env.Replace(USES_ADAP_ANC_STUB = True)
                    env.Replace(USES_VOICE_WAKEUP_STUB = True)
                    env.Replace(USES_FBSPKRPROT_STUB = True)
                    env.Replace(USES_AFESWMAD_STUB = True)
                    env.Replace(USES_LISTEN_FEATURE_EXTRACTION_STUB = True)
                    env.Replace(USES_LISTEN_SIGNAL_PROCESSING_STUB = True)
                    #Add AVS Audio Library Types for this image
                    env.Replace(USES_SRSTRUMEDIAMODULE_STUB=True)
                    env.Replace(USES_HPMP2DECODERMODULE_STUB=True)
                    env.Replace(USES_AC3PACKETIZERMODULE_STUB=True)
                    env.Replace(USES_SPECTRUMANALYZERMODULE_STUB=True)
                    env.Replace(USES_ETSIAMRWBPLUSDECMODULE_STUB=True)
                    env.Replace(USES_FLACDECODERMODULE_STUB=True)
                    env.Replace(USES_HPMP3DECODERMODULE_STUB=True)
                    env.Replace(USES_ETSIEAACPLUSENCODERMODULE_STUB=True)
                    env.Replace(USES_VORBISDECODERMODULE_STUB = True)
                    env.Replace(USES_PASSTHRUFORMATTERMODULE_STUB=True)
                    env.Replace(USES_DTSHDPACKETIZERMODULE_STUB=True)
                    env.Replace(USES_DOLBYMOBILEMODULE_STUB = True)
                    env.Replace(USES_DOLBYPULSEDECODERMODULE_STUB = True)
                    env.Replace(USES_GENERICDEPACKETIZERMODULE_STUB=True)
                    env.Replace(USES_MP3ENCODERMODULE_STUB=True)
                    env.Replace(USES_SBCENCODERMODULE_STUB=True)
                    env.Replace(USES_EAC3PACKETIZERMODULE_STUB=True)
                    env.Replace(USES_WMAPRODECODERMODULE_STUB=True)
                    env.Replace(USES_DOLBYSURROUNDMODULE_STUB=True)
                    env.Replace(USES_QENSEMBLEMODULE_STUB=True)
                    env.Replace(USES_CQCONCERTPLUSLIB_STUB=True)
                    env.Replace(USES_REVERBMODULE_STUB=True)
                    env.Replace(USES_BASSBOOSTMODULE_STUB=True)
                    env.Replace(USES_PBEMODULE_STUB=True)
                    env.Replace(USES_VIRTUALIZERMODULE_STUB=True)
                    env.Replace(USES_DS1LICENSEHANDLER_STUB=True)
                    env.Replace(USES_RS_DRIVER_STUB = True)
                    env.Replace(USES_DML_HAL_STUB = True)
                    env.Replace(USES_AFESPDIFDRIVER_STUB = True)
                    env.Replace(USES_FLUENCE_VOICEPLUS_STRIP = True)
                    env.Replace(USES_WIDE_VOICE_STUB = True)
                    #env.Replace(USES_QUARTET_STUB = True)
                    env.Replace(USES_AFECODECDRIVER_STUB = True)
                    #env.Replace(USES_SLOW_TALK_STUB = True)
                    env.Replace(USES_DOLBYPLUS_ENCODER_STUB = True) 
                    env.Replace(USES_VOCODER_G729AB_STUB = True)
                    env.Replace(USES_AFEHDMIINPUTDRIVER_STUB = True)
                    env.Replace(USES_AFEDMAMANAGER_STUB = True)
                    env.Replace(USES_SRSSS3DMODULE_STUB = True)
                else:
                    print "Dino 1GB Memory Feature set is Selected"
                    env.Replace(USES_ULTRASOUND_STREAM_MGR_STUB=True)
                    env.Replace(USES_AFESLIMBUSDRIVER_STUB = True)
                    env.Replace(USES_ADAP_ANC_STUB = True)
                    env.Replace(USES_VOICE_WAKEUP_STUB = True)
                    env.Replace(USES_FBSPKRPROT_STUB = True)
                    env.Replace(USES_AFESWMAD_STUB = True)
                    env.Replace(USES_LISTEN_FEATURE_EXTRACTION_STUB = True)
                    env.Replace(USES_LISTEN_SIGNAL_PROCESSING_STUB = True)
                    #Add AVS Audio Library Types for this image
                    env.Replace(USES_HPMP2DECODERMODULE_STUB=True)
                    env.Replace(USES_AC3PACKETIZERMODULE_STUB=True)
                    #env.Replace(USES_SPECTRUMANALYZERMODULE_STUB=True)
                    #env.Replace(USES_ETSIAMRWBPLUSDECMODULE_STUB=True)
                    env.Replace(USES_FLACDECODERMODULE_STUB=True)
                    env.Replace(USES_HPMP3DECODERMODULE_STUB=True)
                    env.Replace(USES_ETSIEAACPLUSENCODERMODULE_STUB=True)
                    env.Replace(USES_VORBISDECODERMODULE_STUB = True)
                    env.Replace(USES_PASSTHRUFORMATTERMODULE_STUB=True)
                    env.Replace(USES_DTSHDPACKETIZERMODULE_STUB=True)
                    env.Replace(USES_DOLBYMOBILEMODULE_STRIP = True)
                    env.Replace(USES_DOLBYPULSEDECODERMODULE_STRIP = True)
                    env.Replace(USES_GENERICDEPACKETIZERMODULE_STUB=True)
                    env.Replace(USES_MP3ENCODERMODULE_STUB=True)
                    env.Replace(USES_SBCENCODERMODULE_STUB=True)
                    env.Replace(USES_EAC3PACKETIZERMODULE_STUB=True)
                    #env.Replace(USES_WMAPRODECODERMODULE_STUB=True)
                    #env.Replace(USES_DOLBYSURROUNDMODULE_STUB=True)
                    #env.Replace(USES_QENSEMBLEMODULE_STUB=True)
                    env.Replace(USES_CQCONCERTPLUSLIB_STUB=True)
                    env.Replace(USES_POPLESSEQUALIZERMODULE_STRIP=True)
                    env.Replace(USES_REVERBMODULE=True)
                    env.Replace(USES_BASSBOOSTMODULE_STRIP=True)
                    env.Replace(USES_PBEMODULE=True)
                    env.Replace(USES_VIRTUALIZERMODULE_STRIP=True)
                    env.Replace(USES_VISUALIZERMODULE_STRIP=True)
                    env.Replace(USES_DS1LICENSEHANDLER_STUB=True)
                    env.Replace(USES_RS_DRIVER_STUB = True)
                    env.Replace(USES_DML_HAL_STUB = True)
                    env.Replace(USES_AFESPDIFDRIVER_STUB = True)
                    env.Replace(USES_FLUENCE_VOICEPLUS_STRIP = True)
                    env.Replace(USES_AFECODECDRIVER_STUB = True)
                    env.Replace(USES_DOLBYPLUS_ENCODER_STUB = True) 
                    env.Replace(USES_VOCODER_G729AB_STUB = True)
                    env.Replace(USES_AFEHDMIINPUTDRIVER_STUB = True)
                    env.Replace(USES_AFEDMAMANAGER_STUB = True)
                    env.Replace(USES_SRSSS3DMODULE_STUB = True)
                    env.Replace(USES_WIDE_VOICE_STRIP = True)
                    env.Replace(USES_QUARTET_STRIP = True)
                    env.Replace(USES_SLOW_TALK_STRIP = True)
            
            if env.get('CHIPSET') == "msm8974" or env.get('CHIPSET') == "msm8x26" or env.get('CHIPSET') == "msm8926" or env.get('CHIPSET') == "apq8084" or env.get('CHIPSET') == "msm8962":
                env.Replace(USES_HPMP2DECODERMODULE_STRIP=True)
                env.Replace(USES_SRSTRUMEDIAMODULE_STRIP=True)
                env.Replace(USES_AC3PACKETIZERMODULE_STRIP=True)
                env.Replace(USES_SPECTRUMANALYZERMODULE_STRIP=True)
                env.Replace(USES_ETSIAMRWBPLUSDECMODULE_STRIP=True)
                env.Replace(USES_FLACDECODERMODULE_STRIP=True)
                env.Replace(USES_HPMP3DECODERMODULE_STRIP=True)
                env.Replace(USES_ETSIEAACPLUSENCODERMODULE_STRIP=True)
                env.Replace(USES_VORBISDECODERMODULE_STRIP = True)
                env.Replace(USES_PASSTHRUFORMATTERMODULE_STRIP=True)
                env.Replace(USES_DTSHDPACKETIZERMODULE_STRIP=True)
                env.Replace(USES_DOLBYMOBILEMODULE_STRIP = True)
                env.Replace(USES_DOLBYPULSEDECODERMODULE_STRIP = True)
                env.Replace(USES_GENERICDEPACKETIZERMODULE_STRIP=True)
                env.Replace(USES_MP3ENCODERMODULE_STRIP=True)
                env.Replace(USES_SBCENCODERMODULE_STRIP=True)
                env.Replace(USES_EAC3PACKETIZERMODULE_STRIP=True)
                env.Replace(USES_WMAPRODECODERMODULE_STRIP=True)
                env.Replace(USES_DOLBYSURROUNDMODULE_STRIP=True)
                #env.Replace(USES_EFFECTSMODULE_STRIP=True)
                env.Replace(USES_DS1LICENSEHANDLER_STRIP=True)
                env.Replace(USES_FLUENCE_VOICEPLUS_STRIP = True)
                env.Replace(USES_WIDE_VOICE_STRIP = True)
                #env.Replace(USES_QUARTET_STRIP = True)
                #env.Replace(USES_SLOW_TALK_STRIP = True)
                env.Replace(USES_AFESPDIFDRIVER_STUB = True)
                env.Replace(USES_DOLBYPLUS_ENCODER_STUB = True)
                env.Replace(USES_AFEHDMIINPUTDRIVER_STUB = True)
                env.Replace(USES_AFEDMAMANAGER_STUB = True)
                env.Replace(USES_SRSSS3DMODULE_STUB = True)

        elif env.get('CHIPSET') == "mdm9x25" or env.get('CHIPSET') == "mdm9x35":
            env.Replace(USES_MIDI =False)  
            env.Replace(USES_DTS =True)  
            env.Replace(USES_DTS_HPX_MODULE_STUB = True)

            #Add AVS Voice Library Types for this image
            env.Replace(USES_QUARTET_STRIP = True)
            env.Replace(USES_SAMPLE_SLIP_STRIP = True)
            env.Replace(USES_RVE_STRIP = True)
            #env.Replace(USES_RSBY2_STRIP = True)
            #env.Replace(USES_IIR_FILTER_STRIP = True)
            env.Replace(USES_FLUENCE_VOICEPLUS_STRIP = True)
            #env.Replace(USES_FIR_FILTER_STRIP = True)
            env.Replace(USES_ECHO_CANCELLER_STRIP = True)
            env.Replace(USES_DTMF_DETECT_STRIP = True)
            #env.Replace(USES_CTM_STRIP = True)
            #env.Replace(USES_CROSS_FADE_STRIP = True)
            #env.Replace(USES_CNG_STRIP = True)
            env.Replace(USES_AVC_TX_STRIP = True)
            env.Replace(USES_AVC_RX_STRIP = True)
            #env.Replace(USES_VFE_FILTERS_STRIP = True)
            #env.Replace(USES_TTY_STRIP = True)
            env.Replace(USES_TIME_WARPING_STRIP = True)
            env.Replace(USES_SLOW_TALK_STRIP = True)
            #env.Replace(USES_VOCODER_AMRWB_STRIP = True)
            #env.Replace(USES_VOCODER_AMRNB_STRIP = True)
            #env.Replace(USES_VOCODER_FR_STRIP = True)
            #env.Replace(USES_VOCODER_FOURGV_STRIP = True)
            #env.Replace(USES_VOCODER_EVRC_STRIP = True)
            #env.Replace(USES_VOCODER_EFR_STRIP = True)
            env.Replace(USES_VOCODER_EAMR_STRIP = True)
            #env.Replace(USES_VOCODER_CAPI_STRIP = True)
            #env.Replace(USES_VOLUME_CONTROL_STRIP = True)
            #env.Replace(USES_VOICE_RESAMPLER_STRIP = True)
            #env.Replace(USES_VOCODER_V13K_STRIP = True)
            #env.Replace(USES_VOCODER_HR_STRIP = True)
            #env.Replace(USES_VOCODER_G729AB_STRIP = True)
            #env.Replace(USES_VOCODER_G711_STRIP = True)
            env.Replace(USES_WIDE_VOICE_STRIP = True)

            # Add AVS Afe Library Types for this image
            #env.Replace(USES_ADAP_ANC_STRIP =True)
            #env.Replace(USES_FFSPKRPROT_STRIP=True)
            #env.Replace(USES_DTMFGENERATION_STRIP=True)
            #env.Replace(USES_REMOVECLICKLIB_STRIP=True)
            #env.Replace(USES_AFERIVADRIVER_STRIP=True)
            #env.Replace(USES_AFESLIMBUSDRIVER_STRIP=True)
            env.Replace(USES_AFESPDIFDRIVER_STUB = True)

            #Add AVS Audio Library Types for this image
            env.Replace(USES_AACDECODERMODULE_STUB=True)
            #env.Replace(USES_ETSIBITBUFFLIB_STUB=True)
            #env.Replace(USES_ETSIFFRLIB_STUB=True)
            #env.Replace(USES_ETSIOPLIB_STUB=True)
            env.Replace(USES_ETSIEAACPLUSENCODERMODULE_STUB=True)
            env.Replace(USES_ADPCMDECODERMODULE_STUB=True)
            env.Replace(USES_ADPCMENCODERMODULE_STUB=True)
            #env.Replace(USES_AMRBITBUFFERLIB_STUB=True)
            #env.Replace(USES_AMRCORELIB_STUB=True)
            #env.Replace(USES_AMRMATHLIB_STUB=True)
            #env.Replace(USES_ETSIAMROPLIB_STUB=True)
            env.Replace(USES_VISUALIZERMODULE_STUB=True)
            env.Replace(USES_VIRTUALIZERMODULE_STUB=True)
            env.Replace(USES_REVERBMODULE_STUB=True)
            env.Replace(USES_BASSBOOSTMODULE_STUB=True)
            env.Replace(USES_POPLESSEQUALIZERMODULE_STUB=True)
            env.Replace(USES_ETSIAMRWBPLUSDECMODULE_STUB=True)
            env.Replace(USES_FLACDECODERMODULE_STUB=True)
            env.Replace(USES_HPMP3DECODERMODULE_STUB=True)
            env.Replace(USES_MP3DECODERMODULE_STUB=True)
            env.Replace(USES_HPMP2DECODERMODULE_STUB=True)
            env.Replace(USES_MP3ENCODERMODULE_STUB=True)            
            env.Replace(USES_SBCENCODERMODULE_STUB=True)
            env.Replace(USES_WMAPRODECODERMODULE_STUB=True)
            env.Replace(USES_WMASTDDECODERMODULE_STUB=True)
            env.Replace(USES_WMASTDV8ENCODERMODULE_STUB=True)
            env.Replace(USES_DOLBYMOBILEMODULE_STUB = True)
            env.Replace(USES_DOLBYPULSEDECODERMODULE_STUB = True)
            env.Replace(USES_DOLBYSURROUNDMODULE_STUB = True)
            env.Replace(USES_AC3PACKETIZERMODULE_STUB = True)
            env.Replace(USES_VORBISDECODERMODULE_STUB = True)
            env.Replace(USES_CSPECTRUMANALYZERLIB_STUB = True)
            if env.get('USES_DTS') == True:
                env.Replace(USES_DTSMODULE_STUB = True)
            env.Replace(USES_SRSTRUMEDIAMODULE_STUB=True)                      
            env.Replace(USES_EAC3PACKETIZERMODULE_STUB=True)
            env.Replace(USES_GENERICDEPACKETIZERMODULE_STUB=True)
            env.Replace(USES_PASSTHRUFORMATTERMODULE_STUB=True)
            env.Replace(USES_ULTRASOUND_STREAM_MGR_STUB=True)
            env.Replace(USES_DOLBYPLUS_ENCODER_STUB = True)
            env.Replace(USES_AFEHDMIINPUTDRIVER_STUB = True)
            env.Replace(USES_AFEDMAMANAGER_STUB = True)
            env.Replace(USES_SRSSS3DMODULE_STUB = True)
        elif env.get('CHIPSET') == "mpq8092":
            env.get('USES_DTS') == True
            env.Replace(USES_DTS_HPX_MODULE_STUB = True)
            #Add AVS Voice Library Types for this image
            env.Replace(USES_QUARTET_STRIP = True)
            env.Replace(USES_SAMPLE_SLIP_STRIP = True)
            env.Replace(USES_RVE_STRIP = True)
            #env.Replace(USES_RSBY2_STRIP = True)
            #env.Replace(USES_IIR_FILTER_STRIP = True)
            env.Replace(USES_FLUENCE_VOICEPLUS_STRIP = True)
            #env.Replace(USES_FIR_FILTER_STRIP = True)
            env.Replace(USES_ECHO_CANCELLER_STRIP = True)
            env.Replace(USES_DTMF_DETECT_STRIP = True)
            #env.Replace(USES_CTM_STRIP = True)
            #env.Replace(USES_CROSS_FADE_STRIP = True)
            #env.Replace(USES_CNG_STRIP = True)
            env.Replace(USES_AVC_TX_STRIP = True)
            env.Replace(USES_AVC_RX_STRIP = True)
            #env.Replace(USES_VFE_FILTERS_STRIP = True)
            #env.Replace(USES_TTY_STRIP = True)
            env.Replace(USES_TIME_WARPING_STRIP = True)
            env.Replace(USES_SLOW_TALK_STRIP = True)
            #env.Replace(USES_VOCODER_AMRWB_STRIP = True)
            #env.Replace(USES_VOCODER_AMRNB_STRIP = True)
            #env.Replace(USES_VOCODER_FR_STRIP = True)
            #env.Replace(USES_VOCODER_FOURGV_STRIP = True)
            #env.Replace(USES_VOCODER_EVRC_STRIP = True)
            #env.Replace(USES_VOCODER_EFR_STRIP = True)
            env.Replace(USES_VOCODER_EAMR_STRIP = True)
            #env.Replace(USES_VOCODER_CAPI_STRIP = True)
            #env.Replace(USES_VOLUME_CONTROL_STRIP = True)
            #env.Replace(USES_VOICE_RESAMPLER_STRIP = True)
            #env.Replace(USES_VOCODER_V13K_STRIP = True)
            #env.Replace(USES_VOCODER_HR_STRIP = True)
            #env.Replace(USES_VOCODER_G729AB_STRIP = True)
            #env.Replace(USES_VOCODER_G711_STRIP = True)
            env.Replace(USES_WIDE_VOICE_STRIP = True)

            # Add AVS Afe Library Types for this image
            #env.Replace(USES_ADAP_ANC_STRIP =True)
            #env.Replace(USES_FFSPKRPROT_STRIP=True)
            #env.Replace(USES_DTMFGENERATION_STRIP=True)
            #env.Replace(USES_REMOVECLICKLIB_STRIP=True)
            #env.Replace(USES_AFERIVADRIVER_STRIP=True)
            #env.Replace(USES_AFESLIMBUSDRIVER_STRIP=True)

            #Add AVS Audio Library Types for this image
            env.Replace(USES_AACDECODERMODULE_STRIP=True)
            #env.Replace(USES_ETSIBITBUFFLIB_STRIP=True)
            #env.Replace(USES_ETSIFFRLIB_STRIP=True)
            #env.Replace(USES_ETSIOPLIB_STRIP=True)
            env.Replace(USES_ETSIEAACPLUSENCODERMODULE_STRIP=True)
            env.Replace(USES_ADPCMDECODERMODULE_STRIP=True)
            env.Replace(USES_ADPCMENCODERMODULE_STRIP=True)
            #env.Replace(USES_AMRBITBUFFERLIB_STRIP=True)
            #env.Replace(USES_AMRCORELIB_STRIP=True)
            #env.Replace(USES_AMRMATHLIB_STRIP=True)
            #env.Replace(USES_ETSIAMROPLIB_STRIP=True)
            env.Replace(USES_ETSIAMRWBPLUSDECMODULE_STRIP=True)
            env.Replace(USES_FLACDECODERMODULE_STRIP=True)
            env.Replace(USES_HPMP3DECODERMODULE_STRIP=True)
            env.Replace(USES_MP3DECODERMODULE_STRIP=True)
            env.Replace(USES_HPMP2DECODERMODULE_STRIP=True)
            env.Replace(USES_SBCENCODERMODULE_STRIP=True)
            env.Replace(USES_WMAPRODECODERMODULE_STRIP=True)
            env.Replace(USES_WMASTDDECODERMODULE_STRIP=True)
            env.Replace(USES_WMASTDV8ENCODERMODULE_STRIP=True)
            env.Replace(USES_DOLBYMOBILEMODULE_STRIP = True)
            env.Replace(USES_DOLBYPULSEDECODERMODULE_STRIP = True)
            env.Replace(USES_DOLBYPLUS_ENCODER_STUB = True)
            env.Replace(USES_SRSSS3DMODULE_STUB = True)

            if env.get('USES_DTS') == True:
                env.Replace(USES_DTSMODULE_STUB = True)

            if env.get('USES_MIDI') == True:
                env.Replace(USES_MIDICOREMODULE_STRIP=True)

            env.Replace(USES_SRSTRUMEDIAMODULE_STRIP=True)

            env.Replace(USES_AC3PACKETIZERMODULE_STRIP=True)

            env.Replace(USES_EAC3PACKETIZERMODULE_STRIP=True)
            env.Replace(USES_GENERICDEPACKETIZERMODULE_STRIP=True)
            env.Replace(USES_PASSTHRUFORMATTERMODULE_STRIP=True)        
        else:        
            print "unsupported target"
    return


