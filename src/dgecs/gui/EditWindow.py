#!/usr/bin/python

#	#	#	#	#	#	#	#	#	#
# DGECS: Description Generator for Evolved Circuits Synthesis		#
# Window for editing Cell contents					#
#									#
# authors: Davide Basilio Bartolini, Matteo Carminati			#
#	#	#	#	#	#	#	#	#	#

import os, sys
from PyQt4 import QtGui, QtCore
import ComponentButton as CB
import ValueEdit as VE

class EditWindow(QtGui.QDialog):
	def __init__(self, individual, curLevel, curPos):
		super(EditWindow, self).__init__()
		self.individual = individual
		self.nextLevel = str(int(curLevel) + 1)
		self.curLevel = curLevel
		self.setWindowTitle('Edit individual Genotype')
        	self.gL = QtGui.QGridLayout()
		self.curPos = curPos
		self.values={}

		if curLevel == '0':
			self.paintLevel('')	
	
	def paintLevel(self, prevDim):
		els = QtGui.QGroupBox()
        	gridLayout = QtGui.QGridLayout()
		testLevel = 'getLevel' + self.curLevel + 'Dimension'
		dim = getattr(self.individual, testLevel)()
		testNextLevel = 'getLevel' + self.nextLevel + 'Dimension'
		nextDim = getattr(self.individual, testNextLevel)()
		getNextName = 'getLevel' + self.nextLevel + 'CompsName'
		nextName = getattr(self.individual, getNextName)()

		getRange = 'getLevel' + self.curLevel + 'Range'
		if dim == 2:
			mx,Mx,my,My = getattr(self.individual, getRange)()
			for x in range(mx,Mx + 1):
				for y in range(my,My + 1):
					#inverted coords so that it looks like in the figures (input from left)
					self.paintWidgets(dim, nextDim, gridLayout, nextName, '(' + str(x) + ',' + str(y) + ')', (x,y), x, y)
		elif dim == 1:
			mx,Mx = getattr(self.individual, getRange)()
			for x in range(mx,Mx + 1):
				self.paintWidgets(dim, nextDim, gridLayout, nextName, '(' + str(x) + ')', (x,), x, 0)
		else:
			raise ValueError('Bad dimension value in selected individual at level %d') % self.curLevel
		els.setLayout(gridLayout)
		self.gL.addWidget(els)
		
		if self.curLevel == '0':
			els.setTitle('Individual')
			exportButton = QtGui.QPushButton('Export Individual')
			self.connect(exportButton, QtCore.SIGNAL('clicked()'), self.showExportDialog)
			self.gL.addWidget(exportButton)
		else:
			getLevName = 'getLevel' + self.curLevel + 'CompsName'
			levName = getattr(self.individual, getLevName)()
			if prevDim == 2:
				levName = levName + '(' + str(self.sender().getCoord0()) + ',' + str(self.sender().getCoord1()) + ')'
			elif prevDim == 1:
				levName = levName + '(' + str(self.sender().getCoord1()) + ')'
			else:
				raise ValueError('Bad dimension value in selected individual at level %d') % self.curLevel
			els.setTitle(self.sender().text())

		if nextDim == 0:
			cancelButton = QtGui.QPushButton('Cancel')
			self.connect(cancelButton, QtCore.SIGNAL('clicked()'), (lambda: self.done(0)))
			saveButton = QtGui.QPushButton('Save values')
			self.connect(saveButton, QtCore.SIGNAL('clicked()'), self.saveValues)
			self.gL.addWidget(cancelButton)
			self.gL.addWidget(saveButton)
		
		self.setLayout(self.gL)

	def paintWidgets(self, dim, nextDim, gridLayout, nextName, posName, thisPos, x, y):
		if nextDim > 0:
			el = CB.ComponentButton(nextName + posName, self.curLevel, x, y)
			self.connect(el, QtCore.SIGNAL('clicked()'), (lambda: self.newLevelWin(dim)))
			if dim == 1:
				gridLayout.addWidget(el, x, 0) 
			elif dim == 2:
				gridLayout.addWidget(el, y, x) 
		else:
			el = QtGui.QLabel(nextName + posName + ':')
			gridLayout.addWidget(el, x, y * 2)

			el = VE.ValueEdit(self.curLevel, x, y)
			el.setInputMask('hhhh')
			pos = self.getPos(thisPos)
			val = self.individual.getValue(pos)
			el.setText(str(val))
			self.values[(x,y)] = {}
			self.values[(x,y)][0] = str(val)
			self.values[(x,y)][1] = (x,y)
			self.connect(el, QtCore.SIGNAL('textChanged(const QString &)'), self.updateValue)
			gridLayout.addWidget(el, x, y * 2 + 1)

	def newLevelWin(self, dim):
		lev = self.sender().getLevel()
		if dim == 2:
			self.curPos[lev] = (self.sender().getCoord0(),self.sender().getCoord1())
		elif dim == 1:
			self.curPos[lev] = (self.sender().getCoord0(),)
		self.nextLevEdit = EditWindow(self.individual, self.nextLevel, self.curPos)
		self.nextLevEdit.paintLevel(dim)
		self.nextLevEdit.setModal(True)
		self.nextLevEdit.show()

	def saveValues(self):
		for val in self.values:
			thisPos = self.values[val][1]
			pos = self.getPos(thisPos)
			self.individual.setValue(pos, self.values[val][0])
		self.done(0)
	
	def updateValue(self, val):
		self.values[(self.sender().getCoord0(),self.sender().getCoord1())][0] = val.toUtf8().data()
	
	def getPos(self, thisPos):
		pos = ()
		for l in range(0,int(self.curLevel)):
			pos += self.curPos[str(l)]
		pos += thisPos
		return pos

	def showExportDialog(self):
		path = QtGui.QFileDialog.getSaveFileName(self, 'Choose export file', os.getcwd(), "Lut contents files (.luts);;All files (*)")
		self.individual.exportFile(path)
