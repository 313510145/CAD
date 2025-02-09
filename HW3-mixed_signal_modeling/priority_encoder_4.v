//Verilog HDL for "ADC", "priority_encoder_4" "functional"

module priority_encoder_4(
	in_15, in_14, in_13, in_12, in_11, in_10, in_9, in_8, in_7, in_6, in_5, in_4, in_3, in_2, in_1, in_0,
	out_3, out_2, out_1, out_0
);

input in_15, in_14, in_13, in_12, in_11, in_10, in_9, in_8, in_7, in_6, in_5, in_4, in_3, in_2, in_1, in_0;
output out_3, out_2, out_1, out_0;

reg [3:0] out;

always@(*) begin
	if (in_15) begin
		out = 4'd15;
	end
	else if (in_14) begin
		out = 4'd14;
	end
	else if (in_13) begin
		out = 4'd13;
	end
	else if (in_12) begin
		out = 4'd12;
	end
	else if (in_11) begin
		out = 4'd11;
	end
	else if (in_10) begin
		out = 4'd10;
	end
	else if (in_9) begin
		out = 4'd9;
	end
	else if (in_8) begin
		out = 4'd8;
	end
	else if (in_7) begin
		out = 4'd7;
	end
	else if (in_6) begin
		out = 4'd6;
	end
	else if (in_5) begin
		out = 4'd5;
	end
	else if (in_4) begin
		out = 4'd4;
	end
	else if (in_3) begin
		out = 4'd3;
	end
	else if (in_2) begin
		out = 4'd2;
	end
	else if (in_1) begin
		out = 4'd1;
	end
	else if (in_0) begin
		out = 4'd0;
	end
	else begin
		out = 4'd0;
	end
end

assign out_3 = out[3];
assign out_2 = out[2];
assign out_1 = out[1];
assign out_0 = out[0];

endmodule
