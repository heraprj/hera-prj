#!/usr/bin/python

#	#	#	#	#	#	#	#	#	#
# DGECS: Description Generator for Evolved Circuits Synthesis		#
# Button for individual components					#
#									#
# authors: Davide Basilio Bartolini, Matteo Carminati			#
#	#	#	#	#	#	#	#	#	#

from PyQt4 import QtGui, QtCore

class ComponentButton(QtGui.QPushButton):
	def __init__(self, label, level, coord0, coord1):
		super(ComponentButton, self).__init__()
		self.setText(label)
		self.level = level
		self.coord0 = int(coord0)
		self.coord1 = int(coord1)

	def getCoord0(self):
		return self.coord0

	def getCoord1(self):
		return self.coord1

	def getLevel(self):
		return self.level
