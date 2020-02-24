/*
 * bp_as.cc
 *
 * @author markw
 *
 * BlackParrot CCE Microcode Assembler
 *
 */

#include "bp_as.h"

bool
Assembler::_iscommentstart(char ch) {
  switch (ch) {
    case  '#':
      return true;
    default:
      return false;
  }
}

bool
Assembler::_iswhitespace(char ch) {
  switch (ch) {
    case  '/':
    case  ',':
    case  ' ':
    case  '\t':
      return true;
    default:
      return false;
  }
}

bool
Assembler::_ishardnewline(char ch) {
  switch (ch) {
    case  '\0':
    case  '\n':
      return true;
    default:
      return false;
  }
}

bool
Assembler::_isnewline(char ch) {
  switch (ch) {
    case  '\0':
    case  '\n':
    case  ';':
      return true;
    default:
      return false;
  }
}

char
Assembler::_lowercase(char ch) {
  if (ch >= 'A' && ch <= 'Z')
    return ch - 'A' + 'a';
  return ch;
}

// TODO: add popd when supported
// TODO: add memory operations if supported
bp_cce_inst_op_e
Assembler::getOp(string &s) {
  if (!s.compare("add") || !s.compare("sub") || !s.compare("lsh") || !s.compare("rsh")
      || !s.compare("and") || !s.compare("or") || !s.compare("xor") || !s.compare("neg")
      || !s.compare("addi") || !s.compare("nop") || !s.compare("inc") || !s.compare("subi")
      || !s.compare("dec") || !s.compare("lshi") || !s.compare("rshi") || !s.compare("not")) {
    return e_op_alu;
  } else if (!s.compare("beq") || !s.compare("bi") || !s.compare("bne")
             || !s.compare("blt") || !s.compare("bgt") || !s.compare("ble") || !s.compare("bge")
             || !s.compare("bs") || !s.compare("bss")
             || !s.compare("beqi") || !s.compare("bz") || !s.compare("bneqi") || !s.compare("bnz")
             || !s.compare("bsi")) {
    return e_op_branch;
  } else if (!s.compare("mov") || !s.compare("movsg") || !s.compare("movgs") || !s.compare("ldflags")
             || !s.compare("movfg") || !s.compare("movgf")
             || !s.compare("movpg") || !s.compare("movgp")
             || !s.compare("movi") || !s.compare("movis")
             || !s.compare("ldflagsi") || !s.compare("clf") || !s.compare("clm")) {
    return e_op_reg_data;
  } else if (!s.compare("sf") || !s.compare("sfz") || !s.compare("andf") || !s.compare("orf")
             || !s.compare("nandf") || !s.compare("norf") || !s.compare("notf")
             || !s.compare("bf") || !s.compare("bfz") || !s.compare("bnz") || !s.compare("bfnot")) {
    return e_op_flag;
  } else if (!s.compare("rdp") || !s.compare("rdw") || !s.compare("rde")
             || !s.compare("wdp") || !s.compare("clp") || !s.compare("clr")
             || !s.compare("wde") || !s.compare("cls") || !s.compare("gad")) {
    return e_op_dir;
  } else if (!s.compare("wfq") || !s.compare("pushq") || !s.compare("pushqc") || !s.compare("popq")
             || !s.compare("poph") || !s.compare("specq") || !s.compare("inv")) {
    return e_op_queue;
  } else {
    printf("Bad Op: %s\n", s.c_str());
    exit(-1);
  }
}

uint8_t
Assembler::getMinorOp(string &s) {
  // ALU
  if (!s.compare("add")) {
    return e_add_op;
  } else if (!s.compare("sub")) {
    return e_sub_op;
  } else if (!s.compare("lsh")) {
    return e_lsh_op;
  } else if (!s.compare("rsh")) {
    return e_rsh_op;
  } else if (!s.compare("and")) {
    return e_and_op;
  } else if (!s.compare("or")) {
    return e_or_op;
  } else if (!s.compare("xor")) {
    return e_xor_op;
  } else if (!s.compare("neg")) {
    return e_neg_op;
  } else if (!s.compare("inc") || !s.compare("addi")|| !s.compare("nop")) {
    return e_addi_op;
  } else if (!s.compare("dec") || !s.compare("subi")) {
    return e_subi_op;
  } else if (!s.compare("lshi")) {
    return e_lshi_op;
  } else if (!s.compare("rshi")) {
    return e_rshi_op;
  } else if (!s.compare("not")) {
    return e_not_op;

  // Branch
  } else if (!s.compare("beq") || !s.compare("bi")) {
    return e_beq_op;
  } else if (!s.compare("bne")) {
    return e_bne_op;
  } else if (!s.compare("blt") || !s.compare("bgt")) {
    return e_blt_op;
  } else if (!s.compare("ble") || !s.compare("bge")) {
    return e_ble_op;
  } else if (!s.compare("beqi") || !s.compare("bz")) {
    return e_beqi_op;
  } else if (!s.compare("bneqi") || !s.compare("bnz")) {
    return e_bneqi_op;
  } else if (!s.compare("bsi")) {
    return e_bsi_op;

  // Reg Data / Move
  } else if (!s.compare("mov")) {
    return e_mov_op;
  } else if (!s.compare("movsg")) {
    return e_movsg_op;
  } else if (!s.compare("movgs") || !s.compare("ldflags")) {
    return e_movgs_op;
  } else if (!s.compare("movfg")) {
    return e_movfg_op;
  } else if (!s.compare("movgf")) {
    return e_movgf_op;
  } else if (!s.compare("movpg")) {
    return e_movpg_op;
  } else if (!s.compare("movgp")) {
    return e_movgp_op;
  } else if (!s.compare("movi")) {
    return e_movi_op;
  } else if (!s.compare("movis") || !s.compare("ldflagsi") || !s.compare("clf")) {
    return e_movis_op;
  } else if (!s.compare("clm")) {
    return e_clm_op;

  // Flag
  } else if (!s.compare("sf") || !s.compare("sfz")) {
    return e_sf_op;
  } else if (!s.compare("andf")) {
    return e_andf_op;
  } else if (!s.compare("orf")) {
    return e_orf_op;
  } else if (!s.compare("nandf")) {
    return e_nandf_op;
  } else if (!s.compare("norf")) {
    return e_norf_op;
  } else if (!s.compare("andf")) {
    return e_andf_op;
  } else if (!s.compare("notf")) {
    return e_notf_op;
  } else if (!s.compare("bf")) {
    return e_bf_op;
  } else if (!s.compare("bfz")) {
    return e_bfz_op;
  } else if (!s.compare("bfnz")) {
    return e_bfnz_op;
  } else if (!s.compare("bfnot")) {
    return e_bfnot_op;

  // Directory
  } else if (!s.compare("rdp")) {
    return e_rdp_op;
  } else if (!s.compare("rdw")) {
    return e_rdw_op;
  } else if (!s.compare("rde")) {
    return e_rde_op;
  } else if (!s.compare("wdp")) {
    return e_wdp_op;
  } else if (!s.compare("clp")) {
    return e_clp_op;
  } else if (!s.compare("clr")) {
    return e_clr_op;
  } else if (!s.compare("wde")) {
    return e_wde_op;
  } else if (!s.compare("wds")) {
    return e_wds_op;
  } else if (!s.compare("gad")) {
    return e_gad_op;

  // Queue
  } else if (!s.compare("wfq")) {
    return e_wfq_op;
  } else if (!s.compare("pushq") || !s.compare("pushqc")) {
    return e_pushq_op;
  } else if (!s.compare("popq")) {
    return e_popq_op;
  } else if (!s.compare("poph")) {
    return e_poph_op;
  } else if (!s.compare("specq")) {
    return e_specq_op;
  } else if (!s.compare("inv")) {
    return e_inv_op;
  } else {
    printf("Bad Minor Op: %s\n", s.c_str());
    exit(-1);
  }
}

bp_cce_inst_opd_e
Assembler::parseOpd(string &s) {
  // GPR
  if (!s.compare("r0")) {
    return e_opd_r0;
  } else if (!s.compare("r1")) {
    return e_opd_r1;
  } else if (!s.compare("r2")) {
    return e_opd_r2;
  } else if (!s.compare("r3")) {
    return e_opd_r3;
  } else if (!s.compare("r4")) {
    return e_opd_r4;
  } else if (!s.compare("r5")) {
    return e_opd_r5;
  } else if (!s.compare("r6")) {
    return e_opd_r6;
  } else if (!s.compare("r7")) {
    return e_opd_r7;

  // Flags
  } else if (!s.compare("rqf")) {
    return e_opd_rqf;
  } else if (!s.compare("ucf")) {
    return e_opd_ucf;
  } else if (!s.compare("nerf")) {
    return e_opd_nerf;
  } else if (!s.compare("ldf")) {
    return e_opd_ldf;
  } else if (!s.compare("pf")) {
    return e_opd_pf;
  } else if (!s.compare("lef")) {
    return e_opd_lef;
  } else if (!s.compare("cf")) {
    return e_opd_cf;
  } else if (!s.compare("cef")) {
    return e_opd_cef;
  } else if (!s.compare("cof")) {
    return e_opd_cof;
  } else if (!s.compare("cdf")) {
    return e_opd_cdf;
  } else if (!s.compare("tf")) {
    return e_opd_tf;
  } else if (!s.compare("rf")) {
    return e_opd_rf;
  } else if (!s.compare("uf")) {
    return e_opd_uf;
  } else if (!s.compare("if")) {
    return e_opd_if;
  } else if (!s.compare("nwbf")) {
    return e_opd_nwbf;
  } else if (!s.compare("sf")) {
    return e_opd_sf;

  // Special
  } else if (!s.compare("reqlce")) {
    return e_opd_req_lce;
  } else if (!s.compare("reqaddr")) {
    return e_opd_req_addr;
  } else if (!s.compare("reqway")) {
    return e_opd_req_way;
  } else if (!s.compare("lruaddr")) {
    return e_opd_lru_addr;
  } else if (!s.compare("lruway")) {
    return e_opd_lru_way;
  } else if (!s.compare("ownerlce")) {
    return e_opd_owner_lce;
  } else if (!s.compare("ownerway")) {
    return e_opd_owner_way;
  } else if (!s.compare("nextcohstate")) {
    return e_opd_next_coh_state;
  } else if (!s.compare("flags")) {
    return e_opd_flags;
  } else if (!s.compare("ucreqsize")) {
    return e_opd_uc_req_size;
  } else if (!s.compare("datalength")) {
    return e_opd_data_length;

  } else if (!s.compare("flagsandmask")) {
    return e_opd_flags_and_mask;

  } else if (!s.compare("shhit")) {
    return e_opd_sharers_hit;
  } else if (!s.compare("shway")) {
    return e_opd_sharers_way;
  } else if (!s.compare("shstate")) {
    return e_opd_sharers_state;

  // Params
  } else if (!s.compare("cceid")) {
    return e_opd_cce_id;
  } else if (!s.compare("numlce")) {
    return e_opd_num_lce;
  } else if (!s.compare("numcce")) {
    return e_opd_num_cce;
  } else if (!s.compare("numwg")) {
    return e_opd_num_wg;
  } else if (!s.compare("autofwdmsg")) {
    return e_opd_auto_fwd_msg;
  } else if (!s.compare("cohstate")) {
    return e_opd_coh_state_default;

  // Queue
  } else if (!s.compare("memresp")) {
    return e_opd_mem_resp_v;
  } else if (!s.compare("lceresp")) {
    return e_opd_lce_resp_v;
  } else if (!s.compare("pending")) {
    return e_opd_pending_v;
  } else if (!s.compare("lcereq")) {
    return e_opd_lce_req_v;
  } else if (!s.compare("lceresptype")) {
    return e_opd_lce_resp_type;
  } else if (!s.compare("memresptype")) {
    return e_opd_mem_resp_type;

  // Default
  } else {
    return 0;
  }
}

uint32_t
Assembler::parseImm(string &s, int immSize) {
  int stoi_res = stoi(s, nullptr, 0);
  if (immSize == 16) {
    if (stoi_res > UINT16_MAX) {
      printf("Bad immediate: %ld\n", stoi_res);
    } else {
      return (uint32_t)stoi_res;
    }
  } else if (immSize == 32) {
    if ((uint32_t)stoi_res > UINT32_MAX) {
      printf("Bad immediate: %ld\n", stoi_res);
    } else {
      return (uint32_t)stoi_res;
    }
  }
  printf("Bad immediate: size: %d, %ld\n", immSize, stoi_res);
  exit(-1);
}

uint32_t
Assembler::parseCohStImm(string &s) {
  if (!s.compare("m")) {
    return 6;
  } else if (!s.compare("e")) {
    return 2;
  } else if (!s.compare("s")) {
    return 1;
  } else if (!s.compare("i")) {
    return 0;
  } else if (!s.compare("o")) {
    return 7;
  } else if (!s.compare("f")) {
    return 3;
  }
  int stoi_res = stoi(s, nullptr, 0);
  if (stoi_res > 7 || stoi_res < 0 || stoi_res == 4 || stoi_res == 5) {
    printf("Bad coh state immediate: %d\n", stoi_res);
    exit(-1);
  } else {
    return (uint32_t)stoi_res;
  }
}

void
Assembler::parseALU(vector<string> *tokens, int n, bp_cce_inst_s *inst) {
  if (tokens->size() == 1) { // nop - translates to addi r0 = r0 + 0
    inst->type_u.itype.src_a = e_opd_r0;
    inst->type_u.itype.imm = 0;
    inst->type_u.itype.dst = e_opd_r0;
  } else if (tokens->size() == 2) { // inc, dec, neg, not - same dst as src
    inst->type_u.alu_op_s.src_a = parseOpd(tokens->at(1));
    inst->type_u.alu_op_s.dst = parseOpd(tokens->at(1));
    if (inst->minor_op == e_inc || inst->minor_op == e_dec) {
      inst->type_u.alu_op_s.src_b = e_src_imm;
      inst->type_u.alu_op_s.imm = 1;
    } else if (inst->minor_op == e_neg) {
      inst->type_u.alu_op_s.src_b = e_src_imm;
      inst->type_u.alu_op_s.imm = 0;
    } else {
      printf("Unknown ALU instruction: %s\n", tokens->at(0).c_str());
      exit(-1);
    }
  } else if (tokens->size() == 3) { // lsh, rsh
    inst->type_u.alu_op_s.src_a = parseSrcOpd(tokens->at(1));
    inst->type_u.alu_op_s.dst = parseDstOpd(tokens->at(1));
    inst->type_u.alu_op_s.src_b = e_src_imm;
  } else if (tokens->size() == 4) { // add, sub, and, or, xor, addi, subi
    inst->type_u.alu_op_s.src_a = parseSrcOpd(tokens->at(1));
    inst->type_u.alu_op_s.src_b = parseSrcOpd(tokens->at(2));
    if (inst->type_u.alu_op_s.src_b == e_src_imm) {
      inst->type_u.alu_op_s.imm = parseImm(tokens->at(2), 16);
    }
    inst->type_u.alu_op_s.dst = parseDstOpd(tokens->at(3));
  } else {
    printf("Unknown ALU instruction: %s\n", tokens->at(0).c_str());
    exit(-1);
  }
}

uint16_t
Assembler::parseTarget(string &s, bool &found) {
  auto labelIt = labels_to_addr.find(s);
  if (labelIt == labels_to_addr.end()) {
    printf("No address found for label: %s\n", s.c_str());
    found = false;
    return 0;
  }
  found = true;
  return labelIt->second;
}

uint16_t
Assembler::getBranchTarget(string &target_str) {
  bool label_found = false;
  uint16_t target = parseTarget(target_str, label_found);
  if (label_found) {
    return target;
  } else {
    return (uint16_t)parseImm(target_str, 16);
  }
}

// TODO: all branch operations must be tagged with a prediction bit
// or, optionally tagged (p=N)?
void
Assembler::parseBranch(vector<string> *tokens, int n, bp_cce_inst_s *inst) {
  // Branch multi-flag operation
  if (!strcmp("bfand", tokens->at(0).c_str()) || !strcmp("bfnand", tokens->at(0).c_str())
      || !strcmp("bfor", tokens->at(0).c_str()) || !strcmp("bfnor", tokens->at(0).c_str())) {
    // Use the software op to set src_a for the CCE ucode decoder
    inst->type_u.branch_op_s.src_a = parseSrcOpd(tokens->at(0));
    // use immediate field as bit mask to indicate which flags will be used in the operation
    for (uint32_t i = 1; i < tokens->size()-1; i++) {
      inst->type_u.branch_op_s.imm |= parseFlagOneHot(tokens->at(i));
    }
    // branch target is last operand
    inst->type_u.branch_op_s.target = getBranchTarget(tokens->at(tokens->size()-1));
    // set src_b to use constant 1
    inst->type_u.branch_op_s.src_b = e_src_special_1;

  // Branch Immediate
  } else if (tokens->size() == 2) {
    inst->type_u.branch_op_s.target = getBranchTarget(tokens->at(1));
  // Branch Flag, Branch Queue Ready, Branch Zero, Branch Not Zero
  } else if (tokens->size() == 3) {
    inst->type_u.branch_op_s.src_a = parseSrcOpd(tokens->at(1));
    if (!strcmp("bf", tokens->at(0).c_str()) || !strcmp("bqv", tokens->at(0).c_str())) {
      inst->type_u.branch_op_s.src_b = e_src_imm;
      inst->type_u.branch_op_s.imm = 1;
    } else if (!strcmp("bsz", tokens->at(0).c_str())) {
      inst->type_u.branch_op_s.src_a = parseSrcOpd(tokens->at(1));
      inst->type_u.branch_op_s.src_b = e_src_imm;
      inst->type_u.branch_op_s.imm = 0;
    } else { // bfz, bz, bnz
      inst->type_u.branch_op_s.src_b = e_src_imm;
      inst->type_u.branch_op_s.imm = 0;
    }
    inst->type_u.branch_op_s.target = getBranchTarget(tokens->at(2));
  // Branch comparing two sources, or Branch Equal Immediate
  } else if (tokens->size() == 4) {
    if (!strcmp("beqi", tokens->at(0).c_str()) || !strcmp("bneqi", tokens->at(0).c_str())) {
      inst->type_u.branch_op_s.src_a = parseSrcOpd(tokens->at(1));
      inst->type_u.branch_op_s.src_b = e_src_imm;
      inst->type_u.branch_op_s.imm = (uint16_t)parseImm(tokens->at(2), 16);
    } else if (!strcmp("bge", tokens->at(0).c_str()) || !strcmp("bgt", tokens->at(0).c_str())) {
      inst->type_u.branch_op_s.src_a = parseSrcOpd(tokens->at(2));
      inst->type_u.branch_op_s.src_b = parseSrcOpd(tokens->at(1));
    } else if (!strcmp("bs", tokens->at(0).c_str())) {
      inst->type_u.branch_op_s.src_a = parseSrcOpd(tokens->at(1));
      inst->type_u.branch_op_s.src_b = parseSrcOpd(tokens->at(2));
      if (inst->type_u.branch_op_s.src_b == e_src_imm) {
        //printf("Immediate used as operand for branch special\n");
        inst->type_u.branch_op_s.imm = (uint16_t)parseImm(tokens->at(2), 16);
      }
    } else if (!strcmp("bsi", tokens->at(0).c_str())) {
      inst->type_u.branch_op_s.src_a = parseSrcOpd(tokens->at(1));
      inst->type_u.branch_op_s.src_b = parseSrcOpd(tokens->at(2));
      if (inst->type_u.branch_op_s.src_b == e_src_imm) {
        inst->type_u.branch_op_s.imm = (uint16_t)parseImm(tokens->at(2), 16);
      } else {
        printf("Invalid immediate used as operand for branch special immediate\n");
        exit(-1);
      }
    } else { // blt, ble
      inst->type_u.branch_op_s.src_a = parseSrcOpd(tokens->at(1));
      inst->type_u.branch_op_s.src_b = parseSrcOpd(tokens->at(2));
    }
    inst->type_u.branch_op_s.target = getBranchTarget(tokens->at(3));
  } else {
    printf("Unknown Branch instruction: %s\n", tokens->at(0).c_str());
  }
}

bp_cce_inst_flag_e
Assembler::parseFlagOneHot(string &s) {
  switch (parseDstOpd(s)) {
    case e_dst_rqf:
      return e_flag_rqf;
      break;
    case e_dst_ucf:
      return e_flag_ucf;
      break;
    case e_dst_nerf:
      return e_flag_nerf;
      break;
    case e_dst_ldf:
      return e_flag_ldf;
      break;
    case e_dst_pf:
      return e_flag_pf;
      break;
    case e_dst_lef:
      return e_flag_lef;
      break;
    case e_dst_cf:
      return e_flag_cf;
      break;
    case e_dst_cef:
      return e_flag_cef;
      break;
    case e_dst_cof:
      return e_flag_cof;
      break;
    case e_dst_cdf:
      return e_flag_cdf;
      break;
    case e_dst_tf:
      return e_flag_tf;
      break;
    case e_dst_rf:
      return e_flag_rf;
      break;
    case e_dst_uf:
      return e_flag_uf;
      break;
    case e_dst_if:
      return e_flag_if;
      break;
    case e_dst_nwbf:
      return e_flag_nwbf;
      break;
    case e_dst_sf:
      return e_flag_sf;
      break;
    default:
      printf("Unknown Flag operand\n");
      exit(-1);
  }
}

void
Assembler::parseMove(vector<string> *tokens, int n, bp_cce_inst_s *inst) {
  if (inst->minor_op == e_movi || inst->minor_op == e_movis) {
    inst->type_u.mov_op_s.dst = parseDstOpd(tokens->at(2));
    if (!strcmp("nextcohst", tokens->at(2).c_str())) {
      inst->type_u.mov_op_s.op.movi.imm = parseCohStImm(tokens->at(1));
    } else {
      inst->type_u.mov_op_s.op.movi.imm = (uint32_t)parseImm(tokens->at(1), 32);
    }
  } else if (inst->minor_op == e_mov || inst->minor_op == e_movf
             || inst->minor_op == e_movsg || inst->minor_op == e_movgs) {
      inst->type_u.mov_op_s.dst = parseDstOpd(tokens->at(2));
      inst->type_u.mov_op_s.op.mov.src = parseSrcOpd(tokens->at(1));
  } else {
    printf("Unknown Move instruction: %s\n", tokens->at(0).c_str());
    exit(-1);
  }
}

void
Assembler::parseFlag(vector<string> *tokens, int n, bp_cce_inst_s *inst) {
  if (inst->minor_op == e_sf || inst->minor_op == e_sfz) {
    inst->type_u.flag_op_s.dst = parseDstOpd(tokens->at(1));
    if (!strcmp("sf", tokens->at(0).c_str())) {
      inst->type_u.flag_op_s.val = 1;
    } else if (!strcmp("sfz", tokens->at(0).c_str())) {
      inst->type_u.flag_op_s.val = 0;
    } else {
      printf("Unknown Flag instruction: %s\n", tokens->at(0).c_str());
      exit(-1);
    }
  } else if (inst->minor_op == e_andf || inst->minor_op == e_orf) {
    printf("andf and orf instructions not yet implemented\n");
    exit(-1);
  } else {
    printf("Unknown Flag instruction: %s\n", tokens->at(0).c_str());
    exit(-1);
  }
}

bp_cce_inst_dir_way_group_sel_e
Assembler::parseDirWgSel(string &s) {
  if (!s.compare("r0")) {
    return e_dir_wg_sel_r0;
  } else if (!s.compare("r1")) {
    return e_dir_wg_sel_r1;
  } else if (!s.compare("r2")) {
    return e_dir_wg_sel_r2;
  } else if (!s.compare("r3")) {
    return e_dir_wg_sel_r3;
  } else if (!s.compare("r4")) {
    return e_dir_wg_sel_r4;
  } else if (!s.compare("r5")) {
    return e_dir_wg_sel_r5;
  } else if (!s.compare("r6")) {
    return e_dir_wg_sel_r6;
  } else if (!s.compare("r7")) {
    return e_dir_wg_sel_r7;
  } else if (!s.compare("req")) {
    return e_dir_wg_sel_req_addr;
  } else if (!s.compare("lru")) {
    return e_dir_wg_sel_lru_way_addr;
  } else {
    printf("Unknown directory way-group select operand: %s\n", s.c_str());
    exit(-1);
  }
}

bp_cce_inst_dir_lce_sel_e 
Assembler::parseDirLceSel(string &s) {
  if (!s.compare("r0")) {
    return e_dir_lce_sel_r0;
  } else if (!s.compare("r1")) {
    return e_dir_lce_sel_r1;
  } else if (!s.compare("r2")) {
    return e_dir_lce_sel_r2;
  } else if (!s.compare("r3")) {
    return e_dir_lce_sel_r3;
  } else if (!s.compare("r4")) {
    return e_dir_lce_sel_r4;
  } else if (!s.compare("r5")) {
    return e_dir_lce_sel_r5;
  } else if (!s.compare("r6")) {
    return e_dir_lce_sel_r6;
  } else if (!s.compare("r7")) {
    return e_dir_lce_sel_r7;
  } else if (!s.compare("req")) {
    return e_dir_lce_sel_req_lce;
  } else if (!s.compare("tr")) {
    return e_dir_lce_sel_transfer_lce;
  } else {
    printf("Unknown directory lce select operand: %s\n", s.c_str());
    exit(-1);
  }
}

bp_cce_inst_dir_way_sel_e
Assembler::parseDirWaySel(string &s) {
  if (!s.compare("r0")) {
    return e_dir_way_sel_r0;
  } else if (!s.compare("r1")) {
    return e_dir_way_sel_r1;
  } else if (!s.compare("r2")) {
    return e_dir_way_sel_r2;
  } else if (!s.compare("r3")) {
    return e_dir_way_sel_r3;
  } else if (!s.compare("r4")) {
    return e_dir_way_sel_r4;
  } else if (!s.compare("r5")) {
    return e_dir_way_sel_r5;
  } else if (!s.compare("r6")) {
    return e_dir_way_sel_r6;
  } else if (!s.compare("r7")) {
    return e_dir_way_sel_r7;
  } else if (!s.compare("req")) {
    return e_dir_way_sel_req_addr_way;
  } else if (!s.compare("lru")) {
    return e_dir_way_sel_lru_way_addr_way;
  } else if (!s.compare("shwayr0")) {
    return e_dir_way_sel_sh_way_r0;
  } else if (!s.compare("tr")) {
    return e_dir_way_sel_tr_lce_way;
  } else {
    printf("Unknown directory way select operand: %s\n", s.c_str());
    exit(-1);
  }
}

bp_cce_inst_dir_tag_sel_e
Assembler::parseDirTagSel(string &s) {
  if (!s.compare("r0")) {
    return e_dir_tag_sel_r0;
  } else if (!s.compare("r1")) {
    return e_dir_tag_sel_r1;
  } else if (!s.compare("r2")) {
    return e_dir_tag_sel_r2;
  } else if (!s.compare("r3")) {
    return e_dir_tag_sel_r3;
  } else if (!s.compare("r4")) {
    return e_dir_tag_sel_r4;
  } else if (!s.compare("r5")) {
    return e_dir_tag_sel_r5;
  } else if (!s.compare("r6")) {
    return e_dir_tag_sel_r6;
  } else if (!s.compare("r7")) {
    return e_dir_tag_sel_r7;
  } else if (!s.compare("req")) {
    return e_dir_tag_sel_req_addr;
  } else if (!s.compare("lru")) {
    return e_dir_tag_sel_lru_way_addr;
  } else if (!s.compare("0")) {
    return e_dir_tag_sel_const_0;
  } else {
    printf("Unknown directory tag select operand: %s\n", s.c_str());
    exit(-1);
  }
}

bp_cce_inst_dir_coh_state_sel_e
Assembler::parseDirCohStSel(string &s) {
  if (!s.compare("r0")) {
    return e_dir_coh_sel_r0;
  } else if (!s.compare("r1")) {
    return e_dir_coh_sel_r1;
  } else if (!s.compare("r2")) {
    return e_dir_coh_sel_r2;
  } else if (!s.compare("r3")) {
    return e_dir_coh_sel_r3;
  } else if (!s.compare("r4")) {
    return e_dir_coh_sel_r4;
  } else if (!s.compare("r5")) {
    return e_dir_coh_sel_r5;
  } else if (!s.compare("r6")) {
    return e_dir_coh_sel_r6;
  } else if (!s.compare("r7")) {
    return e_dir_coh_sel_r7;
  } else if (!s.compare("nextcohst")) {
    return e_dir_coh_sel_next_coh_st;
  } else {
    return e_dir_coh_sel_inst_imm;
  }
}

void
Assembler::parseDir(vector<string> *tokens, int n, bp_cce_inst_s *inst) {
  if (tokens->size() > 1) {
    inst->type_u.dir_op_s.dir_way_group_sel = parseDirWgSel(tokens->at(1));
  }
  if (inst->minor_op == e_rdp) {
    // nothing special to set
  } else if (inst->minor_op == e_rdw) {
    inst->type_u.dir_op_s.dir_lce_sel = parseDirLceSel(tokens->at(2));
    inst->type_u.dir_op_s.dir_tag_sel = parseDirTagSel(tokens->at(3));
  } else if (inst->minor_op == e_rde) {
    inst->type_u.dir_op_s.dir_lce_sel = parseDirLceSel(tokens->at(2));
    inst->type_u.dir_op_s.dir_way_sel = parseDirWaySel(tokens->at(3));
    inst->type_u.dir_op_s.dst = parseDstOpd(tokens->at(4));
  } else if (inst->minor_op == e_wdp) {
    inst->type_u.dir_op_s.pending = (uint8_t)(parseImm(tokens->at(2), 16) & 0x1);
  } else if (inst->minor_op == e_wde || inst->minor_op == e_wds) {
    inst->type_u.dir_op_s.dir_lce_sel = parseDirLceSel(tokens->at(2));
    inst->type_u.dir_op_s.dir_way_sel = parseDirWaySel(tokens->at(3));
    if (inst->minor_op == e_wde) {
      inst->type_u.dir_op_s.dir_tag_sel = parseDirTagSel(tokens->at(4));
      inst->type_u.dir_op_s.dir_coh_state_sel = parseDirCohStSel(tokens->at(5));
      if (inst->type_u.dir_op_s.dir_coh_state_sel == e_dir_coh_sel_inst_imm) {
        inst->type_u.dir_op_s.state = (uint8_t)(parseCohStImm(tokens->at(5)) & 0x7);
      }
    } else if (inst->minor_op == e_wds) {
      inst->type_u.dir_op_s.dir_coh_state_sel = parseDirCohStSel(tokens->at(4));
      if (inst->type_u.dir_op_s.dir_coh_state_sel == e_dir_coh_sel_inst_imm) {
        inst->type_u.dir_op_s.state = (uint8_t)(parseCohStImm(tokens->at(4)) & 0x7);
      }
    } else {
      printf("Unknown Directory instruction\n");
      exit(-1);
    }
  } else if (inst->minor_op == e_gad) {
    // nothing special to set
  } else {
    printf("Unknown Directory instruction\n");
    exit(-1);
  }
}

void
Assembler::parseMisc(vector<string> *tokens, int n, bp_cce_inst_s *inst) {
  if (inst->minor_op == e_stall || inst->minor_op == e_clm
      || inst->minor_op == e_fence) {
    // nothing special to set
  } else {
    printf("Unknown Misc instruction: %s\n", tokens->at(0).c_str());
    exit(-1);
  }
}

bp_cce_inst_src_q_sel_e
Assembler::parseSrcQueue(string &s) {
  if (!s.compare("lcereq")) {
    return e_src_q_lce_req;
  } else if (!s.compare("memresp")) {
    return e_src_q_mem_resp;
  } else if (!s.compare("pending")) {
    return e_src_q_pending;
  } else if (!s.compare("lceresp")) {
    return e_src_q_lce_resp;
  } else {
    printf("Unknown src queue select operand: %s\n", s.c_str());
    exit(-1);
  }
}

bp_cce_inst_dst_q_sel_e
Assembler::parseDstQueue(string &s) {
  if (!s.compare("lcecmd")) {
    return e_dst_q_lce_cmd;
  } else if (!s.compare("memcmd")) {
    return e_dst_q_mem_cmd;
  } else {
    printf("Unknown dst queue select operand: %s\n", s.c_str());
    exit(-1);
  }
}

bp_cce_inst_lce_cmd_lce_sel_e
Assembler::parseLceCmdLceSel(string &s) {
  if (!s.compare("req")) {
    return e_lce_cmd_lce_req_lce;
  } else if (!s.compare("tr")) {
    return e_lce_cmd_lce_tr_lce;
  } else if (!s.compare("r0")) {
    return e_lce_cmd_lce_r0;
  } else if (!s.compare("r1")) {
    return e_lce_cmd_lce_r1;
  } else if (!s.compare("r2")) {
    return e_lce_cmd_lce_r2;
  } else if (!s.compare("r3")) {
    return e_lce_cmd_lce_r3;
  } else if (!s.compare("r4")) {
    return e_lce_cmd_lce_r4;
  } else if (!s.compare("r5")) {
    return e_lce_cmd_lce_r5;
  } else if (!s.compare("r6")) {
    return e_lce_cmd_lce_r6;
  } else if (!s.compare("r7")) {
    return e_lce_cmd_lce_r7;
  } else {
    printf("Bad LCE Cmd LCE select operand\n");
    exit(-1);
  }
}

bp_cce_inst_lce_cmd_addr_sel_e
Assembler::parseLceCmdAddrSel(string &s) {
  if (!s.compare("req")) {
   return e_lce_cmd_addr_req_addr;
  } else if (!s.compare("lru")) {
   return e_lce_cmd_addr_lru_way_addr;
  } else if (!s.compare("r0")) {
    return e_lce_cmd_addr_r0;
  } else if (!s.compare("r1")) {
    return e_lce_cmd_addr_r1;
  } else if (!s.compare("r2")) {
    return e_lce_cmd_addr_r2;
  } else if (!s.compare("r3")) {
    return e_lce_cmd_addr_r3;
  } else if (!s.compare("r4")) {
    return e_lce_cmd_addr_r4;
  } else if (!s.compare("r5")) {
    return e_lce_cmd_addr_r5;
  } else if (!s.compare("r6")) {
    return e_lce_cmd_addr_r6;
  } else if (!s.compare("r7")) {
    return e_lce_cmd_addr_r7;
  } else {
   printf("Bad LCE Cmd Addr select operand\n");
   exit(-1);
  }
}

bp_cce_inst_lce_cmd_way_sel_e
Assembler::parseLceCmdWaySel(string &s) {
  if (!s.compare("r0")) {
   return e_lce_cmd_way_r0;
  } else if (!s.compare("r1")) {
   return e_lce_cmd_way_r1;
  } else if (!s.compare("r2")) {
   return e_lce_cmd_way_r2;
  } else if (!s.compare("r3")) {
   return e_lce_cmd_way_r3;
  } else if (!s.compare("r4")) {
   return e_lce_cmd_way_r4;
  } else if (!s.compare("r5")) {
   return e_lce_cmd_way_r5;
  } else if (!s.compare("r6")) {
   return e_lce_cmd_way_r6;
  } else if (!s.compare("r7")) {
   return e_lce_cmd_way_r7;
  } else if (!s.compare("req")) {
   return e_lce_cmd_way_req_addr_way;
  } else if (!s.compare("tr")) {
   return e_lce_cmd_way_tr_addr_way;
  } else if (!s.compare("shwayr0")) {
    return e_lce_cmd_way_sh_list_r0;
  } else if (!s.compare("lru")) {
    return e_lce_cmd_way_lru_addr_way;
  } else {
   printf("Bad LCE Cmd Way select operand\n");
   exit(-1);
  }
}

bp_cce_inst_mem_cmd_addr_sel_e
Assembler::parseMemCmdAddrSel(string &s) {
  if (!s.compare("req")) {
   return e_mem_cmd_addr_req_addr;
  } else if (!s.compare("lru")) {
   return e_mem_cmd_addr_lru_way_addr;
  } else if (!s.compare("r0")) {
    return e_mem_cmd_addr_r0;
  } else if (!s.compare("r1")) {
    return e_mem_cmd_addr_r1;
  } else if (!s.compare("r2")) {
    return e_mem_cmd_addr_r2;
  } else if (!s.compare("r3")) {
    return e_mem_cmd_addr_r3;
  } else if (!s.compare("r4")) {
    return e_mem_cmd_addr_r4;
  } else if (!s.compare("r5")) {
    return e_mem_cmd_addr_r5;
  } else if (!s.compare("r6")) {
    return e_mem_cmd_addr_r6;
  } else if (!s.compare("r7")) {
    return e_mem_cmd_addr_r7;
  } else {
   printf("Bad Mem Cmd Addr select operand\n");
   exit(-1);
  }
}

bp_cce_inst_spec_cmd_e
Assembler::parseSpecCmd(string &s) {
  if (!s.compare("set")) {
   return e_spec_cmd_set;
  } else if (!s.compare("unset")) {
   return e_spec_cmd_unset;
  } else if (!s.compare("squash")) {
    return e_spec_cmd_squash;
  } else if (!s.compare("fwd_mod")) {
    return e_spec_cmd_fwd_mod;
  } else if (!s.compare("clear")) {
    return e_spec_cmd_clear;
  } else {
   printf("Bad Spec Cmd operand\n");
   exit(-1);
  }
}

void
Assembler::parseQueue(vector<string> *tokens, int n, bp_cce_inst_s *inst) {
  if (inst->minor_op == e_wfq) {
    for (int i = 1; i < n; i++) {
      bp_cce_inst_src_q_sel_e q = parseSrcQueue(tokens->at(i));
      switch (q) {
        case e_src_q_lce_req:
          inst->type_u.queue_op_s.op.wfq.qmask |= (1 << 3);
          break;
        case e_src_q_lce_resp:
          inst->type_u.queue_op_s.op.wfq.qmask |= (1 << 2);
          break;
        case e_src_q_mem_resp:
          inst->type_u.queue_op_s.op.wfq.qmask |= (1 << 1);
          break;
        case e_src_q_pending:
          inst->type_u.queue_op_s.op.wfq.qmask |= (1);
          break;
        default:
          printf("Unknown src queue for WFQ\n");
          exit(-1);
      }
    }
  } else if (inst->minor_op == e_popq || inst->minor_op == e_poph) {
    bp_cce_inst_src_q_sel_e srcQ = parseSrcQueue(tokens->at(1));
    inst->type_u.queue_op_s.op.popq.src_q = srcQ;
    if (srcQ == e_src_q_lce_resp || srcQ == e_src_q_mem_resp) {
      inst->type_u.queue_op_s.op.popq.dst = parseDstOpd(tokens->at(2));
    }
  } else if (inst->minor_op == e_pushq) {
    bp_cce_inst_dst_q_sel_e dstQ = parseDstQueue(tokens->at(1));
    inst->type_u.queue_op_s.op.pushq.dst_q = dstQ;
    // set lce cmd lce, addr, and way select to the 0 select
    inst->type_u.queue_op_s.op.pushq.lce_cmd_lce_sel = e_lce_cmd_lce_0;
    inst->type_u.queue_op_s.op.pushq.lce_cmd_addr_sel = e_lce_cmd_addr_0;
    inst->type_u.queue_op_s.op.pushq.lce_cmd_way_sel = e_lce_cmd_way_0;
    inst->type_u.queue_op_s.op.pushq.mem_cmd_addr_sel = e_mem_cmd_addr_req_addr;
    // parse lce, addr, way, and mem_addr selects
    switch (dstQ) {
      case e_dst_q_lce_cmd:
        inst->type_u.queue_op_s.op.pushq.cmd.lce_cmd =
          (bp_lce_cmd_type_e)(parseImm(tokens->at(2), 16) & 0xF);
        if (tokens->size() > 3) {
          inst->type_u.queue_op_s.op.pushq.lce_cmd_lce_sel = parseLceCmdLceSel(tokens->at(3));
        }
        if (tokens->size() > 4) {
          inst->type_u.queue_op_s.op.pushq.lce_cmd_addr_sel = parseLceCmdAddrSel(tokens->at(4));
        }
        if (tokens->size() > 5) {
          inst->type_u.queue_op_s.op.pushq.lce_cmd_way_sel = parseLceCmdWaySel(tokens->at(5));
        }
        break;
      case e_dst_q_mem_cmd:
        if (tokens->size() == 2) { // pushq memCmd
          inst->type_u.queue_op_s.op.pushq.cmd.mem_cmd = (bp_cce_mem_cmd_type_e)(0);
        } else if (tokens->size() == 3) { // pushq memCmd spec
          //inst->type_u.queue_op_s.op.pushq.cmd.mem_cmd =
          //  (bp_cce_mem_cmd_type_e)(parseImm(tokens->at(2), 16) & 0xF);
          if (!tokens->at(2).compare("spec")) {
            inst->type_u.queue_op_s.op.pushq.speculative = 1;
          } else {
            printf("unrecognized token for speculative pushq memcmd: %s\n", tokens->at(3).c_str());
            exit(-1);
          }
        } else if (tokens->size() == 4) { // pushq memCmd CMD addr_sel
          inst->type_u.queue_op_s.op.pushq.cmd.mem_cmd =
            (bp_cce_mem_cmd_type_e)(parseImm(tokens->at(2), 16) & 0xF);
          inst->type_u.queue_op_s.op.pushq.mem_cmd_addr_sel = parseMemCmdAddrSel(tokens->at(3));
        } else {
          printf("Too many tokens for pushq memcmd\n");
          exit(-1);
        }
        break;
      default:
        printf("Unknown queue\n");
        exit(-1);
    }
  } else if (inst->minor_op == e_specq) {
    inst->type_u.queue_op_s.op.specq.cmd = parseSpecCmd(tokens->at(1));
    if (tokens->size() > 2) {
      inst->type_u.queue_op_s.op.specq.state = (uint8_t)parseCohStImm(tokens->at(2));
    }
  } else if (inst->minor_op == e_inv) {
    // do nothing
  } else {
    printf("Unknown Queue instruction: %d\n", tokens->at(0).c_str());
    exit(-1);
  }
}

void
Assembler::parseTokens(vector<string> *tokens, int n, parsed_inst_s *parsed_inst) {

  bp_cce_inst_s *inst = &(parsed_inst->inst);

  // All instructions
  inst->op = getOp(tokens->at(0));
  inst->minor_op = getMinorOp(tokens->at(0));

  switch (inst->op) {
    case e_op_alu:
      parseALU(tokens, n, inst);
      break;
    case e_op_branch:
      inst->branch = 1;
      parseBranch(tokens, n, inst);
      break;
    case e_op_move:
      parseMove(tokens, n, inst);
      break;
    case e_op_flag:
      parseFlag(tokens, n, inst);
      break;
    case e_op_dir:
      parseDir(tokens, n, inst);
      break;
    case e_op_misc:
      parseMisc(tokens, n, inst);
      break;
    case e_op_queue:
      parseQueue(tokens, n, inst);
      break;
    default:
      printf("Error parsing instruction\n");
      exit(-1);
  }
}

Assembler::Assembler() {
  infp = stdin;
  outfp = stdout;
  line_number = 0;

  printf("instruction length: %d\n", bp_cce_inst_s_width);
}

Assembler::~Assembler() {
  if (infp != stdin) {
    fclose(infp);
  }
  if (outfp != stdout) {
    fclose(outfp);
  }
}

void
Assembler::tokenizeAndLabel() {
  // Read all lines, tokenize, and remove labels (while assigning to addresses)
  while (readLine(input_line, MAX_LINE_LENGTH, infp) > 0) {
    uint16_t addr = line_number-1;

    printf("(%d) %s\n", addr, input_line);

    lines.push_back(string(input_line));

    int numTokens = tokenizeLine(input_line, input_line_tokens);

    numTokens = parseLabels(input_line_tokens, numTokens, addr);

    vector<string> *inst_tokens = new vector<string>();
    for (int i = 0; i < numTokens; i++) {
      inst_tokens->push_back(string(input_line_tokens[i]));
    }
    tokens.push_back(inst_tokens);
    num_tokens.push_back(numTokens);
  }
}

void
Assembler::assemble() {
  // Transform tokenized instructions into instruction struct, then write to output
  parsed_inst_s parsed_inst;
  unsigned int i = 0;
  while (i < tokens.size()) {
    inst = {};
    parseTokens(tokens.at(i), num_tokens.at(i), &parsed_inst);
    writeInstToOutput(&parsed_inst, (uint16_t)i, tokens.at(i)->at(0));
    i++;
  }
}

void
Assembler::parseArgs(int argc, char *argv[]) {
  int i = 1;

  while (i < argc) {
    if (argv[i][0] == '-') {
      switch(argv[i][1]) {
        case  'i':
        case  'I':
          infp = fopen(argv[i + 1], "r");
          if (!infp) {
            printf("Failure to open input file: %s\n", argv[i + 1]);
            exit(__LINE__);
          }
          i += 2;
          break;
        case  'o':
        case  'O':
          outfp = fopen(argv[i + 1], "w");
          if (!outfp) {
            printf("Failure to create output file: %s\n", argv[i + 1]);
            exit(__LINE__);
          }
          i += 2;
          break;
        case  'b':
        case  'B':
          output_format = output_format_ascii_binary;
          ++i;
          break;
        case  'd':
        case  'D':
          output_format = output_format_dbg;
          ++i;
          break;
        default:
          printf("Usage:\n"
            "\t-i <input>   input file\n"
            "\t-o <output>    output file\n"
            "\t-b       output ascii binary\n"
            "\t-d       output debug\n");
          exit(__LINE__);
      }
    } else {
      printf("Try -- for help\n");
      exit(-__LINE__);
    }
  }
}

void
Assembler::printShortField(uint8_t b, int bits, stringstream &ss) {
  int i = 0;
  uint8_t mask = (1 << (bits-1));
  while (i < bits) {
    if (b & mask) {
      ss << "1";
    } else {
      ss << "0";
    }
    mask = mask >> 1;
    ++i;
  }
}

void
Assembler::printLongField(uint16_t b, int bits, stringstream &ss) {
  int i = 0;
  uint16_t mask = (1 << (bits-1));
  while (i < bits) {
    if (b & mask) {
      ss << "1";
    } else {
      ss << "0";
    }
    mask = mask >> 1;
    ++i;
  }
}

void
Assembler::printField(uint32_t b, int bits, stringstream &ss) {
  int i = 0;
  uint32_t mask = (1 << (bits-1));
  while (i < bits) {
    if (b & mask) {
      ss << "1";
    } else {
      ss << "0";
    }
    mask = mask >> 1;
    ++i;
  }
}

void
Assembler::printPad(int bits, stringstream &ss) {
  for (int i = 0; i < bits; i++) {
    ss << "0";
  }
}

void
Assembler::writeInstToOutput(parsed_inst_s *parsed_inst, uint16_t line_number, string &s) {

  stringstream ss;

  bp_cce_inst_s *inst = &(parsed_inst->inst);

  printShortField(inst->predict_taken, 1, ss);
  printShortField(inst->branch, 1, ss);

  switch (parsed_inst->encoding) {
    case e_rtype:
      printPad(bp_cce_inst_rtype_pad, ss);
      printShortField(inst->type_u.rtype.src_b, bp_cce_inst_opd_width, ss);
      printShortField(inst->type_u.rtype.dst, bp_cce_inst_opd_width, ss);
      printShortField(inst->type_u.rtype.src_a, bp_cce_inst_opd_width, ss);
      break;
    case e_itype:
      printLongField(inst->type_u.itype.imm, bp_cce_inst_imm16_width, ss);
      printPad(bp_cce_inst_itype_pad, ss);
      printShortField(inst->type_u.itype.dst, bp_cce_inst_opd_width, ss);
      printShortField(inst->type_u.itype.src_a, bp_cce_inst_opd_width, ss);
      break;
    case e_btype:
      printLongField(inst->type_u.btype.target, bp_cce_inst_addr_width, ss);
      printPad(bp_cce_inst_btype_pad, ss);
      printShortField(inst->type_u.btype.src_b, bp_cce_inst_opd_width, ss);
      printPad(bp_cce_inst_imm4_width, ss);
      printShortField(inst->type_u.btype.src_a, bp_cce_inst_opd_width, ss);
      break;
    case e_bitype:
      printLongField(inst->type_u.bitype.target, bp_cce_inst_addr_width, ss);
      printPad(bp_cce_inst_bitype_pad, ss);
      printLongField(inst->type_u.bitype.imm, bp_cce_inst_imm8_width, ss);
      printShortField(inst->type_u.bitype.src_a, bp_cce_inst_opd_width, ss);
      break;
    case e_bftype:
      printLongField(inst->type_u.bftype.target, bp_cce_inst_addr_width, ss);
      printLongField(inst->type_u.bftype.imm, bp_cce_inst_imm16_width, ss);
      break;
    case e_stype:
      printPad(bp_cce_inst_stype_pad, ss);
      printShortField(inst->type_u.stype.state, bp_coh_bits, ss);
      printShortField(inst->type_u.stype.addr_sel, bp_cce_inst_mux_sel_addr_width, ss);
      printShortField(inst->type_u.stype.dst, bp_cce_inst_opd_width, ss);
      printShortField(inst->type_u.stype.cmd, bp_cce_inst_spec_op_width, ss);
      break;
    case e_dptype:
      printPad(bp_cce_inst_dptype_pad, ss);
      printShortField(inst->type_u.dptype.pending, 1, ss);
      printShortField(inst->type_u.dptype.dst, bp_cce_inst_opd_width, ss);
      printShortField(inst->type_u.dptype.addr_sel, bp_cce_inst_mux_sel_addr_width, ss);
      break;
    case e_dwtype:
      printPad(bp_cce_inst_dwtype_pad, ss);
      printShortField(inst->type_u.dwtype.state, bp_coh_bits, ss);
      printShortField(inst->type_u.dwtype.way_sel, bp_cce_inst_mux_sel_way_width, ss);
      printShortField(inst->type_u.dwtype.lce_sel, bp_cce_inst_mux_sel_lce_width, ss);
      printShortField(inst->type_u.dwtype.state_sel, bp_cce_inst_mux_sel_coh_state_width, ss);
      printShortField(inst->type_u.dwtype.addr_sel, bp_cce_inst_mux_sel_addr_width, ss);
      break;
    case e_drtype:
      printPad(bp_cce_inst_drtype_pad, ss);
      printShortField(inst->type_u.drtype.lru_way_sel, bp_cce_inst_mux_sel_way_width, ss);
      printShortField(inst->type_u.drtype.way_sel, bp_cce_inst_mux_sel_way_width, ss);
      printShortField(inst->type_u.drtype.lce_sel, bp_cce_inst_mux_sel_lce_width, ss);
      printShortField(inst->type_u.drtype.dst, bp_cce_inst_opd_width, ss);
      printShortField(inst->type_u.drtype.addr_sel, bp_cce_inst_mux_sel_addr_width, ss);
      break;
    case e_popq:
      printShortField(inst->type_u.popq.write_pending, 1, ss);
      printPad(bp_cce_inst_popq_pad, ss);
      printShortField(inst->type_u.popq.dst, bp_cce_inst_opd_width, ss);
      printPad(bp_cce_inst_imm2_width, ss);
      printShortField(inst->type_u.popq.src_q, bp_cce_inst_src_q_sel_width, ss);
      break;
    case e_pushq:
      printShortField(inst->type_u.pushq.write_pending, 1, ss);
      printShortField(inst->type_u.pushq.way_or_length.way_sel, bp_cce_inst_mux_sel_way_width, ss);
      printShortField(inst->type_u.pushq.src_a, bp_cce_inst_opd_width, ss);
      printShortField(inst->type_u.pushq.lce_sel, bp_cce_inst_mux_sel_lce_width, ss);
      printShortField(inst->type_u.pushq.addr_sel, bp_cce_inst_mux_sel_addr_width, ss);
      printShortField(inst->type_u.pushq.cmd.lce_cmd, bp_lce_cmd_type_width, ss);
      printShortField(inst->type_u.pushq.spec, 1, ss);
      printShortField(inst->type_u.pushq.custom, 1, ss);
      printShortField(inst->type_u.pushq.dst_q, bp_cce_inst_dst_q_sel_width, ss);
      break;
    default:
      printf("Error parsing instruction\n");
      printf("line: %d\n", line_number);
      exit(-1);
  }

  printShortField(inst->minor_op, bp_cce_inst_minor_op_width, ss);
  printShortField(inst->op, bp_cce_inst_op_width, ss);

  switch (output_format) {
    case  output_format_ascii_binary:
      fprintf(outfp, "%s\n", ss.str().c_str());
      break;
    case  output_format_dbg:
      fprintf(outfp, "(%02X) %5s : %s\n", line_number, s.c_str(), ss.str().c_str());
      break;
  }
}

// read line from input
int
Assembler::readLine(char *s, int maxLineLen, FILE *infp) {
  char ch;
  int n = 0;

  while (n < maxLineLen) {
    // end of file
    if (feof(infp)) {
      if (n > 0)
        return n;
      else
        return -1;
    }

    // read next character
    ch = fgetc(infp);

    // eof character check
    if (feof(infp) && n == 0) {
      return -1;
    }

    // comment character at start of line, discard line
    if (_iscommentstart(ch) && n == 0) {
      // read through newline or EOF
      fgets(s, maxLineLen, infp);
      continue;
    }

    // Skip white space at the start of a line
    if ((_iswhitespace(ch) || _isnewline(ch)) && n == 0) {
      continue;
    }

    // Update the line number if needed
    if (_ishardnewline(ch)) {
      ++line_number;
    }

    // end of line, return
    if (_isnewline(ch) && n != 0) {
      *s = '\0';
      return n;
    }

    // comment in middle of line, consume rest of line and return
    if (_iscommentstart(ch) && n != 0) {
      *s = '\0';
      // consume rest of line, up to new line
      ch = fgetc(infp);
      while (ch) {
        if (_isnewline(ch)) {
          // newline character found, erase whitespace at end of line
          --s;
          --n;
          while (_iswhitespace(*s)) {
            --s;
            --n;
          }
          ++s;
          *s = '\0';
          return n;
        }
        ch = fgetc(infp);
      }
      printf("returning after while loop\n");
      return n;
    }

    *s = _lowercase(ch);
    ++s;
    ++n;
  }
  printf("Long line on input\n");
  exit(-__LINE__);
}

// tokenize line
int
Assembler::tokenizeLine(char* input_line, char tokens[MAX_TOKENS][MAX_LINE_LENGTH]) {
  // Parse the input line into individual tokens
  // current token
  int token = 0;
  // character position within current token
  int i = 0;
  // character iterator for line
  char *s = input_line;

  // initialize tokens to null strings
  for (token = 0; token < MAX_TOKENS; token++) {
    tokens[token][0] = '\0';
  }

  token = 0;
  while (*s) {
    if (token >= MAX_TOKENS) {
      if (!(*s)) {
        printf("Cannot parse: (%d) %s\n", line_number-1, input_line);
        exit(-__LINE__);
      }
      break;
    }

    // whitespace character, terminate this token
    if (_iswhitespace(*s)) {
      tokens[token][i] = '\0';
      i = 0;
      ++token;
      ++s;
      // consume whitespace
      while (*s && _iswhitespace(*s)) {
        ++s;
      }
    // normal character, add to token
    } else {
      tokens[token][i] = *s;
      ++i;
      ++s;
    }
  }

  // after reading last valid character of the line, terminate the last token
  if (!(*s) && i > 0) {
    tokens[token][i] = '\0';
    ++token;
  }

  return token;
}

int
Assembler::parseLabels(char tokens[MAX_TOKENS][MAX_LINE_LENGTH], int n, uint16_t addr) {
  // parse label if present
  string s (tokens[0]);
  if (s.back() == ':') {
    if (labels_to_addr.find(s) == labels_to_addr.end()) {
      s = s.substr(0, s.size()-1);
      labels_to_addr.insert(pair<string, uint16_t>(s, addr));
      // remove the label from the tokens
      for (int i = 1; i < n; i++) {
        memcpy(tokens[i-1], tokens[i], strlen(tokens[i])+1);
      }
      tokens[n-1][0] = '\0';
      return n-1;
    } else {
      printf("Duplicate label: %s\n", tokens[0]);
      exit(-1);
    }
  }
  return n;
}
