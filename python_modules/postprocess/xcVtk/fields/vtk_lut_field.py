 # -*- coding: utf-8 -*-
 
''' Drawing of diagrams showing a law of internal 
    forces (or any other input) on linear elements
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

from vtk.vtkCommonCore import vtkLookupTable
from vtk.vtkRenderingAnnotation import vtkScalarBarActor

class LUTField(object):
    '''Provides de variables involved in the drawing of a
    diagram showing a law of internal forces (or any other input)
    on linear elements

    :ivar fUnitConv:  units conversion factor
    :ivar lookUpTable: object that is used by mapper objects to map scalar values
                  into rga (red-green-blue-alpha transparency) color specification
                  or rga into scalar values. 
    :ivar scalarBar:  legend that indicates to the viewer the correspondence between 
                  color value and data value.
    :ivar mapper:     object that specifies the interface between data and graphic 
                  primitives.
    :ivar actor:      used to represent an entity in a rendering scene. The actor has position, 
                  orientation, scaling, a reference to the defining geometry, rendering 
                  properties and possibly a texture map.
    :ivar valMin: minimum value of the diagram.
    :ivar valMax: maximum value of the diagram.
    '''
    def __init__(self, fUnitConv):
        ''' Constructor.

        :param fUnitConv: unit conversion factor.
        '''
        self.initializeMinMax()
        self.lookUpTable= None
        self.scalarBar= None
        self.scalarBarTitle= None
        self.fUnitConv= fUnitConv
        self.mapper= None
        self.actor= None

    def updateMinMax(self,value):
        ''' Update the minimum and maximum values of the field.

        :param value: value to update the range with.
        ''' 
        self.valMin= min(self.valMin,value)
        self.valMax= max(self.valMax,value)

    def updateMinMaxWithinRange(self, value, rg):
        '''updates the minimum and maximum values of the scalar field
        forcing these extreme values within the range rg=(vmin,vmax)

        All the values less than vmin are represented in blue and those 
        greater than vmax are displayed in red

        :param value: value to update the range with.
        :param rg: range to force.
        '''
        self.valMin= min(self.valMin, max(value,rg[0]))
        self.valMax= max(self.valMax, min(value,rg[1]))

    def initializeMinMax(self, value= 1.0e99):
        '''Initialize minimum and maximum values that hopefully
           will be replaced by the real ones.''' 
        self.valMin= value #Extremely BIG (yes BIG) positive value.
        self.valMax= -self.valMin #Extremely BIG negative value.

    def rangeIsValid(self):
        return (self.valMax > -1e+99) or (self.valMin<1e+99)

    def createLookUpTable(self):
        ''' Creates a  lookUpTable, that is an object used by mapper objects 
        to map scalar values into rga (red-green-blue-alpha transparency) color 
        specification or rga into scalar values. 
        '''
        self.lookUpTable= vtkLookupTable()
        self.lookUpTable.SetNumberOfTableValues(1024)
        self.lookUpTable.SetHueRange(0.667,0)
        if(self.rangeIsValid()): # Range is valid.
            self.lookUpTable.SetTableRange(self.valMin,self.valMax)
        self.lookUpTable.Build()


    def updateLookUpTable(self):
        '''Sets the minimum and maximum scalar values for scalar mapping.
        Scalar values less than minimum and greater than maximum range values 
        are respectively clamped to those minimum and maximum range values
        '''
        self.lookUpTable.SetTableRange(self.valMin,self.valMax)
        self.lookUpTable.Build()

    def updateDiagramActor(self):
        '''Updates the diagram actor scalar range.'''
        self.mapper.SetScalarRange(self.valMin,self.valMax)

    def setScalarBarTitle(self, scalarBarTitle):
        '''Set the title of the scale bar.

        :param scalarBarTitle: title for the field scale bar.
        '''
        self.scalarBarTitle= scalarBarTitle

    def getScalarBarTitle(self):
        ''' Return the title of the field scale bar.'''
        return self.scalarBarTitle

    def creaColorScaleBar(self, orientation=1, title=None):
        '''Creates the scalar bar that indicates to the viewer the correspondence
        between color values and data values

        :param orientation: 1 for horizontal bar, 2 for left-vertical bar 
                             3 for right-vertical bar(defaults to horizontal)
        :param title: title of the new scale bar.
        '''
        if(title is not None):
            self.setScalarBarTitle(scalarBarTitle= title)
        scalarBarTitle= self.getScalarBarTitle()
        self.scalarBar= vtkScalarBarActor()
        
        pos= self.scalarBar.GetPositionCoordinate()
        pos.SetCoordinateSystemToNormalizedViewport()
        if orientation>1: #vertical
            self.scalarBar.SetOrientationToVertical()
            self.scalarBar.SetWidth(0.075)
            self.scalarBar.SetHeight(0.7)
            pos.SetValue(0.04,0.20) if orientation==2 else pos.SetValue(0.87,0.20)
            if scalarBarTitle:
                scalarBarTitle= scalarBarTitle.replace(' ',' \n ')
                self.scalarBar.SetTitle(scalarBarTitle)
        else: #horizontal
            self.scalarBar.SetOrientationToHorizontal()
            self.scalarBar.SetWidth(0.8)
            self.scalarBar.SetHeight(0.055)
            pos.SetValue(0.1,0.07)
            if scalarBarTitle:
                self.scalarBar.SetHeight(0.09)
                self.scalarBar.SetTitle(scalarBarTitle)
        self.scalarBar.SetLookupTable(self.lookUpTable)
        self.scalarBar.Modified()
        #self.scalarBar.SetLabelFormat("%.2f")
        labelTextProperty=  self.scalarBar.GetLabelTextProperty()
        labelTextProperty.ItalicOff()
        return self.scalarBar

    def creaVertColorScaleBar(self):
        '''Creates the scalar bar that indicates to the viewer the correspondence
        between color values and data values
        '''

        self.scalarBar= vtkScalarBarActor()

        pos= self.scalarBar.GetPositionCoordinate()
        pos.SetCoordinateSystemToNormalizedViewport()
        pos.SetValue(0.85,0.25)        

        self.scalarBar.SetOrientationToVertical()
        self.scalarBar.SetWidth(0.1)
        self.scalarBar.SetHeight(0.7)
        self.scalarBar.SetLookupTable(self.lookUpTable)
        self.scalarBar.Modified()
        return self.scalarBar


