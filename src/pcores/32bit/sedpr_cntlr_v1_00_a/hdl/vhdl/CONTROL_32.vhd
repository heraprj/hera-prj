library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity CONTROL_32 is
    PORT  (
    RESET_P                : IN  std_logic;                         -- Reset input Active high
    CLK_50               	: IN  std_logic;
    LOCKED_OUT          	: IN  std_logic;
    RECV_COMMNAD        	: IN  std_logic_vector(31 downto 0);
    BUSY                   : IN  std_logic;
    ROM_ADDRA_32        	: OUT std_logic_VECTOR(7 downto 0);  
    WRITE1                 : OUT  std_logic;
    MEM_EN_OUT             : OUT  std_logic;
    PROG_DONE              : OUT std_logic_vector(31 downto 0)
    );
end CONTROL_32;

architecture Behavioral of CONTROL_32 is


 SIGNAL PROG                 : STD_LOGIC;
 SIGNAL PROG_CNT            :  std_logic_vector(7 downto 0); 
--SIGNAL Delay_CNT            :  std_logic_vector(0 to 7); 
 SIGNAL ICAP_ADDR            :  std_logic_VECTOR(7 downto 0);
 SIGNAL WRITE_ICAP         : STD_LOGIC;
 SIGNAL MEM_EN                : STD_LOGIC; 
 
 
begin
    WRITE1 <= WRITE_ICAP;
    MEM_EN_OUT <= MEM_EN;
    PROG_DONE(0) <= PROG;
    PROG_DONE(31 downto 1) <= (others => '0');
    ROM_ADDRA_32 <= ICAP_ADDR ;
    
PROCESS (RESET_P, LOCKED_OUT, RECV_COMMNAD, CLK_50, BUSY)
begin
    IF (RESET_P = '1' or LOCKED_OUT = '0' or RECV_COMMNAD = x"00000000" ) THEN  
        WRITE_ICAP <= '1';
        PROG <= '0';
        ICAP_ADDR <= "00000000";
        PROG_CNT <= "00000000";
		--  Delay_CNT <= "00000000";
        MEM_EN <= '0';
    ELSIF (CLK_50='1' and CLK_50'event) THEN
        IF (RECV_COMMNAD = x"FFFFFFFF" AND MEM_EN = '0' and PROG <= '0' ) THEN
            MEM_EN   <= '1';
        ELSIF (MEM_EN = '1' and RECV_COMMNAD = x"FFFFFFFF" AND PROG = '0') THEN
        WRITE_ICAP <= '0';
        --  IF (BUSY= '0' and Delay_CNT = "00011111") THEN
			    IF (BUSY= '0') THEN
                IF (PROG_CNT = "01101110") THEN
                    WRITE_ICAP <= '1';
                    PROG <= '1';
                    MEM_EN <= '0';
                ELSE
					--	  Delay_CNT <= "00000000";
                    ICAP_ADDR <= ICAP_ADDR + 1; 
                    PROG_CNT <= PROG_CNT + 1 ; 
                END IF;
				--else
				--		Delay_CNT <= Delay_CNT + 1 ;
            END IF;
        END IF;
    END IF;    
	    
END PROCESS;           

end Behavioral;
