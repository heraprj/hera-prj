#!/usr/bin/python

#	#	#	#	#	#	#	#	#	#
# DGECS: Description Generator for Evolved Circuits Synthesis		#
# Main window UI							#
#									#
# authors: Davide Basilio Bartolini, Matteo Carminati			#
#	#	#	#	#	#	#	#	#	#

import os, sys
from PyQt4 import QtGui, QtCore

# Definition of main frame with DGECS tools
class MainFrame(QtGui.QWidget):
	def __init__(self):
		self.individual = ''
		self.EditWindow= ''

		super(MainFrame, self).__init__()
		self.setWindowTitle('DGECS')
        	gridLayout = QtGui.QGridLayout()
	
		# Combobox for individual structure selection
		indSelect = QtGui.QGroupBox()
		indSelect.setTitle('Select individual type:')
		indSelectL = QtGui.QVBoxLayout()

		indSelectCB = QtGui.QComboBox()
		indSelectCB.addItem("Select target")
		vhdPath = "./vhd"
		try:
			folders=os.listdir(vhdPath)
		except OSError:
			print 'Error: no directory "%s" for supported evolved cirtuits found' % vhdPath
			sys.exit()
		if len(folders) == 0:
			print 'Error: no supported evolved circuits found in directory "%s"' % vhdPath
			sys.exit()
		for chip in folders:
			indSelectCB.addItem(chip)
		self.connect(indSelectCB, QtCore.SIGNAL('currentIndexChanged(int)'), self.changeIndividualType)
		indSelectL.addWidget(indSelectCB)

		indSelect.setLayout(indSelectL)
		gridLayout.addWidget(indSelect, 0, 0)

		# LUTS content definition (from file / by hand)
		LUTs = QtGui.QGroupBox()
		LUTs.setTitle('Edit LUT contents')
		LUTs.setEnabled(False)
		LUTsL = QtGui.QVBoxLayout()

		LUTsButtons = QtGui.QWidget()
		LUTsButtonsL = QtGui.QGridLayout()

		openFile = QtGui.QPushButton('Load from file')
		openFile.setGeometry(10, 10, 60, 35)
		self.connect(openFile, QtCore.SIGNAL('clicked()'), self.showFileDialog)
		LUTsButtonsL.addWidget(openFile, 0, 0)
		
		byHand = QtGui.QPushButton('Edit')
		byHand.setGeometry(10, 10, 60, 35)
		self.connect(byHand, QtCore.SIGNAL('clicked()'), self.showLUTDialog)
		LUTsButtonsL.addWidget(byHand, 0, 1)

		LUTsButtons.setLayout(LUTsButtonsL)
		LUTsL.addWidget(LUTsButtons)
		LUTs.setLayout(LUTsL)
		gridLayout.addWidget(LUTs, 1, 0)

		# Export VHDL files
		export = QtGui.QGroupBox()
		export.setTitle('Export')
		export.setEnabled(False)
		exportL = QtGui.QGridLayout()

		compNameL = QtGui.QLabel('Component name:')
		exportL.addWidget(compNameL, 0, 0)
		compName = QtGui.QLineEdit()
		exportL.addWidget(compName, 0, 1)

		versionL = QtGui.QLabel('Version:')
		exportL.addWidget(versionL, 1, 0)
		version = QtGui.QLineEdit()
		exportL.addWidget(version, 1, 1)
		#Workaround for stupi sbrubuntu (setPlaceholderText does not work)
		try:
			compName.setPlaceholderText('dgecs_component')
			version.setPlaceholderText('V1_00_a')
		except:
			compName.setText('dgecs_component')
			version.setText('V1_00_a')

		exportVHDL = QtGui.QPushButton('Generate VHDL')
		self.connect(exportVHDL, QtCore.SIGNAL('clicked()'), self.showExportVHDLDialog)
		exportL.addWidget(exportVHDL, 2, 0, 1, 2)

		export.setLayout(exportL)
		gridLayout.addWidget(export, 2, 0)

		self.setLayout(gridLayout)

	# Dialog for loading individual structure from file
	def showFileDialog(self):
		filename = QtGui.QFileDialog.getOpenFileName(self, 'Load LUT contents from file', os.getcwd(), "Lut contents files (.luts);;All files (*)")
		if filename != "":
			try:
				self.individual.loadFile(filename)
			except ValueError as e:
				QtGui.QMessageBox.warning(self, 'Error', e.args[0], QtGui.QMessageBox.Ok)
		else:
			print 'Warning: no path received; skipping export'
			
	# Dialog for editing the LUT values
	def showLUTDialog(self):
		e =__import__('gui.EditWindow', globals(), locals(), [''], -1)
		self.editWindow = e.EditWindow(self.individual, '0', {})
		self.editWindow.setModal(True)
		self.editWindow.show()

	# Dialog for exporting VHDL file(s)
	def showExportVHDLDialog(self):
		path = QtGui.QFileDialog.getExistingDirectory(self, 'Choose export directory', '/home', QtGui.QFileDialog.ShowDirsOnly | QtGui.QFileDialog.DontResolveSymlinks)
		if path == '':
			print 'Warning: no path received; skipping export'
			return

		try:
			name = self.layout().itemAt(2).widget().layout().itemAt(1).widget().displayText()
			version = self.layout().itemAt(2).widget().layout().itemAt(3).widget().displayText()
			if name == '':
				name = 'dgecs_component'
			if version == '':
				version = 'V1_00_a'
			self.individual.exportVHDLcomponent(path, name, version)
		except Exception as exc:
			QtGui.QMessageBox.warning(self, 'Error', 'Something went wrong while exporting the VHDL component:\n\n"' + str(exc) + '"', QtGui.QMessageBox.Ok)
		else:
			QtGui.QMessageBox.information(self, 'Exporting VDHL component', 'VHDL component exported succesfully!', QtGui.QMessageBox.Ok)
					

	# Update individual type
	def changeIndividualType(self, index):
		if index == 0:
			self.layout().itemAt(1).widget().setEnabled(False)
			self.layout().itemAt(2).widget().setEnabled(False)
		else:
			vhdPath = "./vhd"
			try:
				folders=os.listdir(vhdPath)
			except OSError:
				print 'Error: no directory "%s" for supported evolved cirtuits found' % vhdPath
				sys.exit()

			name = folders[index - 1]
			dataPath = "./data"
			try:
				folders=os.listdir(dataPath)
			except OSError:
				print 'Error: no directory "%s" found' % dataPath
				sys.exit()

			found = 0
			for f in folders:			
				if f == name+".py":
					found = 1
					break
			if found == 0:
				print 'Error: no file "%s" found in "%s"' % (name, dataPath)
				self.layout().itemAt(1).widget().setEnabled(False)
				self.layout().itemAt(2).widget().setEnabled(False)
			else:
				self.layout().itemAt(1).widget().setEnabled(True)
				self.layout().itemAt(2).widget().setEnabled(True)
				modname = "data."+name
				
				ind_module =__import__(modname, globals(), locals(), [''], -1)
				self.individual = ind_module.Individual(name)

# Main DGECS window
class DGECSWindow(QtGui.QMainWindow):

	# Window initialization
	def __init__(self):
	        QtGui.QMainWindow.__init__(self)
        	self.resize(400, 300)
	        self.setWindowTitle('DGECS')
		mainFrame = MainFrame()
		self.setCentralWidget(mainFrame)
