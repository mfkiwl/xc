# -*- coding: utf-8 -*-
'''Particular geometry organization, linked to a framework of spaced points, 
usually referenced by their indices (i,j,k) that point to the global 
coordinates (x,y,z).

Generally, an ijkGrid contains the grid of points itself and all the 
geometric entities (lines, surfaces, ...) attached to it. Dictionaries with 
those entities, identified by specific names, are created, in order to 
facilitate and accelerate their creation, search, grouping in sets, ...

Several grids can coexist in a FE problem.
'''

from __future__ import print_function

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"


import geom
import math
from misc_utils import data_struct_utils as dsu

class IJKRange(object):
    '''Range of indexes (i,j,k) in the 3D-grid that defines a region 
    bounded by the coordinates associated with those indexes.

    :ivar ijkMin: tuple (or list) with minimum value of indexes 
          (minIindex,minJindex,minKindex) that point to (xmin,ymin,zmin)
    :ivar ijkMax: tuple (or list) with maximum value of indexes 
          (maxIindex,maxJindex,maxKindex) that point to (xmax,ymax,zmax)
    '''
    def __init__(self,ijkMin,ijkMax):
        if type(ijkMin)==list:
            self.ijkMin=tuple(ijkMin)
        else:
            self.ijkMin= ijkMin
        if type(ijkMax)==list:
            self.ijkMax=tuple(ijkMax)
        else:
            self.ijkMax= ijkMax
    def getIMin(self):
        '''return the value of the index I minimum in the range '''
        return self.ijkMin[0]
    def getIMax(self):
        '''return the value of the index I maximum in the range '''
        return self.ijkMax[0]
    def getJMin(self):
        '''return the value of the index J minimum in the range '''
        return self.ijkMin[1]
    def getJMax(self):
        '''return the value of the index J maximum in the range '''
        return self.ijkMax[1]
    def getKMin(self):
        '''return the value of the index K minimum in the range '''
        return self.ijkMin[2]
    def getKMax(self):
      '''return the value of the index K maximum in the range '''
      return self.ijkMax[2]
    def getRange(self,index):
        '''return a list with the range of indexes between minIindex and maxIindex '''
        mn= self.ijkMin[index]
        mx= self.ijkMax[index]+1 #Last index included
        return range(mn,mx)
    def getIRange(self):
        '''return a list with the range of indexes between minIindex
        and maxIindex '''
        return self.getRange(0)
    def getJRange(self):
        '''return a list with the range of index between minJindex and
          maxJindex'''
        return self.getRange(1)

    def getKRange(self):
        '''return a list with the range of indexes between minKindex
        and maxKindex '''
        return self.getRange(2)

    def __str__(self):
        return 'IRange: ' + str(self.getIRange()) + ' JRange: ' + str(self.getJRange()) + ' KRange: ' + str(self.getKRange())

    def extractIncludedIJranges(self,step=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the index K is a constant. Graphically it can be seen as
        the set of planes 'parallel' to global XY included in the region defined
        by the IJKRange 

        :ivar step: K step to select the XY planes (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstIJrang=[IJKRange((imin,jmin,k),(imax,jmax,k)) for k in range(kmin,kmax+1,step)]
        return lstIJrang

    def extractIncludedIKranges(self,step=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the index J is a constant. Graphically it can be seen as
        the set of planes 'parallel' to global XZ included in the region defined
        by the IJKRange 

        :param step: J step to select the XZ planes (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstIKrang=[IJKRange((imin,j,kmin),(imax,j,kmax)) for j in range(jmin,jmax+1,step)]
        return lstIKrang

    def extractIncludedJKranges(self,step=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the index I is a constant. Graphically it can be seen as
        the set of planes 'parallel' to global YZ included in the region defined
        by the IJKRange 

        :param step: I step to select the XZ planes (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstJKrang=[IJKRange((i,jmin,kmin),(i,jmax,kmax)) for i in range(imin,imax+1,step)]
        return lstJKrang

    def extractIncludedIranges(self,stepJ=1,stepK=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the indices J and K are a constant. Graphically 
        it can be seen as the set of lines 'parallel' to global X 
        axis included in the region defined by the IJKRange 

        :param stepJ: step to select the lines in Y direction (defaults to 1)
        :param stepK step to select the lines in Z direction (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstIrang=[IJKRange((imin,j,k),(imax,j,k)) for j in range(jmin,jmax+1,stepJ) for k in range(kmin,kmax+1,stepK)]
        return lstIrang

    def extractIncludedJranges(self,stepI=1,stepK=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the indices I and K are a constant. Graphically it can be seen as
        the set of lines 'parallel' to global Y axis included in the region defined
        by the IJKRange 

        :param stepI: step to select the lines in X direction (defaults to 1)
        :param stepK step to select the lines in Z direction (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstJrang=[IJKRange((i,jmin,k),(i,jmax,k)) for i in range(imin,imax+1,stepI) for k in range(kmin,kmax+1,stepK)]
        return lstJrang

    def extractIncludedKranges(self,stepI=1,stepJ=1):
        '''return a list with all the sub-ranges included in the IJKRange 
        for which the indices I and J are a constant. Graphically it can be seen as
        the set of lines 'parallel' to global Z axis included in the region defined
        by the IJKRange 

        :param stepI: step to select the lines in X direction (defaults to 1)
        :param stepJ step to select the lines in Y direction (defaults to 1)
        '''
        (imin,jmin,kmin)=self.ijkMin
        (imax,jmax,kmax)=self.ijkMax
        lstKrang=[IJKRange((i,j,kmin),(i,j,kmax)) for i in range(imin,imax+1,stepI) for j in range(jmin,jmax+1,stepJ) ]
        return lstKrang



class GridModel(object):
    '''Particular geometry organization, linked to a framework of spaced points, 
    usually referenced by their indices (i,j,k) that point to the global 
    coordinates (x,y,z).

    Generally, an ijkGrid contains the grid of points itself and all the 
    geometric entities (lines, surfaces, ...) attached to it. Dictionaries with 
    those entities, identified by specific names, are created, in order to 
    facilitate and accelerate their creation, search, grouping in sets, ...

    Several grids can coexist in a FE problem.

    :ivar prep: preprocessor
    :ivar xList: - cartesian coord: ordered list of x-coordinates for the grid 
                 - cylindrical coord: ordered list of radius
                 - elliptical coord: ordered list of semi-axes X
    :ivar yList: - cartesian coord: ordered list of y-coordinates for the grid 
                 - cylindrical and elliptical coord: ordered list of angles 
                   expressed in degrees counterclockwise from the X-axis direction
                 -  elliptical coord: ordered list of semi-axes Y
    :ivar zList: ordered list of z-coordinates for the grid.
    :ivar xCentCoo, yCentCoo: x and y coordinates of the center point
          in cylindrical or elliptical coordinates
          (both coordinates default to 0)
    :ivar semiYellipList: (only used for elliptical coordinates) ordered list of 
                    semi-axes Y that correspond with each semi-axis X
                    in xList 
    :ivar dicLin: generated dictionary with all the lines linked to the grid. 
          Each key is the name of a line, the value associated is the line 
          itself.
    :ivar dicHexaedr: generated dictionary with all the hexaedral volumes 
          linked to the grid. 
          Each key is the name of a volume, the value associated is the volume
          itself.
    :ivar dicQuadSurf: generated dictionary with all the surfaces linked to the grid. 
          Each key is the name of a surface, the value associated is the surface 
          itself.


    '''
    def __init__(self,prep,xList,yList,zList,xCentCoo=0,yCentCoo=0,semiYellipList=None):
        self.gridCoo= list()
        self.gridCoo.append(xList)
        self.gridCoo.append(yList)
        self.gridCoo.append(zList)
        self.prep= prep
        self.indices= self.prep.getMultiBlockTopology.get3DNets.new3DNet()
        self.indices.dim(len(self.gridCoo[0]),len(self.gridCoo[1]),len(self.gridCoo[2]))
        self.pointCounter=0
        self.dicHexaedr=dict()
        self.dicQuadSurf=dict()
        self.dicLin=dict()
        self.xCentCoo=xCentCoo
        self.yCentCoo=yCentCoo
        self.semiYellipList=semiYellipList

    def lastXIndex(self):
        return len(self.gridCoo[0])-1
    def lastYIndex(self):
        return len(self.gridCoo[1])-1
    def lastZIndex(self):
        return len(self.gridCoo[2])-1

    
    def getIJKfromXYZ(self,xyz):
        '''Return (i,j,k) indexes that match the closest with (x,y,z) coordinates
        '''
        ijk=(dsu.get_index_closest_inlist(self.gridCoo[0],xyz[0]),
             dsu.get_index_closest_inlist(self.gridCoo[1],xyz[1]),
             dsu.get_index_closest_inlist(self.gridCoo[2],xyz[2]))
        return ijk

    def getPntXYZ(self,xyz):
        '''Return the point of the grid closest to coordinates (x,y,z)'''
        ijk=self.getIJKfromXYZ(xyz)
        pnt=self.getPntGrid(ijk)
        return pnt
    
    def getIJKrangeFromXYZrange(self,xyzRange):
        '''Return an ijkRange that matches with closest coordinates in 
        xyzRange defined as:
        xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=IJKRange(
            (dsu.get_index_closest_inlist(self.gridCoo[0],xyzRange[0][0]),
             dsu.get_index_closest_inlist(self.gridCoo[1],xyzRange[0][1]),
             dsu.get_index_closest_inlist(self.gridCoo[2],xyzRange[0][2])),
            (dsu.get_index_closest_inlist(self.gridCoo[0],xyzRange[1][0]),
             dsu.get_index_closest_inlist(self.gridCoo[1],xyzRange[1][1]),
             dsu.get_index_closest_inlist(self.gridCoo[2],xyzRange[1][2])))
        return ijkRange
    
    def getPntGrid(self,indPnt):
        '''Return the point at indPnt=(i,j,k) index of the grid.

        :param indPnt: grid indices that point to the global (X, Y, Z)
                 coordinates    
        '''
        return self.indices.getPnt(indPnt[0]+1,indPnt[1]+1,indPnt[2]+1)

    def getTagPntGrid(self,indPnt):
        '''Return the tag of the point at indPnt=(i,j,k) index of the grid.

        :param indPnt: grid indices that point to the global (X, Y, Z)
                 coordinates    
        '''
        tagPto= self.indices.getPnt(indPnt[0]+1,indPnt[1]+1,indPnt[2]+1).tag
        return tagPto

    def getXYZcooPnt(self,pnt):
        ''' Return the (x,y,z) coordinates of point pnt'''
        pos=pnt.getPos
        return (pos.x,pos.y,pos.z)

    def getCylCooPnt(self,pnt):
        ''' Return the cylindrical coorfinates of the point pnt
        (radius,angle in degrees from X axis counterclockwise, Z)
        '''
        x,y,z=self.getXYZcooPnt(pnt)
        deltaX=x-self.xCentCoo
        deltaY=y-self.yCentCoo        
        radius=math.sqrt(deltaX**2+deltaY**2)
        angle=math.degrees(math.atan2(deltaY,deltaX))
        if angle<0: angle+=360
        return (radius,angle,z)
        
        
    def gridHexaedrName(self,pt1,pt2,pt3,pt4,pt5,pt6,pt7,pt8):
        '''Return the name of the hexaedral volume defined by 8 points.

        :param pt1,pt2,pt3,pt4,pt5,pt6,pt7,pt8: tags of the points (in right order) 
               that define the hexaedral volume
        '''
        return 'v'+'%06.0f' % pt1 +'%06.0f' % pt2 +'%06.0f' % pt3 +'%06.0f' % pt4+'%06.0f' % pt5 +'%06.0f' % pt6 +'%06.0f' % pt7 +'%06.0f' % pt8
    
    def gridSurfName(self,pt1,pt2,pt3,pt4):
        '''Return the name of the quadrangle surface defined by 4 points.

        :param pt1,pt2,pt3,pt4: tags of the points (in right order) 
               that define the surface 
        '''
        return 's'+'%06.0f' % pt1 +'%06.0f' % pt2 +'%06.0f' % pt3 +'%06.0f' % pt4


    def gridLinName(self,pt1,pt2):
        '''Return the name of the line defined by 2 points

        :param pt1,pt2: tags of the points (in right order) that
               define the line

        '''
        return 'l'+'%06.0f' % pt1 +'%06.0f' % pt2

    def getNmHexaedrInRange(self,ijkRange,closeCyl='N'):
        '''Return a list with the names of the hexedral volumes limited by a volume 
        defined by the coordinates that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 

        :param ijkRange: instance of IJKRange class
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N') (not yet implemented)
        '''
        (imin,jmin,kmin)=ijkRange.ijkMin
        (imax,jmax,kmax)=ijkRange.ijkMax
        indPtsVols=[((i,j,k),(i+1,j,k),(i+1,j+1,k),(i,j+1,k),(i,j,k+1),(i+1,j,k+1),(i+1,j+1,k+1),(i,j+1,k+1)) for i in range(imin,imax) for j in range(jmin,jmax) for k in range(kmin,kmax)]
        nmVols=[self.getNameHexaedr(indPt) for indPt in indPtsVols]
        return nmVols
        
    def getNmSurfInRange(self,ijkRange,closeCyl='N'):
        '''Return a list with the names of the surfaces limited by a volume 
        defined by the coordinates that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 

        :param ijkRange: instance of IJKRange class
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        (imin,jmin,kmin)=ijkRange.ijkMin
        (imax,jmax,kmax)=ijkRange.ijkMax
        'surfaces in XY plane'
        indPtSurfXY=[((i,j,k),(i+1,j,k),(i+1,j+1,k),(i,j+1,k)) for j in range(jmin,jmax) for i in range(imin,imax) for k in range (kmin,kmax+1)]
        if closeCyl.lower()[0]=='y':
            indPtSurfXY+=[((i,jmax,k),(i+1,jmax,k),(i+1,jmin,k),(i,jmin,k)) for i in range(imin,imax) for k in range (kmin,kmax+1)]
        nmSurfXY=[self.getNameQuadGridSurface(indPtsQs) for indPtsQs in indPtSurfXY]
        'surfaces in XZ plane'
        indPtSurfXZ=[((i,j,k),(i,j,k+1),(i+1,j,k+1),(i+1,j,k)) for k in range(kmin,kmax) for i in range(imin,imax) for j in range(jmin,jmax+1)]
        nmSurfXZ=[self.getNameQuadGridSurface(indPtsQs) for indPtsQs in indPtSurfXZ]
        'surfaces in YZ plane'
        indPtSurfYZ=[((i,j,k),(i,j+1,k),(i,j+1,k+1),(i,j,k+1)) for k in range(kmin,kmax) for j in range(jmin,jmax) for i in range(imin,imax+1)]
        if closeCyl.lower()[0]=='y':
            indPtSurfYZ+=[((i,jmax,k),(i,jmin,k),(i,jmin,k+1),(i,jmax,k+1)) for k in range(kmin,kmax) for i in range(imin,imax+1)]
        nmSurfYZ=[self.getNameQuadGridSurface(indPtsQs) for indPtsQs in indPtSurfYZ]
        return (nmSurfXY+nmSurfXZ+nmSurfYZ)

    def getNmLinInRange(self,ijkRange):
        '''Return a list with the names of the surfaces limited by a volume 
        defined by the coordinates that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        (imin,jmin,kmin)=ijkRange.ijkMin
        (imax,jmax,kmax)=ijkRange.ijkMax
        'lines parallel to X-axis'
        indPtLinX=[((i,j,k),(i+1,j,k)) for  i in range(imin,imax) for j in range(jmin,jmax+1) for k in range(kmin,kmax+1)]
        nmLinX=[self.getNameGridLine(indPtsLn) for indPtsLn in indPtLinX]
        'lines parallel to Y-axis'
        indPtLinY=[((i,j,k),(i,j+1,k)) for j in range(jmin,jmax)  for  i in range(imin,imax+1) for k in range(kmin,kmax+1)]
        nmLinY=[self.getNameGridLine(indPtsLn) for indPtsLn in indPtLinY]
        'lines parallel to Z-axis'
        indPtLinZ=[((i,j,k),(i,j,k+1)) for  k in range(kmin,kmax) for i in range(imin,imax+1) for j in range(jmin,jmax+1)]
        nmLinZ=[self.getNameGridLine(indPtsLn) for indPtsLn in indPtLinZ]
        return (nmLinX+nmLinY+nmLinZ)

    def generatePoints(self):
        '''Point generation in a cartesian coordinate system.'''
        points= self.prep.getMultiBlockTopology.getPoints
        inicTag=points.defaultTag
        lstPt=[(i+1,j+1,k+1,self.gridCoo[0][i],self.gridCoo[1][j],self.gridCoo[2][k]) for i in range(len(self.gridCoo[0])) for j in range(len(self.gridCoo[1])) for k in range(len(self.gridCoo[2])) ]
        for p in lstPt:
            (i,j,k,x,y,z)=p
            self.pointCounter+=1
            pnt=points.newPoint(self.pointCounter+inicTag,geom.Pos3d(x,y,z))
            self.indices.setPnt(i,j,k,pnt.tag)

    def generateCylZPoints(self):
        '''Point generation in the following cylindrical coordinate system:

        - Origin: point of coordinates (xCentCoo,yCentCoo,0)
        - Longitudinal axis: Z
        - Azimuth expressed in degrees counterclockwise from the X-axis direction (Z+)
        
        Coordinates are defined:
        
        - x: radial coordinate (radius)
        - y: angular coordinate (degrees counterclockwise from X-axis)
        - z: height coordinate (Z axis)
        '''
        points= self.prep.getMultiBlockTopology.getPoints
        inicTag=points.defaultTag
        lstPt=[(i+1,j+1,k+1,
            self.xCentCoo+self.gridCoo[0][i]*math.cos(math.radians(self.gridCoo[1][j])),
            self.yCentCoo+self.gridCoo[0][i]*math.sin(math.radians(self.gridCoo[1][j])),
            self.gridCoo[2][k]) for i in range(len(self.gridCoo[0]))
            for j in range(len(self.gridCoo[1]))
            for k in range(len(self.gridCoo[2])) ]
        for p in lstPt:
            (i,j,k,x,y,z)=p
            self.pointCounter+=1
            pnt=points.newPoint(self.pointCounter+inicTag,geom.Pos3d(x,y,z))
            self.indices.setPnt(i,j,k,pnt.tag)
            
    def generateEllipZPoints(self):
        '''Point generation in the following elliptical coordinate system:

        - Origin: point of coordinates (xCentCoo,yCentCoo,0)
        - Longitudinal axis: Z
        - Azimuth expressed in degrees counterclockwise from the X-axis direction
        
        Coordinates are defined:
        
        - x: semi-axis X of the ellipse
        - y: angular coordinate (degrees counterclockwise from X-axis)
        - z: height coordinate (Z axis)
        - semiYellipList contains a list of semi-axis Y of the ellipse
          that correspond element to element with the semi-axis X in xList
        '''
        points= self.prep.getMultiBlockTopology.getPoints
        inicTag=points.defaultTag
#        lstPt=[(i+1,j+1,k+1,
#                self.xCentCoo+self.gridCoo[0][i]*math.cos(math.radians(self.gridCoo[1][j])),
#                self.yCentCoo+self.gridCoo[0][i]*math.sin(math.radians(self.gridCoo[1][j])),
#                self.gridCoo[2][k]) for i in range(len(self.gridCoo[0]))
#               for j in range(len(self.gridCoo[1]))
#               for k in range(len(self.gridCoo[2])) ]
        lstPt=list()
        for i in range(len(self.gridCoo[0])):
            sx=self.gridCoo[0][i]
            sy=self.semiYellipList[i]
            for j in range(len(self.gridCoo[1])):
                ang=math.radians(self.gridCoo[1][j])
                paramCoo=math.sqrt((math.cos(ang)/sx)**2+(math.sin(ang)/sy)**2)
                x=self.xCentCoo+1/paramCoo*math.cos(ang)
                y=self.yCentCoo+1/paramCoo*math.sin(ang)
                for k in range(len(self.gridCoo[2])):
                    z=self.gridCoo[2][k]
                    lstPt.append((i+1,j+1,k+1,x,y,z))
        for p in lstPt:
            (i,j,k,x,y,z)=p
            self.pointCounter+=1
            pnt=points.newPoint(self.pointCounter+inicTag,geom.Pos3d(x,y,z))
            self.indices.setPnt(i,j,k,pnt.tag)

    def movePointsRange(self,ijkRange,vDisp):
        '''Move points  in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax), to a new position 
        by applying a 3D displacement.

        :param ijkRange: range for the search
        :param vDisp: xc vector displacement
        '''
        sPtMove=self.getSetPntRange(ijkRange,'sPtMove')
        for p in sPtMove.getPoints:
          p.getPos.x+= vDisp[0]
          p.getPos.y+= vDisp[1]
          p.getPos.z+= vDisp[2]
        sPtMove.clear()
        

    def moveAllPoints(self,vDisp):
        '''Move all points in the grid to a new position
        by pplying a 3D displacement.

        :param vDisp: xc vector displacement
        '''
        ijkRange=IJKRange([0,0,0],[self.lastXIndex(),self.lastYIndex(),self.lastZIndex()])
        self.movePointsRange(ijkRange,vDisp)

    def movePointsXYZRange(self,XYZrange,vDisp):
        '''Move points  in a 3D grid-region limited by an
        xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        to a new position by applying a 3D displacement.

        :param XYZRange: range for the search
        :param vDisp: xc vector displacement
        '''
        ijkRange=self.getIJKrangeFromXYZrange(XYZrange)
        return self.movePointsRange(ijkRange,vDisp)

    def movePointXYZ(self,xyz,vDisp):
        '''Move the nearest point in grid xyz=(x,y,z) coordinates
        to a new position by applying a 3D displacement.

        :param xyz: grid (x,y,z) coordinates of the point
        :param vDisp: xc vector displacement xc.Vector([vx,vy,vz])
        '''
        p=self.getPntXYZ(xyz)
        p.getPos.x+= vDisp[0]
        p.getPos.y+= vDisp[1]
        p.getPos.z+= vDisp[2]
        

    def slopePointsRange(self,ijkRange,slopeX=0,xZeroSlope=0,slopeY=0,yZeroSlope=0,cylCoo=False):
        '''Applies one or two slopes (in X and Y directions) 
        to points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). Only Z coordinate
        of points is modified.
        If cylindrical coordinates (cylCoo=True), x is taken as the radius and
        y is taken as the angle

        :param ijkRange: range for the search.
        :param slopeX: slope in X direction, expressed as deltaZ/deltaX 
                       (defaults to 0 = no slope applied)
        :param xZeroSlope: coordinate X of the "rotation axis".
        :param slopeY: slope in Y direction, expressed as deltaZ/deltaY)
                       (defaults to 0 = no slope applied)
        :param yZeroSlope: coordinate Y of the "rotation axis".
        :param cylCoo: True if cylindric coordinates (defaults to False)
        '''
        sPtMove=self.getSetPntRange(ijkRange,'sPtMove')
        if not cylCoo:
            for p in sPtMove.getPoints:
                p.getPos.z+= slopeX*(p.getPos.x-xZeroSlope)+slopeY*(p.getPos.y-yZeroSlope)
        else:
            for p in sPtMove.getPoints:
                rad,ang,z=self.getCylCooPnt(p)
                p.getPos.z+= slopeX*(rad-xZeroSlope)+slopeY*(ang-yZeroSlope)
        sPtMove.clear()

    def slopePointsXYZrange(self,XYZrange,slopeX=0,xZeroSlope=0,slopeY=0,yZeroSlope=0,cylCoo=False):
        '''Applies one or two slopes (in X and Y directions) 
        to points in a 3D grid-region limited by an
        xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        Only Z coordinate of points is modified.
        If cylindrical coordinates (cylCoo=True), x is taken as the radius and
        y is taken as the angle

        :param XYZrange: range for the search.
        :param slopeX: slope in X direction, expressed as deltaZ/deltaX 
                       (defaults to 0 = no slope applied)
        :param xZeroSlope: coordinate X of the "rotation axis".
        :param slopeY: slope in Y direction, expressed as deltaZ/deltaY)
                       (defaults to 0 = no slope applied)
        :param yZeroSlope: coordinate Y of the "rotation axis".
        :param cylCoo: True if cylindric coordinates (defaults to False)
        '''

        ijkRange=self.getIJKrangeFromXYZrange(XYZrange)
        return self.slopePointsRange(ijkRange,slopeX,xZeroSlope,slopeY,yZeroSlope,cylCoo)
        
    def rotPntsZAxis(self,ijkRange,angle,xyRotCent):
        '''Rotates points in ijkRange around a Z axis passing by xyRotCent.

        :param ijkRange: range for the search.
        :param angle: rotation angle (degrees)
        :param xyRotCent: coordinates [x,y] of the axis Z of rotation
        '''
        theta=math.radians(angle)
        sinTheta=math.sin(theta)
        cosTheta=math.cos(theta)
        sPtMove=self.getSetPntRange(ijkRange,'sPtMove')
        for p in sPtMove.getPoints:
            xp=p.getPos.x
            yp=p.getPos.y
            p.getPos.x= xyRotCent[0]+cosTheta*(xp-xyRotCent[0])-sinTheta*(yp-xyRotCent[1])
            p.getPos.y= xyRotCent[1]+sinTheta*(xp-xyRotCent[0])+cosTheta*(yp-xyRotCent[1])
        sPtMove.clear()
    
    def rotPntsXYZrangeZAxis(self,XYZrange,angle,xyRotCent):
        '''Rotates points in XYZrange around a Z axis passing by xyRotCent.

        :param XYZrange: range for the search.
        :param angle: rotation angle (degrees)
        :param xyRotCent: coordinates [x,y] of the axis Z of rotation
        '''
        ijkRange=self.getIJKrangeFromXYZrange(XYZrange)
        return self.rotPntsZAxis(ijkRange,angle,xyRotCent)

    def rotAllPntsZAxis(self,angle,xyRotCent):
        '''Rotates all points in the grid around a Z axis passing by xyRotCent.

        :param angle: rotation angle (degrees)
        :param xyRotCent: coordinates [x,y] of the axis Z of rotation
        '''
        ijkRange=IJKRange([0,0,0],[self.lastXIndex(),self.lastYIndex(),self.lastZIndex()])
        self.rotPntsZAxis(ijkRange,angle,xyRotCent)
    
    def rotPntsXAxis(self,ijkRange,angle,yzRotCent):
        '''Rotates points in ijkRange around a X axis passing by yzRotCent.

        :param ijkRange: range for the search.
        :param angle: rotation angle (degrees)
        :param yzRotCent: coordinates [y,z] of the axis X of rotation
        '''
        theta=math.radians(angle)
        sinTheta=math.sin(theta)
        cosTheta=math.cos(theta)
        sPtMove=self.getSetPntRange(ijkRange,'sPtMove')
        for p in sPtMove.getPoints:
            yp=p.getPos.y
            zp=p.getPos.z
            p.getPos.y= yzRotCent[0]+cosTheta*(yp-yzRotCent[0])-sinTheta*(zp-yzRotCent[1])
            p.getPos.z= yzRotCent[1]+sinTheta*(yp-yzRotCent[0])+cosTheta*(zp-yzRotCent[1])
        sPtMove.clear()
        
    def rotPntsXYZrangeXAxis(self,XYZrange,angle,yzRotCent):
        '''Rotates points in XYZrange around a X axis passing by yzRotCent.

        :param XYZrange: range for the search.
        :param angle: rotation angle (degrees)
        :param yzRotCent: coordinates [y,z] of the axis X of rotation
        '''
        ijkRange=self.getIJKrangeFromXYZrange(XYZrange)
        self.rotPntsXAxis(ijkRange,angle,xyRotCent)
    
    def rotAllPntsXAxis(self,angle,yzRotCent):
        '''Rotates all points in the grid around a X axis passing by yzRotCent.

        :param angle: rotation angle (degrees)
        :param yzRotCent: coordinates [y,z] of the axis X of rotation
        '''
        ijkRange=IJKRange([0,0,0],[self.lastXIndex(),self.lastYIndex(),self.lastZIndex()])
        self.rotPntsXAxis(ijkRange,angle,yzRotCent)
    
    def rotPntsYAxis(self,ijkRange,angle,xzRotCent):
        '''Rotates points in ijkRange around a Y axis passing by xzRotCent.

        :param ijkRange: range for the search.
        :param angle: rotation angle (degrees)
        :param xzRotCent: coordinates [x,z] of the axis Y of rotation
        '''
        theta=math.radians(angle)
        sinTheta=math.sin(theta)
        cosTheta=math.cos(theta)
        sPtMove=self.getSetPntRange(ijkRange,'sPtMove')
        for p in sPtMove.getPoints:
            xp=p.getPos.x
            zp=p.getPos.z
            p.getPos.x= xzRotCent[0]+cosTheta*(xp-xzRotCent[0])-sinTheta*(zp-xzRotCent[1])
            p.getPos.z= xzRotCent[1]+sinTheta*(xp-xzRotCent[0])+cosTheta*(zp-xzRotCent[1])
        sPtMove.clear()

    def rotPntsXYZrangeYAxis(self,XYZrange,angle,xzRotCent):
        '''Rotates points in ijkRange around a Y axis passing by xzRotCent.

        :param XYZrange: range for the search.
        :param angle: rotation angle (degrees)
        :param xzRotCent: coordinates [x,z] of the axis Y of rotation
        '''
        ijkRange=self.getIJKrangeFromXYZrange(XYZrange)
        self.rotPntsYAxis(ijkRange,angle,xzRotCent)

    def rotAllPntsYAxis(self,angle,xzRotCent):
        '''Rotates all points in the grid around a X axis passing by yzRotCent.

        :param angle: rotation angle (degrees)
        :param xzRotCent: coordinates [z,z] of the axis Y of rotation
        '''
        ijkRange=IJKRange([0,0,0],[self.lastXIndex(),self.lastYIndex(),self.lastZIndex()])
        self.rotPntsYAxis(ijkRange,angle,xzRotCent)
    
    def scaleCoorXPointsRange(self,ijkRange,xOrig,scale):
        '''Applies a scale in X with origin xOrig (fixed axis: X=xOrig) 
        to the points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). Only X coordinate
        of points is modified in the following way:
        x_scaled=xOrig+scale*(x_inic-xOrig)

        :param ijkRange: range for the search.
        :param xOrig: origin X to apply scale (point in axis X=xOrig)
                      are not affected by the transformation 
        :param scale: scale to apply to X coordinate

        '''
        sPtXscale=self.getSetPntRange(ijkRange,'sPtXscale')
        for p in sPtXscale.getPoints:
            xpt=p.getPos.x
            p.getPos.x= xOrig+scale*(xpt-xOrig)
        sPtXscale.clear()

    def scaleCoorXPointsXYZRange(self,XYZrange,xOrig,scale):
        '''Applies a scale in X with origin xOrig (fixed axis: X=xOrig) 
        to the points in a 3D grid-region limited by
        one XYZrange defined as ((xmin,ymin,zmin),(xmax,ymax,zmax))
        Only X coordinate of points is modified in the following way:
        x_scaled=xOrig+scale*(x_inic-xOrig)

        :param XYZrange:((xmin,ymin,zmin),(xmax,ymax,zmax)) range for the search.
        :param xOrig: origin X to apply scale (point in axis X=xOrig)
                      are not affected by the transformation 
        :param scale: scale to apply to X coordinate

        '''
        IJKrange=self.getIJKrangeFromXYZrange(XYZrange)
        self.scaleCoorXPointsRange(IJKrange,xOrig,scale)
        
    def scaleCoorYPointsRange(self,ijkRange,yOrig,scale):
        '''Applies a scale in Y with origin yOrig (fixed axis: y=yOrig) 
        to the points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). Only Y coordinate
        of points is modified in the following way:
        y_scaled=yOrig+scale*(y_inic-yOrig)

        :param ijkRange: range for the search.
        :param yOrig: origin Y to apply scale (point in axis Y=yOrig)
                      are not affected by the transformation 
        :param scale: scale to apply to Y coordinate
        '''
        sPtYscale=self.getSetPntRange(ijkRange,'sPtYscale')
        for p in sPtYscale.getPoints:
            ypt=p.getPos.y
            p.getPos.y= yOrig+scale*(ypt-yOrig)
        sPtYscale.clear()

    def scaleCoorYPointsXYZrange(self,XYZrange,yOrig,scale):
        '''Applies a scale in Y with origin yOrig (fixed axis: y=yOrig) 
        to the points in a 3D grid-region limited by 
        an XYZrange defined as ((xmin,ymin,zmin),(xmax,ymax,zmax))
        Only Y coordinate of points is modified in the following way:
        y_scaled=yOrig+scale*(y_inic-yOrig)

        :param XYZrange:((xmin,ymin,zmin),(xmax,ymax,zmax)) range for the search.
        :param yOrig: origin Y to apply scale (point in axis Y=yOrig)
                      are not affected by the transformation 
        :param scale: scale to apply to Y coordinate
        '''
        IJKrange=self.getIJKrangeFromXYZrange(XYZrange)
        self.scaleCoorYPointsRange(IJKrange,yOrig,scale)
        
    def scaleCoorZPointsRange(self,ijkRange,zOrig,scale):
        '''Applies a scale in Z with origin zOrig (fixed axis: z=zOrig) 
        to the points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). Only Z coordinate
        of points is modified in the following way:
        z_scaled=zOrig+scale*(z_inic-zOrig)

        :param ijkRange: range for the search.
        :param zOrig: origin Z to apply scale (point in axis Z=zOrig)
                      are not affected by the transformation 
        :param scale: scale to apply to Z coordinate
        '''
        sPtZscale=self.getSetPntRange(ijkRange,'sPtZscale')
        for p in sPtZscale.getPoints:
            zpt=p.getPos.z
            p.getPos.z= zOrig+scale*(zpt-zOrig)
        sPtZscale.clear()

    def scaleCoorZPointsXYZrange(self,XYZrange,zOrig,scale):
        '''Applies a scale in Z with origin zOrig (fixed axis: z=zOrig) 
        to the points in a 3D grid-region limited by 
        one XYZrange defined as ((xmin,ymin,zmin),(xmax,ymax,zmax))
        Only Z coordinate of points is modified in the following way:
        z_scaled=zOrig+scale*(z_inic-zOrig)

        :param XYZrange:((xmin,ymin,zmin),(xmax,ymax,zmax)) range for the search.
        :param zOrig: origin Z to apply scale (point in axis Z=zOrig)
                      are not affected by the transformation 
        :param scale: scale to apply to Z coordinate
        '''
        IJKrange=self.getIJKrangeFromXYZrange(XYZrange)
        self.scaleCoorZPointsRange(IJKrange,zOrig,scale)

    def moveCylPointToRadius(self,pnt,radius,xCent=None,yCent=None):
        '''Move point in the cylindrical coordinate system 
        to radius coordinate given as parameter. By default, it will 
        take the center at the center of the self cylindrical coordinate-system, 
        although other X and/or Y coordinates can be given for the new circumference arc.

        :param pnt: point to move
        :param radius: radius 
        '''
        centXcoo= xCent if xCent else self.xCentCoo
        centYcoo= yCent if yCent else self.yCentCoo
        
        vdir=geom.Vector2d(pnt.getPos.x-centXcoo,pnt.getPos.y-centYcoo,).normalized()
        pnt.getPos.x= centXcoo+radius*vdir.x
        pnt.getPos.y= centYcoo+radius*vdir.y
         
        
    def moveCylPointsIJKrangeToRadius(self,ijkRange,radius,xCent=None,yCent=None):
        '''Move points in a 3D grid-region limited by the ijkRange 
        ((imin,jmin,kmin),(imax,jmax,kmax))
        in the cylindrical coordinate system to radius coordinate 
        given as parameter
        '''
        sPtMove=self.getSetPntRange(ijkRange,'sPtMove')
        for pnt in sPtMove.getPoints:
            self.moveCylPointToRadius(pnt,radius,xCent,yCent)
        sPtMove.clear()

    def moveCylPointsXYZrangeToRadius(self,xyzRange,radius,xCent=None,yCent=None):
        '''Move points in a 3D grid-region limited by the xyzRange
        ((xmin,ymin,zmin),(xmax,ymax,zmax))
        in the cylindrical coordinate system to radius coordinate 
        given as parameter
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        self.moveCylPointsIJKrangeToRadius(ijkRange,radius)

    def moveCylPointsIJKRangeToVarRadius(self,ijkRange,RangMin,RangMax,xCent=None,yCent=None):
        '''Move points in a 3D grid-region limited by the ijkRange
        ((imin,jmin,kmin),(imax,jmax,kmax))
        in the cylindrical coordinate system to a varibale radius 
        coordinate given as parameter 
        
        :param ijkRange: range of index
        :param RangMin: radius to which move point of minimum angle in each 
               of the ijRanges included in ijkRange 
        :param RangMax: radius to which move point of maximum angle in each 
               of the ijRanges included in ijkRange 
        '''
        lstIJranges=ijkRange.extractIncludedIJranges()
        for r in lstIJranges:
            (imin,jmin,kmin)=ijkRange.ijkMin
            (imax,jmax,kmax)=ijkRange.ijkMax
            angMin=self.gridCoo[1][jmin]
            angMax=self.gridCoo[1][jmax]
            slope=(RangMax-RangMin)/(angMax-angMin)
            for i in range(imin,imax+1):
                for j in range(jmin,jmax+1):
                    ang=self.gridCoo[1][j]
                    radius=RangMin+slope*(ang-angMin)
                    pnt=self.getPntGrid((i,j,kmin))
                    self.moveCylPointToRadius(pnt,radius,xCent,yCent)
                    
    def moveCylPointsXYXRangeToVarRadius(self,xyzRange,RangMin,RangMax,xCent=None,yCent=None):
        '''Move points in a 3D grid-region limited by the xyzRange
        ((xmin,ymin,zmin),(xmax,ymax,zmax))
        in the cylindrical coordinate system to a varibale radius 
        coordinate given as parameter 
        
        :param ijkRange: range of index
        :param RangMin: radius to which move point of minimum angle in each 
               of the ijRanges included in ijkRange 
        :param RangMax: radius to which move point of maximum angle in each 
               of the ijRanges included in ijkRange 
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        self.moveCylPointsIJKRangeToVarRadius(ijkRange,RangMin,RangMax)
                                        
        
    def movePointsRangeToZcylinder(self,ijkRange,xCent,yCent,R):
        '''Moves the points in the range to make them belong to 
        a cylinder with radius R and axis parallel to global Z passing 
        through the point (xCent,yCent,0).
        '''
        vCent=geom.Vector2d(xCent,yCent)
        setPnt=self.getSetPntRange(ijkRange,'setPnt')
        for p in setPnt.getPoints:
            v=geom.Vector2d(p.getPos.x,p.getPos.y)-vCent
            vdir= v.normalized()
            p.getPos.x=xCent+R*vdir.x
            p.getPos.y=yCent+R*vdir.y
        setPnt.clear()

    def movePointsRangeToXcylinder(self,ijkRange,yCent,zCent,R):
        '''Moves the points in the range to make them belong to 
        a cylinder with radius R and axis parallel to global X passing 
        through the point (0,yCent,zCent)
        '''
        vCent=geom.Vector2d(yCent,zCent)
        setPnt=self.getSetPntRange(ijkRange,'setPnt')
        for p in setPnt.getPoints:
            v=geom.Vector2d(p.getPos.y,p.getPos.z)-vCent
            vdir=v.normalized()
            p.getPos.y=yCent+R*vdir.x
            p.getPos.z=zCent+R*vdir.y
        setPnt.clear()

    def movePointsRangeToYcylinder(self,ijkRange,xCent,zCent,R):
        '''Moves the points in the range to make them belong to 
        a cylinder with radius R and axis parallel to global Y passing 
        through the point (xCent,0,zCent)
        '''
        vCent=geom.Vector2d(xCent,zCent)
        setPnt=self.getSetPntRange(ijkRange,'setPnt')
        for p in setPnt.getPoints:
            v=geom.Vector2d(p.getPos.x,p.getPos.z)-vCent
            vdir=v.normalized()
            p.getPos.x=xCent+R*vdir.x
            p.getPos.z=zCent+R*vdir.y
        setPnt.clear()

    def movePointsEllipse(self,ijkRange,semiAxX,semiAXY):
        ''''Move points in a 3D grid-region limited by the ijkRange 
        in the elliptical coordinate system to an ellipse of
        semi-axes semiAxX and semiAXY given as parameters
        '''
        sPtMove=self.getSetPntRange(ijkRange,'sPtMove')
        for p in sPtMove.getPoints:
            ang=math.atan2(p.getPos.y-self.yCentCoo,p.getPos.x-self.xCentCoo)
            paramCoo=math.sqrt((math.cos(ang)/semiAxX)**2+(math.sin(ang)/semiAXY)**2)
            p.getPos.x=self.xCentCoo+1/paramCoo*math.cos(ang)
            p.getPos.y= self.yCentCoo++1/paramCoo*math.sin(ang)
        sPtMove.clear()
            
    def newHexaedrGridVol(self,volName):
        '''Generate the hexaedral volume defined by the 8 vertex whose tags
        are implicit in the name of the volume.

        :param volName: name given to the grid volume
        return the hexaedral volumen
        '''
        # points= self.prep.getMultiBlockTopology.getPoints
        (tgPt1,tgPt2,tgPt3,tgPt4,tgPt5,tgPt6,tgPt7,tgPt8)=(int(volName[1:7]),int(volName[7:13]),int(volName[13:19]),int(volName[19:25]),int(volName[25:31]),int(volName[31:37]),int(volName[37:43]),int(volName[43:49]))
        volumes= self.prep.getMultiBlockTopology.getBodies
        hv= volumes.newBlockPts(tgPt1,tgPt2,tgPt3,tgPt4,tgPt5,tgPt6,tgPt7,tgPt8)
        hv.name= volName
#        hv.nDivI=1 #initialization values of number of divisions
#        hv.nDivJ=1
#        hv.nDivK=1
        return hv
    
    def newQuadGridSurface(self,surfName):
        '''Generate the quadrangle surface defined by the 4 vertex whose tags
        are implicit in the name of the surface.

        :param surfName: name given to the grid surface
        return the quadrangle surface
        '''
        # points= self.prep.getMultiBlockTopology.getPoints
        (tgPt1,tgPt2,tgPt3,tgPt4)=(int(surfName[1:7]),int(surfName[7:13]),int(surfName[13:19]),int(surfName[19:25]))
        surfaces= self.prep.getMultiBlockTopology.getSurfaces
        qs= surfaces.newQuadSurfacePts(tgPt1,tgPt2,tgPt3,tgPt4)
        qs.name= surfName
        qs.nDivI=1 #initialization values of number of divisions
        qs.nDivJ=1 
        return qs

    def getNameHexaedr(self,ind8Pnt):
        '''Return the name of the hexaedral volume defined by the 8 vertices 
        whose indices in the grid are passed as parameters. 

        :param ind8Pnt: tuple of ordered points defined by their grid indices 
                        (i,j,k)
        :return: the hexaedral volume
        '''
        (pto1,pto2,pto3,pto4,pto5,pto6,pto7,pto8)=tuple([self.getTagPntGrid(ind8Pnt[i]) for i in range(8)])
        nameVol= self.gridHexaedrName(pto1,pto2,pto3,pto4,pto5,pto6,pto7,pto8)
        return nameVol
    
    def getNameQuadGridSurface(self,ind4Pnt):
        '''Return the name of the quadrangle surface defined by the 4 vertices 
        whose indices in the grid are passed as parameters. 

        :param ind4Pnt: tuple of ordered points defined by their grid indices 
                        (i,j,k)
        :return: the quadrangle surface nane
        '''
        (pto1,pto2,pto3,pto4)=tuple([self.getTagPntGrid(ind4Pnt[i]) for i in range(4)])
        nameSurf= self.gridSurfName(pto1,pto2,pto3,pto4)
        return nameSurf

    def getNameGridLine(self,ind2Pnt):
        '''Return the name of the line defined by the 2 vertices 
        whose indices in the grid are passed as parameters. 

        :param ind2Pnt: tuple of ordered points defined by their grid indices 
                        (i,j,k)
        return the line
        '''
        (pto1,pto2)=tuple([self.getTagPntGrid(ind2Pnt[i]) for i in range(2)])
        nameLin= self.gridLinName(pto1,pto2)
        return nameLin

    def newGridLine(self,linName):
        '''Generate the line defined by the 2 end-points whose tags
        are implicit in the name of the line.

        :param linName: name given to the grid line
        return the line
        '''
        #points= self.prep.getMultiBlockTopology.getPoints
        (tgPt1,tgPt2)=(int(linName[1:7]),int(linName[7:15]))
        lines= self.prep.getMultiBlockTopology.getLines
        ln= lines.newLine(tgPt1,tgPt2)
        ln.name= linName
        ln.nDiv=1 #initialization value
        return ln

    def appendHexaedrRange(self,ijkRange,setVol,closeCyl='N'):
        '''generate the hexaedral volumes limited by a region defined by the 
        coordinates that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax)
        and append them to the set named 'setName'.
        Add those volumes to the dictionary dicHexaedr.

        :param ijkRange: instance of IJKRange class
        :param setVol: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        
        '''
        nmVolinRang=self.getNmHexaedrInRange(ijkRange,closeCyl)
        for nameVol in nmVolinRang:
            v= self.newHexaedrGridVol(nameVol)
            self.dicHexaedr[nameVol]= v
            setVol.getBodies.append(v)
            
    def appendSurfRange(self,ijkRange,setSurf,closeCyl='N'):
        '''generate the surfaces limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax)
        and append them to the set named 'setName'.
        Add those surfaces to the dictionary dicQuadSurf.

        :param ijkRange: instance of IJKRange class
        :param setSurf: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        
        '''
        nmSurfinRang=self.getNmSurfInRange(ijkRange,closeCyl)
        for nameSurf in nmSurfinRang:
            s= self.newQuadGridSurface(nameSurf)
            self.dicQuadSurf[nameSurf]= s
            setSurf.getSurfaces.append(s)

    def genHexaedrOneRegion(self,ijkRange,setName,closeCyl='N'):
        '''generate the hexaedral volumes limited by a region defined 
        by the coordinates that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax).
        Return a set with the volumes generated.
        Add those volumes to the dictionary dicHexaedr.

        :param ijkRange: instance of IJKRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
 
        '''
        setVol= self.prep.getSets.defSet(setName)
        self.appendHexaedrRange(ijkRange,setVol,closeCyl)
        return setVol
    
    def genSurfOneRegion(self,ijkRange,setName,closeCyl='N'):
        '''generate the surfaces limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax).
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param ijkRange: instance of IJKRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
 
        '''
        setSurf= self.prep.getSets.defSet(setName)
        self.appendSurfRange(ijkRange,setSurf,closeCyl)
        return setSurf
    
    def genHexaedrOneXYZRegion(self,xyzRange,setName,closeCyl='N'):
        '''generate the hexaedral volumes limited by a region defined by the 
        coordinates
        defined in the range  xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        Return a set with the volumes generated.
        Add those volumes to the dictionary dicHexaedr

        :param xyzRange: range xyz
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
         '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.genHexaedrOneRegion(ijkRange,setName,closeCyl)

    def genSurfOneXYZRegion(self,xyzRange,setName,closeCyl='N'):
        '''generate the surfaces limited by a region defined by the coordinates
        defined in the range  xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param xyzRange: range xyz
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
         '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.genSurfOneRegion(ijkRange,setName,closeCyl)

    def genHexaedrMultiRegion(self,lstIJKRange,setName,closeCyl='N'):
        '''generate the hexaedral volums limited by all the regions included in the 
        list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        Return a set with the volumes generated.
        Add those volumes to the dictionary dicHexaedr

        :param lstIJKRange: list of instances of IJKRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setVol= self.prep.getSets.defSet(setName)
        for rg in lstIJKRange:
            self.appendHexaedrRange(rg,setVol,closeCyl)
        return setVol
    
    def genSurfMultiRegion(self,lstIJKRange,setName,closeCyl='N'):
        '''generate the surfaces limited by all the regions included in the 
        list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param lstIJKRange: list of instances of IJKRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setSurf= self.prep.getSets.defSet(setName)
        for rg in lstIJKRange:
            self.appendSurfRange(rg,setSurf,closeCyl)
        return setSurf

    def genHexaedrMultiXYZRegion(self,lstXYZRange,setName,closeCyl='N'):
        '''generate the hexaedral volumes limited by all the regions included in the 
        list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the coordinates in ranges xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        Return a set with the volumes generated.
        Add those volumes to the dictionary dicHexaedr

        :param lstXYZRange: list of ranges xyz
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
         '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.genHexaedrMultiRegion(lstIJKRange,setName,closeCyl)
    
    
    def genSurfMultiXYZRegion(self,lstXYZRange,setName,closeCyl='N'):
        '''generate the surfaces limited by all the regions included in the 
        list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the coordinates in ranges xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        Return a set with the surfaces generated.
        Add those surfaces to the dictionary dicQuadSurf.

        :param lstXYZRange: list of ranges xyz
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
         '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.genSurfMultiRegion(lstIJKRange,setName,closeCyl)
        
    def appendLinRange(self,ijkRange,setName):
        '''generate the lines limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax)
        and append them to the set named 'setName'. 
        Add those lines to the dictionary dicLin.
        '''
        setLin= self.prep.getSets.getSet(setName)
        nmLinInRang=self.getNmLinInRange(ijkRange)
        for nameLin in nmLinInRang:
            l=self.newGridLine(nameLin)
            self.dicLin[nameLin]=l
            setLin.getLines.append(l)
       
    def genLinOneRegion(self,ijkRange,setName): 
        '''generate the lines limited by a region defined by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        setLin= self.prep.getSets.defSet(setName)
        self.appendLinRange(ijkRange,setName)
        return setLin

    def genLinOneXYZRegion(self,xyzRange,setName): 
        '''generate the lines limited by a region defined by the coordinates
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.genLinOneRegion(ijkRange,setName)
        
    def genLinMultiRegion(self,lstIJKRange,setName):
        '''generate the lines limited by all the regions included in the 
        list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        setLin= self.prep.getSets.defSet(setName)
        for rg in lstIJKRange:
            self.appendLinRange(rg,setName)
        return setLin

    def genLinMultiXYZRegion(self,lstXYZRange,setName):
        '''generate the lines limited by all the regions included in the 
        list of coordinate passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the ranges xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        Return a set with the lines generated.
        Add those lines to the dictionary dicLin.
        '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.genLinMultiRegion(lstIJKRange,setName)
        
    def getSetSurfOneRegion(self,ijkRange,setName,closeCyl='N'):
        '''return the set of surfaces and all the entities (lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax).

        :param ijkRange: instance of IJKRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setSurf= self.prep.getSets.defSet(setName)
        nmSurfinRang=self.getNmSurfInRange(ijkRange,closeCyl)
        for nameSurf in nmSurfinRang:
            if nameSurf in self.dicQuadSurf:
                setSurf.getSurfaces.append(self.dicQuadSurf[nameSurf])
        setSurf.fillDownwards()    
        return setSurf
    
    def getSetHexaedrOneRegion(self,ijkRange,setName,closeCyl='N'):
        '''return the set of hexaedral volumes and all the entities(surfaces,lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax).

        :param ijkRange: instance of IJKRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setVol= self.prep.getSets.defSet(setName)
        nmVolinRang=self.getNmHexaedrInRange(ijkRange,closeCyl)
        for nameVol in nmVolinRang:
            if nameVol in self.dicHexaedr:
                setVol.getBodies.append(self.dicHexaedr[nameVol])
        setVol.fillDownwards()    
        return setVol

    def getSubsetHexaedrOneRegion(self,superset,ijkRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities
        (surfaces,lines, points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetHexaedrOneRegion(ijkRange,'auxSet',closeCyl)
        subset=getSetIntersVol(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
        
    def getSubsetSurfOneRegion(self,superset,ijkRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfOneRegion(ijkRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
        
    def getSetHexaedrOneXYZRegion(self,xyzRange,setName,closeCyl='N'):
        '''return the set of volumes and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.getSetHexaedrOneRegion(ijkRange,setName,closeCyl)
        
    def getSetSurfOneXYZRegion(self,xyzRange,setName,closeCyl='N'):
        '''return the set of surfaces and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.getSetSurfOneRegion(ijkRange,setName,closeCyl)
        
    def getSubsetHexaedrOneXYZRegion(self,superset,xyzRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of volumes and all the entities
        (surfaces,lines, points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetHexaedrOneXYZRegion(xyzRange,'auxSet',closeCyl)
        subset=getSetIntersVol(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset

    def getSubsetSurfOneXYZRegion(self,superset,xyzRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines, 
        points, elements, nodes, ...) associated 
        with them in a region limited by the coordinates
        in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfOneXYZRegion(xyzRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset

    
    def getSetHexaedrMultiRegion(self,lstIJKRange,setName,closeCyl='N'):
        '''return the set of hexaedral volumes and all the entities
        (surfaces, lines, points, elements, nodes, ...) 
        associated with them in a all
        the regions  included in the list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 

        :param lstIJKRange: list of instances of IJKRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setVol= self.prep.getSets.defSet(setName)
        for rg in lstIJKRange:
            nmVolinRang=self.getNmHexaedrInRange(rg,closeCyl)
            for nameVol in nmVolinRang:
                if nameVol in self.dicHexaedr:
                    setVol.getBodies.append(self.dicHexaedr[nameVol])
        setVol.fillDownwards()    
        return setVol

    def getSetSurfMultiRegion(self,lstIJKRange,setName,closeCyl='N'):
        '''return the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 

        :param lstIJKRange: list of instances of IJKRange class
        :param setName: name of the set
        :param closeCyl: 'Y' to close cylinder when using cylindrical coordinate system
                        (defaults to 'N')
        '''
        setSurf= self.prep.getSets.defSet(setName)
        for rg in lstIJKRange:
            nmSurfinRang=self.getNmSurfInRange(rg,closeCyl)
            for nameSurf in nmSurfinRang:
                if nameSurf in self.dicQuadSurf:
                    setSurf.getSurfaces.append(self.dicQuadSurf[nameSurf])
        setSurf.fillDownwards()    
        return setSurf

    def getSubsetHexaedrMultiRegion(self,superset,lstIJKRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of hexaedral volumes and all the entities
        (lines, points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetHexaedrMultiRegion(lstIJKRange,'auxSet',closeCyl)
        subset=getSetIntersVol(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSubsetSurfMultiRegion(self,superset,lstIJKRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfMultiRegion(lstIJKRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetHexaedrMultiXYZRegion(self,lstXYZRange,setName,closeCyl='N'):
        '''return the set of volumes and all the entities(surfaces, lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to coordinates in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.getSetHexaedrMultiRegion(lstIJKRange,setName,closeCyl)
        
    def getSetSurfMultiXYZRegion(self,lstXYZRange,setName,closeCyl='N'):
        '''return the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to coordinates in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.getSetSurfMultiRegion(lstIJKRange,setName,closeCyl)
        
    def getSubsetHexaedrMultiXYZRegion(self,superset,lstXYZRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of volumes and all the entities
        (surfaces, lines, points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to coordinates in range 
        xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetHexaedrMultiXYZRegion(lstXYZRange,'auxSet',closeCyl)
        subset=getSetIntersVol(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSubsetSurfMultiXYZRegion(self,superset,lstXYZRange,nameSubset,closeCyl='N'):
        '''return from the set 'superset' the set of surfaces and all the entities(lines,
        points, elements, nodes, ...) associated with them in a all
        the regions  included in the list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to coordinates in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetSurfMultiXYZRegion(lstXYZRange,'auxSet',closeCyl)
        subset=getSetIntersSurf(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinOneRegion(self,ijkRange,setName):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        setLin= self.prep.getSets.defSet(setName)
        nmLininRang=self.getNmLinInRange(ijkRange)
        for nameLin in nmLininRang:
            if nameLin in self.dicLin:
                setLin.getLines.append(self.dicLin[nameLin])
        setLin.fillDownwards()    
        return setLin
        
    def getSubsetLinOneRegion(self,superset,ijkRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetLinOneRegion(ijkRange,'auxSet')
        subset=getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinOneXYZRegion(self,xyzRange,setName):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.getSetLinOneRegion(ijkRange,setName)

    def getSubsetLinOneXYZRegion(self,superset,xyzRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a region limited by the coordinates
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetLinOneXYZRegion(xyzRange,'auxSet')
        subset=getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinMultiRegion(self,lstIJKRange,setName):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        setLin= self.prep.getSets.defSet(setName)
        for rg in lstIJKRange:
            nmLininRang=self.getNmLinInRange(rg)
            for nameLin in nmLininRang:
                if nameLin in self.dicLin:
                    setLin.getLines.append(self.dicLin[nameLin])
        setLin.fillDownwards()    
        return setLin

    def getSubsetLinMultiRegion(self,superset,lstIJKRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of ijkRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        subset=self.prep.getSets.defSet(nameSubset)
        if self.prep.getSets.exists('auxSet'):
            self.prep.getSets.removeSet('auxSet') 
        auxSet= self.getSetLinMultiRegion(lstIJKRange,'auxSet')
        subset= getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getSetLinMultiXYZRegion(self,lstXYZRange,setName):
        '''return the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.getSetLinMultiRegion(lstIJKRange,setName)
        
    def getSubsetLinMultiXYZRegion(self,superset,lstXYZRange,nameSubset):
        '''return from the set 'superset' the set of lines and all the entities(points, elements, 
        nodes, ...) associated with them in a all the regions  included in the 
        list of xyzRanges passed as parameter.
        Each region defines a volume limited by the coordinates    
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        subset=self.prep.getSets.defSet(nameSubset)
        if self.prep.getSets.exists('auxSet'): self.prep.getSets.removeSet('auxSet') 
        auxSet=self.getSetLinMultiXYZRegion(lstXYZRange,'auxSet')
        subset=getSetIntersLin(superset,auxSet,nameSubset)
        subset.fillDownwards()
        return subset
    
    def getLstLinRange(self,ijkRange):
        '''return a list of lines in a region limited by the coordinates
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        retval=list()
        nmLinInRang=self.getNmLinInRange(ijkRange)
        for nameLin in nmLinInRang:
            if nameLin in self.dicLin:
                retval.append(self.dicLin[nameLin])
        return retval

    def getLstLinXYZRange(self,xyzRange):
        '''return a list of lines in a region limited by the coordinates
        defined in range xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.getLstLinRange(ijkRange)

    def getLstPntRange(self,ijkRange):
        '''return the ordered list points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax)

        :param ijkRange: range for the search
        '''
        retval=list()
        (imin,jmin,kmin)=ijkRange.ijkMin
        (imax,jmax,kmax)=ijkRange.ijkMax
        lstTagPnt=[self.getTagPntGrid(indPnt=(i,j,k)) for i in range(imin,imax+1) for j in range(jmin,jmax+1) for k in range(kmin,kmax+1)] #list of point tags to include in the set
        points= self.prep.getMultiBlockTopology.getPoints
        for tg in lstTagPnt:
          pnt=points.get(tg)
          retval.append(pnt)
        return retval
    
    def getLstPntXYZRange(self,xyzRange):
        '''return the ordered list points in a 3D grid-region limited by 
        coordinates defined in xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.getLstPntRange(ijkRange)
        
    def getSetPntRange(self,ijkRange,setName):
        '''return the set of points in a 3D grid-region limited by 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax)

        :param ijkRange: range for the search
        :param setName: name of the new set of points
        '''
        lstPnt=self.getLstPntRange(ijkRange)
        retval= self.prep.getSets.defSet(setName)
        pntsSet=retval.getPoints
        for pnt in lstPnt:
          pntsSet.append(pnt)
        return retval

    def getSetPntXYZRange(self,xyzRange,setName):
        '''return the set of points in a 3D grid-region limited by 
        coordinates in xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        ijkRange=self.getIJKrangeFromXYZrange(xyzRange)
        return self.getSetPntRange(ijkRange,setName)
        
    def getSetPntMultiRegion(self,lstIJKRange,setName):
        '''return the set of points in a all the 3D grid-regions
        included in the list of ijkRanges passed as parameter
        Each region defines a volume limited by the coordinates    
        that correspond to the indices in the grid 
        ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
        ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
        '''
        retval= self.prep.getSets.defSet(setName)
        pntsSet=retval.getPoints
        for rg in lstIJKRange:
            lstPnt=self.getLstPntRange(rg)
            for pnt in lstPnt:
                pntsSet.append(pnt)
        return retval
    
    def getSetPntMultiXYZRegion(self,lstXYZRange,setName):
        '''return the set of points in a all the 3D grid-regions
        included in the list of xyzRanges passed as parameter
        Each region defines a volume limited by the coordinates    
        defined in xyzRange=((xmin,ymin,zmin),(xmax,ymax,zmax))
        '''
        lstIJKRange=list()
        for rg in lstXYZRange:
            lstIJKRange.append(self.getIJKrangeFromXYZrange(rg))
        return self.getSetPntMultiRegion(lstIJKRange,setName)

    def genSetLinFromLstGridPnt(self,lstGridPnt,setName):
        '''generate the set of lines that joint the successive points defined 
        in 'lstGridPnt' by their indexes in the grid.

        :param lstGridPnt: list of successive points [(i1,j1,k1),(i2,j2,k2), ...]
        :param setName: name of the set.
        '''
        if self.prep.getSets.exists(setName):
            setLin= self.prep.getSets.getSet(setName)
        else:
            setLin= self.prep.getSets.defSet(setName)
        lines= self.prep.getMultiBlockTopology.getLines
        pntTags=[self.getPntGrid(pind).tag for pind in lstGridPnt]
        for i in range(len(pntTags)-1):
            l=lines.newLine(pntTags[i],pntTags[i+1])
            setLin.getLines.append(l)
            nameLin=self.gridLinName(pntTags[i],pntTags[i+1])
            self.dicLin[nameLin]=l
        return setLin

    def getSetLinFromLstGridPnt(self,lstGridPnt,setName):
        '''return the set of lines that joint the successive points defined 
        in 'lstGridPnt' by their indexes in the grid.

        :param lstGridPnt: list of successive points [(i1,j1,k1),(i2,j2,k2), ...]
        :param setName: name of the set.
        '''
        setLin= self.prep.getSets.defSet(setName)
        for i in range(len(lstGridPnt)-1):
            nameLin=self.getNameGridLine((lstGridPnt[i],lstGridPnt[i+1]))
            setLin.getLines.append(self.dicLin[nameLin])
        return setLin
             
    def genSetLinFromMultiLstGridPnt(self,multiLstGridPnt,setName):
        '''generate the set of lines that joint the successive points defined 
        in 'multiLstGridPnt' by their indexes in the grid.

        :param multiLstGridPnt: list of lists of successive points 
        [[(i1,j1,k1),(i2,j2,k2), ...],[],...]
        :param setName: name of the set.
        '''
        for lstp in  multiLstGridPnt:
            setLin=self.genSetLinFromLstGridPnt(lstp,setName)
        return setLin
    
    def getSetLinFromMultiLstGridPnt(self,multiLstGridPnt,setName):
        '''return the set of lines that joint the successive points defined 
        in 'multiLstGridPnt' by their indexes in the grid.

        :param multiLstGridPnt: list of lists of successive points 
        [[(i1,j1,k1),(i2,j2,k2), ...],[],...]
        :param setName: name of the set.
        '''
        for lstp in  multiLstGridPnt:
            setLin=self.getSetLinFromLstGridPnt(lstp,setName)
        return setLin
    
    def genSetLinFromLstXYZPnt(self,lstXYZPnt,setName):
        '''generate the set of lines that joint the successive points defined 
        in 'lstXYZPnt' by their coordinates (x,y,z).

        :param lstXYZPnt: list of successive points [(x1,y1,z1),(x2,y2,z2), ...]
        :param setName: name of the set.
        '''
        lstGridPnt=[self.getIJKfromXYZ(coord) for coord in lstXYZPnt]
        setLin=self.genSetLinFromLstGridPnt(lstGridPnt,setName)
        return setLin
    
    def getSetLinFromLstXYZPnt(self,lstXYZPnt,setName):
        '''return the set of lines that joint the successive points defined 
        in 'lstXYZPnt' by their coordinates (x,y,z).

        :param lstXYZPnt: list of successive points [(x1,y1,z1),(x2,y2,z2), ...]
        :param setName: name of the set.
        '''
        lstGridPnt=[self.getIJKfromXYZ(coord) for coord in lstXYZPnt]
        setLin=self.getSetLinFromLstGridPnt(lstGridPnt,setName)
        return setLin
    
    def genSetLinFromMultiLstXYZPnt(self,multiLstXYZPnt,setName):
        '''generate the set of lines that joint the successive points defined 
        in 'multiLstXYZPnt' by their coordinates (x,y,z)

        :param multiLstXYZPnt: list of lists of successive points 
        [[(x1,y1,z1),(x2,y2,z2), ...],[],...]
        :param setName: name of the set.
        '''
        multiLstGridPnt=list()
        for xyzLst in multiLstXYZPnt:
            indLst=[self.getIJKfromXYZ(coord) for coord in xyzLst]
            multiLstGridPnt.append(indLst)
        setLin=self.genSetLinFromMultiLstGridPnt(multiLstGridPnt,setName)
        return setLin
    
    def getSetLinFromMultiLstXYZPnt(self,multiLstXYZPnt,setName):
        '''generate the set of lines that joint the successive points defined 
        in 'multiLstXYZPnt' by their coordinates (x,y,z)

        :param multiLstXYZPnt: list of lists of successive points 
        [[(x1,y1,z1),(x2,y2,z2), ...],[],...]
        :param setName: name of the set.
        '''
        multiLstGridPnt=list()
        for xyzLst in multiLstXYZPnt:
            indLst=[self.getIJKfromXYZ(coord) for coord in xyzLst]
            multiLstGridPnt.append(indLst)
        setLin=self.getSetLinFromMultiLstGridPnt(multiLstGridPnt,setName)
        return setLin

def getSetIntersVol(set1,set2,setNameInter):
    '''Return a set of volumes, intersection of those in 'set1' and 'set2'

    :param set1, set2: sets containing volumes to search intersection
    :param setNameInter: name of the set of surfaces to return
    '''
    prep=set1.getPreprocessor
    if prep.getSets.exists(setNameInter): prep.getSets.removeSet(setNameInter) 
    setInters=prep.getSets.defSet(setNameInter)
    volSet1=set1.getBodies
    volSet2=set2.getBodies
    volSetInters=setInters.getBodies
    for s in volSet2:
        if s in volSet1: volSetInters.append(s)
    return setInters

def getSetIntersSurf(set1,set2,setNameInter):
    '''Return a set of surfaces intersection of those in 'set1' and 'set2'

    :param set1, set2: sets containing surfaces to search intersection
    :param setNameInter: name of the set of surfaces to return
    '''
    prep=set1.getPreprocessor
    if prep.getSets.exists(setNameInter): prep.getSets.removeSet(setNameInter) 
    setInters=prep.getSets.defSet(setNameInter)
    surfSet1=set1.getSurfaces
    surfSet2=set2.getSurfaces
    surfSetInters=setInters.getSurfaces
    for s in surfSet2:
        if s in surfSet1: surfSetInters.append(s)
    return setInters

def getSetIntersLin(set1,set2,setNameInter):
    '''Return a set of lines intersection of those in 'set1' and 'set2'

    :param set1, set2: sets containing lines to search intersection
    :param setNameInter: name of the set of lines to return
    '''
    prep=set1.getPreprocessor
    if prep.getSets.exists(setNameInter): prep.getSets.removeSet(setNameInter) 
    setInters=prep.getSets.defSet(setNameInter)
    linSet1=set1.getLines
    linSet2=set2.getLines
    linSetInters=setInters.getLines
    for s in linSet2:
        if s in linSet1: linSetInters.append(s)
    return setInters

