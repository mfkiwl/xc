# -*- coding: utf-8 -*-
'''Home made test on picking load combinations for which the capacity factor exceeds the given threshold'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from postprocess import limit_state_data as lsd
from postprocess.config import default_config

cfg=default_config.EnvConfig(resultsPath='tests/',verifPath='aux/')

limitState=lsd.normalStressesResistance # any limit state that has been previously checked
limitState.envConfig=cfg
lCombNormalULS=limitState.getCriticalLCombs(threshold=0.50)

limitState=lsd.shearResistance
limitState.envConfig=cfg
lCombShearULS=limitState.getCriticalLCombs(threshold=0.9)

ratio1=abs(lCombNormalULS['sect1_critical_comb']['ELU408']-27.27)
ratio2=abs(lCombShearULS['sect2_critical_comb']['ELU810']-3.03)
compCritNorm=['ELU408', 'ELU810', 'ELU815']
ratio3=(lCombNormalULS['critical_combNms']==compCritNorm)
compCritShear=['ELU406', 'ELU317', 'ELU408', 'ELU390', 'ELU810']
ratio4=(lCombShearULS['critical_combNms']==compCritShear)
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)

if (ratio1<1e-6) and (ratio2<1e-6) and ratio3 and ratio4:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

