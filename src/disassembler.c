#include <stdlib.h>
#include "cpu.h"
#include "logic.h"
#include "control.h"
#include "jump.h"
#include "load.h"
#include "rotshift.h"
#include "utils.h"

void next_op(struct cpu *cpu)
{
    int mcycles = 0;
    switch (cpu->membus[cpu->regist->pc])
    {
        case 0x00:
            mcycles = nop();
            break;
        case 0x01:
            mcycles = ld_rr_nn(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x02:
            mcycles = ld_rr_a(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x03:
            mcycles = inc_rr(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x04:
            mcycles = inc_r(cpu, &cpu->regist->b);
            break;
        case 0x05:
            mcycles = dec_r(cpu, &cpu->regist->b);
            break;
        case 0x06:
            mcycles = ld_r_u8(cpu, &cpu->regist->b);
            break;
        case 0x07:
            mcycles = rlca(cpu);
            break;
        case 0x08:
            mcycles = ld_nn_sp(cpu);
            break;
        case 0x09:
            mcycles = add_hl_rr(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x0A:
            mcycles = ld_rr_nn(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x0B:
            mcycles = dec_rr(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x0C:
            mcycles = inc_r(cpu, &cpu->regist->c);
            break;
        case 0x0D:
            mcycles = dec_r(cpu, &cpu->regist->c);
            break;
        case 0x0E:
            mcycles = ld_r_u8(cpu, &cpu->regist->c);
            break;
        case 0x0F:
            mcycles = rrca(cpu);
            break;
        case 0x10:
            mcycles = stop(cpu);
            break;
        case 0x11:
            mcycles = ld_rr_nn(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x12:
            mcycles = ld_rr_a(cpu, &cpu->regist->d, &cpu->regist->e);
            break;
        case 0x13:
            mcycles = inc_rr(cpu, &cpu->regist->d, &cpu->regist->e);
            break;
        case 0x14:
            mcycles = inc_r(cpu, &cpu->regist->d);
            break;
        case 0x15:
            mcycles = dec_r(cpu, &cpu->regist->d);
            break;
        case 0x16:
            mcycles = ld_r_u8(cpu, &cpu->regist->d);
            break;
        case 0x17:
            mcycles = rlca(cpu);
            break;
        case 0x18:
            mcycles = jr_e8(cpu);
            break;
        case 0x19:
            mcycles = add_hl_rr(cpu, &cpu->regist->d, &cpu->regist->e);
            break;
        case 0x1A:
            mcycles = ld_a_rr(cpu, &cpu->regist->d, &cpu->regist->e);
            break;
        case 0x1B:
            mcycles = dec_rr(cpu, &cpu->regist->d, &cpu->regist->e);
            break;
        case 0x1C:
            mcycles = inc_r(cpu, &cpu->regist->e);
            break;
        case 0x1D:
            mcycles = dec_r(cpu, &cpu->regist->e);
            break;
        case 0x1E:
            mcycles = ld_r_u8(cpu, &cpu->regist->e);
            break;
        case 0x1F:
            mcycles = rra(cpu);
            break;
        case 0x20:
            mcycles = jr_cc_e8(cpu, get_z(cpu->regist) == 0);
            break;
        case 0x21:
            mcycles = ld_hl_u8(cpu);
            break;
        case 0x22:
            mcycles = ldi_hl_a(cpu);
            break;
        case 0x23:
            mcycles = inc_hl(cpu);
            break;
        case 0x24:
            mcycles = inc_r(cpu, &cpu->regist->h);
            break;
        case 0x25:
            mcycles = dec_r(cpu, &cpu->regist->h);
            break;
        case 0x26:
            mcycles = ld_r_u8(cpu, &cpu->regist->h);
            break;
        case 0x27:
            mcycles = daa(cpu);
            break;
        case 0x28:
            mcycles = jr_cc_e8(cpu, get_z(cpu->regist) == 1);
            break;
        case 0x29:
            mcycles = add_hl_rr(cpu, &cpu->regist->h, &cpu->regist->l);
            break;
        case 0x2A:
            mcycles = ldi_a_hl(cpu);
            break;
        case 0x2B:
            mcycles = dec_hl(cpu);
            break;
        case 0x2C:
            mcycles = inc_r(cpu, &cpu->regist->l);
            break;
        case 0x2D:
            mcycles = dec_r(cpu, &cpu->regist->l);
            break;
        case 0x2E:
            mcycles = ld_r_u8(cpu, &cpu->regist->l);
            break;
        case 0x2F:
            mcycles = cpl(cpu);
            break;
        case 0x30:
            mcycles = jr_cc_e8(cpu, get_c(cpu->regist) == 0);
            break;
        case 0x31:
            mcycles = ld_sp_nn(cpu);
            break;
        case 0x32:
            mcycles = ldd_hl_a(cpu);
            break;
        case 0x33:
            mcycles = inc_sp(cpu);
            break;
        case 0x34:
            mcycles = inc_hl(cpu);
            break;
        case 0x35:
            mcycles = dec_hl(cpu);
            break;
        case 0x36:
            mcycles = ld_hl_u8(cpu);
            break;
        case 0x37:
            mcycles = scf(cpu);
            break;
        case 0x38:
            mcycles = jr_cc_e8(cpu, get_c(cpu->regist) == 1);
            break;
        case 0x39:
            mcycles = add_hl_sp(cpu);
            break;
        case 0x3A:
            mcycles = ldd_a_hl(cpu);
            break;
        case 0x3B:
            mcycles = dec_sp(cpu);
            break;
        case 0x3C:
            mcycles = inc_r(cpu, &cpu->regist->a);
            break;
        case 0x3D:
            mcycles = dec_r(cpu, &cpu->regist->a);
            break;
        case 0x3E:
            mcycles = ldh_a_n(cpu);
            break;
        case 0x3F:
            mcycles = ccf(cpu);
            break;
        case 0x40:
            mcycles = ld_r_r(cpu, &cpu->regist->b, &cpu->regist->b);
            break;
        case 0x41:
            mcycles = ld_r_r(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0x42:
            mcycles = ld_r_r(cpu, &cpu->regist->b, &cpu->regist->d);
            break;
        case 0x43:
            mcycles = ld_r_r(cpu, &cpu->regist->b, &cpu->regist->e);
            break;
        case 0x44:
            mcycles = ld_r_r(cpu, &cpu->regist->b, &cpu->regist->h);
            break;
        case 0x45:
            mcycles = ld_r_r(cpu, &cpu->regist->b, &cpu->regist->l);
            break;
        case 0x46:
            mcycles = ld_r_hl(cpu, &cpu->regist->b);
            break;
        case 0x47:
            mcycles = ld_r_r(cpu, &cpu->regist->b, &cpu->regist->a);
            break;
        case 0x48:
            mcycles = ld_r_r(cpu, &cpu->regist->c, &cpu->regist->b);
            break;
        case 0x49:
            mcycles = ld_r_r(cpu, &cpu->regist->c, &cpu->regist->c);
            break;
        case 0x4A:
            mcycles = ld_r_r(cpu, &cpu->regist->c, &cpu->regist->d);
            break;
        case 0x4B:
            mcycles = ld_r_r(cpu, &cpu->regist->c, &cpu->regist->e);
            break;
        case 0x4C:
            mcycles = ld_r_r(cpu, &cpu->regist->c, &cpu->regist->h);
            break;
        case 0x4D:
            mcycles = ld_r_r(cpu, &cpu->regist->c, &cpu->regist->l);
            break;
        case 0x4E:
            mcycles = ld_r_hl(cpu, &cpu->regist->c);
            break;
        case 0x4F:
            mcycles = ld_r_r(cpu, &cpu->regist->c, &cpu->regist->a);
            break;
        case 0x50:
            mcycles = ld_r_r(cpu, &cpu->regist->d, &cpu->regist->b);
            break;
        case 0x51:
            mcycles = ld_r_r(cpu, &cpu->regist->d, &cpu->regist->c);
            break;
        case 0x52:
            mcycles = ld_r_r(cpu, &cpu->regist->d, &cpu->regist->d);
            break;
        case 0x53:
            mcycles = ld_r_r(cpu, &cpu->regist->d, &cpu->regist->e);
            break;
        case 0x54:
            mcycles = ld_r_r(cpu, &cpu->regist->d, &cpu->regist->h);
            break;
        case 0x55:
            mcycles = ld_r_r(cpu, &cpu->regist->d, &cpu->regist->l);
            break;
        case 0x56:
            mcycles = ld_r_hl(cpu, &cpu->regist->d);
            break;
        case 0x57:
            mcycles = ld_r_r(cpu, &cpu->regist->d, &cpu->regist->a);
            break;
        case 0x58:
            mcycles = ld_r_r(cpu, &cpu->regist->e, &cpu->regist->b);
            break;
        case 0x59:
            mcycles = ld_r_r(cpu, &cpu->regist->e, &cpu->regist->c);
            break;
        case 0x5A:
            mcycles = ld_r_r(cpu, &cpu->regist->e, &cpu->regist->d);
            break;
        case 0x5B:
            mcycles = ld_r_r(cpu, &cpu->regist->e, &cpu->regist->e);
            break;
        case 0x5C:
            mcycles = ld_r_r(cpu, &cpu->regist->e, &cpu->regist->h);
            break;
        case 0x5D:
            mcycles = ld_r_r(cpu, &cpu->regist->e, &cpu->regist->l);
            break;
        case 0x5E:
            mcycles = ld_r_hl(cpu, &cpu->regist->e);
            break;
        case 0x5F:
            mcycles = ld_r_r(cpu, &cpu->regist->e, &cpu->regist->a);
            break;
        case 0x60:
            mcycles = ld_r_r(cpu, &cpu->regist->h, &cpu->regist->b);
            break;
        case 0x61:
            mcycles = ld_r_r(cpu, &cpu->regist->h, &cpu->regist->c);
            break;
        case 0x62:
            mcycles = ld_r_r(cpu, &cpu->regist->h, &cpu->regist->d);
            break;
        case 0x63:
            mcycles = ld_r_r(cpu, &cpu->regist->h, &cpu->regist->e);
            break;
        case 0x64:
            mcycles = ld_r_r(cpu, &cpu->regist->h, &cpu->regist->h);
            break;
        case 0x65:
            mcycles = ld_r_r(cpu, &cpu->regist->h, &cpu->regist->l);
            break;
        case 0x66:
            mcycles = ld_r_hl(cpu, &cpu->regist->h);
            break;
        case 0x67:
            mcycles = ld_r_r(cpu, &cpu->regist->l, &cpu->regist->a);
            break;
        case 0x68:
            mcycles = ld_r_r(cpu, &cpu->regist->l, &cpu->regist->b);
            break;
        case 0x69:
            mcycles = ld_r_r(cpu, &cpu->regist->l, &cpu->regist->c);
            break;
        case 0x6A:
            mcycles = ld_r_r(cpu, &cpu->regist->l, &cpu->regist->d);
            break;
        case 0x6B:
            mcycles = ld_r_r(cpu, &cpu->regist->l, &cpu->regist->e);
            break;
        case 0x6C:
            mcycles = ld_r_r(cpu, &cpu->regist->l, &cpu->regist->h);
            break;
        case 0x6D:
            mcycles = ld_r_r(cpu, &cpu->regist->l, &cpu->regist->l);
            break;
        case 0x6E:
            mcycles = ld_r_hl(cpu, &cpu->regist->l);
            break;
        case 0x6F:
            mcycles = ld_r_r(cpu, &cpu->regist->l, &cpu->regist->a);
            break;
        case 0x70:
            mcycles = ld_hl_r(cpu, &cpu->regist->b);
            break;
        case 0x71:
            mcycles = ld_hl_r(cpu, &cpu->regist->c);
            break;
        case 0x72:
            mcycles = ld_hl_r(cpu, &cpu->regist->d);
            break;
        case 0x73:
            mcycles = ld_hl_r(cpu, &cpu->regist->e);
            break;
        case 0x74:
            mcycles = ld_hl_r(cpu, &cpu->regist->h);
            break;
        case 0x75:
            mcycles = ld_hl_r(cpu, &cpu->regist->l);
            break;
        case 0x76:
            mcycles = halt(cpu);
            break;
        case 0x77:
            mcycles = ld_hl_r(cpu, &cpu->regist->a);
            break;
        case 0x78:
            mcycles = ld_r_r(cpu, &cpu->regist->a, &cpu->regist->b);
            break;
        case 0x79:
            mcycles = ld_r_r(cpu, &cpu->regist->a, &cpu->regist->c);
            break;
        case 0x7A:
            mcycles = ld_r_r(cpu, &cpu->regist->a, &cpu->regist->d);
            break;
        case 0x7B:
            mcycles = ld_r_r(cpu, &cpu->regist->a, &cpu->regist->e);
            break;
        case 0x7C:
            mcycles = ld_r_r(cpu, &cpu->regist->a, &cpu->regist->h);
            break;
        case 0x7D:
            mcycles = ld_r_r(cpu, &cpu->regist->a, &cpu->regist->l);
            break;
        case 0x7E:
            mcycles = ld_r_hl(cpu, &cpu->regist->a);
            break;
        case 0x7F:
            mcycles = ld_r_r(cpu, &cpu->regist->a, &cpu->regist->a);
            break;
        case 0x80:
            mcycles = add_a_r(cpu, &cpu->regist->b);
            break;
        case 0x81:
            mcycles = add_a_r(cpu, &cpu->regist->c);
            break;
        case 0x82:
            mcycles = add_a_r(cpu, &cpu->regist->d);
            break;
        case 0x83:
            mcycles = add_a_r(cpu, &cpu->regist->e);
            break;
        case 0x84:
            mcycles = add_a_r(cpu, &cpu->regist->h);
            break;
        case 0x85:
            mcycles = add_a_r(cpu, &cpu->regist->l);
            break;
        case 0x86:
            mcycles = add_a_hl(cpu);
            break;
        case 0x87:
            mcycles = add_a_r(cpu, &cpu->regist->a);
            break;
        case 0x88:
            mcycles = adc_a_r(cpu, &cpu->regist->b);
            break;
        case 0x89:
            mcycles = adc_a_r(cpu, &cpu->regist->c);
            break;
        case 0x8A:
            mcycles = adc_a_r(cpu, &cpu->regist->d);
            break;
        case 0x8B:
            mcycles = adc_a_r(cpu, &cpu->regist->e);
            break;
        case 0x8C:
            mcycles = adc_a_r(cpu, &cpu->regist->h);
            break;
        case 0x8D:
            mcycles = adc_a_r(cpu, &cpu->regist->l);
            break;
        case 0x8E:
            mcycles = adc_a_hl(cpu);
            break;
        case 0x8F:
            mcycles = adc_a_r(cpu, &cpu->regist->a);
            break;
        case 0x90:
            mcycles = sub_a_r(cpu, &cpu->regist->b);
            break;
        case 0x91:
            mcycles = sub_a_r(cpu, &cpu->regist->c);
            break;
        case 0x92:
            mcycles = sub_a_r(cpu, &cpu->regist->d);
            break;
        case 0x93:
            mcycles = sub_a_r(cpu, &cpu->regist->e);
            break;
        case 0x94:
            mcycles = sub_a_r(cpu, &cpu->regist->h);
            break;
        case 0x95:
            mcycles = sub_a_r(cpu, &cpu->regist->l);
            break;
        case 0x96:
            mcycles = sub_a_hl(cpu);
            break;
        case 0x97:
            mcycles = sub_a_r(cpu, &cpu->regist->a);
            break;
        case 0x98:
            mcycles = sbc_a_r(cpu, &cpu->regist->b);
            break;
        case 0x99:
            mcycles = sbc_a_r(cpu, &cpu->regist->c);
            break;
        case 0x9A:
            mcycles = sbc_a_r(cpu, &cpu->regist->d);
            break;
        case 0x9B:
            mcycles = sbc_a_r(cpu, &cpu->regist->e);
            break;
        case 0x9C:
            mcycles = sbc_a_r(cpu, &cpu->regist->h);
            break;
        case 0x9D:
            mcycles = sbc_a_r(cpu, &cpu->regist->l);
            break;
        case 0x9E:
            mcycles = sbc_a_hl(cpu);
            break;
        case 0x9F:
            mcycles = sbc_a_r(cpu, &cpu->regist->a);
            break;
        case 0xA0:
            mcycles = and_a_r(cpu, &cpu->regist->b);
            break;
        case 0xA1:
            mcycles = and_a_r(cpu, &cpu->regist->c);
            break;
        case 0xA2:
            mcycles = and_a_r(cpu, &cpu->regist->d);
            break;
        case 0xA3:
            mcycles = and_a_r(cpu, &cpu->regist->e);
            break;
        case 0xA4:
            mcycles = and_a_r(cpu, &cpu->regist->h);
            break;
        case 0xA5:
            mcycles = and_a_r(cpu, &cpu->regist->l);
            break;
        case 0xA6:
            mcycles = add_a_hl(cpu);
            break;
        case 0xA7:
            mcycles = and_a_r(cpu, &cpu->regist->a);
            break;
        case 0xA8:
            mcycles = xor_a_r(cpu, &cpu->regist->b);
            break;
        case 0xA9:
            mcycles = xor_a_r(cpu, &cpu->regist->c);
            break;
        case 0xAA:
            mcycles = xor_a_r(cpu, &cpu->regist->d);
            break;
        case 0xAB:
            mcycles = xor_a_r(cpu, &cpu->regist->e);
            break;
        case 0xAC:
            mcycles = xor_a_r(cpu, &cpu->regist->h);
            break;
        case 0xAD:
            mcycles = xor_a_r(cpu, &cpu->regist->d);
            break;
        case 0xAE:
            mcycles = xor_a_hl(cpu);
            break;
        case 0xAF:
            mcycles = xor_a_r(cpu, &cpu->regist->a);
            break;
        case 0xB0:
            mcycles = or_a_r(cpu, &cpu->regist->b);
            break;
        case 0xB1:
            mcycles = or_a_r(cpu, &cpu->regist->c);
            break;
        case 0xB2:
            mcycles = or_a_r(cpu, &cpu->regist->d);
            break;
        case 0xB3:
            mcycles = or_a_r(cpu, &cpu->regist->e);
            break;
        case 0xB4:
            mcycles = or_a_r(cpu, &cpu->regist->h);
            break;
        case 0xB5:
            mcycles = or_a_r(cpu, &cpu->regist->l);
            break;
        case 0xB6:
            mcycles = or_a_hl(cpu);
            break;
        case 0xB7:
            mcycles = or_a_r(cpu, &cpu->regist->a);
            break;
        case 0xB8:
            mcycles = cp_a_r(cpu, &cpu->regist->b);
            break;
        case 0xB9:
            mcycles = cp_a_r(cpu, &cpu->regist->c);
            break;
        case 0xBA:
            mcycles = cp_a_r(cpu, &cpu->regist->d);
            break;
        case 0xBB:
            mcycles = cp_a_r(cpu, &cpu->regist->e);
            break;
        case 0xBC:
            mcycles = cp_a_r(cpu, &cpu->regist->h);
            break;
        case 0xBD:
            mcycles = cp_a_r(cpu, &cpu->regist->l);
            break;
        case 0xBE:
            mcycles = cp_a_hl(cpu);
            break;
        case 0xBF:
            mcycles = cp_a_r(cpu, &cpu->regist->a);
            break;
        case 0xC0:
            mcycles = ret_cc(cpu, get_z(cpu->regist) == 0);
            break;
        case 0xC1:
            mcycles = pop_rr(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0xC2:
            mcycles = jp_cc_nn(cpu, get_z(cpu->regist) == 0);
            break;
        case 0xC3:
            mcycles = jp_nn(cpu);
            break;
        case 0xC4:
            mcycles = call_cc_nn(cpu, get_z(cpu->regist) == 0);
            break;
        case 0xC5:
            mcycles = push_rr(cpu, &cpu->regist->b, &cpu->regist->c);
            break;
        case 0xC6:
            mcycles = add_a_n(cpu);
            break;
        case 0xC7:
            mcycles = rst(cpu, 0x00);
            break;
        case 0xC8:
            mcycles = ret_cc(cpu, get_z(cpu->regist) == 1);
            break;
        case 0xC9:
            mcycles = ret(cpu);
            break;
        case 0xCA:
            mcycles = jp_cc_nn(cpu, get_z(cpu->regist) == 1);
            break;
        case 0xCB:
            //TODO redirect to prefix switcg
            break;
        case 0xCC:
            mcycles = call_cc_nn(cpu, get_z(cpu->regist) == 1);
            break;
        case 0xCD:
            mcycles = call_nn(cpu);
            break;
        case 0xCE:
            mcycles = adc_a_n(cpu);
            break;
        case 0xCF:
            mcycles = rst(cpu, 0x08);
            break;
        case 0xD0:
            mcycles = ret_cc(cpu, get_c(cpu->regist) == 0);
            break;
        case 0xD1:
            mcycles = pop_rr(cpu, &cpu->regist->d, &cpu->regist->e);
            break;
        case 0xD2:
            mcycles = jp_cc_nn(cpu, get_c(cpu->regist) == 0);
            break;
        case 0xD4:
            mcycles = call_cc_nn(cpu, get_c(cpu->regist) == 0);
            break;
        case 0xD5:
            mcycles = push_rr(cpu, &cpu->regist->d, &cpu->regist->e);
            break;
        case 0xD6:
            mcycles = sub_a_n(cpu);
            break;
        case 0xD7:
            mcycles = rst(cpu, 0x10);
            break;
        case 0xD8:
            mcycles = ret_cc(cpu, get_c(cpu->regist) == 1);
            break;
        case 0xD9:
            mcycles = reti(cpu);
            break;
        case 0xDA:
            mcycles = jp_cc_nn(cpu, get_c(cpu->regist) == 1);
            break;
        case 0xDC:
            mcycles = call_cc_nn(cpu, get_c(cpu->regist) == 1);
            break;
        case 0xDE:
            mcycles = sbc_a_n(cpu);
            break;
        case 0xDF:
            mcycles = rst(cpu, 0x18);
            break;
        case 0xE0:
            mcycles = ldh_n_a(cpu);
            break;
        case 0xE1:
            mcycles = pop_rr(cpu, &cpu->regist->h, &cpu->regist->l);
            break;
        case 0xE2:
            mcycles = ldh_c_a(cpu);
            break;
        case 0xE5:
            mcycles = push_rr(cpu, &cpu->regist->h, &cpu->regist->l);
            break;
        case 0xE6:
            mcycles = and_a_n(cpu);
            break;
        case 0xE7:
            mcycles = rst(cpu, 0x20);
            break;
        case 0xE8:
            mcycles = add_sp_e8(cpu);
            break;
        case 0xE9:
            mcycles = jp_hl(cpu);
            break;
        case 0xEA:
            mcycles = ld_nn_a(cpu);
            break;
        case 0xEE:
            mcycles = xor_a_n(cpu);
            break;
        case 0xEF:
            mcycles = rst(cpu, 0x28);
            break;
        case 0xF0:
            mcycles = ldh_a_n(cpu);
            break;
        case 0xF1:
            mcycles = pop_af(cpu);
            break;
        case 0xF2:
            mcycles = ldh_a_c(cpu);
            break;
        case 0xF3:
            mcycles = di(cpu);
            break;
        case 0xF5:
            mcycles = push_rr(cpu, &cpu->regist->a, &cpu->regist->f);
            break;
        case 0xF6:
            mcycles = or_a_n(cpu);
            break;
        case 0xF7:
            mcycles = rst(cpu, 0x30);
            break;
        case 0xF8:
            mcycles = ld_hl_spe8(cpu);
            break;
        case 0xF9:
            mcycles = ld_sp_hl(cpu);
            break;
        case 0xFA:
            mcycles = ld_a_nn(cpu);
            break;
        case 0xFB:
            mcycles = ei(cpu);
            break;
        case 0xFE:
            mcycles = cp_a_n(cpu);
            break;
        case 0xFF:
            mcycles = rst(cpu, 0x38);
            break;
        default:
            //TODO undefined opcode behaviour ?
            break;
    }
}
