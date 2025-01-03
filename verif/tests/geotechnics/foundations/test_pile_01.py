# -*- coding: utf-8 -*-
'''Calculation of displacements and internal forces of a pile taken from project "Reordenación del Enlace de la Pañoleta y Accesos a Camas (Sevilla)", annex nº 13 Structures, pages 93-96'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
import xc
from model import predefined_spaces
from model.geometry import grid_model as gm
from model.mesh import finit_el_model as fem
from materials.ehe import EHE_materials
from materials import typical_materials as tm
from geotechnics.foundations import guia_cimentaciones_oc as guia



# Data
fiPile= 1.5  # pile diameter [m]
Emat= 2.8e6    # elastic modulus of pile material [Pa]
bearCap= 450e4 # total bearing capacity of the pile [N]
pType='endBearing' # type of pile
zGround= 0  # ground elevation
Lpile= 36
soils=[[-1,'sandy',8025e3],
       [-1.85,'clay',8025e3/75.],
       [-6.55,'clay',122e3],
       [-16.45,'clay',18.75e3],
       [-26.25,'clay',4500e4/75.],
       [-36.0,'clay',253e3]] # Properties of the soils:
# sandy soils [(zBottom,type, nh), ...]  where 'zBottom' is the global Z 
#  coordinate of the bottom level of the soil and 'nh' [Pa/m] is the 
#  coefficient corresponding to the compactness of the sandy soil.
# clay soils [(zBottom,type, s), ...]  where 'zBottom' is the global Z 
#  coordinate of the bottom level of the soil and 'su' [Pa/m] is the shear  
#  strength of the saturated cohesive soil.

eSize= 0.24     # length of elements
# loads
Fh=706.9e3+572.5e3
N=-1532e3-1470.1e3
Fh=706.9e3
N=-1532e3

# End data
LeqPile=round(math.pi**0.5*fiPile/2.,3) # equivalent side of the square pile

# Materials
concrete=EHE_materials.HA30
#             *** GEOMETRIC model (points, lines, surfaces) - SETS ***
FEcase= xc.FEProblem()
preprocessor=FEcase.getPreprocessor
prep=preprocessor   # short name
nodes= prep.getNodeHandler
elements= prep.getElementHandler
elements.dimElem= 3
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) # Defines the
# dimension of the space: nodes by three coordinates (x,y,z) and 
# six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)
# coordinates in global X,Y,Z axes for the grid generation
xList=[0]
yList=[0]
zList=[-Lpile,0]
# grid model definition
gridGeom= gm.GridModel(prep,xList,yList,zList)

# Grid geometric entities definition (points, lines, surfaces)
# Points' generation
gridGeom.generatePoints()
# Lines generation
pile_rg=gm.IJKRange((0,0,0),(0,0,1))
pile= gridGeom.genLinOneRegion(ijkRange=pile_rg,setName='pile')

#                         *** MATERIALS *** 
concrProp= tm.MaterialData(name='concrProp',E=concrete.Ecm(),nu=concrete.nuc,rho=concrete.density())
# Geometric sections
# rectangular sections
from materials.sections import section_properties as sectpr
geomSectPile= sectpr.RectangularSection(name='geomSectPile',b=LeqPile,h=LeqPile)
# Elastic material-section
pile_mat= tm.BeamMaterialData(name='pile_mat', section=geomSectPile, material=concrProp)
pile_mat.defElasticShearSection3d(preprocessor=prep)

#                         ***FE model - MESH***
pile_mesh= fem.LinSetToMesh(linSet=pile,matSect=pile_mat,elemSize=eSize,vDirLAxZ=xc.Vector([0,1,0]),elemType='ElasticBeam3d',dimElemSpace=3,coordTransfType='linear')
fem.multi_mesh(preprocessor=prep,lstMeshSets=[pile_mesh])


#                       ***BOUNDARY CONDITIONS***
soilLayers= guia.SoilLayers(soilProfile= soils, groundLevel= zGround)
pileBC= guia.PileFoundation(pileSet=pile,pileDiam=fiPile,E=concrete.Ecm(), pileType='endBearing',pileBearingCapacity=bearCap, soilLayers= soilLayers)
pileBC.generateSpringsPile(alphaKh_x=1,alphaKh_y=1,alphaKv_z=1)
springs=pileBC.springs
springSet=preprocessor.getSets.defSet('springSet')
for e in springs:
    springSet.getElements.append(e)
    # print('z:',e.getCooCentroid(True)[2], ' Kx (t/m):',round(e.getMaterials()[0].E*1e-4,2))
springSet.fillDownwards()
allSets= modelSpace.setSum('allSets',[pile,springSet])
'''
from postprocess.xcVtk.FE_model import vtk_FE_graphic
displaySettings= vtk_FE_graphic.DisplaySettingsFE()
displaySettings.displayMesh(xcSets=allSets,fName= None,caption='Mesh',nodeSize=0.5,scaleConstr=0.10)
'''
pTop= gridGeom.getPntXYZ((0,0,0))
nTop= pTop.getNode()
pBase= gridGeom.getPntXYZ((0,0,-Lpile))
nBase=pBase.getNode()
modelSpace.fixNode('FFF_FF0',nBase.tag)  # no rotation around z axis.
modelSpace.fixNode('FFF_F0F',nTop.tag)  # no rotation around y axis.


# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')

lp0.newNodalLoad(nTop.tag,xc.Vector([Fh,0,N,0,0,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)
lp0.gammaF=1

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

z_node_check=[-0.48,-1.44,-4.32,-12.72,-21.36,-35.28]
ux_comp=[0.0095,0.0092,0.0075,0.002,-0.0001,0.00002]
ux_calc=list()
Kx_comp=[138.67e4,288.9e4,329.4e4,50.62e4,1620e4,683.1e4]
Kx_calc=list()
V_comp=[68.71e4,58.77e4,26.07e4,1.52e4,-5.35e4,0.06e4]
V_calc=list()
M_comp=[-210.51e4,-147.87e4,-23e4,36.57e4,18.75e4,-0.03e4]
M_calc=list()

for z in z_node_check:
    p=geom.Pos3d(0,0,z)
    n=pile.getNodes.getNearestNode(p)
    ux= n.getDisp[0]
    ux_calc.append(ux)
    e_s=springSet.getElements.getNearestElement(p)
    Kx=e_s.getMaterials()[0].E
    Kx_calc.append(Kx)
    p=geom.Pos3d(0,0,z-0.24/2.)
    e_p=pile.getElements.getNearestElement(p)
    V=e_p.getVy2
    V_calc.append(V)
    M=e_p.getMz2
    M_calc.append(M)
    
nVal=len(z_node_check)
ac_ux=0
for i in range(len(ux_comp)):
    ac_ux+=((ux_comp[i]-ux_calc[i])/ux_comp[i])**2
err_ux=math.sqrt(ac_ux/nVal)
ac_Kx=0
for i in range(len(Kx_comp)):
    ac_Kx+=((Kx_comp[i]-Kx_calc[i])/Kx_comp[i])**2
err_Kx=math.sqrt(ac_Kx/nVal)
ac_V=0
for i in range(len(V_comp)):
    ac_V+=((V_comp[i]-V_calc[i])/V_comp[i])**2
err_V=math.sqrt(ac_V/nVal)
ac_M=0
for i in range(len(M_comp)):
    ac_M+=((M_comp[i]-M_calc[i])/M_comp[i])**2
err_M=math.sqrt(ac_M/nVal)

'''
print('err_ux= ', err_ux)
print('err_Kx= ', err_Kx)
print('err_V= ', err_V)
print('err_M= ', err_M)
'''

import os
os.system("rm -f -r /tmp/annex") # Clean after yourself.
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err_ux)<0.1 and abs(err_Kx)<5e-5 and abs(err_V)<0.5 and abs(err_M)<0.5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


'''
# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

oh.displayNodeValueDiagram(itemToDisp= 'k_x', caption= 'Spring stiffness x direction', defaultDirection= 'X', defaultValue= 0.0)
oh.displayNodeValueDiagram(itemToDisp= 'k_y', caption= 'Spring stiffness y direction', defaultDirection= 'Y', defaultValue= 0.0)
oh.displayDispRot(itemToDisp='uX',setToDisplay=pile)
oh.displayIntForcDiag(itemToDisp='Vy',setToDisplay=pile)
oh.displayIntForcDiag(itemToDisp='Mz',setToDisplay=pile)
'''
