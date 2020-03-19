/**
 *  Name:
 *    bp_fe_icache_tag_info.vh
 *
 *  Description:
 *    icache tag_mem format.
 */

`ifndef BP_FE_ICACHE_TAG_INFO_VH
`define BP_FE_ICACHE_TAG_INFO_VH

`include "bp_common_me_if.vh"

`define declare_bp_fe_icache_tag_info_s(ptag_width_mp) \
  typedef struct packed {                              \
    logic [`bp_coh_bits-1:0]     coh_state;            \
    logic [ptag_width_mp-1:0]    tag;                  \
  } bp_fe_icache_tag_info_s

`define bp_fe_icache_tag_info_width(ptag_width_mp) \
  (ptag_width_mp+`bp_coh_bits)

`endif

