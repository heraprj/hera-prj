library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity ehw_core_logic is
	Port (CLOCK, RESET : in  std_logic;
				in_Reg     	 : in  STD_LOGIC_VECTOR (7 downto 0);
				out_Reg 		 : out STD_LOGIC_VECTOR (31 downto 0);
				state_Reg	 : out STD_LOGIC_VECTOR (7 downto 0);
				ctrl_Reg 	 : in  STD_LOGIC_VECTOR (7 downto 0));
end ehw_core_logic;

architecture Behavioral of ehw_core_logic is
	signal control_signal : std_logic_vector(7 downto 0) := (others => '0'); --Da attaccare ai pin: CLK, CE e SR
	signal inputRegister  : std_logic_vector(7 downto 0) := (others => '0'); --Da attaccare agli ingressi della prima colonna
	signal outputRegister : std_logic_vector(7 downto 0) := (others => '0'); --Connesso all'uscita dell'individuo
	
	
	type STATE_TYPE is (WAIT_DATA, ASSERT_CE , DEASSERT_ALL, CLOCK_1, CLOCK_2, CLOCK_3, CLOCK_4, RESET_FF);
	attribute ENUM_ENCODING: STRING;
	attribute ENUM_ENCODING of STATE_TYPE:type is "000 001 010 011 100 101 110 111";
	
	signal current_state : STATE_TYPE := WAIT_DATA;
	
	component SingleCell
		port (inputReg  : in std_logic_vector(7 downto 0);
				outputReg : out std_logic_vector(31 downto 0);
				CLOCK 	 : in std_logic_vector(3 downto 0);
				CE 		 : in std_logic;
				SR			 : in std_logic);
	end component;
begin

	SingleCellInstance : SingleCell port map (CE => control_signal(0), SR => control_signal(5), inputReg => inputRegister(7 downto 0), CLOCK =>control_signal(4 downto 1), outputReg => out_Reg(31 downto 0));
	
	-- out_Reg(7 downto 0)	 <= outputRegister(7 downto 0);
	-- out_Reg(15 downto 8)  <= control_signal(7 downto 0);
	-- out_Reg(23 downto 16) <= inputRegister(7 downto 0) when ctrl_Reg(0) = '1' and ctrl_Reg(0)'event;
	
	--process (CLOCK, RESET)
	--begin
		--if RESET = '1' then
		--	current_state <= WAIT_DATA;
		--elsif CLOCK = '1' and CLOCK'event then
		--	current_state <= new_state;
		--end if;
	--end process;

	process (CLOCK, RESET)	
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
					control_signal(5) <= '1'; --Attivazione SR
					current_state <= DEASSERT_ALL;
					
				
				when ASSERT_CE =>
					inputRegister <= in_Reg(7 downto 0);
					control_signal(0) <= '1'; --Attivazione CE
					
					current_state <= CLOCK_1;
				
				when CLOCK_1 =>
					control_signal(1) <= '1';
				
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
					
					current_state <= DEASSERT_ALL;
				
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
	
end Behavioral;

