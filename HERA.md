# HERA - HowTo #
This HowTo briefly describes how to use the files available in this project.
The HERA hardware evolution process takes place on a FPGA and comprises both hardware and software components. The hardware architecture is defined by a bitstream to be loaded on the FPGA; this architecture comprises a processor to execute the evolutionary algorithm. An increasing number of modules of the evolutionary algorithm are being ported to hardware, but there is still the need of a software portion.

## Available Data ##
The data available in this project is split in two locations:
  * A [Downloads page](http://code.google.com/p/hera-prj/downloads/list), with archives for binaries, architectures, and components, ...
  * A [git repository](http://code.google.com/p/hera-prj/source/browse/), with source code for the EA and the tools

## Supported Devices ##
The files available in this project allow to use the HERA evolvable hardware framework on supported FPGAs; by now, the supported devices include:
  * Xilinx Virtex-4 XC4VFX12-FF668-10

Work is in progress for supporting Virtex-5 FPGAs, and the relevant architecture will be posted as soon as it gets stable enough for public release.

## Using Binaries ##
Binaries (i.e., a bitstream to be loaded on the FPGA and an elf executable for the processor) are provided for some architecture/evolution process couples. This is the fastest way to test the HERA framework, and it permits to run the evolutionary process for a the provided circuits on the supported devices. The binaries are provided in compressed archives containing a bitstream (.bit file), an executable (.elf file), and a README with specifications of the nature of the package. Moreover, also some standalone bitsreams are provided.

The bitstreams and binary packages may be downloaded on the FPGA and executed with the suitable Xilinx tools (e.g., impact, xmd, ...).

The available bitstreams (found on [Downloads page](http://code.google.com/p/hera-prj/downloads/list)) include, at least, the following evolutionary architectures:
  * [hera8bit\_XC4VFX12.bit](http://code.google.com/p/hera-prj/downloads/detail?name=hera8bit_XC4VFX12.bit&can=2&q=) -> supports the evolution of circuits with a 8bit-wide data path on the XC4VFX12 FPGA.
  * [hera8bit\_sedprc\_hwf\_XC4VFX12.bit](http://code.google.com/p/hera-prj/downloads/detail?name=hera8bit_sedprc_hwf_XC4VFX12.bit&can=2&q=) -> same as the previous one, but with hardware-accelerated fitness evaluation and a faster reconfiguration component.
  * [hera32bit\_XC4VFX12.bit](http://code.google.com/p/hera-prj/downloads/detail?name=hera32bit_XC4VFX12.bit&can=2&q=) -> supports the evolution of circuits with a 32bit-wide data path on the XC4VFX12 FPGA.
  * [hera32bit\_sedprc\_hwf\_XC4VFX12.bit](http://code.google.com/p/hera-prj/downloads/detail?name=hera32bit_sedprc_hwf_XC4VFX12.bit&can=2&q=) -> same as the previous one, but with hardware-accelerated fitness evaluation and a faster reconfiguration component.
These bitstreams are useful when used to run an evolutionary process, which may be done by executing the EA binaries (see below). More bitstreams will be published as new architectures become stable.

The available binary packages (including both the EA executable and the bitstream of the suitable architecture) may be downloaded from the [Downloads page](http://code.google.com/p/hera-prj/downloads/list) and include, at least, the following evolutionary processes (refer to the READMEs in the archives for further information):
  * [8bit parity generator](http://code.google.com/p/hera-prj/downloads/detail?name=pargen_8bit.tar.bz2&can=2&q=)

## Using Sources ##
Also the sources for both the EA and some architectures are available.
The C sources of the EA are available in the src/ga-ppc directory of the [git repository](http://code.google.com/p/hera-prj/source/browse/#git%2Fsrc%2Fga-ppc) (see the [README ](http://code.google.com/p/hera-prj/source/browse/src/ga-ppc/README) for further information).
The architectures are available, as Xilinx xps projects, under the [Downloads page](http://code.google.com/p/hera-prj/downloads/list); at least the following architectures are available:
  * [hera8bit.tar.bz2](http://code.google.com/p/hera-prj/downloads/detail?name=hera8bit.tar.bz2&can=2&q=)
  * [hera8bit\_sedpr.tar.bz2](http://code.google.com/p/hera-prj/downloads/detail?name=hera8bit_sedpr.tar.bz2&can=2&q=)
  * [hera32bit.tar.bz2](http://code.google.com/p/hera-prj/downloads/detail?name=hera32bit.tar.bz2&can=2&q=)
  * [hera32bit\_sedpr.tar.bz2](http://code.google.com/p/hera-prj/downloads/detail?name=hera32bit_sedpr.tar.bz2&can=2&q=)
All the above architectures are engineered for the Xilinx Virtex-4 XC4VFX12 chip and may be opened with the suitable Xilinx tools.

For more information regarding how to deal with the sources, drop an email to the developers at heraprj -at- gmail -dot- com.