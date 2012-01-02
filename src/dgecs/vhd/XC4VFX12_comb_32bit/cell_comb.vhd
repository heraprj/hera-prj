--	--	--	--	--	--	--	--	--	--
-- DGECS: Description Generator for Evolved Circuits Synthesis		--
-- VHDL specification of the basic cell for the V4 XC4VFX12		--
--									--
-- authors: Davide Basilio Bartolini, Matteo Carminati			--
--	--	--	--	--	--	--	--	--	--

library ieee;
use ieee.std_logic_1164.all;

entity cell is
	generic(
		values:	in std_logic_vector(0 to 15)
	);
	port(
		I:	in std_logic_vector(3 downto 0);
		O:	out std_logic
	);
end cell; 

architecture lut of cell is

signal CFG0: std_logic_vector(7 downto 0); --First config byte
signal CFG1: std_logic_vector(7 downto 0); --Second config byte

begin

	CFG0 <= values(0 to 7);
	CFG1 <= values(8 to 15);

	O <=	CFG1(0) when I = "0000" else
		CFG1(1) when I = "0001" else
		CFG1(2) when I = "0010" else
		CFG1(3) when I = "0011" else
		CFG1(4) when I = "0100" else
		CFG1(5) when I = "0101" else
		CFG1(6) when I = "0110" else
		CFG1(7) when I = "0111" else
		CFG0(0) when I = "1000" else
		CFG0(1) when I = "1001" else
		CFG0(2) when I = "1010" else
		CFG0(3) when I = "1011" else
		CFG0(4) when I = "1100" else
		CFG0(5) when I = "1101" else
		CFG0(6) when I = "1110" else
		CFG0(7) when I = "1111";
end lut;
