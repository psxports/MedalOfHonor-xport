#include "psx.h"
#include "game_draft_signatures.h"
#include "game_level_signatures.h"

uint32 level_sub_8005C210(uint32 object, uint32 data, uint32 flags)
{
    uint32 message;
    FUNCTION_MARKER(0x8005C210u, "LEVEL.BIN");
    message = level_sub_8005D148(1u, flags, r_u16(data), object);
    level_sub_800662CC(message, object, 0u, 0u);
    return data + 4u;
}

static uint32 script_stack(uint32 index)
{
    return 0x800AD260u + index * 8u;
}

static uint32 script_object(uint32 handle)
{
    return handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
}

static uint32 script_unsigned_float(uint32 value)
{
    uint32 converted;
    if ((sint32)value >= 0)
        return moh_draft_missing_call(0x8001D590u, value);
    converted = moh_draft_missing_call(0x8001D590u, (value & 1u) | (value >> 1));
    return moh_draft_missing_call(0x8001D930u, converted, converted);
}

static void script_undefined_result(uint32 opcode)
{
    /* TODO The original reads uninitialized result words for this first unsupported pair */
    moh_draft_fail_fast("Undefined arithmetic result in LEVEL script", 0x800672E0u);
}

void level_sub_800672E0(uint32 packet)
{
    uint32 selected, base, offset, opcode, top, lower, pair, pair_bytes = 0u;
    uint32 pair_guest = 0u, pair_is_guest = 0u;
    uint32 result_type = 0u, result_value = 0u, result_defined = 0u;
    uint32 source, kind, operand, actor, trigger, address, entry, value;
    uint32 a, b, supported, comparison, callback, target;

    selected = moh_draft_missing_call(0x8006602Cu,
        script_object(r_u32(packet + 4u)), r_u32(packet + 16u));
    if (selected == 0xFFFFFFFFu) return;
    w_u32(0x8009D5A8u, 0xFFFFFFFFu);
    base = r_u32(0x8009D5ACu);
    base += r_u32(base + selected * 4u + 8u);
    offset = r_u32(packet) - base;
    for (;;) {
        top = r_u32(0x8009D5A8u);
        opcode = (uint32)r_u8(base + offset++);
        switch (opcode) {
        case 0u:
            break;
        case 1u:
            if ((sint32)top >= 0) w_u32(0x8009D5A8u, top - 1u);
            break;
        case 2u:
            w_u32(0x8009D5A8u, top + 1u);
            source = (uint32)r_u8(base + offset++);
            if (source - 0xF4u < 12u) {
                kind = (uint32)r_u8(base + offset++);
                if (kind - 0xF7u < 3u || kind - 0xFDu < 3u) {
                    offset = moh_draft_missing_call(0x8006949Cu, offset, 2u);
                    pair_guest = (uint32)r_u8(base + offset);
                    pair_is_guest = 1u;
                    /* TODO Preserve the original byte-to-low-guest-pointer indirection */
                    operand = (uint32)r_u16(pair_guest);
                    offset += 2u;
                } else {
                    operand = (uint32)r_u8(base + offset++);
                }
                actor = 0u;
                trigger = 0u;
                if (source >= 0xFAu) {
                    if (kind - 0xF7u < 3u || kind - 0xFDu < 3u) {
                        if (source >= 0xFDu)
                            actor = script_object(r_u32(packet + 4u + (source - 0xFDu) * 4u));
                        if (source == 0xFDu) trigger = r_u32(packet + 16u);
                        if (source == 0xFEu) trigger = r_u32(packet + 20u);
                    } else {
                        if (source <= 0xFCu)
                            actor = script_object(r_u32(packet + 4u + (source - 0xFAu) * 4u));
                        if (source == 0xFAu) trigger = r_u32(packet + 16u);
                        if (source == 0xFBu) trigger = r_u32(packet + 20u);
                    }
                } else {
                    if (kind - 0xF7u < 3u || kind - 0xFDu < 3u) {
                        if (source >= 0xF7u)
                            actor = script_object(r_u32(packet + 4u + (source - 0xF7u) * 4u));
                        if (source == 0xF7u) trigger = r_u32(packet + 16u);
                        if (source == 0xF8u) trigger = r_u32(packet + 20u);
                    } else {
                        if (source <= 0xF6u)
                            actor = script_object(r_u32(packet + 4u + (source - 0xF4u) * 4u));
                        if (source == 0xF4u) trigger = r_u32(packet + 16u);
                        if (source == 0xF5u) trigger = r_u32(packet + 20u);
                    }
                }
                if (kind == 16u) {
                    entry = script_stack(r_u32(0x8009D5A8u));
                    w_u32(entry, kind);
                    w_u32(entry + 4u, source - 0xFAu < 6u ? actor : trigger);
                } else {
                    moh_draft_missing_call(0x80065F34u, actor, trigger, operand);
                }
                break;
            }
            if (source - 0xF0u < 4u) {
                kind = (uint32)r_u8(base + offset++);
                actor = script_object(r_u32(packet + 4u));
                trigger = r_u32(packet + 16u);
                if (source == 0xF1u || source == 0xF3u) {
                    offset = moh_draft_missing_call(0x8006949Cu, offset, 2u);
                    pair_guest = (uint32)r_u8(base + offset);
                    pair_is_guest = 1u;
                    /* TODO Preserve the original byte-to-low-guest-pointer indirection */
                    operand = (uint32)r_u16(pair_guest);
                    offset += 2u;
                } else {
                    operand = (uint32)r_u8(base + offset++);
                }
                if (source < 0xF2u)
                    address = moh_draft_missing_call(0x80066000u, actor, trigger, operand);
                else
                    address = base + r_u32(base) * 4u + 4u + operand;
                if (kind && kind <= 9u) {
                    entry = script_stack(r_u32(0x8009D5A8u));
                    w_u32(entry, 11u);
                    value = (uint32)r_u8(address);
                    if (kind < 9u) value = (value >> ((kind - 1u) & 31u)) & 1u;
                    w_u32(entry + 4u, value);
                }
                break;
            }
            if (source == 9u) {
                address = base + offset;
                moh_draft_missing_call(0x80064858u, 0x8003CC54u, (uint32)r_u8(address));
                entry = script_stack(r_u32(0x8009D5A8u));
                w_u32(entry, 11u);
                w_u32(entry + 4u, (uint32)r_u8(address));
                ++offset;
            } else if (source >= 10u && source <= 14u) {
                entry = script_stack(r_u32(0x8009D5A8u));
                w_u32(entry, source);
                offset = moh_draft_missing_call(0x8006949Cu, offset, 4u);
                value = r_u32(base + offset);
                offset += 4u;
                entry = script_stack(r_u32(0x8009D5A8u));
                w_u32(entry + 4u, value);
            } else if (source == 15u) {
                address = base + offset++;
                entry = script_stack(r_u32(0x8009D5A8u));
                w_u32(entry, 15u);
                value = (uint32)r_u8(address);
                w_u32(entry + 4u, base + offset);
                offset += value + 1u;
            } else if (source == 16u) {
                entry = script_stack(r_u32(0x8009D5A8u));
                w_u32(entry, 16u);
                offset = moh_draft_missing_call(0x8006949Cu, offset, 4u);
                address = base + offset;
                offset += 4u;
                w_u32(script_stack(r_u32(0x8009D5A8u)) + 4u, address);
            } else {
                return;
            }
            break;
        case 3u:
            source = (uint32)r_u8(base + offset++);
            if (source - 0xF0u < 4u) {
                kind = (uint32)r_u8(base + offset++);
                actor = script_object(r_u32(packet + 4u));
                trigger = r_u32(packet + 16u);
                if (source == 0xF1u || source == 0xF3u) {
                    offset = moh_draft_missing_call(0x8006949Cu, offset, 2u);
                    pair_guest = (uint32)r_u8(base + offset);
                    pair_is_guest = 1u;
                    /* TODO Preserve the original byte-to-low-guest-pointer indirection */
                    operand = (uint32)r_u16(pair_guest);
                    offset += 2u;
                } else {
                    operand = (uint32)r_u8(base + offset++);
                }
                if (source < 0xF2u)
                    address = moh_draft_missing_call(0x80066000u, actor, trigger, operand);
                else
                    address = base + r_u32(base) * 4u + 4u + operand;
                if (kind && kind < 9u) {
                    value = r_u32(script_stack(r_u32(0x8009D5A8u)) + 4u);
                    operand = 1u << ((kind - 1u) & 31u);
                    if (value) w_u8(address, (uint8)((uint32)r_u8(address) | operand));
                    else w_u8(address, (uint8)((uint32)r_u8(address) & ~operand));
                } else if (kind == 9u) {
                    entry = script_stack(r_u32(0x8009D5A8u));
                    value = r_u32(entry);
                    if (value == 10u || value == 11u)
                        w_u8(address, r_u8(entry + 4u));
                    else if (value == 14u) {
                        value = moh_draft_missing_call(0x8001D340u, r_u32(entry + 4u));
                        w_u8(address, (uint8)value);
                    }
                }
            }
            w_u32(0x8009D5A8u, r_u32(0x8009D5A8u) - 1u);
            break;
        case 5u:
            lower = top - 1u;
            w_u32(0x8009D5A8u, lower);
            w_u32(script_stack(lower), 11u);
            value = 0u;
            if (r_u32(script_stack(lower) + 4u))
                value = r_u32(script_stack(top) + 4u) != 0u;
            w_u32(script_stack(lower) + 4u, value);
            break;
        case 6u:
            lower = top - 1u;
            w_u32(script_stack(lower), 11u);
            value = r_u32(script_stack(lower) + 4u) != 0u || r_u32(script_stack(top) + 4u) != 0u;
            w_u32(0x8009D5A8u, lower);
            w_u32(script_stack(lower) + 4u, value);
            break;
        case 7u:
            entry = script_stack(top);
            value = r_u32(entry + 4u);
            w_u32(entry, 11u);
            w_u32(entry + 4u, value == 0u);
            break;
        case 4u:
        case 8u:
        case 9u:
        case 10u:
        case 11u:
        case 12u:
        case 13u:
        case 14u:
        case 15u:
            lower = top - 1u;
            address = script_stack(top);
            entry = script_stack(lower);
            pair_bytes = (uint32)r_u8(entry) | ((uint32)r_u8(address) << 8);
            pair = pair_is_guest ? (uint32)r_u16(pair_guest) : pair_bytes;
            value = 0u;
            supported = 0u;
            a = r_u32(address + 4u);
            b = r_u32(entry + 4u);
            if (pair == 0x0A0Au || pair == 0x0A0Bu || pair == 0x0B0Au || pair == 0x0B0Bu) {
                if (opcode >= 12u) {
                    result_type = pair == 0x0B0Bu ? 11u : 10u;
                    if (opcode == 12u) result_value = a + b;
                    else if (opcode == 13u) result_value = a - b;
                    else if (opcode == 14u) result_value = a * b;
                    else if (pair == 0x0B0Bu) {
                        if (!b) moh_draft_fail_fast("Script unsigned division by zero", 0x800687B4u);
                        result_value = a / b;
                    } else {
                        if (!b || (a == 0x80000000u && b == 0xFFFFFFFFu)) {
                            moh_draft_fail_fast("Script signed division trap", b ? 0x80068764u : 0x8006874Cu);
                        }
                        result_value = (uint32)((sint32)a / (sint32)b);
                    }
                    result_defined = 1u;
                } else if (opcode == 4u) value = a == b;
                else if (pair == 0x0B0Bu) {
                    if (opcode == 8u) value = a < b;
                    if (opcode == 9u) value = a > b;
                    if (opcode == 10u) value = a <= b;
                    if (opcode == 11u) value = a >= b;
                } else {
                    if (opcode == 8u) value = (sint32)a < (sint32)b;
                    if (opcode == 9u) value = (sint32)a > (sint32)b;
                    if (opcode == 10u) value = (sint32)a <= (sint32)b;
                    if (opcode == 11u) value = (sint32)a >= (sint32)b;
                }
            } else {
                if (pair == 0x0A0Eu) {
                    a = moh_draft_missing_call(0x8001D590u, a);
                    b = r_u32(entry + 4u);
                    supported = 1u;
                } else if (pair == 0x0B0Eu) {
                    a = script_unsigned_float(a);
                    b = r_u32(script_stack(r_u32(0x8009D5A8u) - 1u) + 4u);
                    supported = 1u;
                } else if (pair == 0x0E0Au) {
                    b = moh_draft_missing_call(0x8001D590u, b);
                    a = r_u32(address + 4u);
                    supported = 1u;
                } else if (pair == 0x0E0Bu) {
                    b = script_unsigned_float(b);
                    a = r_u32(address + 4u);
                    supported = 1u;
                } else if (pair == 0x0E0Eu) {
                    supported = 1u;
                }
                if (supported) {
                    if (opcode >= 12u) {
                        result_type = 14u;
                        if (opcode == 12u) target = 0x8001D930u;
                        else if (opcode == 13u) target = 0x8001D3F0u;
                        else if (opcode == 14u) target = 0x8001D450u;
                        else target = 0x8001D1F0u;
                        result_value = moh_draft_missing_call(target, a, b);
                        result_defined = 1u;
                    } else {
                        if (opcode == 4u) target = 0x8001D7B0u;
                        else if (opcode == 8u) target = 0x8001D880u;
                        else if (opcode == 9u) target = 0x8001D7E0u;
                        else if (opcode == 10u) target = 0x8001D700u;
                        else target = 0x8001D660u;
                        comparison = moh_draft_missing_call(target, a, b);
                        if (opcode == 4u) value = comparison == 0u;
                        if (opcode == 8u) value = (sint32)comparison < 0;
                        if (opcode == 9u) value = (sint32)comparison > 0;
                        if (opcode == 10u) value = (sint32)comparison <= 0;
                        if (opcode == 11u) value = (sint32)comparison >= 0;
                    }
                } else if (opcode == 4u && pair == 0x0F0Fu) {
                    value = moh_draft_missing_call(0x8001ACC8u, a, b) == 0u;
                } else if (opcode == 4u && pair == 0x1010u) {
                    value = a == b;
                }
            }
            top = r_u32(0x8009D5A8u) - 1u;
            w_u32(0x8009D5A8u, top);
            entry = script_stack(top);
            if (opcode >= 12u) {
                if (!result_defined) script_undefined_result(opcode);
                /* TODO Unsupported arithmetic pairs reuse the previous original result words */
                w_u32(entry, result_type);
                w_u32(entry + 4u, result_value);
            } else {
                w_u32(entry, 11u);
                w_u32(entry + 4u, value);
            }
            break;
        case 16u:
            offset = moh_draft_missing_call(0x8006949Cu, offset, 2u);
            address = base + offset;
            moh_draft_missing_call(0x80064858u, 0x8003CCB0u,
                (uint32)r_u16(address), r_u32(script_stack(r_u32(0x8009D5A8u)) + 4u));
            top = r_u32(0x8009D5A8u);
            value = r_u32(script_stack(top) + 4u);
            w_u32(0x8009D5A8u, top - 1u);
            if (!value) offset = (uint32)r_u16(address);
            else offset += 2u;
            break;
        case 17u:
            offset = moh_draft_missing_call(0x8006949Cu, offset, 2u);
            address = base + offset;
            moh_draft_missing_call(0x80064858u, 0x8003CC54u, (uint32)r_u16(address));
            offset = (uint32)r_u16(address);
            break;
        case 18u:
            value = (uint32)r_u8(base + offset++);
            callback = r_u32(0x8009D5C0u + value * 4u);
            /* TODO Callback retention of the packet view depends on the selected handler */
            moh_draft_missing_call(callback, packet + 4u, base, selected);
            break;
        case 19u:
            offset = moh_draft_missing_call(0x8006949Cu, offset, 2u);
            value = (uint32)r_u16(base + offset);
            offset += 2u;
            callback = r_u32(0x8009D5C0u + value * 4u);
            moh_draft_missing_call(callback, packet + 4u, base, selected);
            break;
        case 20u:
            return;
        case 21u:
            top = r_u32(0x8009D5A8u) + 1u;
            w_u32(0x8009D5A8u, top);
            entry = script_stack(top);
            w_u32(entry, 11u);
            value = (uint32)r_u8(base + r_u32(base) * 4u + 4u);
            w_u32(entry + 4u, value == 0u);
            break;
        case 22u:
            w_u8(base + r_u32(base) * 4u + 4u, 1u);
            break;
        case 23u:
            kind = (uint32)r_u8(base + offset++);
            if (kind == 9u) {
                value = (uint32)r_u8(base + offset++);
            } else if (kind == 10u || kind == 11u) {
                offset = moh_draft_missing_call(0x8006949Cu, offset, 4u);
                value = (uint32)r_u8(base + offset);
                offset += 4u;
            } else {
                moh_draft_missing_call(0x80015B28u, 0x8003CC58u, kind);
                return;
            }
            moh_draft_missing_call(0x80064858u, 0x8003CC54u, value);
            break;
        default:
            moh_draft_missing_call(0x80015B28u, 0x8003CCC0u, opcode);
            return;
        }
    }
}
