# -*- coding: utf-8 -*-
from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


import os
from misc_utils import log_messages as lmsg
from misc_utils import data_struct_utils as su
from postprocess.xcVtk import vtk_graphic_base
from postprocess.xcVtk.FE_model import vtk_FE_graphic
from postprocess.xcVtk.fields import fields
from postprocess.control_vars import *
from postprocess import control_vars

import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable

def convert_to_eps(fileName, epsFileName):
    ''' Uses ImageMagick convert command to create a postcript file.

    :param fileName: name of the graphic file to convert.
    :param epsFileName: postscript file to create.
    '''
    os.system("convert "+ fileName + " " + epsFileName)

class setToDisplay(object):
    '''Defines the description of a set of elements to be used in the graphics
    and reports associated with the calculation

    :ivar elSet:    set of elements
    :ivar genDescr: general description
    :ivar sectDescr: ordered list with the descriptions that apply to each of the sections that configures the element.
    '''
    def __init__(self, elSet, genDescr='', sectDescr=[]):
        ''' Constructor.

        :param elSet:    set of elements
        :param genDescr: general description
        :param sectDescr: ordered list with the descriptions that apply to each of the sections that configures the element.
        '''
        self.elSet=elSet
        self.elSet.fillDownwards()
        self.genDescr=genDescr
        self.sectDescr=sectDescr

class FigureBase(object):
    ''' Base of the objects used to create figures.

    :ivar pLabel: part label; something like 'wall' or '2ndFloorDeck'
    :ivar limitStateLabel; limit state check label; Something like "Fatigue" or "CrackControl"
    :ivar figDescr: figure description; text to insert as caption in the figure file and int the LaTeX file.
    :ivar units: units displayed; something like '[MPa]' or 'radians'...
    :ivar reinfDescr: reinforcement description; sSomething like "horizontal reinforcement."
    :ivar sz: LaTeX size for the figure.
    '''
    def __init__(self,pLabel,limitStateLabel,figDescr,reinfDescr=None,units=None,sz= "90mm"):
        ''' Figure base constructor.

        :param pLabel: part label; something like 'wall' or '2ndFloorDeck'
        :param limitStateLabel; limit state check label; Something like "Fatigue" or "CrackControl"
        :param figDescr: figure description; text to insert as caption in the figure file and int the LaTeX file.
        :param units: units displayed; something like '[MPa]' or 'radians'...
        :param reinfDescr: reinforcement description; sSomething like "horizontal reinforcement."
        :param sz: LaTeX size for the figure.
        '''
        self.partLabel= pLabel #Something like 'wall' or '2ndFloorDeck'
        self.limitStateLabel= limitStateLabel #Something like "Fatigue" or "CrackControl"
        self.attributeName= ''
        self.figDescription= figDescr #Text to insert as caption in the LaTeX file.
        self.unitsLabel= units # Somethin like '[MPa]' or 'radians'...
        self.reinforcementDescription= reinfDescr #Something like "horizontal reinforcement."
        self.figSize= sz #LaTeX size for the figure.
        self.cameraParameters= vtk_graphic_base.CameraParameters('XYZPos')
      
    def getCaption(self):
        ''' Return the figure caption.'''
        retval= self.partLabel+'. '+self.figDescription
        if(self.unitsLabel!=None):
            retval+= ' ['+ self.unitsLabel +']'
        if(self.reinforcementDescription!=None):
            retval+= '. ' + self.reinforcementDescription
        return retval
    
    def getFileName(self):
        ''' Return the file name to store the bitmap.'''
        return su.slugify(self.partLabel+self.limitStateLabel+self.attributeName)
    
    def insertIntoLatex(self, fichLatexFigs, fichLatexList, fichFig, labelText):
        '''
        :param fichLatexFigs: latex file to insert graphic into
        :param fichFig: name of the file that contains the graphic
                        (complete path without extension).
        '''
        fichLatexFigs.write('\\begin{figure}\n' )
        fichLatexFigs.write('\\centering\n' )
        fichLatexFigs.write('\\includegraphics[width=' + self.figSize + ']{' + fichFig + '}\n' )
        fichLatexFigs.write('\\caption{'+ self.getCaption() + '} ' )
        fichLatexFigs.write('\\label{'+ labelText + '}\n' )
        fichLatexFigs.write('\\end{figure}\n \n')
        fichLatexList.write('\\item Figure \\ref{' )
        fichLatexList.write(labelText)
        fichLatexList.write('}: ' + self.getCaption() + '\n' )

class SlideDefinition(FigureBase):
    ''' Slide definition.

    :ivar field: field to display in the slide.
    :ivar diagrams: list of diagrams to display in the slide.
    '''
    def __init__(self,pLabel,limitStateLabel,figDescr,reinfDescr=None,units=None,sz= "90mm"):
        ''' Slide constructor.

        :param pLabel: part label; something like 'wall' or '2ndFloorDeck'
        :param limitStateLabel; limit state check label; Something like "Fatigue" or "CrackControl"
        :param figDescr: figure description; text to insert as caption in the figure file and int the LaTeX file.
        :param reinfDescr: reinforcement description; sSomething like "horizontal reinforcement."
        :param units: units displayed; something like '[MPa]' or 'radians'...
        :param sz: LaTeX size for the figure.
        '''
        super(SlideDefinition,self).__init__(pLabel,limitStateLabel,figDescr,reinfDescr,units,sz)
        self.field= None
        self.diagrams= list()

    def setupDiagrams(self):
        ''' Call addDiagram on each diagram of the list.'''
        for d in self.diagrams:
            d.addDiagram()

    def genGraphicFile(self, displaySettings, xcSet, graphFileName, convertToEPS= False):
        ''' Create a graphic file.

        :param displaySettings: variables that define the ouput device.
        :param xcSet: set to display.
        :param graphFileName: name of the graphic file.
        :param convertToEPS: if true use ImageMagick convert to create a postrscript file.
        '''
        preprocessor= self.getPreprocessor
        jpegName= graphFileName+".jpeg"
        epsName= graphFileName+".eps"
        self.setupDiagrams()
        displaySettings.cameraParameters= self.cameraParameters
        if(self.field):
            self.field.plot(preprocessor, displaySettings, graphFileName)
        else:
            displaySettings.displayMesh(xcSet,None,self.diagrams,jpegName,self.getCaption())
        if(convertToEPS):
            convert_to_eps(jpegName,epsName)
  

class FigureDefinition(SlideDefinition):
    ''' Definition of a figure.

    :ivar attributeName: name of the attribute to display.
    :ivar argument: argument for the attribute.
    '''
    def __init__(self, pLabel, limitStateLabel, attrName, argument, figDescr, reinfDescr=None, units=None, sz= "90mm"):
        ''' Figure constructor.

        :param pLabel: part label as defined in model; something like 'wall' or '2ndFloorDeck'
        :param limitStateLabel; limit state check label; Something like "Fatigue" or "CrackControl"
        :param figDescr: figure description; text to insert as caption in the figure file and int the LaTeX file.
        :param units: units displayed; something like '[MPa]' or 'radians'...
        :param reinfDescr: reinforcement description; sSomething like "horizontal reinforcement."
        :param sz: LaTeX size for the figure.
        '''
        super(FigureDefinition,self).__init__(pLabel,limitStateLabel,figDescr,reinfDescr,units,sz)
        self.attributeName= attrName
        self.argument= argument

    def defField(self, xcSet):
        ''' Define field.'''
        #self.field= fields.ExtrapolatedScalarField(self.attributeName,"getProp",None,1.0,xcSet)
        self.field= fields.get_scalar_field_from_control_var(attributeName=self.attributeName,argument=self.argument,xcSet=xcSet,component=None,fUnitConv=1.0,rgMinMax=None)

    def genGraphicFile(self,displaySettings, xcSet, graphFileName, convertToEPS= False):
        ''' Create a graphic file.

        :param displaySettings: variables that define the ouput device.
        :param xcSet: set to display.
        :param graphFileName: name of the graphic file.
        :param convertToEPS: if true use ImageMagick convert to create a postrscript file.
        '''
        jpegName= graphFileName+".jpeg"
        epsName= graphFileName+".eps"
        self.defField(xcSet)
        self.field.plot(displaySettings,jpegName,self.getCaption())
        if(convertToEPS):
            convert_to_eps(jpegName,epsName)
 
class TakePhotos(object):
    '''Generation of bitmaps with analysis and design results.

    :ivar pthGraphOutput: path to the directory to put the graphics in.
    :ivar pthTextOutput: path to the directory to put the texts in.
    :ivar fichLatexFigs: Latex file to include figures (defaults to None).
    :ivar fichLatexList: Latex file with figures list (defaults to None).
    '''
    
    def __init__(self,xcSet):
        ''' Constructor.

        :param xcSet: set containing the objects that will be displayed.
        '''
        self.displaySettings= None
        self.xcSet= xcSet
        self.displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        self.pthGraphOutput= '/tmp/'  #Directory to put the graphics in.
        self.pthTextOutput= '/tmp/'  #Directory to put the texts in.
        self.fichLatexFigs= None #Latex file to include figures.
        self.fichLatexList= None #Latex file with figures list.

    def insertFigureLatex(self, figDef, conta, fichFig,labelText):
        '''

        :param fichLatexFigs: latex file to insert graphic into
        :param fichFig: name of the file that contains the graphic
                        (complete path without extension).
        '''
        figDef.insertIntoLatex(self.fichLatexFigs,self.fichLatexList,fichFig,labelText)
        if((conta>0) & (conta % 10==0)):
            self.fichLatexFigs.write('\\clearpage\n' )

    def displayFigures(self,figDefinitionList,LatexFigsFilename,LatexListFilename):
        '''Creates graphics files from figure definition list.

        :param nmbLstIss: name of the lists that contains the results to display.
        :param LatexFilename: name of the LaTeX file to write the graphics on.
        '''
        conta= 0
        self.fichLatexFigs= open(self.pthTextOutput+LatexFigsFilename,'w')
        self.fichLatexList= open(self.pthTextOutput+LatexListFilename,'w')
        self.fichLatexList.write('\\begin{itemize}\n' )
        for figDef in figDefinitionList:
            bitmapFilename= self.pthGraphOutput+figDef.getFileName()
            figDef.genGraphicFile(self.displaySettings, self.xcSet,bitmapFilename)
            conta+= 1
            self.insertFigureLatex(figDef,conta,bitmapFilename,"fg_"+figDef.getFileName())
        self.fichLatexFigs.close()
        self.fichLatexList.write('\\end{itemize}\n' )
        self.fichLatexList.close()
        return

    def plotFigures(self,preprocessor,figDefinitionList,LatexFigsFilename,LatexListFilename):
        lmsg.warning('plotFigures DEPRECATED; use displayFigures.')
        self.displayFigures(figDefinitionList,LatexFigsFilename,LatexListFilename)

class PartToDisplay(object):
    def __init__(self,partName, surfaceList,reinforcementLabels):
        ''' PartsToDisplay constructor

        :param partName: name assigned to the part to show.
        :param surfaceList: list of surfaces to show (with the names used in the model).
        :param reinforcementLabels: labels to identify the rebars (longitudinal reinf, vertical reinf,...).    
        '''
        self.partName= partName
        self.surfaceList= surfaceList
        self.reinforcementLabels= reinforcementLabels
      
    def getShortName(self):
        return su.slugify(self.partName)
    
    def getElements(self):
        '''Returns a list of the elements of this part.'''
        retval= list()
        for sup in self.surfaceList:
            elSup= sup.elements
            for elem in elSup:
                retval.append(elem)
        return retval
    
    def getElementSet(self, preprocessor):
        ''' Return the elements contained in this object in a XC set.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        elementSetName= self.getShortName()+'_elementSet'
        elems= self.elements
        # Define the set
        self.xcSet= preprocessor.getSets.defSet(elementSetName)
        for e in elems:
            self.xcSet.elements.append(e)
        self.xcSet.fillDownwards()
        return self.xcSet
    
    def display(self,preprocessor, tp, resultsToDisplay):
        '''Generate an image for every result to display

        :param preprocessor: preprocessor of the finite element problem.
        :param tp: TakePhoto object to use to capture the image.
        :param resultToDisplay: collection of results to be displayed.
        '''
        elementSet= self.getElementSet(preprocessor)
        tp.elementSetName= elementSet.name
        resultsToDisplay.display(tp,self)
       

class PartToDisplayContainer(dict):
    ''' Parts to display in figures... '''
    
    def __init__(self,lst):
        for l in lst:
            self.add(l)
        
    def add(self,part):
        self[part.getShortName()]= part
      
    def display(self, preprocessor, tp, resultsToDisplay):
        '''Display results for each part.

        :param preprocessor: preprocessor of the finite element problem.
        :param tp: TakePhoto object to use to capture the image.
        :param resultToDisplay: collection of results to be displayed.
        '''
        #Load properties to display:
        fName= resultsToDisplay.limitStateData.getOutputDataFileName()
        control_vars.readControlVars(preprocessor= preprocessor, inputFileName= fName)
        for k in self.keys():
            part= self[k]
            part.display(preprocessor, tp, resultsToDisplay)

def plotStressStrainFibSet(fiberSet,title,fileName=None,nContours=None,pointSize=50, fiberShape='o'):
    '''Represents graphically the cross-section current stresses and strains.
    The graphics are generated by a triangulation from the x,y coordinates of
    the fibers.

    :param fiberSet: set of fibers to be represented
    :param title:    general title for the graphic
    :param fileName: name of the graphic file (defaults to None: no file generated)
    :param nContours: number of contours to be generated . If 
           nContours=0 or nContours=None, then each fiber is represented by a
           marker (defaults to None).
    :param pointSize: size of the circles to represent each of the fibers in the 
           set in the case that nContours=0 or nContours=None (defaults to 50)
    :param fiberShape: marker to represent each fiber, in case nContours = 0 or 
                   None.e.g.: "o"->circle, "s"->square, "p"->pentagon (defaults
                   to circle)
    '''
    lsYcoo=list()
    lsZcoo=list()
    lsStrain=list()
    lsStress=list()
    for f in fiberSet:
        fpos=f.getPos()
        lsYcoo.append(fpos.x)
        lsZcoo.append(fpos.y)
        lsStrain.append(f.getStrain())
        lsStress.append(f.getForce()/f.getArea()/1e6)
    fig,(ax1,ax2) = plt.subplots(1,2, sharex=True, sharey=True)
    fig.suptitle(title, fontsize=20)
    ax1.set_title('Strain')
    if nContours in [None,0]:
        im1=ax1.scatter(lsYcoo,lsZcoo,s=pointSize,c=lsStrain,marker=fiberShape)
        im2=ax2.scatter(lsYcoo,lsZcoo,s=pointSize,c=lsStress,marker=fiberShape)
    else:
        im1=ax1.tricontourf(lsYcoo,lsZcoo,lsStrain, nContours)
        im2=ax2.tricontourf(lsYcoo,lsZcoo,lsStress, nContours)
    divider1 = make_axes_locatable(ax1)
    cax1= divider1.append_axes("right", size="20%", pad=0.05)
    cbar1= plt.colorbar(im1,cax=cax1)
    if(__debug__):
        if(not cbar1):
            AssertionError('Can\'t set color bar.')
    ax2.set_title('Stress')
  #  im2=ax2.tricontourf(lsYcoo,lsZcoo,lsStress, nContours)
  #  im2=ax2.scatter(lsYcoo,lsZcoo,50,lsStress)
    divider2 = make_axes_locatable(ax2)
    cax2= divider2.append_axes("right", size="20%", pad=0.05)
    cbar2= plt.colorbar(im2,cax=cax2)
    if(__debug__):
        if(not cbar2):
            AssertionError('Can\'t set color bar.')
    if(fileName!=None):
        plt.savefig(fileName)
    plt.show()
    return
