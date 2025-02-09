//Verilog HDL for "PLL_D", "frequency_divider" "functional"

module frequency_divider(in, rst, out);

input in;
input rst;
output reg out;

reg count;

wire count_comb;
wire out_comb;

assign count_comb = ((count == 15) ? 0 : (count + 1));
assign out_comb = ((count < 8) ? 0 : 1);

always@(posedge in or negedge rst) begin
	if (!rst) begin
		count <= 0;
		out <= 0;
	end
	else begin
		count <= count_comb;
		out <= out_comb;
	end
end

endmodule
