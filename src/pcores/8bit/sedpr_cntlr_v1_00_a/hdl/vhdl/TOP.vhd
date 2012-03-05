library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity driver is
port(
	FPGA_CPU_RESET_B 	: IN std_logic;
	USER_CLK 			: IN STD_LOGIC;
	mem_enable			: OUT STD_LOGIC;
	DATA_IN				: IN std_logic_vector(0 to 31);
	ROM_ADDRA_32		: OUT std_logic_VECTOR(0 to 7); 
	START						: IN std_logic_vector(0 to 31);	
	PROG_DONE			: OUT std_logic_vector(0 to 31)
	);
end driver;

architecture Behavioral of driver is

	
COMPONENT  CONTROL_32
	 PORT  (
    RESET_P                : IN  std_logic;
    CLK_50                : IN  std_logic;
    LOCKED_OUT          : IN  std_logic;
    RECV_COMMNAD        : IN  std_logic_vector(0 to 31);
    BUSY                    : IN  std_logic;
    ROM_ADDRA_32        : OUT std_logic_VECTOR(0 to 7);  
    WRITE1                : OUT  std_logic;
    MEM_EN_OUT            : OUT  std_logic;
    PROG_DONE            : OUT std_logic_vector(0 to 31)
    );
	end component;
	
	component ICAP_VIRTEX4
	generic (
	ICAP_WIDTH: string := "X8" );
		port
		(	BUSY 			: out std_ulogic;
			O 				: out std_logic_vector(31 downto 0);
			CE 			: in std_ulogic;
			CLK 			: in std_ulogic;
			I 				: in std_logic_vector(31 downto 0);
			WRITE 		: in std_ulogic 	);
	 end component;

	SIGNAL ICAP_WRITE	: STD_LOGIC; 
	SIGNAL BUSY : STD_LOGIC;
	SIGNAL BUSY1 : STD_LOGIC;
	SIGNAL PROG : std_logic_vector(0 to 31);
	SIGNAL ROM_ADDRA_32_INT : std_logic_vector(0 to 7);
	SIGNAL MEM_ENABLE_INT : STD_LOGIC;
	
	SIGNAL DATA_OUT  : std_logic_vector(31 downto 0);
	SIGNAL ICAP_DATA : std_logic_vector(31 downto 0);
	SIGNAL Temp_data : std_logic_vector(31 downto 0);
	
begin

ROM_ADDRA_32 <= ROM_ADDRA_32_INT;
mem_enable <= MEM_ENABLE_INT;
--PROG_DONE <= ICAP_DATA(15 downto 0)& ROM_ADDRA_32_INT & START(0) & MEM_ENABLE_INT & BUSY & ICAP_WRITE & USER_CLK & FPGA_CPU_RESET_B & '1' & PROG(0);
--PROG_DONE <= ICAP_DATA(31 downto 12)& ROM_ADDRA_32_INT & START(0) & MEM_ENABLE_INT & ICAP_WRITE & PROG(0);

PROG_DONE <= PROG;
data_swap: for i in 0 to 31 generate  ------ ----------------------------------system1
    ICAP_DATA(i)  <= DATA_IN(31-i);
end generate;

--ICAP_DATA <= DATA_IN;----------------------------------------------------------system2

--ICAP_DATA <= DATA_IN(0 to 7) & DATA_IN(8 to 15) & DATA_IN(16 to 23) & DATA_IN(24 to 31); ----system3


--ICAP_DATA <= DATA_IN(24 to 31) & DATA_IN(16 to 23) & DATA_IN(8 to 15) & DATA_IN(0 to 7); --system4

--data_swap: for i in 0 to 31 generate  ------ -------------------------------------------------system5
--    Temp_data(i)  <= DATA_IN(31-i);
--end generate;
--ICAP_DATA <= Temp_data(7 downto 0) & Temp_data(15 downto 8) & Temp_data(23 downto 16) & Temp_data(31 downto 24); 

--- system6-----------------
--ICAP_DATA(31) <= DATA_IN(7);
--ICAP_DATA(30) <= DATA_IN(6);
--ICAP_DATA(29) <= DATA_IN(5);
--ICAP_DATA(28) <= DATA_IN(4);
--ICAP_DATA(27) <= DATA_IN(3);
--ICAP_DATA(26) <= DATA_IN(2);
--ICAP_DATA(25) <= DATA_IN(1);
--ICAP_DATA(24) <= DATA_IN(0);
--
--ICAP_DATA(23) <= DATA_IN(15);
--ICAP_DATA(22) <= DATA_IN(14);
--ICAP_DATA(21) <= DATA_IN(13);
--ICAP_DATA(20) <= DATA_IN(12);
--ICAP_DATA(19) <= DATA_IN(11);
--ICAP_DATA(18) <= DATA_IN(10);
--ICAP_DATA(17) <= DATA_IN(9);
--ICAP_DATA(16) <= DATA_IN(8);
--
--ICAP_DATA(15) <= DATA_IN(23);
--ICAP_DATA(14) <= DATA_IN(22);
--ICAP_DATA(13) <= DATA_IN(21);
--ICAP_DATA(12) <= DATA_IN(20);
--ICAP_DATA(11) <= DATA_IN(19);
--ICAP_DATA(10) <= DATA_IN(18);
--ICAP_DATA(9) <= DATA_IN(17);
--ICAP_DATA(8) <= DATA_IN(16);
--
--ICAP_DATA(7) <= DATA_IN(31);
--ICAP_DATA(6) <= DATA_IN(30);
--ICAP_DATA(5) <= DATA_IN(29);
--ICAP_DATA(4) <= DATA_IN(28);
--ICAP_DATA(3) <= DATA_IN(27);
--ICAP_DATA(2) <= DATA_IN(26);
--ICAP_DATA(1) <= DATA_IN(25);
--ICAP_DATA(0) <= DATA_IN(24);



u1 : CONTROL_32
	PORT MAP (
	RESET_P			=> FPGA_CPU_RESET_B,
	CLK_50			=> USER_CLK,
	LOCKED_OUT  	=> '1',
   RECV_COMMNAD	=> START,
	BUSY				=>	BUSY	,		
	ROM_ADDRA_32	=> ROM_ADDRA_32_INT,
	WRITE1			=> ICAP_WRITE,
	MEM_EN_OUT		=> MEM_ENABLE_INT,
	PROG_DONE		=> PROG
	);
	
u4:ICAP_VIRTEX4
generic map (
ICAP_WIDTH => "X32") -- "X8" or "X32"
    port map (
  -- clk   => USER_CLK,
   clk   => USER_CLK,
   ce    => ICAP_WRITE ,
   write => ICAP_WRITE,
   i     => ICAP_DATA,
   busy  => BUSY,
   o     => DATA_OUT
    );
	 
--u5:ICAP_VIRTEX4
--generic map (
--ICAP_WIDTH => "X32") -- "X8" or "X32"
--    port map (
--  -- clk   => USER_CLK,
--    clk   => USER_CLK,
--   ce    => '1',
--   write => '1',
--   i     => IACP_DATA,
--   busy  => BUSY1,
--   o     => DATA_OUT1
--    );

end Behavioral;
