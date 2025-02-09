//Verilog HDL for "PLL_D", "testbench" "functional"

`timescale 1ns/1ps

module testbench(out);

output reg out;

initial begin
	out = 1'b0;
	#1
	out = 1'b1;
end

endmodule
