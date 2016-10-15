#============================================================================
#  Name:                                                                     
#    tbc_cleanpack.py 
#
#  Description:                                                              
#    None 
#                                                                            
# Copyright (c) 2015 by QUALCOMM, Incorporated.  All Rights Reserved.        
#============================================================================
#                                                                            
# *** AUTO GENERATED FILE - DO NOT EDIT                                      
#                                                                            
# GENERATED: Tue Mar 17 23:55:59 2015 
#============================================================================
def exists(env):
   return env.Detect('tcfg_cleanpack')

def generate(env):
   env.AddUsesFlags('USES_CLEAN_PACK',  True)
