--	--	--	--	--	--	--	--	--	--
-- DGECS: Description Generator for Evolved Circuits Synthesis		--
-- VHDL specification of an individual (Virtex4 XC4VFX12)		--
--									--
-- authors: Davide Basilio Bartolini, Matteo Carminati			--
--	--	--	--	--	--	--	--	--	--

library ieee;
use ieee.std_logic_1164.all;

entity individual is
	generic(
		values: in std_logic_vector(0 to 8191)
	);
	port(	
		I:	in std_logic_vector(31 downto 0);
		O:	out std_logic_vector(31 downto 0)
	);
end individual; 

architecture modules_connections of individual is
component module
	generic(
		values: in std_logic_vector(0 to 511)
	);
	port(	
		I:	in std_logic_vector(7 downto 0);
		O:	out std_logic_vector(7 downto 0)
	);
end component;

signal O_M00, O_M01, O_M02, O_M03 : std_logic_vector(7 downto 0) := (others => '0'); --1st to 2nd colunm modules wires
signal O_M10, O_M11, O_M12, O_M13 : std_logic_vector(7 downto 0) := (others => '0'); --2nd to 3rd colunm modules wires 
signal O_M20, O_M21, O_M22, O_M23 : std_logic_vector(7 downto 0) := (others => '0'); --3rd to 4th colunm modules wires

begin
	-- Module 00
	M00 :	module	generic map(
				values => values(0 to 511)
			)
			port map(
				I(7 downto 0) => I(7 downto 0),
				O => O_M00
			);
	-- Module 01
	M01 :	module	generic map(
				values => values(512 to 1023)
			)
			port map(
				I(7 downto 0) => I(15 downto 8),
				O => O_M01
			);
	-- Module 02
	M02 :	module	generic map(
				values => values(1024 to 1535)
			)
			port map(
				I(7 downto 0) => I(23 downto 16),
				O => O_M02
			);
	-- Module 03
	M03 :	module	generic map(
				values => values(1536 to 2047)
			)
			port map(
				I(7 downto 0) => I(31 downto 24),
				O => O_M03
			);
	-- Module 10
	M10 :	module	generic map(
				values => values(2048 to 2559)
			)
			port map(
				I(1 downto 0) => O_M03(1 downto 0),
				I(3 downto 2) => O_M02(1 downto 0),
				I(5 downto 4) => O_M01(1 downto 0),
				I(7 downto 6) => O_M00(1 downto 0),
				O => O_M10
			);
	-- Module 11
	M11 :	module	generic map(
				values => values(2560 to 3071)
			)
			port map(
				I(1 downto 0) => O_M03(3 downto 2),
				I(3 downto 2) => O_M02(3 downto 2),
				I(5 downto 4) => O_M01(3 downto 2),
				I(7 downto 6) => O_M00(3 downto 2),
				O => O_M11
			);
	-- Module 12
	M12 :	module	generic map(
				values => values(3072 to 3583)
			)
			port map(
				I(1 downto 0) => O_M03(5 downto 4),
				I(3 downto 2) => O_M02(5 downto 4),
				I(5 downto 4) => O_M01(5 downto 4),
				I(7 downto 6) => O_M00(5 downto 4),
				O => O_M12
			);
	-- Module 13
	M13 :	module	generic map(
				values => values(3584 to 4095)
			)
			port map(
				I(1 downto 0) => O_M03(7 downto 6),
				I(3 downto 2) => O_M02(7 downto 6),
				I(5 downto 4) => O_M01(7 downto 6),
				I(7 downto 6) => O_M00(7 downto 6),
				O => O_M13
			);
	-- Module 20
	M20 :	module	generic map(
				values => values(4096 to 4607)
			)
			port map(
				I(1 downto 0) => O_M13(7 downto 6),
				I(3 downto 2) => O_M12(7 downto 6),
				I(5 downto 4) => O_M11(7 downto 6),
				I(7 downto 6) => O_M10(7 downto 6),
				O => O_M20
			);
	-- Module 21
	M21 :	module	generic map(
				values => values(4608 to 5119)
			)
			port map(
				I(1 downto 0) => O_M13(5 downto 4),
				I(3 downto 2) => O_M12(5 downto 4),
				I(5 downto 4) => O_M11(5 downto 4),
				I(7 downto 6) => O_M10(5 downto 4),
				O => O_M21
			);
	-- Module 22
	M22 :	module	generic map(
				values => values(5120 to 5631)
			)
			port map(
				I(1 downto 0) => O_M13(3 downto 2),
				I(3 downto 2) => O_M12(3 downto 2),
				I(5 downto 4) => O_M11(3 downto 2),
				I(7 downto 6) => O_M10(3 downto 2),
				O => O_M22
			);
	-- Module 23
	M23 :	module	generic map(
				values => values(5632 to 6143)
			)
			port map(
				I(1 downto 0) => O_M13(1 downto 0),
				I(3 downto 2) => O_M12(1 downto 0),
				I(5 downto 4) => O_M11(1 downto 0),
				I(7 downto 6) => O_M10(1 downto 0),
				O => O_M23
			);
	-- Module 30
	M30 :	module	generic map(
				values => values(6144 to 6655)
			)
			port map(
				I(1 downto 0) => O_M23(1 downto 0),
				I(3 downto 2) => O_M22(1 downto 0),
				I(5 downto 4) => O_M21(1 downto 0),
				I(7 downto 6) => O_M20(1 downto 0),
				O => O(31 downto 24)
			);
	-- Module 31
	M31 :	module	generic map(
				values => values(6656 to 7167)
			)
			port map(
				I(1 downto 0) => O_M23(3 downto 2),
				I(3 downto 2) => O_M22(3 downto 2),
				I(5 downto 4) => O_M21(3 downto 2),
				I(7 downto 6) => O_M20(3 downto 2),
				O => O(23 downto 16)
			);
	-- Module 32
	M32 :	module	generic map(
				values => values(7168 to 7679)
			)
			port map(
				I(1 downto 0) => O_M23(5 downto 4),
				I(3 downto 2) => O_M22(5 downto 4),
				I(5 downto 4) => O_M21(5 downto 4),
				I(7 downto 6) => O_M20(5 downto 4),
				O => O(15 downto 8)
			);
	-- Module 33
	M33 :	module	generic map(
				values => values(7680 to 8191)
			)
			port map(
				I(1 downto 0) => O_M23(7 downto 6),
				I(3 downto 2) => O_M22(7 downto 6),
				I(5 downto 4) => O_M21(7 downto 6),
				I(7 downto 6) => O_M20(7 downto 6),
				O => O(7 downto 0)
			);

end modules_connections;
