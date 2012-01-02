#!/usr/bin/python

#	#	#	#	#	#	#	#	#	#
# DGECS: Description Generator for Evolved Circuits Synthesis		#
# XC4VFX12_comb Individual class					#
#									#
# authors: Davide Basilio Bartolini, Matteo Carminati			#
#	#	#	#	#	#	#	#	#	#

import Individual
import os
import shutil, fileinput

class Cell:
	def __init__(self):
		self.value = '0000'

	def setValue(self, val):
		self.value = val

	def getValue(self):
		return self.value

class Column:
	def __init__(self):
		self.cells = {}
		for c in range(0, 8):
			self.cells[c] = Cell()

	def getCell(self, pos):
		return self.cells[pos]

class Module:
	def __init__(self):
		self.columns = {}
		for c in range(0, 4):
			self.columns[c] = Column()

	def getColumn(self, pos):
		return self.columns[pos]

class Individual(Individual.AbstractIndividual):
	def __init__(self, name):
		self.name = name
		self.modules = {}
		for My in range(0, 4):
			for Mx in range(0, 4):
				self.modules[(Mx, My)] = Module()

	def reset(self):
		for My in range(0, 4):
			for Mx in range(0, 4):
				for C in range(0, 4):
					for L in range(0, 8):
						pos = (My, Mx, C, L)
						self.setCell(pos, '0000')
	
	def getName(self):
		return self.name

	def setCell(self, pos, val):
		Mx = pos[0]
		My = pos[1]
		Co = pos[2]
		Ce = pos[3]
		self.modules[(Mx, My)].getColumn(Co).getCell(Ce).setValue(val)

	def getCell(self, pos):
		Mx = pos[0]
		My = pos[1]
		Co = pos[2]
		Ce = pos[3]
		return self.modules[(Mx, My)].getColumn(Co).getCell(Ce).getValue()

	def loadFile(self, path):
		print 'Loading from file %s..' % path
		fileInd = open(path, 'r')
		for i in range(0, 16):
			line = fileInd.readline()			
			arrayModule = line.split(' ')
			if arrayModule[0] != 'M':
				raise ValueError('Line %s: M was expected as first character of the line' % str(i*9 + 1))
			elif len(arrayModule) < 4:
				raise ValueError('Line %s: too few parameters!' % str(i*9 + 1))
#			elif len(arrayModule) > 4:
#				raise ValueError('Line %s: extra parameters will be ignored!' % str(i*9 + 1))
			for j in range(0, 4):
				line = fileInd.readline()			
				arrayColumn = line.split(' ')
				if arrayColumn[0] != 'C':
					raise ValueError('Line %s: C was expected as first character of the line' % str(i*9 + j*2 + 2))
				elif len(arrayColumn) < 3:
					raise ValueError('Line %s: too few parameters!' % str(i*9 + j*2 + 2))
#				elif len(arrayColumn) > 3:
#					raise ValueError('Line %s: extra parameters will be ignored!' % str(i*9 + j*2 + 2))
				line = fileInd.readline()			
				arrayLut = line.split(' ')
				if len(arrayLut) < 9:
					raise ValueError('Line %s: too few vaues in this line!' % str(i*9 + j*2 + 3))
				for k in range(0, 8):
#					print arrayModule[1], arrayModule[2], arrayColumn[1], k, arrayLut[k]
					half = len(arrayLut[k]) / 2
					lut1 = arrayLut[k][:half]
					lut2 = arrayLut[k][half:]
					if lut1 != lut2:
						self.reset()
						raise ValueError('The provided file does not encode a combinatorial circuit!')
					else:
						try:
							int(lut1, 16)
						except ValueError, TypeError:
							self.reset()
							raise ValueError('Line %s - entry %s: not an hexadecimal value!' % (str(i*9 + j*2 + 3), str(k + 1)))
					pos = (int(arrayModule[1]), int(arrayModule[2]), int(arrayColumn[1]), int(k))
					self.setCell(pos, lut1)
		fileInd.close()

	def exportFile(self, path):
		print 'Saving to file %s..' % path
		fileInd = open(path, 'w')
		for i in range(0,4):
			for j in range(0,4):
				fileInd.write('M ' + str(i) + ' ' + str(j) + ' \n')
				for k in range(0, 4):
					fileInd.write('C ' + str(k) + ' \n')
					for l in range(0,8):
						pos = (i,j,k,l)
						fileInd.write(str(self.getCell(pos)) + str(self.getCell(pos)) + ' ')
					fileInd.write('\n')
		fileInd.close()

	def exportVHDLcomponent(self, path, comp_name, comp_version):
		files = os.listdir('./vhd/' + self.name)

		newPath = path + '/' + comp_name + '_' + comp_version

		try:
			os.mkdir(newPath)
		except OSError as err:
			print 'Warning: %s' % err

		for f in files:
			ext = f.split('.')
			if ext[len(ext) - 1] == 'vhd':
				shutil.copyfile('./vhd/' + self.name + '/' + f, newPath + '/' + f)

		orig = newPath + '/dgecs_component.vhd'
		dest = newPath + '/' + comp_name + '.vhd'
		try:
			newComp = ''
			for line in open(orig):
				line = line.replace('dgecs_component', comp_name)
				line = line.replace('_v1_00_a', '_' +  comp_version)
				newComp += line
			open(dest, 'w').write(newComp)
			if(orig != dest):
				os.remove(orig)

		except Exception as err:
			rmFiles = os.listdir(newPath)
			for f in rmFiles:
				os.remove(newPath + '/' + f)
			os.rmdir(newPath)
			raise IOError('Error in patching vhd files: %s', str(err))

		values = ''
		for i in range(0, 4):
			for j in range(0, 4):
				for k in range(0, 4):
					for l in range(0, 8):
						values += self.getValue((i, j, k, l))

		print 'Writing lut values to user logic:\n' + values
		user_logicPath = newPath + '/user_logic.vhd'

		try:
			newUl = ''
			for line in open(user_logicPath):
				newUl += line.replace('__BITSTREAM__', values)
			open(user_logicPath, 'w').write(newUl)

		except Exception as err:
			print 'Warning: %s' % err

	def getLevels(self):
		return 4 #4 levels: Individual, Module, Column, Cell

	def getLevel0Dimension(self):
		return 2 #The individual is a 2D matrix of modules

	def getLevel0Range(self):
		return 0,3,0,3

	def getLevel1Dimension(self):
		return 1 #The module is an array of columns

	def getLevel1Range(self):
		return 0,3

	def getLevel1CompsName(self):
		return 'Module'

	def getLevel2Dimension(self):
		return 1 #The column is an array of cells

	def getLevel2Range(self):
		return 0,7

	def getLevel2CompsName(self):
		return 'Column'

	def getLevel3Dimension(self):
		return 0 #The cell is simply a LUT thruth-table value

	def getLevel3CompsName(self):
		return 'Cell'

	def getValue(self, pos):
		return self.getCell(pos);

	def setValue(self, pos, val):
		return self.setCell(pos, val)

if __name__ == '__main__':
	import sys, os
	from optparse import OptionParser
	parser = OptionParser()
	parser.add_option("-i", "--input_file", dest="input_path", help="Input file with cells contents")
	parser.add_option("-o", "--output_file", dest="output_path", help="Output for vhdl description")
	parser.add_option("-n", "--comp_name", dest="comp_name", help="Component name")
	parser.add_option("-v", "--version", dest="version", help="Component version")
	(options, args) = parser.parse_args()

	inp = options.input_path
	out = options.output_path
	comp_name = options.comp_name
	version = options.version

	if inp == None or out == None or name == None or version == None:
		if inp == None:
			inp = 'export_luts_c/example_32.luts'
		if out == None:
			out = os.getenv("HOME")
		if comp_name == None:
			comp_name = 'dgecs_component'
		if version == None:
			version = 'V1_00_a'

		print 'Will run with the following I/O files; proceed?'
		print '(input: ' + inp + ' , output: ' + out + ', component name: ' + comp_name + ', version: ' + version + ')'
		print 'Yry -h for help'
		raw_input('Enter to go on; ^C to exit')
	
	name = os.path.basename(__file__)
	name = name.split('.')
	ind = Individual(name[0])
	ind.loadFile(inp)
	ind.exportVHDLcomponent(out, comp_name, version)
