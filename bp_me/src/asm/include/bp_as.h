/*
 * bp_as.h
 *
 * @author markw
 *
 */

#ifndef BP_AS_H
#define BP_AS_H

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>

#include <string>
#include <map>
#include <vector>
#include <sstream>

#include "bp_cce_inst.h"

#define MAX_LINE_LENGTH 2048
#define MAX_TOKENS  20

using namespace std;

enum {
  output_format_ascii_binary = 1,
  output_format_dbg,
};

typedef struct {
  bp_cce_inst_type_e encoding;
  bp_cce_inst_s      inst;
} parsed_inst_s;


class Assembler
{
protected:
  FILE *infp;
  FILE *outfp;
  int  output_format;

  uint16_t line_number;
  char input_line[MAX_LINE_LENGTH];
  char input_line_tokens[MAX_TOKENS][MAX_LINE_LENGTH];

  vector<string> lines;
  vector<vector<string>*> tokens;
  vector<int> num_tokens;

  // Map of labels from assembly code to instruction addresses
  map<string, uint16_t> labels_to_addr;

  // Input Parsing Helpers
  bool _iscommentstart(char ch);
  bool _iswhitespace(char ch);
  bool _ishardnewline(char ch);
  bool _isnewline(char ch);
  char _lowercase(char ch);

  // Assembler Helper Functions
  bp_cce_inst_op_e getOp(string &s);
  uint8_t getMinorOp(string &s);

  uint32_t parseImm(string &s, int immSize);
  uint32_t parseCohStImm(string &s);
  uint16_t getBranchTarget(string &target_str);

  bp_cce_inst_opd_e parseOpd(string &s);
  bp_cce_inst_spec_op_e parseSpecCmd(string &s);
  bp_cce_inst_flag_onehot_e parseFlagOneHot(string &s);

  bp_cce_inst_mux_sel_addr_e parseAddrSel(string &s);
  bp_cce_inst_mux_sel_lce_e parseLceSel(string &s);
  bp_cce_inst_mux_sel_way_e parseWaySel(string &s);
  bp_cce_inst_mux_sel_coh_state_e parseCohStateSel(string &s);
  bp_cce_inst_src_q_sel_e parseSrcQueue(string &s);
  //bp_cce_inst_src_q_e parseSrcQueueOnehot(string &s);
  bp_cce_inst_dst_q_sel_e parseDstQueue(string &s);
  //bp_cce_inst_dst_q_e parseDstQueueOnehot(string &s);

  int readLine(char *s, int maxLineLen, FILE *infp);
  int tokenizeLine(char* input_line, char tokens[MAX_TOKENS][MAX_LINE_LENGTH]);
  int parseLabels(char tokens[MAX_TOKENS][MAX_LINE_LENGTH], int n, uint16_t addr);
  void parseTokens(vector<string> *tokens, int n, bp_cce_inst_s *inst);

  void printShortField(uint8_t b, int bits, stringstream &ss);
  void printLongField(uint16_t b, int bits, stringstream &ss);
  void printField(uint32_t b, int bits, stringstream &ss);
  void printPad(int bits, stringstream &ss);
  void writeInstToOutput(bp_cce_inst_s *inst, uint16_t line_number, string &s);

  void parseALU(vector<string> *tokens, int n, bp_cce_inst_s *inst);
  void parseBranch(vector<string> *tokens, int n, bp_cce_inst_s *inst);
  void parseMove(vector<string> *tokens, int n, bp_cce_inst_s *inst);
  void parseFlag(vector<string> *tokens, int n, bp_cce_inst_s *inst);
  void parseDir(vector<string> *tokens, int n, bp_cce_inst_s *inst);
  void parseMisc(vector<string> *tokens, int n, bp_cce_inst_s *inst);
  void parseQueue(vector<string> *tokens, int n, bp_cce_inst_s *inst);

  uint16_t parseTarget(string &s, bool &found);

public:
  // Public Functions
  Assembler();
  Assembler(int output_format);
  ~Assembler();

  void parseArgs(int argc, char *argv[]);
  void tokenizeAndLabel();
  void assemble();

};

#endif
