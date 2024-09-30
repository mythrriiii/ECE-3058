//***********************************************************
// ECE 3058 Architecture Concurrency and Energy in Computation
//
// MIPS Processor System Verilog Behavioral Model
//
// School of Electrical & Computer Engineering
// Georgia Institute of Technology
// Atlanta, GA 30332
//
//  Engineer:   Zou, Ivan
//  Module:     core_tb
//  Functionality:
//      This is the testbed for the Single cycle RISCV processor
//
//***********************************************************
`timescale 1ns / 1ns

module Core_tb;

// Clock and Reset signals to simulate as input into core
	logic clk = 1;
	logic mem_enable;
	logic reset;

	// local variables to display for testbench
	logic[6:0] cycle_count;
	
	integer i;
	initial
	begin
		cycle_count = 0;

		// do the simulation
		$dumpfile("Core_Simulation.vcd");

		// dump all the signals into the vcd waveforem file
		$dumpvars(0, Core_tb);

		reset = 1'b1;
		mem_enable = 1'b1;

		// Set the Test instructions and preset MEM and Regfile here if desired

		// Some sample test instructions to get you started 

		#1
		// NOP since first instruction is skipped 
		core_proc.MainMemory.data_RAM[0] = 8'h00;
		core_proc.MainMemory.data_RAM[1] = 8'h00;
		core_proc.MainMemory.data_RAM[2] = 8'h00;
		core_proc.MainMemory.data_RAM[3] = 8'h00;

		// addi x5, x5, 80
		core_proc.MainMemory.data_RAM[4] = 8'h05;
		core_proc.MainMemory.data_RAM[5] = 8'h02;
		core_proc.MainMemory.data_RAM[6] = 8'h82;
		core_proc.MainMemory.data_RAM[7] = 8'h93;
		// x5 = 80

		// addi x4, x5, 20
		core_proc.MainMemory.data_RAM[8] = 8'h01;
		core_proc.MainMemory.data_RAM[9] = 8'h42;
		core_proc.MainMemory.data_RAM[10] = 8'h82;
		core_proc.MainMemory.data_RAM[11] = 8'h13;
		// x4 = 100

		// add x10, x5, x4
		core_proc.MainMemory.data_RAM[12] = 8'h00;
		core_proc.MainMemory.data_RAM[13] = 8'h42;
		core_proc.MainMemory.data_RAM[14] = 8'h85;
		core_proc.MainMemory.data_RAM[15] = 8'h33;
		// x10 = 180

		// sub x12, x5, x4
		core_proc.MainMemory.data_RAM[16] = 8'h40;
		core_proc.MainMemory.data_RAM[17] = 8'h42;
		core_proc.MainMemory.data_RAM[18] = 8'h86;
		core_proc.MainMemory.data_RAM[19] = 8'h33;
		// x12 = -20
		
		// sw x10, 256(x5)
		core_proc.MainMemory.data_RAM[20] = 8'h10;
		core_proc.MainMemory.data_RAM[21] = 8'hA2;
		core_proc.MainMemory.data_RAM[22] = 8'hA0;
		core_proc.MainMemory.data_RAM[23] = 8'h23;
		// mem location 336 = 180
		
		// invalid sw
		// sw x10, 257(x5)
		core_proc.MainMemory.data_RAM[24] = 8'h10;
		core_proc.MainMemory.data_RAM[25] = 8'hA2;
		core_proc.MainMemory.data_RAM[26] = 8'hA0;
		core_proc.MainMemory.data_RAM[27] = 8'hA3;
		// nothing should change, mem location 336 = 180
		
		// lw x13, 256(x5)
		core_proc.MainMemory.data_RAM[28] = 8'h10;
		core_proc.MainMemory.data_RAM[29] = 8'h02;
		core_proc.MainMemory.data_RAM[30] = 8'hA6;
		core_proc.MainMemory.data_RAM[31] = 8'h83;
		// x13 = 180
		
		// invalid lw
		// lw x5, 257(x5)
		core_proc.MainMemory.data_RAM[32] = 8'h10;
		core_proc.MainMemory.data_RAM[33] = 8'h12;
		core_proc.MainMemory.data_RAM[34] = 8'hA2;
		core_proc.MainMemory.data_RAM[35] = 8'h83;
		// nothing should change, x5 should be 80

		// slts x3, x10, x12
		core_proc.MainMemory.data_RAM[36] = 8'h00;
		core_proc.MainMemory.data_RAM[37] = 8'hC5;
		core_proc.MainMemory.data_RAM[38] = 8'h21;
		core_proc.MainMemory.data_RAM[39] = 8'hB3;
		// x3 = 0
		
		// slts x3, x12, x10
		core_proc.MainMemory.data_RAM[40] = 8'h00;
		core_proc.MainMemory.data_RAM[41] = 8'hA6;
		core_proc.MainMemory.data_RAM[42] = 8'h21;
		core_proc.MainMemory.data_RAM[43] = 8'hB3;
		// x3 = 1
		
		// slts x3, x12, x12
		core_proc.MainMemory.data_RAM[44] = 8'h00;
		core_proc.MainMemory.data_RAM[45] = 8'hC6;
		core_proc.MainMemory.data_RAM[46] = 8'h21;
		core_proc.MainMemory.data_RAM[47] = 8'hB3;
		// x3 = 0
		
		// sub x8, x12, x5
		core_proc.MainMemory.data_RAM[48] = 8'h40;
		core_proc.MainMemory.data_RAM[49] = 8'h56;
		core_proc.MainMemory.data_RAM[50] = 8'h04;
		core_proc.MainMemory.data_RAM[51] = 8'h33;
		// x8 = -100
		
		// slts x18, x8, x12
		core_proc.MainMemory.data_RAM[52] = 8'h00;
		core_proc.MainMemory.data_RAM[53] = 8'hC4;
		core_proc.MainMemory.data_RAM[54] = 8'h29;
		core_proc.MainMemory.data_RAM[55] = 8'h33;
		// x18 = 1
		
		// slts x16, x5, x4
		core_proc.MainMemory.data_RAM[56] = 8'h00;
		core_proc.MainMemory.data_RAM[57] = 8'h42;
		core_proc.MainMemory.data_RAM[58] = 8'hA8;
		core_proc.MainMemory.data_RAM[59] = 8'h33;
		// x16 = 1
		
		// addi x7, x0, 190
		core_proc.MainMemory.data_RAM[60] = 8'h0B;
		core_proc.MainMemory.data_RAM[61] = 8'hE0;
		core_proc.MainMemory.data_RAM[62] = 8'h03;
		core_proc.MainMemory.data_RAM[63] = 8'h93;
		// x7 = 190
		
		// slts x9, x10, x7
		core_proc.MainMemory.data_RAM[64] = 8'h00;
		core_proc.MainMemory.data_RAM[65] = 8'h75;
		core_proc.MainMemory.data_RAM[66] = 8'h24;
		core_proc.MainMemory.data_RAM[67] = 8'hB3;
		// x9 = 1
		
		// slts x10, x9, x7
		core_proc.MainMemory.data_RAM[68] = 8'h00;
		core_proc.MainMemory.data_RAM[69] = 8'h74;
		core_proc.MainMemory.data_RAM[70] = 8'hA5;
		core_proc.MainMemory.data_RAM[71] = 8'h33;
		// x10 = 1
		
		// addi x7, x0, -190
		core_proc.MainMemory.data_RAM[72] = 8'hF4;
		core_proc.MainMemory.data_RAM[73] = 8'h20;
		core_proc.MainMemory.data_RAM[74] = 8'h03;
		core_proc.MainMemory.data_RAM[75] = 8'h93;
		// x7 = -190
		
		// slts x8, x0, x7
		core_proc.MainMemory.data_RAM[76] = 8'h00;
		core_proc.MainMemory.data_RAM[77] = 8'h70;
		core_proc.MainMemory.data_RAM[78] = 8'h24;
		core_proc.MainMemory.data_RAM[79] = 8'h33;
		// x8 = 0
		
		// addi x20, x0, 100
		core_proc.MainMemory.data_RAM[80] = 8'h06;
		core_proc.MainMemory.data_RAM[81] = 8'h40;
		core_proc.MainMemory.data_RAM[82] = 8'h0A;
		core_proc.MainMemory.data_RAM[83] = 8'h13;
		// x20 = 100
		
		// addi x21, x20, -20
		core_proc.MainMemory.data_RAM[84] = 8'hFE;
		core_proc.MainMemory.data_RAM[85] = 8'hCA;
		core_proc.MainMemory.data_RAM[86] = 8'h0A;
		core_proc.MainMemory.data_RAM[87] = 8'h93;
		// x21 = 80
		
		// addi x22, x0, 80
		core_proc.MainMemory.data_RAM[88] = 8'h05;
		core_proc.MainMemory.data_RAM[89] = 8'h00;
		core_proc.MainMemory.data_RAM[90] = 8'h0B;
		core_proc.MainMemory.data_RAM[91] = 8'h13;
		// x22 = 80
		
		// addi x23, x21, -120
		core_proc.MainMemory.data_RAM[92] = 8'hF8;
		core_proc.MainMemory.data_RAM[93] = 8'h8A;
		core_proc.MainMemory.data_RAM[94] = 8'h8B;
		core_proc.MainMemory.data_RAM[95] = 8'h93;
		// x23 = -40
		
		// addi x24, x23, -120
		core_proc.MainMemory.data_RAM[96] = 8'hF8;
		core_proc.MainMemory.data_RAM[97] = 8'h8B;
		core_proc.MainMemory.data_RAM[98] = 8'h8C;
		core_proc.MainMemory.data_RAM[99] = 8'h13;
		// x24 = -160
		
		// add x15, x15, x23
		core_proc.MainMemory.data_RAM[24] = 8'h01;
		core_proc.MainMemory.data_RAM[25] = 8'h77;
		core_proc.MainMemory.data_RAM[26] = 8'h87;
		core_proc.MainMemory.data_RAM[27] = 8'hB3;
		// x15 = -40
		
		// sw x23, 100(x23)
		core_proc.MainMemory.data_RAM[28] = 8'h07;
		core_proc.MainMemory.data_RAM[29] = 8'h7B;
		core_proc.MainMemory.data_RAM[30] = 8'hA2;
		core_proc.MainMemory.data_RAM[31] = 8'h23;
		// M[60] = -40
		
		// addi x23, x23, 1000
		core_proc.MainMemory.data_RAM[32] = 8'h3E;
		core_proc.MainMemory.data_RAM[33] = 8'h8B;
		core_proc.MainMemory.data_RAM[34] = 8'h8B;
		core_proc.MainMemory.data_RAM[35] = 8'h93;
		// x23 = 960
		
		// lw x23, 100(x15)
		core_proc.MainMemory.data_RAM[36] = 8'h06;
		core_proc.MainMemory.data_RAM[37] = 8'h47;
		core_proc.MainMemory.data_RAM[38] = 8'hAB;
		core_proc.MainMemory.data_RAM[39] = 8'h83;
		// x23 = -40

		// addi x23, x23, -20
		core_proc.MainMemory.data_RAM[40] = 8'hFE;
		core_proc.MainMemory.data_RAM[41] = 8'hCB;
		core_proc.MainMemory.data_RAM[42] = 8'h8B;
		core_proc.MainMemory.data_RAM[43] = 8'h93;
		// x23 = -60

		// slts x2, x24, x23
		core_proc.MainMemory.data_RAM[44] = 8'h01;
		core_proc.MainMemory.data_RAM[45] = 8'h7C;
		core_proc.MainMemory.data_RAM[46] = 8'h21;
		core_proc.MainMemory.data_RAM[47] = 8'h33;
		// x2 = 1

		// addi x15, x15, -20
		core_proc.MainMemory.data_RAM[48] = 8'hFE;
		core_proc.MainMemory.data_RAM[49] = 8'hC7;
		core_proc.MainMemory.data_RAM[50] = 8'h87;
		core_proc.MainMemory.data_RAM[51] = 8'h93;
		// x15 = -60

		// jal x10, -24
		core_proc.MainMemory.data_RAM[52] = 8'hFE;
		core_proc.MainMemory.data_RAM[53] = 8'h9F;
		core_proc.MainMemory.data_RAM[54] = 8'hF5;
		core_proc.MainMemory.data_RAM[55] = 8'h6F;
		// x10 = PC + 4, PC = PC - 24

		#5 reset = 1'b0;

		#50 $finish;
	end

	always
		#1 clk <= clk + 1;

	always @(posedge clk) begin
		if (~reset)
			cycle_count <= cycle_count + 1;
	end

	Core core_proc(
		// Inputs
		.clock(clk),
		.reset(reset),
		.mem_en(mem_enable)
	);

endmodule
