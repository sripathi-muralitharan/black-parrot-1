/**
 *
 * Name:
 *   bp_cce_inst_stall.v
 *
 * Description:
 *   The stall unit collects the current instruction and status of other functional units to
 *   determine whether or not a stall must occur.
 *
 *   A stall prevents any changes to architectural state in the cycle asserted, and causes the
 *   current instruction to be replayed next cycle.
 *
 */

module bp_cce_inst_stall
  import bp_cce_pkg::*;
  #(parameter max_counter_val_p = "inv"
    , localparam cnt_width_lp   = `BSG_SAFE_CLOG2(max_counter_val_p)
    )
  (input                                         clk_i
   , input                                       reset_i
   , input bp_cce_inst_decoded_s                 decoded_inst_i

   // input queue valid signals
   , input                                       lce_req_v_i
   , input                                       lce_resp_v_i
   , input                                       mem_resp_v_i
   , input                                       pending_v_i

   // output queue ready signals
   , input                                       lce_cmd_ready_i
   , input                                       mem_cmd_ready_i

   // Messague Unit resource busy signals

   // message unit is busy doing something - block all ucode interactions
   , input                                       msg_busy_i
   // auto-forwarding using a resource
   , input                                       msg_pending_w_busy_i
   , input                                       msg_lce_cmd_busy_i
   , input                                       msg_lce_resp_busy_i
   , input                                       msg_mem_resp_busy_i
   , input                                       msg_spec_r_busy_i
   // TODO: might not be needed, directory writes from msg unit only happen
   // when msg_busy_i should be high when inv cmd processing
   , input                                       msg_dir_w_busy_i

   // Directory busy (e.g., processing read)
   , input                                       dir_busy_i

   // Stall outputs
   , output logic                                stall_o
   // TODO: move stall counter to bp_cce_perfmon?
   , output logic [cnt_width_lp-1:0]             stall_count_o
  );

  /*
  logic [`BSG_SAFE_CLOG2(max_counter_val_p+1)-1:0] cnt_lo;
  bsg_counter_clear_up
    #(.max_val_p(max_counter_val_p)
      ,.init_val_p(0)
      )
    stall_counter
     (.clk_i(clk_i)
      ,.reset_i(reset_i)
      ,.clear_i(decoded_inst_i.clr_stall_cnt)
      ,.up_i(stall_o)
      ,.count_o(cnt_lo)
      );
  assign stall_count_o = cnt_lo[0+:cnt_width_lp];
  */
  assign stall_count_o = '0;

  // Note: these signals exist to suppress a warning about the valid/ready
  // needing to be inout signals for this module when building with VCS.
  wire lce_req_v = lce_req_v_i;
  wire lce_resp_v = lce_resp_v_i;
  wire mem_resp_v = mem_resp_v_i;
  wire pending_v = pending_v_i;
  wire lce_cmd_ready = lce_cmd_ready_i;
  wire mem_cmd_ready = mem_cmd_ready_i;

  wire [`bp_cce_num_src_q-1:0] wfq_v_vec = {lce_req_v, lce_resp_v, mem_resp_v, pending_v};
  wire [`bp_cce_num_src_q-1:0] wfq_mask = decoded_inst_i.imm[0+:`bp_cce_num_src_q];

  always_comb begin
    stall_o = 1'b0;

    // Microcode instruction stalls - resource not ready

    // Message receive
    stall_o |= (decoded_inst_i.lce_req_yumi & ~lce_req_v);
    stall_o |= (decoded_inst_i.lce_resp_yumi & ~lce_resp_v);
    stall_o |= (decoded_inst_i.mem_resp_yumi & ~mem_resp_v);
    stall_o |= (decoded_inst_i.pending_yumi & ~pending_v);

    // Message send
    stall_o |= (decoded_inst_i.lce_cmd_v & ~lce_cmd_ready);
    stall_o |= (decoded_inst_i.mem_cmd_v & ~mem_cmd_ready);

    // Wait for queue operation
    stall_o |= (decoded_inst_i.wfq_v & ~(|(wfq_mask & wfq_v_vec)));


    // Functional Unit induced stalls

    // Directory is busy after a read - be safe and block execution until read is done
    stall_o |= dir_busy_i;

    // Message unit is stalling all ucode, i.e., invalidation instruction is executing
    stall_o |= msg_busy_i;

    // Message Unit Structural Hazards
    stall_o |= (decoded_inst_i.pending_w_v & msg_pending_w_busy_i);
    stall_o |= (decoded_inst_i.lce_cmd_v & msg_lce_cmd_busy_i);
    stall_o |= (decoded_inst_i.lce_resp_yumi & msg_lce_resp_busy_i);
    stall_o |= (decoded_inst_i.mem_resp_yumi & msg_mem_resp_busy_i);
    stall_o |= (decoded_inst_i.spec_r_v & msg_spec_r_busy_i);
    stall_o |= (decoded_inst_i.dir_w_v & msg_dir_w_busy_i);
    // TODO: also stall directory read if message unit writing directory?
    stall_o |= (decoded_inst_i.dir_r_v & msg_dir_w_busy_i);

  end

endmodule
