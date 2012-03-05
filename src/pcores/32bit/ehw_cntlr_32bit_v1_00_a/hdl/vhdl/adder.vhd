
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;


-- This macro outputs a binary count of the number of input bits that
-- are active at one time. It effectively sums 16 1-bit numbers.


LIBRARY IEEE;
USE IEEE.Std_Logic_1164.all;

ENTITY adder IS
port(
clk: IN std_logic;
reset: IN std_logic;
a :  IN Std_Logic_vector(1 TO 16);
granularity  : in  STD_LOGIC_VECTOR  (2 downto 0);
sum : OUT Std_Logic_vector(4 downto 0)

 );
END adder;




ARCHITECTURE beh2 OF adder IS



FUNCTION sum2bits (a : std_logic_vector(1 to 2) ) RETURN
std_logic_vector IS
VARIABLE s : std_logic_vector(1 downto 0);
BEGIN
s(0) := a(1) XOR a(2);
s(1) := a(1) AND a(2);
RETURN s;
END FUNCTION;

FUNCTION sum4bits (a : std_logic_vector(1 to 4) ) RETURN
std_logic_vector IS
VARIABLE sa, sb : std_logic_vector(1 downto 0);
VARIABLE s : std_logic_vector(2 downto 0);
BEGIN
sa := sum2bits(a(1 to 2));
sb := sum2bits(a(3 to 4));
s(0) := sa(0) XOR sb(0);
s(1) := (sa(0) AND sb(0)) OR (sa(1) XOR sb(1));
s(2) := sa(1) AND sb(1);
RETURN s;
END FUNCTION;

FUNCTION sum8bits (a : std_logic_vector(1 to 8 )) RETURN
std_logic_vector IS
VARIABLE sa, sb : std_logic_vector(2 downto 0);
VARIABLE c1 : std_logic;
VARIABLE s : std_logic_vector(3 downto 0);

BEGIN
sa := sum4bits(a(1 to 4));
sb := sum4bits(a(5 to 8) );
s(0) := sa(0) XOR sb(0);
c1 := sa(0) AND sb(0);
s(1) := c1 XOR (sa(1) XOR sb(1));
s(2) := (c1 AND (sa(1) OR sb(1))) OR (sa(1) AND sb(1)) OR (sa(2) XOR
sb(2));
s(3) := sa(2) AND sb(2);
RETURN s;
END FUNCTION;

FUNCTION sum16bits (a : std_logic_vector(1 to 16) ) RETURN
std_logic_vector IS
VARIABLE sa, sb : std_logic_vector(3 downto 0);
VARIABLE c1, c2 : std_logic;
VARIABLE s : std_logic_vector(4 downto 0);
BEGIN
sa := sum8bits(a(1 to 8));
sb := sum8bits(a(9 to 16));
s(0) := sa(0) XOR sb(0);
c1 := sa(0) AND sb(0);
s(1) := c1 XOR (sa(1) XOR sb(1));
c2 := (c1 AND (sa(1) OR sb(1))) OR (sa(1) AND sb(1));
s(2) := c2 XOR (sa(2) XOR sb(2));
s(3) := (c2 AND (sa(2) OR sb(2))) OR (sa(2) AND sb(2)) OR (sa(3) XOR
sb(3));
s(4) := sa(3) AND sb(3);
RETURN s;
END FUNCTION;


begin

process (clk, reset)
begin
if (reset = '1')then
		sum <= "00000";

elsif (clk='1' and clk' event) then

 case granularity  is
				when "000" =>
					sum <= "00000" ;
				when "001" =>
					sum <= "0000" & a(1);
				when "010" =>
					sum <= "000" &sum2bits (a(1 to 2));
				when "011" =>
					sum <= "00" & sum4bits (a(1 to 4));
				when "100" =>
					sum <= '0' & sum8bits (a(1 to 8));
				when "101" =>
					sum <= sum16bits(a);
				 when "110" =>
					sum <= sum16bits(a); 
					
	when others => sum <= "00000" ;
			end case;
  
 
end if;
end process;

END ARCHITECTURE beh2;


