TARBALL = ece3058_lab2_submission.tar.gz

all: sim

core_tb: ./include/core_pkg.sv ./testbench/core_tb.sv Core.sv Fetch.sv Instr_Mem.sv IF_Stage.sv ID_Stage.sv Stall_Control.sv ALU.sv EX_Stage.sv FWD_Control.sv DRAM.sv DFlipFlop.sv Register_File.sv Mem_Stage.sv WB_Stage.sv
	iverilog -g2012 -I include -s Core_tb -o $@ $^

sim: core_tb
	vvp $<

submit: clean
	tar -czvf $(TARBALL) $(wildcard *.sv) include/core_pkg.sv
	@echo
	@echo 'Submission tarball written to' $(TARBALL)
	@echo 'Please decompress it yourself and make sure it looks right!'
	@echo 'Then submit it to Gradescope'


clean:
	rm -f core_tb Core_Simulation.vcd $(TARBALL)

