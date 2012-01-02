#!/usr/bin/python

#	#	#	#	#	#	#	#	#	#
# DGECS: Description Generator for Evolved Circuits Synthesis		#
# Abstract class for an Individual					#
#									#
# authors: Davide Basilio Bartolini, Matteo Carminati			#
#	#	#	#	#	#	#	#	#	#

class AbstractIndividual:

	def __init__(self, name):
		raise NotImplementedError

	def loadFile(self, path):
		raise NotImplementedError

	def exportFile(self, path):
		raise NotImplementedError

	def exportVHDLcomponent(self):
		raise NotImplementedError

	def getLevels(self):
		raise NotImplementedError

	def getLevel0Dimension(self):
		if self.getLevels > 0:
			raise NotImplementedError

	def getLevel0Range(self):
		if self.getLevels > 0:
			raise NotImplementedError

	def getLevel1Dimension(self):
		if self.getLevels > 1:
			raise NotImplementedError

	def getLevel1Range(self):
		if self.getLevels > 1:
			raise NotImplementedError

	def getLevel1CompsName(self):
		if self.getLevels > 1:
			raise NotImplementedError

	def getLevel2Dimension(self):
		if self.getLevels > 2:
			raise NotImplementedError

	def getLevel2Range(self):
		if self.getLevels > 2:
			raise NotImplementedError

	def getLevel2CompsName(self):
		if self.getLevels > 2:
			raise NotImplementedError

	def getLevel3Dimension(self):
		if self.getLevels > 3:
			raise NotImplementedError

	def getLevel3CompsName(self):
		if self.getLevels > 3:
			raise NotImplementedError
