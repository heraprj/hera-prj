--	--	--	--	--	--	--	--	--	--
-- DGECS: Description Generator for Evolved Circuits Synthesis		--
-- VHDL specification of a column (Virtex4 XC4VFX12)			--
--									--
-- authors: Davide Basilio Bartolini, Matteo Carminati			--
--	--	--	--	--	--	--	--	--	--

library ieee;
use ieee.std_logic_1164.all;

entity column is
	generic(
		values: in std_logic_vector(0 to 127)
	);
	port(
		I:	in std_logic_vector(7 downto 0);
		O:	out std_logic_vector(7 downto 0)
	);
end column; 

architecture cells_connections of column is
component cell 
	generic(
		values:	in std_logic_vector(0 to 15)
	);
	port(	
		I:	in std_logic_vector(3 downto 0);
		O:	out std_logic
	);
end component;
begin
	-- Cell 0
	C0 :	cell	generic map(
				values => values(0 to 15)
			)
			port map(
				I(0) => I(0),
				I(1) => I(1),
				I(2) => I(2),
				I(3) => I(3),
				O => O(0)
			);
	-- Cell 1
	C1 :	cell 	generic map(
				values => values(16 to 31)
			)
			port map(
				I(0) => I(4),
				I(1) => I(5),
				I(2) => I(6),
				I(3) => I(7),
				O => O(1)
			);
	-- Cell 2
	C2 :	cell  	generic map(
				values => values(32 to 47)
			)
			port map(
				I(0) => I(0),
				I(1) => I(1),
				I(2) => I(2),
				I(3) => I(3),
				O => O(2)
			);
	-- Cell 3
	C3 :	cell  	generic map(
				values => values(48 to 63)
			)
			port map(
				I(0) => I(4),
				I(1) => I(5),
				I(2) => I(6),
				I(3) => I(7),
				O => O(3)
			);
	-- Cell 4
	C4 :	cell	generic map(
				values => values(64 to 79)
			)
			port map(
				I(0) => I(0),
				I(1) => I(1),
				I(2) => I(2),
				I(3) => I(3),
				O => O(4)
			);
	-- Cell 5
	C5 :	cell  	generic map(
				values => values(80 to 95)
			)
			port map(
				I(0) => I(4),
				I(1) => I(5),
				I(2) => I(6),
				I(3) => I(7),
				O => O(5)
			);
	-- Cell 6
	C6 :	cell  	generic map(
				values => values(96 to 111)
			)
			port map(
				I(0) => I(0),
				I(1) => I(1),
				I(2) => I(2),
				I(3) => I(3),
				O => O(6)
			);
	-- Cell 7
	C7 :	cell  	generic map(
				values => values(112 to 127)
			)
			port map(
				I(0) => I(4),
				I(1) => I(5),
				I(2) => I(6),
				I(3) => I(7),
				O => O(7)
			);
end cells_connections;
