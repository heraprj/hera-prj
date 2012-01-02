--	--	--	--	--	--	--	--	--	--
-- DGECS: Description Generator for Evolved Circuits Synthesis		--
-- VHDL specification of a module (Virtex4 XC4VFX12)			--
--									--
-- authors: Davide Basilio Bartolini, Matteo Carminati			--
--	--	--	--	--	--	--	--	--	--

library ieee;
use ieee.std_logic_1164.all;

entity module is
	generic(
		values: in std_logic_vector(0 to 511)
	);
	port(	
		I:	in std_logic_vector(7 downto 0);
		O:	out std_logic_vector(7 downto 0)
	);
end module; 

architecture columns_connections of module is
component column 
	generic(
		values: in std_logic_vector(0 to 127)
	);
	port(
		I:	in std_logic_vector(7 downto 0);
		O:	out std_logic_vector(7 downto 0)
	);
end component;

signal C0_to_C1: std_logic_vector(7 downto 0); --1st to 2nd colunm wire
signal C1_to_C2: std_logic_vector(7 downto 0); --2nd to 3rd colunm wire
signal C2_to_C3: std_logic_vector(7 downto 0); --3rd to 4th colunm wire

begin
	-- Column 0
	C0 :	column	generic map(
				values => values(0 to 127)
			)
			port map(
				I => I,
				O => C0_to_C1
			);
	-- Column 1
	C1 :	column	generic map(
				values => values(128 to 255)
			)
			port map(
				I => C0_to_C1,
				O => C1_to_C2
			);
	-- Column 2
	C2 :	column	generic map(
				values => values(256 to 383)
			)
			port map(
				I => C1_to_C2,
				O => C2_to_C3
			);
	-- Column 3
	C3 :	column	generic map(
				values => values(384 to 511)
			)
			port map(
				I => C2_to_C3,
				O => O
			);

end columns_connections;
