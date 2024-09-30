import CORE_PKG::*;

module ALU (
  // General Inputs
  input logic reset,

  // Inputs from decode
  input logic alu_enable_ip,
  input alu_opcode_e alu_operator_ip,
  input logic [31:0] alu_operand_a_ip,
  input logic [31:0] alu_operand_b_ip, 

  // Outputs to LSU, MEM, and Fetch
  output logic [31:0] alu_result_op,
  output logic alu_valid_op
);

  always_comb begin
    case(alu_operator_ip) 
      ALU_ADD: begin
        alu_result_op = $signed(alu_operand_a_ip) + $signed(alu_operand_b_ip);
        alu_valid_op = 1;
      end
      ALU_SUB: begin
        alu_result_op = $signed(alu_operand_a_ip) - $signed(alu_operand_b_ip);
        alu_valid_op = 1;
      end
	  ALU_SLTS: begin
		if ($signed($signed(alu_operand_a_ip) - $signed(alu_operand_b_ip)) < 0) begin
			alu_result_op = 1'b1;
		end
		else begin
			alu_result_op = 1'b0;
		end
		alu_valid_op = 1;
	  end
    endcase
  end

endmodule
