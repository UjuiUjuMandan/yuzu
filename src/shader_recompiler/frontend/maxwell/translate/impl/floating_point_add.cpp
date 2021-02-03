// Copyright 2021 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "common/common_types.h"
#include "shader_recompiler/exception.h"
#include "shader_recompiler/frontend/maxwell/translate/impl/common_encoding.h"
#include "shader_recompiler/frontend/maxwell/translate/impl/impl.h"

namespace Shader::Maxwell {
namespace {

void FADD(TranslatorVisitor& v, u64 insn, bool sat, bool cc, bool ftz, FpRounding fp_rounding,
          const IR::U32& src_b, bool abs_a, bool neg_a, bool abs_b, bool neg_b) {
    union {
        u64 raw;
        BitField<0, 8, IR::Reg> dest_reg;
        BitField<8, 8, IR::Reg> src_a;
    } const fadd{insn};

    if (sat) {
        throw NotImplementedException("FADD SAT");
    }
    if (cc) {
        throw NotImplementedException("FADD CC");
    }
    const IR::U32 op_a{v.ir.FPAbsNeg(v.X(fadd.src_a), abs_a, neg_a)};
    const IR::U32 op_b{v.ir.FPAbsNeg(src_b, abs_b, neg_b)};
    IR::FpControl control{
        .no_contraction{true},
        .rounding{CastFpRounding(fp_rounding)},
        .fmz_mode{ftz ? IR::FmzMode::FTZ : IR::FmzMode::None},
    };
    v.X(fadd.dest_reg, v.ir.FPAdd(op_a, op_b, control));
}

void FADD(TranslatorVisitor& v, u64 insn, const IR::U32& src_b) {
    union {
        u64 raw;
        BitField<39, 2, FpRounding> fp_rounding;
        BitField<44, 1, u64> ftz;
        BitField<45, 1, u64> neg_b;
        BitField<46, 1, u64> abs_a;
        BitField<47, 1, u64> cc;
        BitField<48, 1, u64> neg_a;
        BitField<49, 1, u64> abs_b;
        BitField<50, 1, u64> sat;
    } const fadd{insn};

    FADD(v, insn, fadd.sat != 0, fadd.cc != 0, fadd.ftz != 0, fadd.fp_rounding, src_b,
         fadd.abs_a != 0, fadd.neg_a != 0, fadd.abs_b != 0, fadd.neg_b != 0);
}
} // Anonymous namespace

void TranslatorVisitor::FADD_reg(u64 insn) {
    FADD(*this, insn, GetReg20(insn));
}

void TranslatorVisitor::FADD_cbuf(u64) {
    throw NotImplementedException("FADD (cbuf)");
}

void TranslatorVisitor::FADD_imm(u64) {
    throw NotImplementedException("FADD (imm)");
}

void TranslatorVisitor::FADD32I(u64) {
    throw NotImplementedException("FADD32I");
}

} // namespace Shader::Maxwell
