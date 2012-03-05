library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity driver is
port(
	FPGA_CPU_RESET_B 	: IN std_logic;
	USER_CLK 			: IN STD_LOGIC;
	DATA_IN				: IN std_logic_vector(31 downto 0);
	mem_enable			: OUT STD_LOGIC;
	ROM_ADDRA_32		: OUT std_logic_VECTOR(7 downto 0); 
	START						: IN std_logic_vector(31 downto 0);	
	PROG_DONE			: OUT std_logic_vector(31 downto 0)
	);
end driver;

architecture Behavioral of driver is

	
COMPONENT  CONTROL_32
	 PORT  (
    RESET_P                : IN  std_logic;
    CLK_50                : IN  std_logic;
    LOCKED_OUT          : IN  std_logic;
    RECV_COMMNAD        : IN  std_logic_vector(31 downto 0);
    BUSY                    : IN  std_logic;
    ROM_ADDRA_32        : OUT std_logic_VECTOR(7 downto 0);  
    WRITE1                : OUT  std_logic;
    MEM_EN_OUT            : OUT  std_logic;
    PROG_DONE            : OUT std_logic_vector(31 downto 0)
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
	SIGNAL PROG : std_logic_vector(31 downto 0);
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
ICAP_DATA  <= DATA_IN;

--data_swap: for i in 0 to 31 generate  ------ ----------------------------------system1
--    PROG_DONE(i)  <= PROG(31-i);
--end generate;

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
