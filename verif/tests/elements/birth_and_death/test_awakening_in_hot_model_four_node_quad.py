# -*- coding: utf-8 -*-
''' Check that adding an element to an existing "hot" model: a model that has
    been already analyzed gives the correct results. Check 3D elastic 
    beam element.
'''
from __future__ import print_function

import os
import sys
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions


__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

l= 1 # Plate side
F= 1e3 # Force magnitude

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

## Mesh
#
#       ^ F/2          ^ F/2
#       |              |
#   <-- +-------------+ --> F/2
# F/2   | n4       n3 |
#       |             |
#       |             |
#       |             |
#       |             |
#       | n1       n2 |
#      >+-------------+ --> F/2
#       ^             ^             

### Nodes.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l,0)
n3= nodes.newNodeXY(l,l)
n4= nodes.newNodeXY(0,l)

### Materials definition
E= 2.1e6 # Steel Young's modulus [kg/cm2].
nu= 0.3 # Poisson's ratio.
h= 0.1 # Plate thickness
quadMat= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "quadMat",E,nu,rho= 0)


# Load value
loadValueUp= xc.Vector([F/2,F/2])
loadValueDown= xc.Vector([F/2,-F/2])
          
### Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= quadMat.name
quadA= elements.newElement("FourNodeQuad",xc.ID([n1.tag, n2.tag, n3.tag, n4.tag]))
quadA.physicalProperties.thickness= h
quadB= elements.newElement("FourNodeQuad",xc.ID([n1.tag, n2.tag, n3.tag, n4.tag]))
quadB.physicalProperties.thickness= h

### Constraints
modelSpace.fixNode("00", n1.tag)
modelSpace.fixNode("F0", n2.tag)


# Deactivate quadB element.
quadBSet= modelSpace.defSet(elements=[quadB])
quadBSet.fillDownwards()
modelSpace.deactivateElements(quadBSet, freezeDeadNodes= False)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag, loadValueDown)
lp0.newNodalLoad(n3.tag, loadValueUp)
lp0.newNodalLoad(n4.tag, -loadValueDown)
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

## Compute solution
solProc= predefined_solutions.PlainNewtonRaphson(feProblem, printFlag= 0)
result= solProc.solve(calculateNodalReactions= True, reactionCheckTolerance= 1e-6)

Ra= n1.getReaction[0] # Horizontal reaction.
ratio0= abs(Ra+F/2)/(F/2)
N1Aa= quadA.getMeanInternalForce("sigma_11")
N2Aa= quadA.getMeanInternalForce("sigma_22")
ratio1= abs(N1Aa-F)/F+abs(N2Aa-F)/F
N1Ba= quadB.getMeanInternalForce("sigma_11")
N2Ba= quadB.getMeanInternalForce("sigma_22")
ratio2= abs(N1Ba-F)/F+abs(N2Ba-F)/F
Ux1a= (n2.getDisp[0]+n3.getDisp[0])/2.0
# Ux1aRef= F/E MESH TOO COARSE TO GET A CORRECT VALUE OF DISPLACEMENT.
# ratio2= abs(Ux1a-Ux1aRef)/Ux1aRef

## Activate the second element (nothing must change).
modelSpace.activateElements(quadBSet)

# Solve again.
result= solProc.solve(calculateNodalReactions= True)

Rb= n1.getReaction[0] # Horizontal reaction.
ratio3= abs(Rb+F/2)/(F/2)
N1Ab= quadA.getMeanInternalForce("sigma_11")
N2Ab= quadA.getMeanInternalForce("sigma_22")
ratio4= abs(N1Ab-F)/F+abs(N2Ab-F)/F
N1Bb= quadB.getMeanInternalForce("sigma_11")
N2Bb= quadB.getMeanInternalForce("sigma_22")
ratio5= abs(N1Bb)/F+abs(N2Bb)/F # No axial force in the awakened element.
Ux1b= (n2.getDisp[0]+n3.getDisp[0])/2.0 # Horizontal displacement.
Ux1bRef= Ux1a # No additional displacement.
ratio6= abs(Ux1b-Ux1bRef)/Ux1bRef

# Define a new load.
lp1= modelSpace.newLoadPattern(name= '1')
lp1.newNodalLoad(n2.tag, loadValueDown)
lp1.newNodalLoad(n3.tag, loadValueUp)
lp1.newNodalLoad(n4.tag, -loadValueDown)
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp1.name)

## Solve again.
result= solProc.solve(calculateNodalReactions= True)

# Check results.
Rc= n1.getReaction[0] # Horizontal reaction.
ratio7= abs(Rc+F)/F
N1Ac= quadA.getMeanInternalForce("sigma_11")
N2Ac= quadA.getMeanInternalForce("sigma_22")
ratio8= abs(N1Ac-1.5*F)/(1.5*F)+abs(N2Ac-1.5*F)/(1.5*F)
N1Bc= quadB.getMeanInternalForce("sigma_11")
N2Bc= quadB.getMeanInternalForce("sigma_22")
ratio9= abs(N1Bc-0.5*F)/(0.5*F)+abs(N2Bc-0.5*F)/(0.5*F) # No axial force in the awakened element.
Ux1c= (n2.getDisp[0]+n3.getDisp[0])/2.0 # Horizontal displacement.
Ux1cRef= 1.5*Ux1bRef # 1.5 times the previous displacement.
ratio10= abs(Ux1c-Ux1cRef)/Ux1cRef

'''
print('Ra= ', Ra, ' ratio0= ', ratio0)
print('N1Aa= ', N1Aa, 'N2Aa= ', N2Aa, ' ratio1= ', ratio1)
print('N1Ba= ', N1Ba, 'N2Ba= ', N2Ba, ' ratio2= ', ratio2)
print('Rb= ', Rb, ' ratio3= ', ratio3)
print('N1Ab= ', N1Ab, ' ratio4= ', ratio4)
print('N1Bb= ', N1Bb, ' ratio5= ', ratio5)
print('Ux1b= ', Ux1b, 'Ux1bRef= ', Ux1bRef, ' ratio6= ', ratio6)
print('Rc= ', Rc, ' ratio7= ', ratio7)
print(quadA.tag, 'N1Ac= ', N1Ac, 'N2Ac= ', N2Ac, ' ratio8= ', ratio8)
print(quadB.tag, 'N1Bc= ', N1Bc, 'N2Bc= ', N2Bc, ' ratio9= ', ratio9)
print('Ux1c= ', Ux1c, ' ratio10= ', ratio10)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-5) &(abs(ratio1)<1e-8) & (abs(ratio3)<1e-8) & (abs(ratio4)<1e-8) & (abs(ratio5)<1e-8) & (abs(ratio6)<1e-8) & (abs(ratio7)<1e-8) & (abs(ratio8)<1e-8) & (abs(ratio9)<1e-8) & (abs(ratio10)<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLocalAxes()
# # oh.displayReactions()
# oh.displayLoads()
# # oh.displayDispRot(itemToDisp='uX')
# # oh.displayIntForcDiag(itemToDisp='Vy')
# oh.displayIntForcDiag(itemToDisp='Mz', setToDisplay= quadBSet)
# oh.displayIntForcDiag(itemToDisp='My', setToDisplay= quadBSet)

