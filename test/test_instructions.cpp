
#include <scope/test.h>

#include "instructions.h"

#include <stdexcept>

SCOPE_TEST(instructionSizes) {
  SCOPE_ASSERT_EQUAL(4u, sizeof(Instruction));

  Instruction i;
  i.Size = 0;
  SCOPE_ASSERT_EQUAL(1u, i.wordSize());
  SCOPE_ASSERT_EQUAL(4u, i.byteSize());
  i.Size = 1;
  SCOPE_ASSERT_EQUAL(2u, i.wordSize());
  SCOPE_ASSERT_EQUAL(8u, i.byteSize());
  i.Size = 2;
  SCOPE_ASSERT_EQUAL(5u, i.wordSize());
  SCOPE_ASSERT_EQUAL(20u, i.byteSize());
  i.Size = 3;
  SCOPE_ASSERT_EQUAL(9u, i.wordSize());
  SCOPE_ASSERT_EQUAL(36u, i.byteSize());
}

SCOPE_TEST(makeLit) {
  Instruction i = Instruction::makeLit('a');
  SCOPE_ASSERT_EQUAL(LIT_OP, i.OpCode);
  SCOPE_ASSERT_EQUAL(1u, i.wordSize());
  SCOPE_ASSERT_EQUAL('a', i.Op.Literal);
  SCOPE_ASSERT_EQUAL("Literal 0x61/'a'", i.toString());
}

SCOPE_TEST(makeJump) {
  Instruction i = Instruction::makeJump(11);
  SCOPE_ASSERT_EQUAL(JUMP_OP, i.OpCode);
  SCOPE_ASSERT_EQUAL(1u, i.wordSize());
  SCOPE_ASSERT_EQUAL(11u, i.Op.Offset);
  SCOPE_ASSERT_EQUAL("Jump 0x0000000b/11", i.toString());
  SCOPE_EXPECT(Instruction::makeJump(1 << 24), std::overflow_error);
}

SCOPE_TEST(makeMatch) {
  Instruction i = Instruction::makeMatch();
  SCOPE_ASSERT_EQUAL(MATCH_OP, i.OpCode);
  SCOPE_ASSERT_EQUAL(1u, i.wordSize());
  SCOPE_ASSERT_EQUAL("Match", i.toString());
}

SCOPE_TEST(makeSaveLabel) {
  Instruction i = Instruction::makeSaveLabel(3);
  SCOPE_ASSERT_EQUAL(SAVE_LABEL_OP, i.OpCode);
  SCOPE_ASSERT_EQUAL(1u, i.wordSize());
  SCOPE_ASSERT_EQUAL(3u, i.Op.Offset);
  SCOPE_ASSERT_EQUAL("SaveLabel 3", i.toString());
  SCOPE_EXPECT(Instruction::makeSaveLabel(1 << 24), std::overflow_error);
}

SCOPE_TEST(makeEither) {
  Instruction i = Instruction::makeEither('a', 'B');
  SCOPE_ASSERT_EQUAL(EITHER_OP, i.OpCode);
  SCOPE_ASSERT_EQUAL(1u, i.wordSize());
  SCOPE_ASSERT_EQUAL('a', i.Op.Range.First);
  SCOPE_ASSERT_EQUAL('B', i.Op.Range.Last);
  SCOPE_ASSERT_EQUAL("Either 0x61/'a', 0x42/'B'", i.toString());
}

SCOPE_TEST(makeRange) {
  Instruction i = Instruction::makeRange('A', 'Z');
  SCOPE_ASSERT_EQUAL(RANGE_OP, i.OpCode);
  SCOPE_ASSERT_EQUAL(1u, i.wordSize());
  SCOPE_ASSERT_EQUAL('A', i.Op.Range.First);
  SCOPE_ASSERT_EQUAL('Z', i.Op.Range.Last);
  SCOPE_ASSERT_EQUAL("Range 0x41/'A'-0x5a/'Z'", i.toString());
  SCOPE_EXPECT(Instruction::makeRange('Z', 'A'), std::range_error);
}

SCOPE_TEST(makeFork) {
  Instruction i = Instruction::makeFork(1027);
  SCOPE_ASSERT_EQUAL(FORK_OP, i.OpCode);
  SCOPE_ASSERT_EQUAL(1u, i.wordSize());
  SCOPE_ASSERT_EQUAL(1027u, i.Op.Offset);
  SCOPE_ASSERT_EQUAL("Fork 0x00000403/1027", i.toString());
  SCOPE_EXPECT(Instruction::makeFork(1 << 24), std::overflow_error);
}