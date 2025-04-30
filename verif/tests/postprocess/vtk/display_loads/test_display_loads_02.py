# -*- coding: utf-8 -*-
from __future__ import division 
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import xc
import math
from model import predefined_spaces
from model.geometry import grid_model as gm
from materials import typical_materials as tm
from model.mesh import finit_el_model as fem
from actions import loads
from actions import load_cases as lcases

#Data
r1= 1
r2= 1.2
ndiv= 36
angIncr=360/ndiv
rList=[r1,r2]
angList=[i*angIncr for i in range(ndiv)]
zList=[0]

t= 10e-3
matRho= 7850
#
area= math.pi*(r2**2-r1**2)
volume= area*t
mass= volume*matRho
grav= 10 #Gravity acceleration (m/s2)
weight= mass*grav

FEcase= xc.FEProblem()
preprocessor=FEcase.getPreprocessor
prep=preprocessor   #short name
nodes= prep.getNodeHandler
elements= prep.getElementHandler
elements.dimElem= 3
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

gridGeom=gm.GridModel(prep,rList,angList,zList,xCentCoo=0,yCentCoo=0)
gridGeom.generateCylZPoints()

ring=gridGeom.genSurfOneXYZRegion([(r1,0,0),(r2,angList[-1],0)],setName='ring',closeCyl='Y')


steel= tm.defElasticIsotropic3d(preprocessor=preprocessor, name='steel', E=172e9, nu=0.3, rho= matRho)
ring_mat=tm.defMembranePlateFiberSection(preprocessor,name='ring_mat',h=t ,nDMaterial= steel)
ring_mesh= fem.SurfSetToMesh(surfSet=ring,matSect=ring_mat,elemSize=0.5,elemType='ShellMITC4')
fem.multi_mesh(prep,[ring_mesh])

# Constraints
xcTotalSet= modelSpace.getTotalSet()
constrainedNodes= list()
for n in xcTotalSet.nodes:
    modelSpace.fixNode000_FFF(n.tag)
    constrainedNodes.append(n)
    

selfWeight= loads.InertialLoad(name='selfWeight', lstSets=[ring], vAccel=xc.Vector( [0.0,0.0,-grav])) # Ana uses -accel

D=lcases.LoadCase(preprocessor=prep,name="D",loadPType="default",timeSType="constant_ts")
D.create()
D.addLstLoads([selfWeight])
modelSpace.addLoadCaseToDomain("D")

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)


from postprocess import output_handler as outHndl
oh=outHndl.OutputHandler(modelSpace)
# oh.displayLoads()

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayLoads(fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
