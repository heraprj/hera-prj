project new /home/micro/hera32bit/pcores/ehw_core_v3_00_a/devl/projnav/ehw_core.xise;
project set family virtex4;
project set device xc4vfx12;
project set package ff668;
project set speed -10;
project set top_level_module_type HDL;
project set synthesis_tool "XST (VHDL/Verilog)";
lib_vhdl new ehw_core_v3_00_a;
xfile add /home/micro/hera32bit/pcores/ehw_core_v3_00_a/hdl/vhdl/ehw_core.vhd -lib_vhdl ehw_core_v3_00_a;
xfile add /home/micro/hera32bit/pcores/ehw_core_v3_00_a/hdl/vhdl/user_logic.vhd -lib_vhdl ehw_core_v3_00_a;
lib_vhdl new proc_common_v3_00_a;
xfile add /opt/Xilinx/12.4/ISE_DS/EDK/hw/XilinxProcessorIPLib/pcores/proc_common_v3_00_a/hdl/vhdl/proc_common_pkg.vhd -lib_vhdl proc_common_v3_00_a;
xfile add /opt/Xilinx/12.4/ISE_DS/EDK/hw/XilinxProcessorIPLib/pcores/proc_common_v3_00_a/hdl/vhdl/ipif_pkg.vhd -lib_vhdl proc_common_v3_00_a;
xfile add /opt/Xilinx/12.4/ISE_DS/EDK/hw/XilinxProcessorIPLib/pcores/proc_common_v3_00_a/hdl/vhdl/or_muxcy.vhd -lib_vhdl proc_common_v3_00_a;
xfile add /opt/Xilinx/12.4/ISE_DS/EDK/hw/XilinxProcessorIPLib/pcores/proc_common_v3_00_a/hdl/vhdl/or_gate128.vhd -lib_vhdl proc_common_v3_00_a;
xfile add /opt/Xilinx/12.4/ISE_DS/EDK/hw/XilinxProcessorIPLib/pcores/proc_common_v3_00_a/hdl/vhdl/family_support.vhd -lib_vhdl proc_common_v3_00_a;
xfile add /opt/Xilinx/12.4/ISE_DS/EDK/hw/XilinxProcessorIPLib/pcores/proc_common_v3_00_a/hdl/vhdl/pselect_f.vhd -lib_vhdl proc_common_v3_00_a;
xfile add /opt/Xilinx/12.4/ISE_DS/EDK/hw/XilinxProcessorIPLib/pcores/proc_common_v3_00_a/hdl/vhdl/counter_f.vhd -lib_vhdl proc_common_v3_00_a;
lib_vhdl new plbv46_slave_single_v1_01_a;
xfile add /opt/Xilinx/12.4/ISE_DS/EDK/hw/XilinxProcessorIPLib/pcores/plbv46_slave_single_v1_01_a/hdl/vhdl/plb_address_decoder.vhd -lib_vhdl plbv46_slave_single_v1_01_a;
xfile add /opt/Xilinx/12.4/ISE_DS/EDK/hw/XilinxProcessorIPLib/pcores/plbv46_slave_single_v1_01_a/hdl/vhdl/plb_slave_attachment.vhd -lib_vhdl plbv46_slave_single_v1_01_a;
xfile add /opt/Xilinx/12.4/ISE_DS/EDK/hw/XilinxProcessorIPLib/pcores/plbv46_slave_single_v1_01_a/hdl/vhdl/plbv46_slave_single.vhd -lib_vhdl plbv46_slave_single_v1_01_a;
project close;
