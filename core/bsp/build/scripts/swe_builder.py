#===============================================================================
#
# Builder to generate a tracer header files containing a list of sw events
#
# GENERAL DESCRIPTION
#    Contains builder(s) defenitions
#
# Copyright (c) 2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.adsp/2.2/bsp/build/scripts/swe_builder.py#2 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 5/18/12    ts      Generate a tracer_events.h containting a list of sw events
#===============================================================================

import os
import string
import datetime
from SCons.Script import *

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
SWE_ENV_DESCRIPTOR = 'SWE_INFO_COLLECTOR'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return True

def generate(env):
    # Generate function for swe builder.
    # Sets up the action, scanner and builder function to be called by
    # clients to pass SW event details to the builder.

    rootenv = env.get('IMAGE_ENV')
    rootenv[SWE_ENV_DESCRIPTOR] = []
    env.Append(QDSS_TRACER_SWE = True) # Signal to call builder
    env.Replace(USES_QDSS_SWE = True)  # Define SW events

    # Add method AddSWEInfo to the environment.
    # AddSWEInfo is the function that needs to be called by the clients to
    # pass a list of SW event info they plan to use for generating
    # SW events
    rootenv.AddMethod(swe_append_list, "AddSWEInfo")

    # Load builder into enviroment
    swe_actn = env.GetBuilderAction(swe_builder, action_source=None)
    swe_bldr = env.Builder(action=swe_actn, emitter=swe_emitter, multi=True)
    rootenv.Append(BUILDERS = {'SWEBuilder' : swe_bldr})

    return


def swe_emitter(target, source, env):
    # So that tracer_event_ids.h is generated when any clients have
    # updated their SW Event names-value pair info

    rootenv = env.get('IMAGE_ENV')
    source.append(env.Value(rootenv[SWE_ENV_DESCRIPTOR]))

    return (target, source)

# Snippet from core/power/npa/build/SConscript
def determine_subsystem( env ):
    # Order important! We iterate through the dict in this order
    image_names = ['MODEM', 'PRONTO', 'APPS', 'SENSOR', 'AUDIO', 'ADSP']
    image_tags = {
        'MODEM'        : ['MODEM_PROC', 'QDSP6_SW_IMAGE'],
        'PRONTO'       : ['WCN_IMAGE', 'CBSP_WCN_IMAGE', 'CORE_WCN'],
        'APPS'         : ['APPS_IMAGE', 'CBSP_APPS_IMAGE'],
        'SENSOR'       : ['CORE_QDSP6_SENSOR_SW'],
        'AUDIO'        : ['CORE_QDSP6_AUDIO_SW'],
        'ADSP'         : ['ADSP_PROC', 'CORE_QDSP6_SW'],
        }
    env_var_names = set(env.gvars().keys())
    for i in image_names:
        if env_var_names.intersection(set(image_tags[i])):
            return i

    return None


def swe_builder(target, source, env):
    # Create the event header files (tracer_event_ids.h and
    # tracer_event_tbl.h)
    rootenv = env.get('IMAGE_ENV')

    #tracer_event_ids argument MUST be passed
    try:
        tracer_event_ids = str(target[0])
    except:
        env.PrintError("SWE Builder: Event file argument not passed")
        exit()

    try:
        tracer_event_ids_h = open(env.RealPath(tracer_event_ids), 'w')
    except IOError:
        env.PrintError("Could not open tracer files for write")
        exit()

    # Want these tags to physically appear in both header files.
    curtime = datetime.datetime.now()
    # Set "subsys" according to the build being placed in.
    # Valid values: 'MODEM', 'ADSP', 'PRONTO', 'VENUS', 'RPM','SENSOR','AUDIO'
    # Determine_subsystem is a default - if it cannot determine the image,
    # subsys can be hardcoded according to the build (subsys = 'MODEM').
    subsys = determine_subsystem(env)
    header_tags = '#ifndef EVENT_TABLE_TAG\n' \
                  '#define EVENT_TABLE_TAG \"{0}' + \
                      curtime.strftime("%Y%m%d%H%M") + '\"\n' \
                  '#define EVENT_PROCESSOR \"' + \
                      env.get('PROC_CONFIG') + '\"\n' \
                  '#define EVENT_SUBSYSTEM ' + subsys + '\n' \
                  '#endif\n\n'

    # Generate the tracer event id enumeration file
    event_id_filename = os.path.split(tracer_event_ids)[1].split('.')[0]
    tracer_event_ids_h.write('#ifndef _'+event_id_filename.upper()+'_H\n')
    tracer_event_ids_h.write('#define _'+event_id_filename.upper()+'_H\n\n')
    tracer_event_ids_h.write('// File autogenerated by swe_builder.\n\n')
    tracer_event_ids_h.write(header_tags.format('QDSS_TAG_'))

    tracer_event_ids_h.write('enum tracer_event_id_enum {\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_RESERVE_0=0,\n')
    if 'USES_QDSS_SWE' in env:
        for item in rootenv[SWE_ENV_DESCRIPTOR]:
            tracer_event_ids_h.write('\t')
            tracer_event_ids_h.write(item[0])
            tracer_event_ids_h.write(',\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_ID_MAX,\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_RESERVE_LAST=0x00FFFFFF,\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_ALL=-1,\n')
    tracer_event_ids_h.write('};\n\n')

    maskVal = 0
    maskCnt = 1    #Account for TRACER_EVENT_RESERVE_0 event
    tracer_event_ids_h.write('#ifndef TRACER_EVENT_PREENABLE_SCONS\n')
    tracer_event_ids_h.write('#define TRACER_EVENT_PREENABLE_SCONS {\\\n')
    if 'USES_QDSS_SWE' in env:
        for item in rootenv[SWE_ENV_DESCRIPTOR]:
            if 'T' == item[2].upper():
               maskVal = maskVal | (1 << maskCnt)
            maskCnt += 1
            if 32 <= maskCnt:
                tracer_event_ids_h. write('\t' + hex(maskVal) + ',\\\n')
                maskVal = 0
                maskCnt = 0
        if 0 < maskCnt:
            tracer_event_ids_h. write('\t' + hex(maskVal) + '\\\n')
    else:
        tracer_event_ids_h.write('\t0\\\n')
    tracer_event_ids_h.write('}\n')
    tracer_event_ids_h.write('#endif\n\n')


    tracer_event_ids_h.write('#endif // _'+event_id_filename.upper()+'_H\n\n')
    tracer_event_ids_h.close()

    #tracer_event_tbl argument MAY be passed
    try:
        tracer_event_tbl = str(target[1])
    except:
        return

    if tracer_event_tbl != "":
        try:
            tracer_event_tbl_h = open(env.RealPath(tracer_event_tbl), 'w')
            event_table_filename=os.path.split(tracer_event_tbl)[1].split('.')[0]
        except IOError:
            env.PrintError("Could not open tracer files for write")
            exit()

        # Generate the tracer event table file
        tracer_event_tbl_h.write('#ifndef _'+event_table_filename.upper()+'_H\n')
        tracer_event_tbl_h.write('#define _'+event_table_filename.upper()+'_H\n\n')
        tracer_event_tbl_h.write('// File autogenerated by swe_builder.\n\n')
        tracer_event_tbl_h.write('#include \"'+event_id_filename+'.h\"\n\n')
        tracer_event_tbl_h.write(header_tags.format(''))

        tracer_event_tbl_h.write('typedef struct _tracer_event_item {\n')
        tracer_event_tbl_h.write('\tenum tracer_event_id_enum evt_id;\n')
        tracer_event_tbl_h.write('\tchar *evt_str;\n')
        tracer_event_tbl_h.write('\tchar pre_enable;\n')
        tracer_event_tbl_h.write('} tracer_event_item;\n\n')

        tracer_event_tbl_h.write('tracer_event_item tracer_event_tbl[] = {\n')
        tracer_event_tbl_h.write('\t{TRACER_EVENT_RESERVE_0,"",\'F\'},\n')
        if 'USES_QDSS_SWE' in env:
            for item in rootenv[SWE_ENV_DESCRIPTOR]:
                tracer_event_tbl_h.write('\t{')
                tracer_event_tbl_h.write(item[0])
                tracer_event_tbl_h.write(',"')
                tracer_event_tbl_h.write(item[1])
                tracer_event_tbl_h.write('",\'')
                tracer_event_tbl_h.write(item[2].upper())
                tracer_event_tbl_h.write('\'},\n')
        tracer_event_tbl_h.write('\t{TRACER_EVENT_ID_MAX,"",\'F\'}\n')
        tracer_event_tbl_h.write('};\n\n')
        tracer_event_tbl_h.write('#endif // _'+event_table_filename.upper()+'_H\n\n')
        tracer_event_tbl_h.close()

    return


def swe_append_list(env, target, source):
    # When AddSWEInfo is called by the client, the list of SW event
    # name-desc-precfg triplets passed by the clients are stored
    # in a list here.

    rootenv = env.get('IMAGE_ENV')

    if 'USES_QDSS_SWE' in env:
        for item in source:
            item[0]=item[0].split('=')[0]        # Drop assignment portion
            if len(item) == 2:                   # Convert old definition
                item.append('F')
            rootenv[SWE_ENV_DESCRIPTOR].append(item)
    return

