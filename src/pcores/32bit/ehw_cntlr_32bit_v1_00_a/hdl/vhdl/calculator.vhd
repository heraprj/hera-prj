----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    15:12:19 04/05/2011 
-- Design Name: 
-- Module Name:    calculator - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;


-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity calculator is
Port (CLOCK, RESET : in  std_logic;
				In_EHW    	 : in  STD_LOGIC_VECTOR (31 downto 0);
				In_Memory 	 : in STD_LOGIC_VECTOR  (31 downto 0);
				Do    		 : in  STD_LOGIC;
				granularity  : in  STD_LOGIC_VECTOR  (2 downto 0);
				Fitness 		 : out STD_LOGIC_VECTOR (31 downto 0)
				
				);
end calculator;



architecture Behavioral of calculator is

signal a : std_logic_vector(31 downto 0);
signal ones_cnt1 : std_logic_vector(4 downto 0);
signal ones_cnt2 : std_logic_vector(4 downto 0);
signal Fitness_int : std_logic_vector(31 downto 0);
--signal granularity : std_logic_vector(2 downto 0);

component adder is
port(
clk: IN std_logic;
reset: IN std_logic;
a :  IN Std_Logic_vector(1 TO 16);
granularity  : in  STD_LOGIC_VECTOR  (2 downto 0);
sum : OUT Std_Logic_vector(4 downto 0));
end component;

component adder1 is
port(
clk: IN std_logic;
reset: IN std_logic;
a :  IN Std_Logic_vector(1 TO 16);
granularity  : in  STD_LOGIC_VECTOR  (2 downto 0);
sum : OUT Std_Logic_vector(4 downto 0));
end component;


begin

a <= In_EHW xnor In_Memory;
Fitness <= Fitness_int;
--granularity <=  granularity;

adderinst1: adder1
port map (
clk => CLOCK,
reset => RESET,
a => a(31 downto 16),
granularity  => granularity, 
sum => ones_cnt1);

adderinst2: adder
port map (
clk => CLOCK,
reset => RESET,
a => a(15 downto 0),
granularity  => granularity, 
sum => ones_cnt2);

 process (CLOCK , RESET, Do)
 begin
 if ( RESET = '1') then
 Fitness_int <= x"00000000";
 elsif (CLOCK='1' and CLOCK' event) then
          if (Do = '1')then
            Fitness_int <= Fitness_int + ones_cnt1+ ones_cnt2;
				else Fitness_int <= Fitness_int;
          end if;
 end if;
 end process;
end Behavioral;

