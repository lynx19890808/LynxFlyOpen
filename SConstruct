import os
import sys
import rtconfig

# if os.getenv('RTT_ROOT'):
    # RTT_ROOT = os.getenv('RTT_ROOT')
# else:
    # RTT_ROOT = os.path.join(Dir('#').get_abspath(), 'rt-thread')
RTT_ROOT = os.path.join(Dir('#').get_abspath(), 'rt-thread')

sys.path = sys.path + [os.path.join(RTT_ROOT, 'tools')]
from building import *

TARGET = 'rtthread-stm32f4xx.' + rtconfig.TARGET_EXT

env = Environment(tools = ['mingw'],
	AS = rtconfig.AS, ASFLAGS = rtconfig.AFLAGS,
	CC = rtconfig.CC, CCFLAGS = rtconfig.CFLAGS,
	AR = rtconfig.AR, ARFLAGS = '-rc',
	LINK = rtconfig.LINK, LINKFLAGS = rtconfig.LFLAGS)
env.PrependENVPath('PATH', rtconfig.EXEC_PATH)

Export('RTT_ROOT')
Export('rtconfig')

# prepare building environment
objs = PrepareBuilding(env, RTT_ROOT, has_libcpu=False)

# build program 
program = env.Program(TARGET, objs)

# end building 
EndBuilding(TARGET, program)
