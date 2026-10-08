/* Unverified draft C; integration is deferred */
#include "game_draft_signatures.h"
#include "xport_trace.h"

sint32 xport_bios_enter_critical(void);
void xport_bios_exit_critical(void);

uint32 sub_8001BE24(uint32 output, uint32 format, uint32 arguments)
{
    FUNCTION_MARKER(0x8001BE24u, "SLUS_009.74");
    uint32 written = 0u, flags, value, guest_text, digit_table, source;
    sint32 character, width, precision, length, base, i;
    unsigned char temporary[512], *text;
    unsigned char sign;
    /* TODO Variadic argument-list packing and temporary buffer guest-view lifetime */
    character = (sint8)r_u8(format);
    while (character) {
     if (character != '%') { w_u8(output + written++, (uint32)character); ++format; character = (sint8)r_u8(format); continue; }
     flags = r_u32(0x8002E168u); width = (sint32)r_u32(0x8002E16Cu); precision = (sint32)r_u32(0x8002E170u);
     for (;;) {
      character = (sint8)r_u8(++format);
      if (character == '-') flags |= 1u;
      else if (character == '+') flags |= 2u;
      else if (character == ' ') flags = (flags & ~0xFF00u) | 0x2000u;
      else if (character == '#') flags |= 4u;
      else if (character == '0') flags |= 8u;
      else break;
     }
     if (character == '*') {
      width = (sint32)r_u32(arguments); arguments += 4u;
      if (width < 0) { width = (sint32)(0u - (uint32)width); flags |= 1u; }
      character = (sint8)r_u8(++format);
     } else while ((uint32)(character - '0') < 10u) {
      width = (sint32)((uint32)width * 10u + (uint32)(character - '0'));
      character = (sint8)r_u8(++format);
     }
     if (character == '.') {
      character = (sint8)r_u8(++format);
      if (character == '*') { precision = (sint32)r_u32(arguments); arguments += 4u; character = (sint8)r_u8(++format); }
      else while ((uint32)(character - '0') < 10u) {
       precision = (sint32)((uint32)precision * 10u + (uint32)(character - '0'));
       character = (sint8)r_u8(++format);
      }
      if (precision >= 0) flags |= 16u;
     }
     if (flags & 1u) flags &= ~8u;
     while (character == 'h' || character == 'l' || character == 'L') {
      flags |= character == 'h' ? 32u : character == 'l' ? 64u : 128u;
      character = (sint8)r_u8(++format);
     }
     text = temporary + sizeof(temporary); length = 0; guest_text = 0u;
     if (character == 'n') {
      source = r_u32(arguments); arguments += 4u;
      if (flags & 32u) w_u16(source, written); else w_u32(source, written);
      goto next_character;
     }
     if (character == '%') { w_u8(output + written++, '%'); goto next_character; }
     if (character == 'c') { *--text = r_u8(arguments); arguments += 4u; length = 1; }
     else if (character == 's') {
      guest_text = r_u32(arguments); arguments += 4u;
      if (flags & 4u) {
       length = r_u8(guest_text++);
       if ((flags & 16u) && precision < length) length = precision;
      } else if (flags & 16u) {
       /* TODO Missing bounded string search SDK adapter */
       source = moh_draft_missing_call(0x800265ACu, guest_text, 0u, (uint32)precision);
       length = source ? (sint32)(source - guest_text) : precision;
      } else {
       /* TODO Missing strlen SDK adapter */
       length = (sint32)moh_draft_missing_call(0x8001AC88u, guest_text);
      }
     } else if (character == 'd' || character == 'i' || character == 'u' || character == 'o' || character == 'x' || character == 'X' || character == 'p') {
      value = r_u32(arguments); arguments += 4u;
      sign = (unsigned char)(flags >> 8);
      base = character == 'o' ? 8 : character == 'x' || character == 'X' || character == 'p' ? 16 : 10;
      if (character == 'd' || character == 'i') {
       if (flags & 32u) value = (uint32)(sint16)value;
       if ((sint32)value < 0) { value = 0u - value; sign = '-'; }
       else if (flags & 2u) sign = '+';
      } else {
       if (flags & 32u) value &= 65535u;
       if (character == 'u') sign = 0u;
      }
      if (character == 'p') { precision = 8; flags |= 80u; }
      if (!(flags & 16u)) {
       if (flags & 8u) {
        precision = width;
        if (base == 10 && sign) precision = (sint32)((uint32)precision - 1u);
        if (base == 16 && (flags & 4u)) precision = (sint32)((uint32)precision - 2u);
       }
       if (precision <= 0) precision = 1;
      }
      digit_table = character == 'x' ? 0x80014A20u : 0x80014A0Cu;
      while (value) {
       uint32 digit = value % (uint32)base; value /= (uint32)base;
       *--text = base == 16 ? r_u8(digit_table + digit) : (unsigned char)('0' + digit); ++length;
      }
      if (base == 8 && (flags & 4u) && length && *text != '0') { *--text = '0'; ++length; }
      while (length < precision) { *--text = '0'; ++length; }
      if (base == 10 && sign) { *--text = sign; ++length; }
      if (base == 16 && (flags & 4u)) { *--text = (unsigned char)character; *--text = '0'; length += 2; }
     } else break;
     if (length < width && !(flags & 1u)) {
      do { w_u8(output + written++, ' '); --width; } while (length < width);
     }
     /* TODO Missing memcpy SDK adapter with local temporary buffer views */
     if (character == 's') source = guest_text;
     else source = moh_draft_local_address(text, (uint32)length);
     moh_draft_missing_call(0x800265FCu, output + written, source, (uint32)length);
     written += (uint32)length;
     for (i = length; i < width; ++i) w_u8(output + written++, ' ');
    next_character:
     character = (sint8)r_u8(++format);
    }
    w_u8(output + written, 0u);
    return written;
}

uint32 sub_80017408(uint32 a1)
{
    FUNCTION_MARKER(0x80017408u, "SLUS_009.74");
    uint32 command = 0x08000000u, changed, i, table, mode, resolution, scale;
    sint32 width, height, top, bottom, left, right, span, limit_left, limit_right, limit_top, limit_bottom;
    /* TODO Resolve GPU command and logging callbacks */
    if (r_u8(0x8002DF1Au) >= 2u) moh_draft_indirect_call(r_u32(0x8002DF14u), 0x80014944u, a1);
    table = r_u32(0x8002DF10u);
    moh_draft_indirect_call(r_u32(table + 16u), ((r_u16(a1 + 2u) & 1023u) << 10) | (r_u16(a1) & 1023u) | 0x05000000u);
    changed = r_u32(0x8002DF94u) != r_u32(a1 + 16u);
    for (i = 0u; i < 8u; i += 2u) if (r_u16(0x8002DF84u + i) != r_u16(a1 + i)) changed = 1u;
    if (changed) {
     mode = sub_80022560(); w_u8(a1 + 18u, mode);
     if ((mode & 255u) == 1u) command |= 8u;
     if (r_u8(a1 + 17u)) command |= 16u;
     if (r_u8(a1 + 16u)) command |= 32u;
     if (r_u8(0x8002DF1Bu)) command |= 128u;
     width = (sint16)r_u16(a1 + 4u); height = (sint16)r_u16(a1 + 6u);
     if (width >= 281) {
      if (width < 353) command |= 1u;
      else if (width < 401) command |= 64u;
      else if (width < 561) command |= 2u;
      else command |= 3u;
     }
     if (height >= (r_u8(a1 + 18u) ? 289 : 257)) command |= 36u;
     moh_draft_indirect_call(r_u32(r_u32(0x8002DF10u) + 16u), command);
     w_u8(a1 + 18u, 8u);
    }
    changed = r_u8(a1 + 18u) == 8u;
    for (i = 0u; i < 8u; i += 2u) if (r_u16(0x8002DF8Cu + i) != r_u16(a1 + 8u + i)) changed = 1u;
    if (changed) {
     mode = sub_80022560(); w_u8(a1 + 18u, mode); mode &= 255u;
     top = (sint16)r_u16(a1 + 10u) + (mode ? 19 : 16);
     height = (sint16)r_u16(a1 + 14u); bottom = top + (height ? height : 240);
     width = (sint16)r_u16(a1 + 4u);
     resolution = width < 281 ? 0u : width < 353 ? 1u : width < 401 ? 2u : width < 561 ? 3u : 4u;
     scale = r_u8(0x8002E014u + resolution);
     i = 4u * (5u * mode + resolution);
     left = r_u16(0x8002DFECu + i);
     span = (sint32)r_u16(0x8002DFEEu + i) - left;
     left = (sint32)((uint32)left + (uint32)((sint16)r_u16(a1 + 8u) * (sint32)scale));
     width = (sint16)r_u16(a1 + 12u);
     if (width) span = (sint32)((uint32)span * (uint32)width) >> 8;
     right = (sint32)((uint32)left + (uint32)span);
     limit_left = mode ? 540 : 500; limit_right = mode ? 3220 : 3250;
     limit_top = mode ? 19 : 16; limit_bottom = mode ? 303 : 257;
     if (left < limit_left) left = limit_left; else if (left > limit_right) left = limit_right;
     width = left + 4 * (sint32)scale;
     if (right >= width) { width = mode ? 3260 : 3290; if (right < width + 1) width = right; }
     right = (sint16)width;
     if (top < limit_top) top = limit_top; else if (top > limit_bottom) top = limit_bottom;
     height = top + 2;
     if (bottom >= height) { height = mode ? 305 : 258; if (bottom < height + 1) height = bottom; }
     bottom = (sint16)height;
     moh_draft_indirect_call(r_u32(r_u32(0x8002DF10u) + 16u), (((uint32)right & 4095u) << 12) | ((uint32)left & 4095u) | 0x06000000u);
     moh_draft_indirect_call(r_u32(r_u32(0x8002DF10u) + 16u), (((uint32)bottom & 1023u) << 10) | ((uint32)top & 1023u) | 0x07000000u);
    }
    /* TODO Missing memcpy adapter */
    moh_draft_missing_call(0x800154ECu, 0x8002DF84u, a1, 20u);
    return a1;
}

uint32 sub_80023F04(uint32 a1)
{
    FUNCTION_MARKER(0x80023F04u, "SLUS_009.74");
    uint32 hardware = r_u32(0x800347ACu), channel, counter, count, i;
    w_u16(hardware + 10u, 64u); w_u16(hardware + 10u, 0u); w_u16(hardware + 8u, 13u); w_u16(hardware + 14u, 136u);
    sub_800251D8(r_u8(a1 + 232u) == 8u ? 80u : 145u);
    channel = r_u32(0x80034774u); hardware = r_u32(0x800347ACu);
    w_u16(hardware + 10u, channel ? 0x3003u : 0x1003u);
    counter = 0x8003478Cu + 4u * channel;
    if ((sint32)r_u32(counter) >= 0) {
     while ((sint32)r_u32(0x8003478Cu + 4u * r_u32(0x80034774u)) > 0) {
      counter = 0x8003478Cu + 4u * r_u32(0x80034774u);
      count = r_u32(counter) - 1u; w_u32(counter, count);
      /* TODO Resolve refill callback */
      moh_draft_indirect_call(r_u32(0x80034754u), r_u32(a1 + 12u) + 240u * count);
     }
     counter = 0x8003478Cu + 4u * r_u32(0x80034774u);
     if (!r_u32(counter)) {
      w_u32(counter, 0xFFFFFFFFu);
      moh_draft_indirect_call(r_u32(0x80034754u), a1);
      moh_draft_indirect_call(r_u32(0x80034758u), a1);
     }
    }
    hardware = r_u32(0x800347ACu);
    if (!(r_u16(hardware + 4u) & 512u)) goto command_done;
    w_u16(hardware + 10u, r_u16(hardware + 10u) | 16u);
    if (!(r_u16(hardware + 4u) & 512u)) {
     w_u32(r_u32(0x800347A8u), 0xFFFFFF7Fu); goto command_done;
    }
    while (!sub_800251F8()) {}
    w_u8(r_u32(0x800347ACu), 1u); sub_800251D8(2000u);
    if (!sub_800247C8()) return 0u;
    /* TODO Callee outside selected set */
    moh_draft_missing_call(0x80024858u); sub_800251D8(430u);
    for (i = 0u; i < 2u; ++i) {
     while (!(r_u32(r_u32(0x800347A8u)) & 128u)) if (sub_800251F8()) return 0u;
     w_u8(r_u32(0x800347ACu), i ? 1u : 66u); sub_800251D8(60u);
     if (!sub_800247C8()) return 0u;
     moh_draft_missing_call(0x80024858u);
     if (!i) sub_800251D8(430u);
    }
    return 0u;
    command_done:
    return r_u8(a1 + 80u) ? r_u8(a1 + 55u) == 0u : 1u;
}

sint32 sub_80018830(uint32 callback, uint32 data, uint32 size, uint32 argument)
{
    FUNCTION_MARKER(0x80018830u, "SLUS_009.74");
    uint32 entry, i, saved_mask;
    sub_80018FB8();
    while (((r_u32(0x8002E040u) + 1u) & 63u) == r_u32(0x8002E044u)) {
     if (sub_80018FEC()) return -1;
     sub_80018AE0();
    }
    saved_mask = sub_8001CBAC(0u); w_u32(0x8002E048u, saved_mask); w_u32(0x8002DF20u, 1u);
    if (r_u8(0x8002DF19u) && (r_u32(0x8002E040u) != r_u32(0x8002E044u) ||
     (r_u32(r_u32(0x8002E02Cu)) & 0x01000000u) || r_u32(0x8002DF24u))) {
     sub_8001CAD8(2u, 0x80018AE0u);
     if (size) {
      for (i = 0u; (sint32)i < (sint32)size / 4; ++i)
       w_u32(0x800CA310u + 96u * r_u32(0x8002E040u) + 4u * i, r_u32(data + 4u * i));
      entry = 0x800CA310u + 96u * r_u32(0x8002E040u);
     } else entry = data;
     w_u32(0x800CA308u + 96u * r_u32(0x8002E040u), entry);
     w_u32(0x800CA30Cu + 96u * r_u32(0x8002E040u), argument);
     w_u32(0x800CA304u + 96u * r_u32(0x8002E040u), callback);
     w_u32(0x8002E040u, (r_u32(0x8002E040u) + 1u) & 63u);
     sub_8001CBAC(r_u32(0x8002E048u)); sub_80018AE0();
     return (r_u32(0x8002E040u) - r_u32(0x8002E044u)) & 63u;
    }
    while (!(r_u32(r_u32(0x8002E020u)) & 0x04000000u)) {}
    /* TODO Resolve queued callback adapter */
    moh_draft_indirect_call(callback, data, argument);
    sub_8001CBAC(r_u32(0x8002E048u)); return 0;
}

uint32 sub_80020F48(uint32 a1)
{
    FUNCTION_MARKER(0x80020F48u, "SLUS_009.74");
    uint32 hardware = r_u32(0x800302BCu), polls, i;
    w_u32(r_u32(0x800302CCu), r_u32(r_u32(0x800302CCu)) | 0xB0000u);
    w_u32(0x800302D8u, 0u); w_u32(0x800302DCu, 0u); w_u16(0x800302D4u, 0u);
    w_u16(hardware + 384u, 0u); w_u16(hardware + 386u, 0u); w_u16(hardware + 426u, 0u); sub_800218D8();
    hardware = r_u32(0x800302BCu); w_u16(hardware + 384u, 0u); w_u16(hardware + 386u, 0u);
    polls = 0u;
    while (r_u16(r_u32(0x800302BCu) + 430u) & 2047u) {
     if (++polls >= 0xF01u) {
      /* TODO Missing timeout printf adapter */
      moh_draft_missing_call(0x80015B28u, 0x80014D1Cu, 0x80014D2Cu); break;
     }
    }
    w_u32(0x800302E0u, 2u); w_u32(0x800302E4u, 3u); w_u32(0x800302E8u, 8u); w_u32(0x800302ECu, 7u);
    hardware = r_u32(0x800302BCu);
    w_u16(hardware + 428u, 4u); w_u16(hardware + 388u, 0u); w_u16(hardware + 390u, 0u);
    w_u16(hardware + 396u, 65535u); w_u16(hardware + 398u, 65535u);
    w_u16(hardware + 408u, 0u); w_u16(hardware + 410u, 0u);
    for (i = 0u; i < 10u; ++i) w_u16(0x800D0134u + 2u * i, 0u);
    if (!a1) {
     w_u16(0x800302D4u, 512u);
     hardware = r_u32(0x800302BCu);
     w_u16(hardware + 400u, 0u); w_u16(hardware + 402u, 0u); w_u16(hardware + 404u, 0u); w_u16(hardware + 406u, 0u);
     w_u16(hardware + 432u, 0u); w_u16(hardware + 434u, 0u); w_u16(hardware + 436u, 0u); w_u16(hardware + 438u, 0u);
     sub_800211C8(0x800302FCu, 16u);
     hardware = r_u32(0x800302BCu);
     for (i = 0u; i < 24u; ++i, hardware += 16u) {
      w_u16(hardware, 0u); w_u16(hardware + 2u, 0u); w_u16(hardware + 4u, 0x3FFFu);
      w_u16(hardware + 6u, 512u); w_u16(hardware + 8u, 0u); w_u16(hardware + 10u, 0u);
     }
     hardware = r_u32(0x800302BCu); w_u16(hardware + 392u, 65535u); w_u16(hardware + 394u, 255u);
     sub_800218D8(); sub_800218D8(); sub_800218D8(); sub_800218D8();
     hardware = r_u32(0x800302BCu); w_u16(hardware + 396u, 65535u); w_u16(hardware + 398u, 255u);
     sub_800218D8(); sub_800218D8(); sub_800218D8(); sub_800218D8();
    }
    w_u32(0x800302F0u, 1u); w_u16(r_u32(0x800302BCu) + 426u, 0xC000u);
    w_u32(0x800302F4u, 0u); w_u32(0x800302F8u, 0u);
    return 0u;
}

uint32 sub_80017AF8(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x80017AF8u, "SLUS_009.74");
    uint32 words = 7u, xy, extent, color;
    sint32 width, height, max_width, max_height;
    w_u32(a1 + 4u, sub_80017D88((uint32)(sint16)r_u16(a2), (uint32)(sint16)r_u16(a2 + 2u)));
    w_u32(a1 + 8u, sub_80017E20((uint32)(sint16)(r_u16(a2 + 4u) + r_u16(a2) - 1u),
     (uint32)(sint16)(r_u16(a2 + 2u) + r_u16(a2 + 6u) - 1u)));
    w_u32(a1 + 12u, sub_80017EB8((uint32)(sint16)r_u16(a2 + 8u), (uint32)(sint16)r_u16(a2 + 10u)));
    w_u32(a1 + 16u, sub_80017D68(r_u8(a2 + 23u), r_u8(a2 + 22u), r_u16(a2 + 20u)));
    w_u32(a1 + 20u, sub_80017ED4(a2 + 12u)); w_u32(a1 + 24u, 0xE6000000u);
    if (r_u8(a2 + 24u)) {
     xy = r_u32(a2); width = (sint16)r_u16(a2 + 4u); height = (sint16)r_u16(a2 + 6u);
     max_width = (sint16)r_u16(0x8002DF1Cu) - 1; max_height = (sint16)r_u16(0x8002DF1Eu) - 1;
     if (width < 0) width = 0; else if (width > max_width) width = max_width;
     if (height < 0) height = 0; else if (height > max_height) height = max_height;
     extent = ((uint32)width & 65535u) | ((uint32)height << 16);
     color = ((uint32)r_u8(a2 + 27u) << 16) | ((uint32)r_u8(a2 + 26u) << 8) | r_u8(a2 + 25u);
     if ((xy & 63u) || (extent & 63u)) {
      xy = ((r_u16(a2) - r_u16(a2 + 8u)) & 65535u) | ((r_u16(a2 + 2u) - r_u16(a2 + 10u)) << 16);
      color |= 0x60000000u;
     } else color |= 0x02000000u;
     words = 10u;
     w_u32(a1 + 28u, color); w_u32(a1 + 32u, xy); w_u32(a1 + 36u, extent);
    }
    w_u8(a1 + 3u, words - 1u);
    return words - 1u;
}

uint32 sub_8001827C(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x8001827Cu, "SLUS_009.74");
    sint32 width, height, product, words;
    uint32 blocks, remainder;
    sub_80018FB8();
    width = (sint16)r_u16(a1 + 4u);
    if (width < 0) width = 0; else if (width > (sint16)r_u16(0x8002DF1Cu)) width = (sint16)r_u16(0x8002DF1Cu);
    height = (sint16)r_u16(a1 + 6u);
    w_u16(a1 + 4u, (uint32)width);
    if (height < 0) height = 0; else if (height > (sint16)r_u16(0x8002DF1Eu)) height = (sint16)r_u16(0x8002DF1Eu);
    product = width * height; w_u16(a1 + 6u, (uint32)height);
    words = (product + 1) / 2;
    if (words <= 0) return 0xFFFFFFFFu;
    blocks = (uint32)((product + 1 + ((uint32)(product + 1) >> 31)) >> 5);
    remainder = (uint32)words - 16u * blocks;
    while (!(r_u32(r_u32(0x8002E020u)) & 0x04000000u)) if (sub_80018FEC()) return 0xFFFFFFFFu;
    w_u32(r_u32(0x8002E020u), 0x04000000u); w_u32(r_u32(0x8002E01Cu), 0x01000000u);
    w_u32(r_u32(0x8002E01Cu), 0xA0000000u); w_u32(r_u32(0x8002E01Cu), r_u32(a1));
    w_u32(r_u32(0x8002E01Cu), r_u32(a1 + 4u));
    while (remainder--) { w_u32(r_u32(0x8002E01Cu), r_u32(a2)); a2 += 4u; }
    if (blocks) {
     w_u32(r_u32(0x8002E020u), 0x04000002u); w_u32(r_u32(0x8002E024u), a2);
     w_u32(r_u32(0x8002E028u), (blocks << 16) | 16u); w_u32(r_u32(0x8002E02Cu), 0x01000201u);
    }
    return 0u;
}

uint32 sub_80024328(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x80024328u, "SLUS_009.74");
    uint32 io, prior, threshold, buffer;
    if ((sint32)a2 < 0) {
     io = r_u32(0x800347ACu); prior = r_u8(io);
     w_u8(a1 + 68u, 255u); w_u8(a1 + 69u, 1u); w_u8(r_u32(a1 + 64u), ~a2);
     while (!(r_u16(r_u32(0x800347ACu) + 4u) & 1u)) {}
     /* TODO Timeout helper parameter retained from prior reads */
     while (!sub_800251F8()) {}
     w_u8(r_u32(0x800347ACu), ~a2); return prior;
    }
    threshold = 136u;
    if ((r_u8(r_u32(a1 + 60u)) >> 4) == 8u && r_u8(a1 + 68u) >= 9u) threshold = 34u;
    w_u32(0x800D0A94u, 430u); w_u32(0x800D0A90u, r_u16(0x1F801120u)); w_u32(0x800D0A98u, r_u16(0x1F801124u));
    while (!(r_u16(r_u32(0x800347ACu) + 4u) & 2u)) {}
    io = r_u32(0x800347ACu); prior = r_u8(io); w_u16(io + 14u, threshold);
    while (!(r_u32(r_u32(0x800347A8u)) & 128u)) if (sub_800251F8()) return 0xFFFFFFECu;
    w_u8(r_u32(0x800347ACu), a2);
    if (threshold == 34u) {
     w_u32(r_u32(0x800347A8u), 0xFFFFFF7Fu);
     io = r_u32(0x800347ACu); w_u16(io + 10u, r_u16(io + 10u) | 16u);
    }
    buffer = r_u32(a1 + 60u) + r_u8(a1 + 68u);
    w_u8(a1 + 69u, r_u8(a1 + 69u) + 1u); w_u8(buffer, prior); w_u8(a1 + 68u, r_u8(a1 + 68u) + 1u);
    return prior;
}

sint32 sub_80027BDC(void)
{
    FUNCTION_MARKER(0x80027BDCu, "SLUS_009.74");
    uint32 result;
    /* TODO Missing CD subsystem registration and command adapters */
    moh_draft_missing_call(0x800224B0u, 0x8001526Cu);
    moh_draft_missing_call(0x80015B28u, 0x80015278u, 0x80035324u);
    w_u8(0x80035061u, 0u); w_u8(0x80035060u, 0u);
    w_u32(0x80035048u, 0u); w_u32(0x80035044u, 0u); w_u32(0x80035054u, 0u); w_u32(0x80035050u, 0u);
    sub_8001CA78(); sub_8001CAA8(2u, 0x80027DC8u);
    w_u8(r_u32(0x80035308u), 1u);
    while (r_u8(r_u32(0x8003530Cu)) & 7u) {
     w_u8(r_u32(0x80035308u), 1u); w_u8(r_u32(0x8003530Cu), 7u); w_u8(r_u32(0x80035318u), 7u);
    }
    w_u8(0x80035322u, 0u); w_u8(0x80035321u, r_u8(0x80035322u)); w_u8(0x80035320u, 2u);
    w_u8(r_u32(0x80035308u), 0u); w_u8(r_u32(0x8003530Cu), 0u); w_u32(r_u32(0x80035310u), 0x1325u);
    moh_draft_missing_call(0x80027584u, 1u, 0u, 0u, 0u);
    if (r_u32(0x80035050u) & 16u) moh_draft_missing_call(0x80027584u, 1u, 0u, 0u, 0u);
    if (moh_draft_missing_call(0x80027584u, 10u, 0u, 0u, 0u)) return -1;
    if (moh_draft_missing_call(0x80027584u, 12u, 0u, 0u, 0u)) return -1;
    result = moh_draft_missing_call(0x8002703Cu, 0u, 0u);
    return result == 2u ? 0 : -1;
}

uint32 sub_800211C8(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x800211C8u, "SLUS_009.74");
    uint32 initial, hardware, chunk, consumed, polls, result;
    hardware = r_u32(0x800302BCu);
    initial = r_u16(hardware + 430u) & 2047u;
    w_u16(hardware + 422u, r_u16(0x800302D4u));
    sub_800218D8();
    while (a2) {
     chunk = a2 < 65u ? a2 : 64u;
     hardware = r_u32(0x800302BCu);
     for (consumed = 0u; (sint32)consumed < (sint32)chunk; consumed += 2u) {
      w_u16(hardware + 424u, r_u16(a1)); a1 += 2u;
     }
     hardware = r_u32(0x800302BCu);
     w_u16(hardware + 426u, (r_u16(hardware + 426u) & 0xFFCFu) | 16u); sub_800218D8();
     polls = 0u;
     while (r_u16(r_u32(0x800302BCu) + 430u) & 1024u) {
      if (++polls >= 0xF01u) {
       /* TODO Missing timeout diagnostic adapter */
       moh_draft_missing_call(0x80015B28u, 0x80014D1Cu, 0x80014D3Cu); break;
      }
     }
     a2 -= chunk; sub_800218D8(); sub_800218D8();
    }
    hardware = r_u32(0x800302BCu); w_u16(hardware + 426u, r_u16(hardware + 426u) & 0xFFCFu);
    result = r_u16(hardware + 430u) & 2047u; polls = 0u;
    while (result != initial) {
     if (++polls >= 0xF01u) return moh_draft_missing_call(0x80015B28u, 0x80014D1Cu, 0x80014D50u);
     result = r_u16(r_u32(0x800302BCu) + 430u) & 2047u;
    }
    return result;
}

uint32 sub_80036C14(uint32 size, uint32 owner, uint32 flags)
{
    FUNCTION_MARKER(0x80036C14u, "SLUS_009.74");
    uint32 status_word, units, first, previous, node, count, tag, used, free_units, serial;
    if (!size) return 0u;
    /* Preserve native interrupt state while updating the game heap */
    status_word = (uint32)xport_bios_enter_critical();
    units = ((size + 15u) >> 4) + 1u;
    first = r_u32(0x80039C40u); previous = first; node = r_u32(previous + 4u);
    for (;;) {
     count = r_u32(node);
     if (count >= units) break;
     previous = node;
     if (node == first) goto allocation_failed;
     node = r_u32(node + 4u);
    }
    if (count == units) {
     uint32 next;
     if (node == previous) goto allocation_failed;
     next = r_u32(node + 4u);
     w_u32(previous + 4u, next);
     w_u32(previous + 12u, 0xB562439Fu - (r_u32(previous) + next + r_u32(previous + 8u)));
    } else {
     uint32 remaining = count - units;
     w_u32(node, remaining);
     w_u32(node + 12u, 0xB562439Fu - (remaining + r_u32(node + 4u) + r_u32(node + 8u)));
     node += remaining << 4;
    }
    w_u32(0x80039C40u, previous);
    serial = r_u32(0x80039D70u); tag = flags | 0x80000000u | serial;
    free_units = r_u32(0x80039D88u) - units; used = r_u32(0x80039D78u) + units;
    w_u32(node + 12u, 0xB562439Fu - (units + owner + tag));
    w_u32(node, units); w_u32(node + 4u, owner); w_u32(node + 8u, tag);
    w_u32(0x80039D88u, free_units); w_u32(0x80039D78u, used);
    if ((sint32)r_u32(0x80039D58u) < (sint32)used) w_u32(0x80039D58u, used);
    if ((sint32)free_units < (sint32)r_u32(0x80039D80u)) w_u32(0x80039D80u, free_units);
    w_u32(0x80039D70u, serial + 0x10000u);
    if (status_word) xport_bios_exit_critical();
    return node + 16u;
    allocation_failed:
    if (status_word) xport_bios_exit_critical();
    return 0u;
}

sint32 sub_80018FEC(void)
{
    FUNCTION_MARKER(0x80018FECu, "SLUS_009.74");
    uint32 old, mask;
    /* TODO Missing VSync and printf adapters */
    if ((sint32)r_u32(0x8002E054u) >= (sint32)moh_draft_missing_call(0x80015DB8u, 0xFFFFFFFFu)) {
     old = r_u32(0x8002E058u); w_u32(0x8002E058u, old + 1u);
     if ((sint32)old <= 983040) return 0;
    }
    moh_draft_missing_call(0x80015B28u, 0x8001495Cu,
     (r_u32(0x8002E040u) - r_u32(0x8002E044u)) & 63u,
     r_u32(r_u32(0x8002E020u)), r_u32(r_u32(0x8002E02Cu)), r_u32(r_u32(0x8002E024u)));
    mask = sub_8001CBAC(0u);
    w_u32(0x8002E044u, 0u); w_u32(0x8002E050u, mask);
    w_u32(0x8002E040u, r_u32(0x8002E044u));
    w_u32(r_u32(0x8002E02Cu), 1025u);
    w_u32(r_u32(0x8002E03Cu), r_u32(r_u32(0x8002E03Cu)) | 2048u);
    w_u32(r_u32(0x8002E020u), 0x02000000u); w_u32(r_u32(0x8002E020u), 0x01000000u);
    sub_8001CBAC(r_u32(0x8002E050u));
    return -1;
}

uint32 sub_80037C70(uint32 a1, uint32 a2, uint32 a3)
{
    FUNCTION_MARKER(0x80037C70u, "SLUS_009.74");
    uint32 destination = r_u32(0x80039D34u), length;
    uint32 pair, slot, next, limit;
    /* TODO Missing strlen adapter */
    length = moh_draft_missing_call(0x8001AC88u, a1);
    while (length >= 4u) { w_u32(destination, r_u32(a1)); destination += 4u; a1 += 4u; length -= 4u; }
    while (length) { w_u8(destination++, r_u8(a1++)); --length; }
    w_u8(destination, 0u);
    pair = r_u32(0x80039D3Cu); slot = r_u32(0x80039D40u);
    w_u32(pair, r_u32(a2)); w_u32(pair + 4u, r_u32(a2 + 4u)); w_u32(slot, a3);
    next = r_u32(0x80039D34u) + 16u; limit = r_u32(0x80039D38u);
    w_u32(0x80039D3Cu, pair + 8u); w_u32(0x80039D34u, next); w_u32(0x80039D40u, slot + 4u);
    if (limit < next) {
     w_u32(0x80039D34u, 0x80038E38u); w_u32(0x80039D3Cu, 0x80039638u); w_u32(0x80039D40u, 0x80039A38u);
     return 0x80039A38u;
    }
    return 0x80040000u;
}

uint32 sub_800233D4(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x800233D4u, "SLUS_009.74");
    uint32 mode = a1 & 255u;
    sub_80023AD0(mode, a2);
    w_u32(0x8003461Cu, 0u); w_u32(0x80034618u, 0u);
    if (mode == 2u && r_u8(0x800345D8u) != 1u &&
     (r_u8(0x800345D8u) != 9u || !(r_u32(0x80034608u) & 0xFF00FFu)))
     w_u8(0x80034624u, 1u);
    if (r_u8(0x800345ECu) & 16u) mode = 5u;
    if (r_u32(0x800345F0u) == 31u) sub_800234E4(mode, a2);
    else if (r_u32(0x800345F0u) == 32u) sub_800236C8(mode, a2);
    else {
     /* TODO Callee outside selected function set */
     moh_draft_missing_call(0x80023950u, mode, a2);
    }
    if (!r_u32(0x80034618u)) w_u32(0x800345F0u, 33u);
    return 33u;
}

uint32 sub_80024238(uint32 a1)
{
    FUNCTION_MARKER(0x80024238u, "SLUS_009.74");
    uint32 index = r_u32(0x80034778u), callback = r_u32(0x800347BCu + index * 4u);
    sint32 value;
    w_u32(0x80034778u, index + 1u);
    /* TODO Resolve callback signatures and preserved incoming context */
    value = (sint32)moh_draft_indirect_call(callback, a1);
    if (value < 0) return moh_draft_indirect_call(r_u32(0x80034734u), (uint32)value);
    index = r_u32(0x80034778u);
    if (index) {
     if (index != 3u || r_u8(r_u32(a1 + 60u)) != 128u) {
      sub_800251D8(60u);
      if (!sub_800247C8()) moh_draft_indirect_call(r_u32(0x80034734u), 0xFFFFFFFDu);
     }
     index = r_u32(0x80034778u);
    }
    if ((sint32)index >= 5) w_u32(0x80034778u, index - 1u);
    return index - 1u;
}

uint32 sub_8003740C(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x8003740Cu, "SLUS_009.74");
    uint32 word, result, i;
    sub_80037370(1u, 16u, 0x80037D90u);
    word = 0u;
    for (i = 0u; i < 4u; ++i) { word += (uint32)r_u8(0x80037E2Eu + i) << (8u * i); w_u32(a1, word); }
    word = 0u;
    for (i = 0u; i < 3u; ++i) { word += (uint32)r_u8(0x80037E36u + i) << (8u * i); w_u32(a2, word); }
    result = (uint32)r_u8(0x80037E39u) << 24;
    w_u32(a2, (uint32)((sint32)(word + result) >> 11));
    return result;
}

uint32 sub_800252E0(uint32 a1)
{
    FUNCTION_MARKER(0x800252E0u, "SLUS_009.74");
    uint32 mode;
    /* TODO Resolve stored callbacks */
    if (r_u32(0x80034774u) == r_u32(0x80034784u) && r_u32(0x80034770u)) {
     moh_draft_indirect_call(r_u32(0x80034764u));
     moh_draft_indirect_call(r_u32(0x80034760u));
    }
    if (r_u32(0x800347B4u)) {
     moh_draft_indirect_call(r_u32(0x8003474Cu), r_u32(a1 + 12u));
     moh_draft_indirect_call(r_u32(0x8003474Cu), r_u32(a1 + 12u) + 240u);
    }
    mode = r_u8(a1 + 55u);
    return sub_8002454C(a1, mode ? mode : 66u);
}

void sub_80036B20(uint32 start, uint32 size)
{
    FUNCTION_MARKER(0x80036B20u, "SLUS_009.74");
    uint32 header, units, tail;
    w_u32(0x80039D50u, start); w_u32(0x80039D54u, size);
    /* TODO Missing printf adapter and unspecified variadic carriers */
    moh_draft_missing_call(0x80015B28u, 0x80036078u, start, size);
    header = (start + 15u) & ~15u;
    units = (size >> 4) - 2u; tail = header + (units << 4);
    w_u32(0x80039D88u, units); w_u32(0x80039D80u, units);
    w_u32(header, units); w_u32(header + 12u, 0xB562439Fu - (units + header));
    w_u32(0x80039D68u, start); w_u32(0x80039D60u, size);
    w_u32(0x80039C40u, header); w_u32(0x80039D58u, 0u); w_u32(0x80039D78u, 0u);
    w_u32(header + 4u, header); w_u32(header + 8u, 0u); w_u32(0x80039C44u, header);
    w_u32(tail, 0u); w_u32(tail + 4u, header); w_u32(tail + 8u, 0u);
    w_u32(tail + 12u, 0xB562439Fu - header);
    w_u32(0x80039C48u, tail); w_u32(0x80039D70u, 0x10000u);
    moh_draft_missing_call(0x80015B28u, 0x800360E0u);
}

uint32 sub_80017348(uint32 a1)
{
    FUNCTION_MARKER(0x80017348u, "SLUS_009.74");
    uint32 table;
    /* TODO Resolve GPU callback adapters */
    if (r_u8(0x8002DF1Au) >= 2u) moh_draft_indirect_call(r_u32(0x8002DF14u), 0x80014910u, a1);
    sub_80017AF8(a1 + 28u, a1);
    table = r_u32(0x8002DF10u);
    w_u32(a1 + 28u, r_u32(a1 + 28u) | 0xFFFFFFu);
    moh_draft_indirect_call(r_u32(table + 8u), r_u32(table + 24u), a1 + 28u, 64u, 0u);
    /* TODO Missing memcpy adapter */
    moh_draft_missing_call(0x800154ECu, 0x8002DF28u, a1, 92u);
    return a1;
}

uint32 sub_800170AC(uint32 a1, uint32 a2, uint32 a3)
{
    FUNCTION_MARKER(0x800170ACu, "SLUS_009.74");
    uint32 table;
    sub_80016EA0(0x800148C0u, a1);
    if (!r_u16(a1 + 4u) || !r_u16(a1 + 6u)) return 0xFFFFFFFFu;
    w_u32(0x8002DFBCu, (a3 << 16) | (a2 & 65535u));
    w_u32(0x8002DFB8u, r_u32(a1)); w_u32(0x8002DFC0u, r_u32(a1 + 4u));
    table = r_u32(0x8002DF10u);
    /* TODO Resolve GPU enqueue adapter */
    return moh_draft_indirect_call(r_u32(table + 8u), r_u32(table + 24u), 0x8002DFB0u, 20u, 0u);
}

uint32 sub_80019130(uint32 a1)
{
    FUNCTION_MARKER(0x80019130u, "SLUS_009.74");
    uint32 data = r_u32(0x8002E01Cu);
    w_u32(r_u32(0x8002E020u), 0x10000007u);
    if ((r_u32(data) & 0xFFFFFFu) == 2u) {
     if (!(a1 & 8u)) return 1u;
     w_u32(r_u32(0x8002E020u), 0x09000001u); return 2u;
    }
    w_u32(data, (r_u32(r_u32(0x8002E020u)) & 0x3FFFu) | 0xE1001000u);
    (void)r_u32(r_u32(0x8002E01Cu));
    return 0u;
}

sint32 sub_80037370(uint32 sectors, uint32 sector, uint32 destination)
{
    FUNCTION_MARKER(0x80037370u, "SLUS_009.74");
    uint32 location[2];
    unsigned char result_buffer[8];
    sint32 result;
    /* TODO Missing CD adapters and local buffer guest-view lifetime */
    moh_draft_missing_call(0x8001ADD8u, sector, moh_draft_local_address(location, sizeof(location)));
    do {
     while (!moh_draft_missing_call(0x80019220u, moh_draft_local_address(location, sizeof(location)), sectors, destination, 128u)) {
      moh_draft_missing_call(0x8001AD98u); sub_80016E38(0u);
      moh_draft_missing_call(0x80015DB8u, 0u); moh_draft_missing_call(0x80015DB8u, 0u); moh_draft_missing_call(0x80015DB8u, 0u);
     }
     do { result = (sint32)moh_draft_missing_call(0x800195D0u, moh_draft_local_address(result_buffer, sizeof(result_buffer))); } while (result > 0);
    } while (result);
    return result;
}

uint32 sub_80017D88(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x80017D88u, "SLUS_009.74");
    sint32 x = (sint16)a1, y = (sint16)a2;
    sint32 max_x = (sint16)r_u16(0x8002DF1Cu) - 1;
    sint32 max_y = (sint16)r_u16(0x8002DF1Eu) - 1;
    if (x < 0) x = 0; else if (x > max_x) x = max_x;
    if (y < 0) y = 0; else if (y > max_y) y = max_y;
    return (((uint32)y & 1023u) << 10) | ((uint32)x & 1023u) | 0xE3000000u;
}

uint32 sub_8001B730(uint32 context, uint32 value, uint32 mode, uint32 entry_callback)
{
    FUNCTION_MARKER(0x8001B730u, "SLUS_009.74");
    uint32 object;
    /* TODO Incoming v0 callback carrier is an explicit draft parameter */
    object = moh_draft_indirect_call(entry_callback, context, value, mode);
    if (moh_draft_indirect_call(r_u32(0x80034750u), object)) return 0u;
    w_u8(object + 70u, 1u);
    w_u32(object + 20u, 0x8001B7C4u);
    w_u8(object + 81u, value);
    w_u8(object + 82u, mode);
    w_u32(object + 24u, 0x8001B81Cu);
    w_u8(object + 83u, value == r_u8(object + 228u));
    return 1u;
}

uint32 sub_80028AD4(void)
{
    FUNCTION_MARKER(0x80028AD4u, "SLUS_009.74");
    uint32 node = r_u32(0x800CBCFCu) + 32u * r_u32(0x800CBD04u);
    uint32 result, callback, word, i;
    w_u16(node, 2u);
    /* Paired unaligned words are copied as guest bytes */
    word = 0u;
    for (i = 0u; i < 4u; ++i) word |= (uint32)r_u8(node + 28u + i) << (i * 8u);
    for (i = 0u; i < 4u; ++i) w_u8(0x800D1678u + i, word >> (i * 8u));
    result = r_u32(node + 8u);
    w_u32(0x800D167Cu, result);
    w_u32(0x800CBD04u, r_u32(0x800CBD00u));
    callback = r_u32(0x800CBCC4u);
    /* TODO Resolve completion callback */
    if (callback) result = moh_draft_indirect_call(callback, callback);
    w_u32(0x800CBCF4u, 0u);
    return result;
}

uint32 sub_800369EC(uint32 status)
{
    FUNCTION_MARKER(0x800369ECu, "SLUS_009.74");
    uint32 result;
    /* TODO Missing CD read and diagnostic adapters */
    if ((status & 255u) == 2u) {
     result = moh_draft_missing_call(0x80019640u, r_u32(0x800385A0u));
     w_u32(0x80038598u, 0u);
     return result;
    }
    result = moh_draft_missing_call(0x80019220u, 0x8003859Cu, r_u32(0x80038594u), r_u32(0x80038590u), 128u);
    if ((sint32)result <= 0) moh_draft_missing_call(0x80015B28u, 0x80035F04u);
    return moh_draft_missing_call(0x80015B28u, 0x80035F44u);
}

uint32 sub_800216C4(uint32 a1, uint32 a2, uint32 a3)
{
    FUNCTION_MARKER(0x800216C4u, "SLUS_009.74");
    if (r_u32(0x800302D8u)) sub_800211C8(a1, a2);
    else {
     uint32 transfer = (uint32)r_u16(0x800302D4u) << (r_u32(0x800302E4u) & 31u);
     sub_80021444(2u, transfer, a3);
     /* TODO Helper returns transfer state through argument registers */
     sub_80021444(1u, moh_draft_missing_call(0x80021704u, 5u), moh_draft_missing_call(0x80021704u, 6u));
     sub_80021444(3u, a1, a2);
    }
    return a2;
}

uint32 sub_80023B80(void)
{
    FUNCTION_MARKER(0x80023B80u, "SLUS_009.74");
    uint32 result, mode;
    w_u32(0x800345D4u, 0u);
    /* TODO Missing CD SDK adapter */
    moh_draft_missing_call(0x80027A18u);
    result = 2u;
    w_u32(0x8003461Cu, 0u);
    w_u32(0x80034618u, 0u);
    if (r_u32(0x800345F4u) == 2u) {
     mode = r_u32(0x800345F8u);
     result = mode == 11u ? 17u : 16u;
     if (mode == 11u || mode == 17u || mode == 16u) {
      result = 0x800345F4u;
      w_u32(0x800345F4u, 1u); w_u32(0x800345F8u, 11u);
     }
    }
    return result;
}

uint32 sub_80023D0C(void)
{
    FUNCTION_MARKER(0x80023D0Cu, "SLUS_009.74");
    uint32 io = r_u32(0x800347A8u);
    uint32 callback;
    if (!(r_u32(io + 4u) & 1u) || !(r_u32(io) & 1u)) return 0u;
    callback = r_u32(0x8003475Cu);
    /* TODO Resolve optional callback */
    if (callback) moh_draft_indirect_call(callback);
    return 1u;
}

uint32 sub_8001F55C(uint32 a1, uint32 a2, uint32 a3)
{
    FUNCTION_MARKER(0x8001F55Cu, "SLUS_009.74");
    uint32 request, result;
    /* TODO Missing SDK request and completion adapters */
    request = moh_draft_missing_call(0x8001A070u, a1 & 255u, a2, 0u, 0u);
    if (!request) return 0u;
    do { result = moh_draft_missing_call(0x8001A658u, request, a3) & 255u; } while (!result);
    return result == 2u;
}

uint32 sub_800218D8(void)
{
    FUNCTION_MARKER(0x800218D8u, "SLUS_009.74");
    volatile uint32 accumulator = 13u;
    uint32 i;
    for (i = 0u; i < 60u; ++i) accumulator *= 13u;
    return 0u;
}

sint32 sub_80037318(uint32 filename, uint32 destination)
{
    FUNCTION_MARKER(0x80037318u, "SLUS_009.74");
    return moh_native_load_overlay(filename, destination);
}

void sub_80023C60(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x80023C60u, "SLUS_009.74");
    uint32 i;
    if (!a1) return;
    if (!a2) { w_u8(a1, 0u); return; }
    for (i = 0u; i < 8u; ++i) w_u8(a1 + i, r_u8(a2 + i));
}

uint32 sub_80022E64(uint32 a1)
{
    FUNCTION_MARKER(0x80022E64u, "SLUS_009.74");
    if ((sint32)r_u32(0x80034618u) > 0) return 0u;
    w_u32(0x800345F0u, 32u);
    /* TODO Retained a1 carrier requires an ABI adapter */
    return sub_80022EA4(a1 & 255u, moh_draft_missing_call(0x80022E8Cu));
}

uint32 sub_800266BC(uint32 environment)
{
    FUNCTION_MARKER(0x800266BCu, "SLUS_009.74");
    /* TODO Native setjmp continuation adapter; do not synthesize guest register slots */
    return moh_draft_missing_call(0x800266BCu, environment);
}

uint32 sub_80018FB8(void)
{
    FUNCTION_MARKER(0x80018FB8u, "SLUS_009.74");
    /* TODO Missing VSync adapter */
    uint32 result = moh_draft_missing_call(0x80015DB8u, 0xFFFFFFFFu) + 240u;
    w_u32(0x8002E054u, result);
    w_u32(0x8002E058u, 0u);
    return result;
}

uint32 sub_8001CAA8(uint32 index, uint32 callback)
{
    FUNCTION_MARKER(0x8001CAA8u, "SLUS_009.74");
    /* TODO Resolve callback table target */
    return moh_draft_indirect_call(r_u32(r_u32(0x8002F258u) + 8u), index, callback);
}

uint32 sub_8001CB3C(uint32 slot, uint32 callback)
{
    FUNCTION_MARKER(0x8001CB3Cu, "SLUS_009.74");
    return moh_draft_indirect_call(r_u32(r_u32(0x8002F258u) + 20u), slot, callback);
}

void sub_80036AF8(void)
{
    FUNCTION_MARKER(0x80036AF8u, "SLUS_009.74");
    sub_80036B20(r_u32(0x80039D50u), r_u32(0x80039D54u));
}

uint32 sub_80029420(uint32 a1)
{
    FUNCTION_MARKER(0x80029420u, "SLUS_009.74");
    w_u32(0x800302F0u, a1 != 1u);
    return 1u;
}

uint32 sub_8001D0F0(uint32 callback)
{
    FUNCTION_MARKER(0x8001D0F0u, "SLUS_009.74");
    return sub_8001CAD8(3u, callback);
}

uint32 sub_8002ACD0(uint32 callback)
{
    FUNCTION_MARKER(0x8002ACD0u, "SLUS_009.74");
    return sub_8001CAD8(4u, callback);
}

uint32 sub_8001D180(void)
{
    FUNCTION_MARKER(0x8001D180u, "SLUS_009.74");
    return sub_80022D34();
}

uint32 sub_800203F8(void)
{
    FUNCTION_MARKER(0x800203F8u, "SLUS_009.74");
    /* TODO Callee outside selected set */
    return moh_draft_missing_call(0x80021B80u, 0u);
}

uint32 sub_8001FC58(void)
{
    FUNCTION_MARKER(0x8001FC58u, "SLUS_009.74");
    /* TODO Callee outside selected set */
    return moh_draft_missing_call(0x80029460u, 1u);
}

void sub_80028BB0(uint32 a1, uint32 a2, uint32 a3)
{
    FUNCTION_MARKER(0x80028BB0u, "SLUS_009.74");
    w_u32(0x800CBCE8u, a1);
    w_u32(0x800CBD10u, a2);
    w_u32(0x800CBCE4u, a3);
}

uint32 sub_8001CBAC(uint32 a1)
{
    FUNCTION_MARKER(0x8001CBACu, "SLUS_009.74");
    uint32 address = r_u32(0x8002F260u);
    uint32 previous = r_u16(address);
    w_u16(address, a1);
    return previous;
}

uint32 sub_80029448(void)
{
    FUNCTION_MARKER(0x80029448u, "SLUS_009.74");
    return r_u32(0x800302F0u) != 1u;
}

uint32 sub_8001AD38(void)
{
    FUNCTION_MARKER(0x8001AD38u, "SLUS_009.74");
    xport_bios_exit_critical();
    return 0u;
}

uint32 sub_8001AF88(void)
{
    FUNCTION_MARKER(0x8001AF88u, "SLUS_009.74");
    return (uint32)xport_bios_enter_critical();
}

uint32 sub_80022E38(void)
{
    FUNCTION_MARKER(0x80022E38u, "SLUS_009.74");
    return r_u8(0x80034601u);
}

void sub_80022DFC(uint32 a1)
{
    FUNCTION_MARKER(0x80022DFCu, "SLUS_009.74");
    w_u32(0x800D0A74u, a1);
}

void sub_80022DF0(uint32 a1)
{
    FUNCTION_MARKER(0x80022DF0u, "SLUS_009.74");
    w_u32(0x800D0A70u, a1);
}

