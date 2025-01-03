# -*- coding: utf-8 -*-
''' Trivial test of TDConcreteMC10NL material constitutive model.  

Based on the example: https://portwooddigital.com/2023/05/28/minimal-creep-and-shrinkage-example/
'''

from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from materials import typical_materials

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

fc = -24.80  
fcu= -2.48
epscu= -0.00350
fct= 2.00
Ec= 22820.0
Ecm= 24950.0
wgt= 0.000024


# Creep parameters.
epsba= -0.000046 
epsbb= 1.0
epsda= -0.001329
epsdb= 726.77

# Shrinkage parameters.
phiba= 0.1379 
phibb= 1.0 
phida= 2.544  
phidb= 1782.0

cem = 1.0

# Other Input Parameters:
tDry = 14.0
beta = 0.7 # can be changed (softening parameter)

## Concrete able to creep.
tdConcrete= typical_materials.defTDConcreteMC10NL(preprocessor= preprocessor, name= 'tdConcrete', fc= fc, fcu= fcu, epscu= epscu, ft= fct, Ec= Ec, Ecm= Ecm, beta= beta, age= tDry, epsba= epsba, epsbb= epsbb, epsda= epsda, epsdb= epsdb, phiba= phiba, phibb= phibb, phida= phida, phidb= phidb, tcast= 0.0, cem= cem)

error= (tdConcrete.fpc-fc)**2 # 1
error+= (tdConcrete.fcu-fcu)**2 # 2
error+= (tdConcrete.epscu-epscu)**2 # 3
error+= (tdConcrete.ft-fct)**2 # 4
error+= (tdConcrete.Ec-Ec)**2 # 5
error+= (tdConcrete.Ecm-Ecm)**2 # 6
error+= (tdConcrete.beta-beta)**2 # 7
error+= (tdConcrete.age-tDry)**2 # 8
error+= (tdConcrete.epsba-epsba)**2 # 9
error+= (tdConcrete.epsbb-epsbb)**2 # 10
error+= (tdConcrete.epsda-epsda)**2 # 11
error+= (tdConcrete.epsdb-epsdb)**2 # 12
error+= (tdConcrete.phiba-phiba)**2 # 13
error+= (tdConcrete.phibb-phibb)**2 # 14
error+= (tdConcrete.phida-phida)**2 # 15
error+= (tdConcrete.phidb-phidb)**2 # 16
error+= (tdConcrete.tcast)**2 # 17
error+= (tdConcrete.cem-cem)**2 # 18
error= math.sqrt(error)

# Check Et value.
ratio0= abs(tdConcrete.Et-Ec)/Ec

# print(ratio0)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(error)<1e-12 and abs(ratio0)<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

