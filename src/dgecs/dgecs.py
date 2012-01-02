#!/usr/bin/python

#	#	#	#	#	#	#	#	#	#
# DGECS: Description Generator for Evolved Circuits Synthesis		#
#									#
# authors: Davide Basilio Bartolini, Matteo Carminati			#
#	#	#	#	#	#	#	#	#	#

import sys
from PyQt4 import QtGui
from gui import MainWindow

# Simply run the GUI
dgecs = QtGui.QApplication(sys.argv)
dgecsWindow = MainWindow.DGECSWindow()
dgecsWindow.show()
sys.exit(dgecs.exec_())
