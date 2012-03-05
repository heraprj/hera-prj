library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


entity ehw_core_logic is
	Port (			CLOCK		 : in  std_logic;
				RESET		 : in  std_logic;
				in_Reg     	 : in  STD_LOGIC_VECTOR (31 downto 0);
				out_Reg 	 : out STD_LOGIC_VECTOR (31 downto 0);
				ctrl_Reg 	 : in  STD_LOGIC_VECTOR (7 downto 0);	
				state_Reg	 : out STD_LOGIC_VECTOR (7 downto 0)
				);
end ehw_core_logic;


architecture Behavioral of ehw_core_logic is
	signal control_signal : std_logic_vector(7 downto 0) := (others => '0');  -- Signal for pins: CLK, CE e SR
	signal inputRegister  : std_logic_vector(31 downto 0) := (others => '0'); -- Local input register
	signal outputRegister : std_logic_vector(31 downto 0) := (others => '0'); -- Local output register

	signal out_0_0, out_0_1, out_0_2, out_0_3 : std_logic_vector(7 downto 0) := (others => '0'); -- first column intermediate outputs 
	signal out_1_0, out_1_1, out_1_2, out_1_3 : std_logic_vector(7 downto 0) := (others => '0'); -- second column intermediate outputs
	signal out_2_0, out_2_1, out_2_2, out_2_3 : std_logic_vector(7 downto 0) := (others => '0'); -- third column intermediate outputs	
	signal dummy0, dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8, dummy9 : std_logic_vector(23 downto 0) := (others => '0');
	signal dummy10, dummy11, dummy12, dummy13, dummy14, dummy15 : std_logic_vector(23 downto 0) := (others => '0');

	type STATE_TYPE is (WAIT_DATA, ASSERT_CE , DEASSERT_ALL, CLOCK_1, CLOCK_2, CLOCK_3, CLOCK_4, RESET_FF);
	attribute ENUM_ENCODING: STRING;
	attribute ENUM_ENCODING of STATE_TYPE:type is "000 001 010 011 100 101 110 111";
	
	signal current_state : STATE_TYPE := WAIT_DATA;
	
	component SingleCell
		port (
			inputReg	: in std_logic_vector(7 downto 0);
			outputReg	: out std_logic_vector(31 downto 0);
			CLOCK 	 	: in std_logic_vector(3 downto 0);
			CE	 	: in std_logic;
			SR		: in std_logic
		);
	end component;

begin


	--First Column
	inputCell0 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg => inputRegister(7 downto 0), 
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_0_0,
		outputReg(23 downto 0) => dummy0
	);

	inputCell1 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg => inputRegister(15 downto 8), 
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_0_1,
		outputReg(23 downto 0) => dummy1
	);

	inputCell2 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg => inputRegister(23 downto 16), 
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_0_2,
		outputReg(23 downto 0) => dummy2
	);

	inputCell3 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg => inputRegister(31 downto 24), 
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_0_3,
		outputReg(23 downto 0) => dummy3
	);

	--Second Column
	middleleftCell0 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_0_0(1 downto 0),
		inputReg(5 downto 4) => out_0_1(1 downto 0),
 		inputReg(3 downto 2) => out_0_2(1 downto 0),
		inputReg(1 downto 0) => out_0_3(1 downto 0),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_1_0,
		outputReg(23 downto 0) => dummy4
	);

	middleleftCell1 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_0_0(3 downto 2),
		inputReg(5 downto 4) => out_0_1(3 downto 2),
 		inputReg(3 downto 2) => out_0_2(3 downto 2),
		inputReg(1 downto 0) => out_0_3(3 downto 2),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_1_1,
		outputReg(23 downto 0) => dummy5
	);

	middleleftCell2 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_0_0(5 downto 4),
		inputReg(5 downto 4) => out_0_1(5 downto 4),
 		inputReg(3 downto 2) => out_0_2(5 downto 4),
		inputReg(1 downto 0) => out_0_3(5 downto 4),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_1_2,
		outputReg(23 downto 0) => dummy6
	);

	middleleftCell3 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_0_0(7 downto 6),
		inputReg(5 downto 4) => out_0_1(7 downto 6),
 		inputReg(3 downto 2) => out_0_2(7 downto 6),
		inputReg(1 downto 0) => out_0_3(7 downto 6),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_1_3,
		outputReg(23 downto 0) => dummy7
	);


	--Third Column
	middlerightCell0 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_1_0(7 downto 6),
		inputReg(5 downto 4) => out_1_1(7 downto 6),
 		inputReg(3 downto 2) => out_1_2(7 downto 6),
		inputReg(1 downto 0) => out_1_3(7 downto 6),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_2_0,
		outputReg(23 downto 0) => dummy8
	);

	middlerightCell1 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_1_0(5 downto 4),
		inputReg(5 downto 4) => out_1_1(5 downto 4),
 		inputReg(3 downto 2) => out_1_2(5 downto 4),
		inputReg(1 downto 0) => out_1_3(5 downto 4),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_2_1,
		outputReg(23 downto 0) => dummy9
	);

	middlerightCell2 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_1_0(3 downto 2),
		inputReg(5 downto 4) => out_1_1(3 downto 2),
 		inputReg(3 downto 2) => out_1_2(3 downto 2),
		inputReg(1 downto 0) => out_1_3(3 downto 2),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_2_2,
		outputReg(23 downto 0) => dummy10
	);

	middlerightCell3 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_1_0(1 downto 0),
		inputReg(5 downto 4) => out_1_1(1 downto 0),
 		inputReg(3 downto 2) => out_1_2(1 downto 0),
		inputReg(1 downto 0) => out_1_3(1 downto 0),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => out_2_3,
		outputReg(23 downto 0) => dummy11
	);

	--Fourth Column
	outputCell0 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_2_0(1 downto 0),
		inputReg(5 downto 4) => out_2_1(1 downto 0),
 		inputReg(3 downto 2) => out_2_2(1 downto 0),
		inputReg(1 downto 0) => out_2_3(1 downto 0),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => outputRegister(31 downto 24),
		outputReg(23 downto 0) => dummy12
	);

	outputCell1 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_2_0(3 downto 2),
		inputReg(5 downto 4) => out_2_1(3 downto 2),
 		inputReg(3 downto 2) => out_2_2(3 downto 2),
		inputReg(1 downto 0) => out_2_3(3 downto 2),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => outputRegister(23 downto 16),
		outputReg(23 downto 0) => dummy13
	);

	outputCell2 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_2_0(5 downto 4),
		inputReg(5 downto 4) => out_2_1(5 downto 4),
 		inputReg(3 downto 2) => out_2_2(5 downto 4),
		inputReg(1 downto 0) => out_2_3(5 downto 4),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => outputRegister(15 downto 8),
		outputReg(23 downto 0) => dummy14
	);

	outputCell3 : SingleCell port map (
		CE => control_signal(0), 
		SR => control_signal(5), 
		inputReg(7 downto 6) => out_2_0(7 downto 6),
		inputReg(5 downto 4) => out_2_1(7 downto 6),
 		inputReg(3 downto 2) => out_2_2(7 downto 6),
		inputReg(1 downto 0) => out_2_3(7 downto 6),
		CLOCK =>control_signal(4 downto 1), 
		outputReg(31 downto 24) => outputRegister(7 downto 0),
		outputReg(23 downto 0) => dummy15
	);

	--Final output signal
	--out_Reg(63 downto 48) <= (others => '0');
	--out_Reg(47 downto 40) <= ctrl_Reg;
	--out_Reg(39 downto 32)  <= control_signal;
	out_Reg(31 downto 0) <= outputRegister;


	process (CLOCK, RESET)	

	variable counter : natural := 0;

	begin
		if RESET = '1' then
			current_state <= WAIT_DATA;
			state_Reg <= (others => '0');
		
		elsif(CLOCK = '1' and CLOCK'event) then
			case current_state is
				when WAIT_DATA =>
					state_Reg <= (1 => '1', others => '0');
					
					if ctrl_Reg(0) = '1' then
						current_state <= ASSERT_CE;
					elsif ctrl_Reg(1) = '1' then
						current_state <= RESET_FF;
					end if;
				
				when RESET_FF =>
					control_signal(5) <= '1'; --SR
					current_state <= DEASSERT_ALL;
					
				
				when ASSERT_CE =>
					inputRegister <= in_Reg(31 downto 0);
					control_signal(0) <= '1'; --CE
					counter := 0;
					current_state <= CLOCK_1;
				
				when CLOCK_1 =>
					control_signal(4) <= '0';
					control_signal(1) <= '1';
					counter := counter + 1;
					current_state <= CLOCK_2;
				
				when CLOCK_2 =>
					control_signal(1) <= '0';
					control_signal(2) <= '1';
					
					current_state <= CLOCK_3;
					
				when CLOCK_3 =>
					control_signal(2) <= '0';
					control_signal(3) <= '1';
					
					current_state <= CLOCK_4;
				
				when CLOCK_4 =>
					control_signal(3) <= '0';
					control_signal(4) <= '1';
					if(counter = 4) then
						current_state <= DEASSERT_ALL;
					else
						current_state <= CLOCK_1;
					end if;
				
				when DEASSERT_ALL =>
					state_Reg(0) <= '1';
					control_signal(5 downto 0) <= (others => '0');
					
					if ctrl_Reg(0) = '0' and ctrl_Reg(1) = '0'  then
						current_state <= WAIT_DATA;
					end if;
				when others => current_state <= WAIT_DATA;
			end case;
		end if;
	end process;
	
end architecture;
