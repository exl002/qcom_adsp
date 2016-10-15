#==============================================================================
# Target Build ID Config Script.
#
# Copyright (c) 2010 Qualcomm Technologies Inc.  All Rights Reserved
#==============================================================================

def exists(env):
   return env.Detect('tcfg_AAAAAAAAQ_data')

def generate(env):
   # Save the tcfg_bid_data in the env
   env['TCFG_BID_IMAGE'] = 'MODEM_PROC'

   env.AddUsesFlags('USES_BLAST', from_builds_file = True)
   env.AddUsesFlags('USES_DAL', from_builds_file = True)
   env.AddUsesFlags('USES_DEVCFG_MULTIPLE_CONFIG', from_builds_file = True)
   env.AddUsesFlags('USES_DOG_STUBS', from_builds_file = True)
   env.AddUsesFlags('USES_ERR_INJECT_CRASH', from_builds_file = True)
   env.AddUsesFlags('USES_MBNTOOLS', from_builds_file = True)
   env.AddUsesFlags('USES_NO_STRIP_NO_ODM', from_builds_file = True)
   env.AddUsesFlags('USES_QURT', from_builds_file = True)
   env.AddUsesFlags('USES_RCINIT', from_builds_file = True)
   env.AddUsesFlags('USES_RELOC_ENABLE_AND_ALIGN', from_builds_file = True)
