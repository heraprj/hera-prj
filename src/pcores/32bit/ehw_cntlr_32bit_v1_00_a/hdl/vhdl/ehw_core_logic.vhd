library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity ehw_core_logic is
	Port (	CLOCK        : in  std_logic;
				RESET 		 : in  std_logic;
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
end ehw_core_logic;


architecture Behavioral of ehw_core_logic is
	signal control_signal : std_logic_vector(7 downto 0) := (others => '0');  -- Signal for pins: CLK, CE e SR
	signal inputRegister  : std_logic_vector(31 downto 0) := (others => '0'); -- Local input register
	signal outputRegister : std_logic_vector(31 downto 0) := (others => '0'); -- Local output register
	
	signal program_count : std_logic_vector(11 downto 0) := (others => '0'); -- Local output register

	signal out_0_0, out_0_1, out_0_2, out_0_3 : std_logic_vector(7 downto 0) := (others => '0'); -- first column intermediate outputs 
	signal out_1_0, out_1_1, out_1_2, out_1_3 : std_logic_vector(7 downto 0) := (others => '0'); -- second column intermediate outputs
	signal out_2_0, out_2_1, out_2_2, out_2_3 : std_logic_vector(7 downto 0) := (others => '0'); -- third column intermediate outputs	
	signal dummy0, dummy1, dummy2, dummy3, dummy4, dummy5, dummy6, dummy7, dummy8, dummy9 : std_logic_vector(23 downto 0) := (others => '0');
	signal dummy10, dummy11, dummy12, dummy13, dummy14, dummy15 : std_logic_vector(23 downto 0) := (others => '0');

	type STATE_TYPE is (START, ASSERT_CE , DEASSERT_ALL, CLOCK_1, CLOCK_2, CLOCK_3, CLOCK_4, RESET_FF);
	attribute ENUM_ENCODING: STRING;
	attribute ENUM_ENCODING of STATE_TYPE:type is "000 001 010 011 100 101 110 111";
	
	signal current_state : STATE_TYPE := START;
	
	signal PROG_DONE : std_logic_vector(31 downto 0) := (others => '0'); --Cell output
	signal Memory_read_address_int_ip: std_logic_vector(11 downto 0) := (others => '0');
	signal Memory_read_address_int_op: std_logic_vector(11 downto 0) := (others => '0');
	signal Do_int: std_logic;
	signal Do: std_logic;
	signal Reset_cal_int: std_logic;
	signal Reset_cal: std_logic;
	
	component SingleCell
		port (
			inputReg	: in std_logic_vector(7 downto 0);
			outputReg	: out std_logic_vector(31 downto 0);
			CLOCK 	 	: in std_logic_vector(3 downto 0);
			CE	 	: in std_logic;
			SR		: in std_logic
		);
	end component;
	
component	calculator 
Port (CLOCK, RESET : in  std_logic;
				In_EHW    	 : in  STD_LOGIC_VECTOR (31 downto 0);
				In_Memory 	 : in STD_LOGIC_VECTOR  (31 downto 0);
				Do    		 : in  STD_LOGIC;
				granularity  : in  STD_LOGIC_VECTOR  (2 downto 0);
				Fitness 		 : out STD_LOGIC_VECTOR (31 downto 0)
				
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

calculatorInstance : calculator
	port map (
	CLOCK => CLOCK,
	RESET => Reset_cal,
   In_EHW  =>   outputRegister,	 	
	In_Memory =>  MEM_OUT,
	Do    	=> 	Do, 
	granularity => granularity,
	Fitness 	=> out_Reg	
	);

Memory_read_address_ip <= Memory_read_address_int_ip;
Memory_read_address_op <= Memory_read_address_int_op;
PROG <= PROG_DONE;
Do <= Do_int;
Reset_cal <= Reset_cal_int;

inputRegister <= in_Reg(31 downto 0);

process (CLOCK, RESET, START_P, PROG_DONE)	

variable counter : natural := 0;

	begin
		if (RESET = '1' or START_P = x"0000000")then
			current_state <= START;
			Do_int <= '0';
			Memory_read_address_int_ip <= "000000000000";
			Memory_read_address_int_op <= "000000000000";
			program_count <= "000000000000";
			PROG_DONE <= x"00000000";
			Reset_cal_int <= '1';
			
		elsif(CLOCK = '1' and CLOCK'event) then
		
		if ( START_P = x"FFFFFFFF" and PROG_DONE = x"00000000") then
		
			case current_state is
				when START =>
					
					control_signal(5 downto 0) <= (others => '0');
					if(Memory_read_address_int_ip = "000000000000") then
					
								
									case test_case is
										when "0000" =>
												program_count <= "000000000000";
										when "0001" => ---256
												program_count <= "000011111111";
										when "0010" =>  ---512
												program_count <= "000111111111";
										when "0011" =>  ---1024
												program_count <= "001111111111";
										when "0100" =>  ---2048
												program_count <= "011111111111";
										when "0101" =>  ---4096
												program_count <= "111111111111";
									when others => program_count <= "111111111111";
									end case;
					end if;
					Reset_cal_int <= '0';
					current_state <= RESET_FF;
					
								
				when RESET_FF =>
					control_signal(5) <= '1'; --Activate SR
					Do_int <= '0';
					current_state <= DEASSERT_ALL;
					
				when DEASSERT_ALL =>
					
					
					control_signal(5 downto 0) <= (others => '0');
					
					current_state <= ASSERT_CE;
					
				when ASSERT_CE =>
					--inputRegister <= in_Reg(31 downto 0);
					control_signal(0) <= '1'; --Activate CE
					
					current_state <= CLOCK_1;
				
				when CLOCK_1 =>
				   control_signal(4) <= '0';
					control_signal(1) <= '1';
					
					if(Memory_read_address_int_ip < "000000000100") then
						Do_int <= '0';
						
					else
						Do_int <= '1';
					end if;  
					
					current_state <= CLOCK_2;
				
				when CLOCK_2 =>
					control_signal(1) <= '0';
					control_signal(2) <= '1';
					Do_int <= '0';
					current_state <= CLOCK_3;
					
				when CLOCK_3 =>
					control_signal(2) <= '0';
					control_signal(3) <= '1';
					
					current_state <= CLOCK_4;
				
				when CLOCK_4 =>
					control_signal(3) <= '0';
					control_signal(4) <= '1';					
					current_state <= CLOCK_1;
					
					if(Memory_read_address_int_ip < "111111111111") then
						Memory_read_address_int_ip <= Memory_read_address_int_ip + 1;
					end if;
					
					if(Memory_read_address_int_ip > "000000000011") then
						Memory_read_address_int_op  <= Memory_read_address_int_op + 1;
					end if;

					if ( Memory_read_address_int_op = program_count) then
						PROG_DONE <= x"FFFFFFFD" ;
					end if;
					
				when others => current_state <= START;
			end case;
			end if;
			
			   if ( START_P = x"FFFFFFFF" and PROG_DONE = x"FFFFFFFD") then
		           Do_int <= '1';
					  control_signal(5 downto 0) <= (others => '0');
					  PROG_DONE <= x"FFFFFFFE" ;
				end if;
				
				if ( START_P = x"FFFFFFFF" and PROG_DONE = x"FFFFFFFE") then
		           Do_int <= '0';
					  PROG_DONE <= x"FFFFFFFF" ;
					  control_signal(5 downto 0) <= (others => '0');
				end if;
		end if;
	end process;
	
end architecture;