#!/usr/bin/python

#	#	#	#	#	#	#	#	#	#
# DGECS: Description Generator for Evolved Circuits Synthesis		#
# LineEdit for changing Cell value					#
#									#
# authors: Davide Basilio Bartolini, Matteo Carminati			#
#	#	#	#	#	#	#	#	#	#

from PyQt4 import QtGui, QtCore

class ValueEdit(QtGui.QLineEdit):
	def __init__(self, level, coord0, coord1):
		super(ValueEdit, self).__init__()
		self.level = level
		self.coord0 = int(coord0)
		self.coord1 = int(coord1)

	def getCoord0(self):
		return self.coord0

	def getCoord1(self):
		return self.coord1

	def getLevel(self):
		return self.level
