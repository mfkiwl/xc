# -*- coding: utf-8 -*-
'''Test taken from example 5-001 of SAP2000 verification manual.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import xc
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Material definition
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",E= 1e6, nu= 0.25, rho= 0.0)
# Mesh
## Nodes.
nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
nod9= nodes.newNodeXYZ(0,0,0)
nod10= nodes.newNodeXYZ(1,0,0)
nod11= nodes.newNodeXYZ(1,1,0)
nod12= nodes.newNodeXYZ(0,1,0)
nod13= nodes.newNodeXYZ(0,0,1)
nod14= nodes.newNodeXYZ(1,0,1)
nod15= nodes.newNodeXYZ(1,1,1)
nod16= nodes.newNodeXYZ(0,1,1)

## Elements.
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast3d.name
brick= elements.newElement("Brick",xc.ID([nod9.tag,nod10.tag,nod11.tag,nod12.tag,nod13.tag,nod14.tag,nod15.tag,nod16.tag]))

constraints= preprocessor.getBoundaryCondHandler
# Constrain the displacement of the nodes in the base.
nod9.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod10.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod11.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod12.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')

lp0.newNodalLoad(nod13.tag, xc.Vector([0,0,-1]))
lp0.newNodalLoad(nod14.tag, xc.Vector([0,0,-1]))
lp0.newNodalLoad(nod15.tag, xc.Vector([0,0,-1]))
lp0.newNodalLoad(nod16.tag, xc.Vector([0,0,-1]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

R9= nod9.getReaction
R10= nod10.getReaction
R11= nod11.getReaction
R12= nod12.getReaction


R= R9+R10+R11+R12
ratio1= (R-xc.Vector([0,0,4])).Norm()

''' 
print("R9= ",R9)
print("R10= ",R10)
print("R11= ",R11)
print("R12= ",R12)
print("R= ",R)
print("ratio1= ",ratio1)
   '''

# # Output stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
# oh.displayStresses('sigma_11')
# oh.displayStresses('sigma_22')
# oh.displayStresses('sigma_33')

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayStresses(itemToDisp= 'sigma_33',fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName) and (ratio1<1e-12)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName)
