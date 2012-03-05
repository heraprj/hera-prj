------------------------------------------------------------------------------
-- user_logic.vhd - entity/architecture pair
------------------------------------------------------------------------------
--
-- ***************************************************************************
-- ** Copyright (c) 1995-2010 Xilinx, Inc.  All rights reserved.            **
-- **                                                                       **
-- ** Xilinx, Inc.                                                          **
-- ** XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"         **
-- ** AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND       **
-- ** SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,        **
-- ** OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,        **
-- ** APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION           **
-- ** THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,     **
-- ** AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE      **
-- ** FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY              **
-- ** WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE               **
-- ** IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR        **
-- ** REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF       **
-- ** INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS       **
-- ** FOR A PARTICULAR PURPOSE.                                             **
-- **                                                                       **
-- ***************************************************************************
--
------------------------------------------------------------------------------
-- Filename:          user_logic.vhd
-- Version:           1.00.a
-- Description:       User logic.
-- Date:              Fri Apr  8 10:35:26 2011 (by Create and Import Peripheral Wizard)
-- VHDL Standard:     VHDL'93
------------------------------------------------------------------------------
-- Naming Conventions:
--   active low signals:                    "*_n"
--   clock signals:                         "clk", "clk_div#", "clk_#x"
--   reset signals:                         "rst", "rst_n"
--   generics:                              "C_*"
--   user defined types:                    "*_TYPE"
--   state machine next state:              "*_ns"
--   state machine current state:           "*_cs"
--   combinatorial signals:                 "*_com"
--   pipelined or register delay signals:   "*_d#"
--   counter signals:                       "*cnt*"
--   clock enable signals:                  "*_ce"
--   internal version of output port:       "*_i"
--   device pins:                           "*_pin"
--   ports:                                 "- Names begin with Uppercase"
--   processes:                             "*_PROCESS"
--   component instantiations:              "<ENTITY_>I_<#|FUNC>"
------------------------------------------------------------------------------

-- DO NOT EDIT BELOW THIS LINE --------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library proc_common_v3_00_a;
use proc_common_v3_00_a.proc_common_pkg.all;

-- DO NOT EDIT ABOVE THIS LINE --------------------

--USER libraries added here

------------------------------------------------------------------------------
-- Entity section
------------------------------------------------------------------------------
-- Definition of Generics:
--   C_SLV_AWIDTH                 -- Slave interface address bus width
--   C_SLV_DWIDTH                 -- Slave interface data bus width
--   C_NUM_REG                    -- Number of software accessible registers
--   C_NUM_MEM                    -- Number of memory spaces
--
-- Definition of Ports:
--   Bus2IP_Clk                   -- Bus to IP clock
--   Bus2IP_Reset                 -- Bus to IP reset
--   Bus2IP_Addr                  -- Bus to IP address bus
--   Bus2IP_CS                    -- Bus to IP chip select for user logic memory selection
--   Bus2IP_RNW                   -- Bus to IP read/not write
--   Bus2IP_Data                  -- Bus to IP data bus
--   Bus2IP_BE                    -- Bus to IP byte enables
--   Bus2IP_RdCE                  -- Bus to IP read chip enable
--   Bus2IP_WrCE                  -- Bus to IP write chip enable
--   IP2Bus_Data                  -- IP to Bus data bus
--   IP2Bus_RdAck                 -- IP to Bus read transfer acknowledgement
--   IP2Bus_WrAck                 -- IP to Bus write transfer acknowledgement
--   IP2Bus_Error                 -- IP to Bus error response
------------------------------------------------------------------------------

entity user_logic is
  generic
  (
    -- ADD USER GENERICS BELOW THIS LINE ---------------
    --USER generics added here
    -- ADD USER GENERICS ABOVE THIS LINE ---------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol parameters, do not add to or delete
    C_SLV_AWIDTH                   : integer              := 32;
    C_SLV_DWIDTH                   : integer              := 32;
    C_NUM_REG                      : integer              := 4;
    C_NUM_MEM                      : integer              := 2
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );
  port
  (
    -- ADD USER PORTS BELOW THIS LINE ------------------
    --USER ports added here
    -- ADD USER PORTS ABOVE THIS LINE ------------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol ports, do not add to or delete
    Bus2IP_Clk                     : in  std_logic;
    Bus2IP_Reset                   : in  std_logic;
    Bus2IP_Addr                    : in  std_logic_vector(0 to C_SLV_AWIDTH-1);
    Bus2IP_CS                      : in  std_logic_vector(0 to C_NUM_MEM-1);
    Bus2IP_RNW                     : in  std_logic;
    Bus2IP_Data                    : in  std_logic_vector(0 to C_SLV_DWIDTH-1);
    Bus2IP_BE                      : in  std_logic_vector(0 to C_SLV_DWIDTH/8-1);
    Bus2IP_RdCE                    : in  std_logic_vector(0 to C_NUM_REG-1);
    Bus2IP_WrCE                    : in  std_logic_vector(0 to C_NUM_REG-1);
    IP2Bus_Data                    : out std_logic_vector(0 to C_SLV_DWIDTH-1);
    IP2Bus_RdAck                   : out std_logic;
    IP2Bus_WrAck                   : out std_logic;
    IP2Bus_Error                   : out std_logic
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );

  attribute SIGIS : string;
  attribute SIGIS of Bus2IP_Clk    : signal is "CLK";
  attribute SIGIS of Bus2IP_Reset  : signal is "RST";

end entity user_logic;

------------------------------------------------------------------------------
-- Architecture section
------------------------------------------------------------------------------

architecture IMP of user_logic is


COMPONENT ehw_core_logic is
	Port (	CLOCK        : in  std_logic;
				RESET        : in  std_logic;
				in_Reg     	 : in  STD_LOGIC_VECTOR (31 downto 0);
				MEM_OUT      : in  STD_LOGIC_VECTOR (31 downto 0);
				granularity  : in  STD_LOGIC_VECTOR (2 downto 0);
				test_case	 : in  STD_LOGIC_VECTOR (3 downto 0);
				START_P 		 : in STD_LOGIC_VECTOR (31 downto 0);
				PROG  		 : out STD_LOGIC_VECTOR (31 downto 0);
				out_Reg 		 : out STD_LOGIC_VECTOR (31 downto 0);
				Memory_read_address_ip	 : out STD_LOGIC_VECTOR (11 downto 0);
				Memory_read_address_op	 : out STD_LOGIC_VECTOR (11 downto 0)
				);
end COMPONENT;

component DP_RAM_EHW
	port (
	clka: in std_logic;
	ena: in std_logic;
	wea: in std_logic_vector(0 downto 0);
	addra: in std_logic_vector(11 downto 0);
	dina: in std_logic_vector(31 downto 0);
	douta: out std_logic_vector(31 downto 0);
	clkb: in std_logic;
	enb: in std_logic;
	web: in std_logic_vector(0 downto 0);
	addrb: in std_logic_vector(11 downto 0);
	dinb: in std_logic_vector(31 downto 0);
	doutb: out std_logic_vector(31 downto 0));
end component;

  ------------------------------------------
  -- Signals for user logic slave model s/w accessible register example
  ------------------------------------------
  signal slv_reg0                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal slv_reg1                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal slv_reg2                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal slv_reg3                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal slv_reg_write_sel              : std_logic_vector(0 to 3);
  signal slv_reg_read_sel               : std_logic_vector(0 to 3);
  signal slv_ip2bus_data                : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal slv_read_ack                   : std_logic;
  signal slv_write_ack                  : std_logic;

  ------------------------------------------
  -- Signals for user logic memory space example
  ------------------------------------------
signal mem_data_out0                   	: std_logic_vector(0 to C_SLV_DWIDTH-1);
signal mem_data_out1                   	: std_logic_vector(0 to C_SLV_DWIDTH-1);
signal mem_address                    	: std_logic_vector(0 to 11);
signal mem_select                     	: std_logic_vector(0 to 1);
signal mem_read_enable                	: std_logic;
signal mem_read_enable_dly1           	: std_logic;
signal mem_read_req                   	: std_logic;
signal mem_ip2bus_data                	: std_logic_vector(0 to C_SLV_DWIDTH-1);
signal mem_read_ack_dly1              	: std_logic;
signal mem_read_ack                   	: std_logic;
signal mem_write_ack					: std_logic;

signal data_in							: std_logic_vector(0 to C_SLV_DWIDTH-1);
signal write_enable	                    : std_logic_vector(0 to 0);
signal mem_data_out_int0 				: std_logic_vector(31 downto 0);
signal mem_data_out_int1 				: std_logic_vector(31 downto 0);
signal data_in_int 						: std_logic_vector(31 downto 0);
signal slv_reg1_int 					: std_logic_vector(31 downto 0);
signal mem_address_int 					: std_logic_vector(11 downto 0);

signal core_mem_address_ip				: std_logic_vector(11 downto 0);
signal core_mem_address_op         		: std_logic_vector(11 downto 0);
signal core_mem_data               		: std_logic_vector(31 downto 0);
signal Result_data               		: std_logic_vector(31 downto 0);
  
signal web						  		: std_logic_vector(0 to 0);
signal dinb								: std_logic_vector(31 downto 0);

begin

	Inst_ehw_core_logic: ehw_core_logic PORT MAP(
		CLOCK 	 => Bus2IP_Clk,
		RESET 	 => Bus2IP_Reset,
		in_Reg 	 => core_mem_data (31 downto 0),
		MEM_OUT   => Result_data(31 downto 0),
		granularity => slv_reg0(0 to 2),
		test_case => slv_reg0(3 to 6),
		START_P   => slv_reg2(0 to 31),
		PROG      => slv_reg3(0 to 31),
		out_Reg   => slv_reg1(0 to 31),
		Memory_read_address_ip => core_mem_address_ip,
		Memory_read_address_op => core_mem_address_op
			);
	
  ------------------------------------------
  -- Example code to read/write user logic slave model s/w accessible registers
  -- 
  -- Note:
  -- The example code presented here is to show you one way of reading/writing
  -- software accessible registers implemented in the user logic slave model.
  -- Each bit of the Bus2IP_WrCE/Bus2IP_RdCE signals is configured to correspond
  -- to one software accessible register by the top level template. For example,
  -- if you have four 32 bit software accessible registers in the user logic,
  -- you are basically operating on the following memory mapped registers:
  -- 
  --    Bus2IP_WrCE/Bus2IP_RdCE   Memory Mapped Register
  --                     "1000"   C_BASEADDR + 0x0
  --                     "0100"   C_BASEADDR + 0x4
  --                     "0010"   C_BASEADDR + 0x8
  --                     "0001"   C_BASEADDR + 0xC
  -- 
  ------------------------------------------
  slv_reg_write_sel <= Bus2IP_WrCE(0 to 3);
  slv_reg_read_sel  <= Bus2IP_RdCE(0 to 3);
  slv_write_ack     <= Bus2IP_WrCE(0) or Bus2IP_WrCE(1) or Bus2IP_WrCE(2) or Bus2IP_WrCE(3);
  slv_read_ack      <= Bus2IP_RdCE(0) or Bus2IP_RdCE(1) or Bus2IP_RdCE(2) or Bus2IP_RdCE(3);

  -- implement slave model software accessible register(s)
  SLAVE_REG_WRITE_PROC : process( Bus2IP_Clk ) is
  begin

    if Bus2IP_Clk'event and Bus2IP_Clk = '1' then
      if Bus2IP_Reset = '1' then
        slv_reg0 <= (others => '0');
        slv_reg2 <= (others => '0');
        
      else
        case slv_reg_write_sel is
          when "1000" =>
            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
                slv_reg0(byte_index*8 to byte_index*8+7) <= Bus2IP_Data(byte_index*8 to byte_index*8+7);
              end if;
            end loop;
          
          when "0010" =>
            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
                slv_reg2(byte_index*8 to byte_index*8+7) <= Bus2IP_Data(byte_index*8 to byte_index*8+7);
              end if;
            end loop;
          
          when others => null;
        end case;
      end if;
    end if;

  end process SLAVE_REG_WRITE_PROC;

  -- implement slave model software accessible register(s) read mux
  SLAVE_REG_READ_PROC : process( slv_reg_read_sel, slv_reg1, slv_reg3 ) is
  begin

    case slv_reg_read_sel is
      when "0100" => slv_ip2bus_data <= slv_reg1;
      when "0001" => slv_ip2bus_data <= slv_reg3;
      when others => slv_ip2bus_data <= (others => '0');
    end case;

  end process SLAVE_REG_READ_PROC;

  ------------------------------------------
  -- Example code to access user logic memory region
  -- 
  -- Note:
  -- The example code presented here is to show you one way of using
  -- the user logic memory space features. The Bus2IP_Addr, Bus2IP_CS,
  -- and Bus2IP_RNW IPIC signals are dedicated to these user logic
  -- memory spaces. Each user logic memory space has its own address
  -- range and is allocated one bit on the Bus2IP_CS signal to indicated
  -- selection of that memory space. Typically these user logic memory
  -- spaces are used to implement memory controller type cores, but it
  -- can also be used in cores that need to access additional address space
  -- (non C_BASEADDR based), s.t. bridges. This code snippet infers
  -- 2 256x32-bit (byte accessible) single-port Block RAM by XST.
  ------------------------------------------
  mem_select      <= Bus2IP_CS;
  mem_read_enable <= ( Bus2IP_CS(0) or Bus2IP_CS(1) ) and Bus2IP_RNW;
  mem_read_ack    <= mem_read_ack_dly1;
  mem_write_ack   <= ( Bus2IP_CS(0) or Bus2IP_CS(1) ) and not(Bus2IP_RNW);
  mem_address     <= Bus2IP_Addr(C_SLV_AWIDTH-14 to C_SLV_AWIDTH-3);

  write_enable (0)  <= ( Bus2IP_CS(0) or Bus2IP_CS(1) ) and not(Bus2IP_RNW);
  data_in <= Bus2IP_Data;
  
  -- implement single clock wide read request
  mem_read_req    <= mem_read_enable and not(mem_read_enable_dly1);
  
  BRAM_RD_REQ_PROC : process( Bus2IP_Clk ) is
  begin

    if ( Bus2IP_Clk'event and Bus2IP_Clk = '1' ) then
      if ( Bus2IP_Reset = '1' ) then
        mem_read_enable_dly1 <= '0';
      else
        mem_read_enable_dly1 <= mem_read_enable;
      end if;
    end if;

  end process BRAM_RD_REQ_PROC;

  -- this process generates the read acknowledge 1 clock after read enable
  -- is presented to the BRAM block. The BRAM block has a 1 clock delay
  -- from read enable to data out.
  BRAM_RD_ACK_PROC : process( Bus2IP_Clk ) is
  begin

    if ( Bus2IP_Clk'event and Bus2IP_Clk = '1' ) then
      if ( Bus2IP_Reset = '1' ) then
        mem_read_ack_dly1 <= '0';
      else
        mem_read_ack_dly1 <= mem_read_req;
      end if;
    end if;

  end process BRAM_RD_ACK_PROC;

DP_MEM_Stimu : DP_RAM_EHW
		port map (
			clka => Bus2IP_Clk,
			ena => mem_select(0),
			wea => write_enable,
			addra => mem_address_int,
			dina => data_in_int,
			douta => mem_data_out_int0,
			clkb => Bus2IP_Clk,
			enb => '1',
			web => web,
			addrb => core_mem_address_ip,
			dinb => dinb,
			doutb => core_mem_data);

DP_MEM_Result : DP_RAM_EHW
		port map (
			clka => Bus2IP_Clk,
			ena => mem_select(1),
			wea => write_enable,
			addra => mem_address_int,
			dina => data_in_int,
			douta => mem_data_out_int1,
			clkb => Bus2IP_Clk,
			enb => '1',
			web => web,
			addrb => core_mem_address_op,
			dinb => dinb,
			doutb => Result_data);			
			
---implement data swap for memory	
data_swap_stimul: for i in 0 to 31 generate  ------ ----------------------------------system1
    mem_data_out0(i)  <= mem_data_out_int0(31-i);
end generate;	

data_swap_result: for i in 0 to 31 generate  ------ ----------------------------------system1
    mem_data_out1(i)  <= mem_data_out_int1(31-i);
end generate;	

data_swap_address: for i in 0 to 11 generate  ------ ----------------------------------system1
    mem_address_int(i)  <= mem_address(11-i);
end generate;	

data_IN_swap: for i in 0 to 31 generate  ------ ----------------------------------system1
    data_in_int(i)  <= data_in(31-i);
end generate;

  -- implement Block RAM read mux
  MEM_IP2BUS_DATA_PROC : process( mem_data_out0, mem_data_out1, mem_select ) is
  begin

    case mem_select is
      when "10" => mem_ip2bus_data <= mem_data_out0;
      when "01" => mem_ip2bus_data <= mem_data_out1;
      when others => mem_ip2bus_data <= (others => '0');
    end case;

  end process MEM_IP2BUS_DATA_PROC;

  ------------------------------------------
  -- Example code to drive IP to Bus signals
  ------------------------------------------
  IP2Bus_Data  <= slv_ip2bus_data when slv_read_ack = '1' else
                  mem_ip2bus_data when mem_read_ack = '1' else
                  (others => '0');

  IP2Bus_WrAck <= slv_write_ack or mem_write_ack;
  IP2Bus_RdAck <= slv_read_ack or mem_read_ack;
  IP2Bus_Error <= '0';
   web <= (others =>'0');
	dinb <= (others =>'0');

end IMP;
 
