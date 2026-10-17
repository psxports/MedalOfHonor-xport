#include "psx.h"
#include "game_draft_signatures.h"
#include <stdint.h>

/* Unverified draft from full original pseudocode */
uint32 sub_80026AD8(void)
{
    uint8 response[8];
    uint32 port, interrupt, count = 0u, index, error = 0u, command, status, code;
    w_u8(r_u32(0x80035308u), 1u);
    port = r_u32(0x8003530Cu);
    interrupt = r_u8(port) & 7u;
    if (!interrupt) return 0u;
    while (interrupt != (r_u8(port) & 7u)) interrupt = r_u8(port) & 7u;
    do
    {
        if (!(r_u8(r_u32(0x80035308u)) & 32u)) break;
        response[count++] = r_u8(r_u32(0x80035314u));
    } while (count < 8u);
    for (index = count; index < 8u; ++index) response[index] = 0u;
    w_u8(r_u32(0x80035308u), 1u);
    w_u8(r_u32(0x8003530Cu), 7u);
    w_u8(r_u32(0x80035318u), 7u);
    if (interrupt != 3u || r_u32(0x80035208u + 4u * r_u8(0x80035061u)))
    {
        if (!(r_u32(0x80035050u) & 16u) && (response[0] & 16u))
            w_u32(0x80035058u, r_u32(0x80035058u) + 1u);
        error = response[0] & 29u;
        w_u32(0x80035050u, response[0]);
        w_u32(0x80035054u, response[1]);
    }
    if (interrupt == 5u && r_s32(0x8003504Cu) >= 3)
    {
        moh_draft_missing_call(0x80015B28u, 0x800151A4u);
        if (r_s32(0x8003504Cu) >= 3)
        {
            command = r_u8(0x80035061u);
            status = r_u32(0x80035050u);
            code = r_u32(0x80035054u);
            moh_draft_missing_call(0x80015B28u, 0x800151B0u, r_u32(0x80035068u + 4u * command), status, code);
        }
    }
    switch (interrupt)
    {
    case 1u:
        if (error && count == 1u) error = 0u;
        w_u8(0x80035321u, error ? 5u : 1u);
        for (index = 0u; index < 8u; ++index) w_u8(0x800D0D2Cu + index, response[index]);
        w_u8(r_u32(0x80035308u), 0u);
        w_u8(r_u32(0x8003530Cu), 0u);
        return 4u;
    case 2u:
        w_u8(0x80035320u, error ? 5u : 2u);
        for (index = 0u; index < 8u; ++index) w_u8(0x800D0D24u + index, response[index]);
        return 2u;
    case 3u:
        if (error)
        {
            w_u8(0x80035320u, 5u);
            for (index = 0u; index < 8u; ++index) w_u8(0x800D0D24u + index, response[index]);
            return 2u;
        }
        if (r_u32(0x80035108u + 4u * r_u8(0x80035061u)))
        {
            w_u8(0x80035320u, 3u);
            for (index = 0u; index < 8u; ++index) w_u8(0x800D0D24u + index, response[index]);
            return 1u;
        }
        w_u8(0x80035320u, 2u);
        for (index = 0u; index < 8u; ++index) w_u8(0x800D0D24u + index, response[index]);
        return 2u;
    case 4u:
        w_u8(0x80035322u, 4u);
        w_u8(0x80035321u, r_u8(0x80035322u));
        for (index = 0u; index < 8u; ++index) w_u8(0x800D0D34u + index, response[index]);
        for (index = 0u; index < 8u; ++index) w_u8(0x800D0D2Cu + index, response[index]);
        return 4u;
    case 5u:
        w_u8(0x80035321u, 5u);
        w_u8(0x80035320u, r_u8(0x80035321u));
        for (index = 0u; index < 8u; ++index) w_u8(0x800D0D24u + index, response[index]);
        for (index = 0u; index < 8u; ++index) w_u8(0x800D0D2Cu + index, response[index]);
        return 6u;
    default:
        moh_draft_missing_call(0x800224B0u, 0x800151CCu);
        moh_draft_missing_call(0x80015B28u, 0x800151E0u, interrupt);
        return 0u;
    }
}

/* Unverified draft from full original pseudocode */
static uint32 moh_pad_wait_ack(uint32 port)
{
    uint32 timer = r_u16(0x1F801120u);
    uint32 irq = r_u32(0x800347A8u), status;
    if (!port) port = r_u32(0x800347ACu);
    w_u32(irq, 0xFFFFFF7Fu);
    status = r_u16(port + 4u);
    w_u32(0x800D0A94u, 60u);
    w_u32(0x800D0A90u, timer);
    while (status & 128u)
    {
        if (sub_800251F8()) return 0u;
        status = r_u16(r_u32(0x800347ACu) + 4u);
    }
    port = r_u32(0x800347ACu);
    w_u16(port + 10u, r_u16(port + 10u) | 16u);
    return 1u;
}

uint32 sub_80025528(uint32 owner)
{
    uint32 mode, left, response, result, port, buffer, index;
    uint32 offset = 0xFFFFFF10u, peer, peer_offset, pending, count, record = 0u;
    sint32 child = -1;
    if (r_u32(0x800347B8u)) moh_draft_indirect_call(r_u32(0x80034740u), owner);
    mode = r_u32(0x800347B8u);
    if (mode)
    {
        for (;;)
        {
            left = r_u32(0x800347B0u) - 1u;
            w_u32(0x800347B0u, left);
            if ((sint32)left <= 0) break;
            if (child >= 0)
            {
                buffer = r_u32(owner + 12u);
                moh_draft_indirect_call(r_u32(0x80034740u), buffer + offset);
            }
            response = moh_draft_indirect_call(r_u32(0x8003473Cu), owner, 1u) & 255u;
            result = sub_8002454C(owner, response);
            if ((sint32)result < 0) return result;
            if (!moh_pad_wait_ack(0u)) return 0xFFFFFFFDu;
            ++child;
            offset += 240u;
            if (child >= 4) break;
        }
    }
    peer = r_u32(0x80034774u) == 0u;
    left = r_u32(0x800347B0u);
    if ((sint32)left >= 2)
    {
        pending = 0x8003478Cu + 4u * peer;
        peer_offset = 240u * peer;
        for (;;)
        {
            count = r_u32(pending);
            if ((sint32)count < 0) break;
            if ((sint32)count > 0)
            {
                buffer = r_u32(r_u32(0x80034768u) + peer_offset + 12u);
                record = buffer + 240u * count - 240u;
                moh_draft_indirect_call(r_u32(0x80034754u), record);
            }
            count = r_u32(pending);
            if (count == 3u)
            {
                moh_draft_indirect_call(r_u32(0x80034754u), record - 240u);
                w_u32(pending, 1u);
            }
            else if (count == 4u) w_u32(pending, 3u);
            else if ((sint32)count >= 0 && (sint32)count < 2)
            {
                record = r_u32(0x80034768u) + peer_offset;
                moh_draft_indirect_call(r_u32(0x80034754u), record);
                moh_draft_indirect_call(r_u32(0x80034758u), record);
                w_u32(pending, 0xFFFFFFFFu);
            }
            response = moh_draft_indirect_call(r_u32(0x8003473Cu), owner, mode) & 255u;
            result = sub_80024328(owner, response);
            if ((sint32)result < 0) return result;
            if (!moh_pad_wait_ack(0u)) return 0xFFFFFFFDu;
            left = r_u32(0x800347B0u) - 1u;
            w_u32(0x800347B0u, left);
            if ((sint32)left < 2) break;
        }
    }
    left = r_u32(0x800347B0u) - 1u;
    w_u32(0x800347B0u, left);
    while ((sint32)left > 0)
    {
        response = moh_draft_indirect_call(r_u32(0x8003473Cu), owner, mode) & 255u;
        result = sub_80024328(owner, response);
        if ((sint32)result < 0) return result;
        port = r_u32(0x800347ACu);
        if (r_u16(port + 14u) != 34u && !moh_pad_wait_ack(port)) return 0xFFFFFFFDu;
        left = r_u32(0x800347B0u) - 1u;
        w_u32(0x800347B0u, left);
    }
    port = r_u32(0x800347ACu);
    while (!(r_u16(port + 4u) & 2u)) {}
    index = r_u8(owner + 68u);
    w_u8(owner + 68u, index + 1u);
    port = r_u32(0x800347ACu);
    buffer = r_u32(owner + 60u);
    response = r_u8(port);
    w_u8(buffer + index, response);
    moh_draft_indirect_call(r_u32(0x80034734u), 0u);
    return 0u;
}

/* Unverified draft from full original pseudocode */
uint32 sub_80027584(uint32 command, uint32 parameters, uint32 result_buffer, uint32 asynchronous)
{
    uint32 index, parameter_count, port, value, status;
    command &= 255u;
    if (r_s32(0x8003504Cu) >= 2)
        moh_draft_missing_call(0x80015B28u, 0x80015214u, r_u32(0x80035068u + 4u * command));
    if (r_u32(0x80035288u + 4u * command) && !parameters)
    {
        if (r_s32(0x8003504Cu) > 0)
            moh_draft_missing_call(0x80015B28u, 0x8001521Cu, r_u32(0x80035068u + 4u * command));
        return 0xFFFFFFFEu;
    }
    sub_8002703C(0u, 0u);
    if (command == 2u)
        for (index = 0u; index < 4u; ++index)
            w_u8(0x8003505Cu + index, r_u8(parameters + index));
    if (command == 14u) w_u8(0x80035060u, r_u8(parameters));
    w_u8(0x80035320u, 0u);
    if (r_u32(0x80035188u + 4u * command)) w_u8(0x80035321u, 0u);
    w_u8(r_u32(0x80035308u), 0u);
    parameter_count = 0x80035288u + 4u * command;
    index = 0u;
    if (r_s32(parameter_count) > 0)
    {
        do
        {
            port = r_u32(0x80035318u);
            value = r_u8(parameters + index);
            w_u8(port, value);
            value = r_u32(parameter_count);
            ++index;
        } while ((sint32)index < (sint32)value);
    }
    port = r_u32(0x80035314u);
    w_u8(0x80035061u, command);
    w_u8(port, command);
    if (asynchronous) return 0u;
    w_u32(0x800D0D3Cu, moh_draft_missing_call(0x80015DB8u, 0xFFFFFFFFu) + 960u);
    w_u32(0x800D0D40u, 0u);
    status = r_u8(0x80035320u);
    w_u32(0x800D0D44u, 0x8001522Cu);
    while (!status)
    {
        if (moh_draft_cd_timeout()) return 0xFFFFFFFFu;
        moh_draft_cd_dispatch(0x80027880u);
        status = r_u8(0x80035320u);
    }
    if (result_buffer)
        for (index = 0u; index < 8u; ++index)
            w_u8(result_buffer + index, r_u8(0x800D0D24u + index));
    return r_u8(0x80035320u) == 5u ? 0xFFFFFFFFu : 0u;
}

/* Unverified draft from full original pseudocode */
uint32 sub_8002307C(void)
{
    uint32 timeout = r_u32(0x80034618u), count, state, phase, command;
    uint8 parameters[8];
    if ((sint32)timeout > 0)
    {
        --timeout;
        w_u32(0x80034618u, timeout);
        if (!timeout) return moh_draft_missing_call(0x8002334Cu);
    }
    count = r_u32(0x80034614u);
    if ((sint32)count > 0) w_u32(0x80034614u, count - 1u);
    count = r_u32(0x80034620u);
    if ((sint32)count > 0) w_u32(0x80034620u, count - 1u);
    state = r_u32(0x800345F4u);
    if (state != 2u) goto dispatch_callback;
    phase = r_u32(0x800345F8u);
    if (phase == 11u) goto dispatch_callback;
    if (phase == 12u)
    {
        if (r_u32(0x80034728u))
        {
            timeout = r_u32(0x80034618u);
            parameters[0] = 0u;
            if ((sint32)timeout <= 0)
            {
                w_u32(0x800345F0u, 32u);
                /* TODO Bind the original addressable command parameter */
                sub_80022EA4(14u, moh_draft_local_address(parameters, sizeof(parameters)));
            }
            w_u32(0x80034728u, 0u);
        }
        else
        {
            if (r_s32(0x80034618u) <= 0)
            {
                w_u32(0x800345F0u, 32u);
                sub_80022EA4(1u, 0u);
            }
            w_u32(0x80034728u, 1u);
        }
        goto dispatch_callback;
    }
    if (phase == 13u)
    {
        timeout = r_u32(0x80034618u);
        w_u32(0x80034728u, 0u);
        if ((sint32)timeout <= 0) goto send_status;
    }
    else if (phase == 14u)
    {
        command = r_u32(0x800345FCu);
        if (command == 22u)
        {
            count = r_u32(0x8003460Cu);
            timeout = r_u32(0x80034618u);
            w_u32(0x8003460Cu, count + 1u);
            if ((sint32)timeout <= 0) goto send_status;
        }
        else if (command == 23u)
        {
            if (r_s32(0x80034618u) <= 0)
            {
                w_u32(0x800345F0u, 32u);
                sub_80022EA4(19u, 0u);
            }
        }
        else if (command == 21u || command == 24u)
        {
            if (r_s32(0x80034618u) <= 0) goto send_status;
        }
    }
    else if (phase == 15u)
    {
        if (!r_u32(0x80034614u)) goto command_complete;
    }
    else if (phase == 16u || phase == 17u)
    {
        if (r_u8(phase == 16u ? 0x80034608u : 0x8003460Au)) goto command_complete;
        if (r_s32(0x80034618u) <= 0) goto send_status;
    }
    goto dispatch_callback;
command_complete:
    w_u32(0x800345F4u, 1u);
    w_u32(0x800345F8u, 11u);
    goto dispatch_callback;
send_status:
    w_u32(0x800345F0u, 32u);
    sub_80022EA4(1u, 0u);
dispatch_callback:
    if (r_u32(0x800D0A70u) && r_u32(0x800345D4u))
        moh_draft_indirect_call(r_u32(0x800D0A70u));
    state = r_u32(0x800345F4u);
    if ((state == 1u && !r_u8(0x8003460Bu)) || state == 3u)
    {
        if (r_s32(0x80034618u) <= 0)
        {
            w_u32(0x800345F0u, 33u);
            return sub_80022EA4(1u, 0u);
        }
        return 33u;
    }
    return 3u;
}

/* Unverified draft from full original pseudocode */
uint32 sub_800236C8(uint32 event, uint32 result_buffer)
{
    uint32 phase = r_u32(0x800345F8u), command, result, count, callback;
    if (phase == 12u) w_u32(0x800345F8u, 13u);
    if ((event & 255u) != 2u)
    {
        result = r_u8(0x800345ECu) & 16u;
        if (result)
        {
            w_u32(0x800345F4u, 2u);
            w_u32(0x800345F8u, 12u);
        }
        else
        {
            phase = r_u32(0x800345F8u);
            result = 1u;
            if (phase - 16u < 2u)
            {
                w_u32(0x800345F4u, 1u);
                w_u32(0x800345F8u, 11u);
                result = 11u;
            }
        }
        return result;
    }
    phase = r_u32(0x800345F8u);
    if (phase == 13u)
    {
        if (r_u8(0x800345ECu) & 16u) return 14u;
        w_u32(0x800345F8u, 14u);
        count = r_u32(0x80034610u);
        w_u32(0x800345F4u, 2u);
        w_u32(0x800345FCu, 21u);
        w_u32(0x80034610u, count + 1u);
        return count + 1u;
    }
    if (phase == 14u)
    {
        command = r_u32(0x800345FCu);
        if (command == 21u)
        {
            if (!(r_u8(0x800345ECu) & 16u))
            {
                w_u32(0x800345F4u, 2u);
                w_u32(0x800345F8u, 14u);
                w_u32(0x800345FCu, 22u);
                w_u32(0x8003460Cu, 0u);
            }
            return 22u;
        }
        if (command == 22u)
        {
            if (!(r_u8(0x800345ECu) & 2u))
            {
                if (r_s32(0x8003460Cu) < 301) return 1u;
                callback = r_u32(0x800D0A7Cu);
                w_u32(0x800345F4u, 3u);
                if (callback) return moh_draft_indirect_call(r_u32(0x800D0A7Cu), 5u, result_buffer);
                return 3u;
            }
            result = 23u;
        }
        else if (command == 23u) result = 24u;
        else if (command == 24u)
        {
            result = r_u8(0x800345ECu);
            if (result != 2u) return result;
            callback = r_u32(0x800D0A7Cu);
            w_u32(0x800345F4u, 1u);
            w_u32(0x800345F8u, 11u);
            w_u32(0x800345FCu, 0u);
            if (callback) return moh_draft_indirect_call(r_u32(0x800D0A7Cu), 2u, result_buffer);
            return 11u;
        }
        else return 24u;
        w_u32(0x800345F4u, 2u);
        w_u32(0x800345F8u, 14u);
        w_u32(0x800345FCu, result);
        return result;
    }
    if (phase - 16u >= 2u) return 0u;
    result = r_u32(0x80034620u);
    if (result) return result;
    result = r_u8(0x800345ECu) & 2u;
    if (result) return result;
    callback = r_u32(0x800D0A78u);
    w_u32(0x800345F4u, 1u);
    w_u32(0x800345F8u, 11u);
    if (callback && r_u32(0x800345D4u))
        moh_draft_indirect_call(r_u32(0x800D0A78u), 5u, result_buffer);
    result = r_u32(0x800D0A74u);
    if (result)
    {
        result = r_u32(0x800345D4u);
        if (result) return moh_draft_indirect_call(r_u32(0x800D0A74u), 5u, result_buffer);
    }
    return result;
}

/* Unverified draft from full original pseudocode */
uint32 sub_80021444(uint32 mode, uint32 address, uint32 size)
{
    uint32 base, expected, actual, count;
    if (mode == 2u)
    {
        uint32 shift = r_u32(0x800302E4u);
        base = r_u32(0x800302BCu);
        actual = address >> (shift & 31u);
        w_u16(0x800302D4u, actual);
        w_u16(base + 422u, actual);
        return 0u;
    }
    if (mode == 0u || mode == 1u)
    {
        base = r_u32(0x800302BCu);
        expected = r_u16(0x800302D4u);
        actual = r_u16(base + 422u);
        w_u32(0x8003030Cu, mode == 0u);
        for (count = 1u; actual != expected; ++count)
        {
            if (count >= 0xF01u) return 0xFFFFFFFEu;
            actual = r_u16(base + 422u);
        }
        base = r_u32(0x800302BCu);
        actual = r_u16(base + 426u);
        w_u16(base + 426u, mode ? ((actual & 0xFFCFu) | 0x20u) : (actual | 0x30u));
        return 0u;
    }
    if (mode != 3u) return 0u;
    expected = r_u32(0x8003030Cu) == 1u ? 0x30u : 0x20u;
    base = r_u32(0x800302BCu);
    actual = r_u16(base + 426u) & 0x30u;
    for (count = 1u; actual != expected; ++count)
    {
        if (count >= 0xF01u) return 0xFFFFFFFEu;
        actual = r_u16(base + 426u) & 0x30u;
    }
    if (r_u32(0x8003030Cu) == 1u)
        moh_draft_missing_call(0x800218B0u);
    else
        sub_80021888();
    w_u32(0x80030310u, address);
    base = r_u32(0x800302C0u);
    address = r_u32(0x80030310u);
    count = (size >> 6) + ((size & 0x3Fu) != 0u);
    w_u32(0x80030314u, count);
    w_u32(base, address);
    count = r_u32(0x80030314u);
    base = r_u32(0x800302C4u);
    w_u32(base, (count << 16) | 0x10u);
    expected = r_u32(0x8003030Cu) == 1u ? 0x01000200u : 0x01000201u;
    base = r_u32(0x800302C8u);
    w_u32(base, expected);
    return 0u;
}

/* Unverified draft from full original pseudocode */
uint32 sub_8001804C(uint32 rectangle, uint32 color)
{
    sint32 width = r_s16(rectangle + 4u), height, limit;
    uint32 width_word = r_u16(rectangle + 4u), height_word, clamped_width, clamped_height;
    uint32 unaligned, status, port, extent;
    clamped_width = 0u;
    if (width >= 0)
    {
        limit = r_s16(0x8002DF1Cu) - 1;
        clamped_width = r_u16(0x8002DF1Cu) - 1u;
        if (limit >= width) clamped_width = width_word;
    }
    height = r_s16(rectangle + 6u);
    height_word = r_u16(rectangle + 6u);
    w_u16(rectangle + 4u, clamped_width);
    clamped_height = 0u;
    if (height >= 0)
    {
        limit = r_s16(0x8002DF1Eu) - 1;
        clamped_height = r_u16(0x8002DF1Eu) - 1u;
        if (limit >= height) clamped_height = height_word;
    }
    unaligned = r_u16(rectangle) & 63u;
    w_u16(rectangle + 6u, clamped_height);
    if (unaligned || (r_u16(rectangle + 4u) & 63u))
    {
        w_u32(0x800CA2B4u, 0x080CA2DCu);
        w_u32(0x800CA2C4u, 0xE6000000u);
        port = r_u32(0x8002E020u);
        w_u32(0x800CA2B8u, 0xE3000000u);
        w_u32(0x800CA2BCu, 0xE4FFFFFFu);
        w_u32(0x800CA2C0u, 0xE5000000u);
        status = r_u32(port);
        w_u32(0x800CA2CCu, (color & 0xFFFFFFu) | 0x60000000u);
        w_u32(0x800CA2C8u, (status & 0x7FFu) | ((color >> 31) << 10) | 0xE1000000u);
        w_u32(0x800CA2D0u, r_u32(rectangle));
        extent = r_u32(rectangle + 4u);
        w_u32(0x800CA2DCu, 0x03FFFFFFu);
        w_u32(0x800CA2D4u, extent);
        w_u32(0x800CA2E0u, sub_800187DC(3u) | 0xE3000000u);
        w_u32(0x800CA2E4u, sub_800187DC(4u) | 0xE4000000u);
        w_u32(0x800CA2E8u, sub_800187DC(5u) | 0xE5000000u);
    }
    else
    {
        w_u32(0x800CA2B4u, 0x05FFFFFFu);
        w_u32(0x800CA2B8u, 0xE6000000u);
        status = r_u32(r_u32(0x8002E020u));
        w_u32(0x800CA2C0u, (color & 0xFFFFFFu) | 0x02000000u);
        w_u32(0x800CA2BCu, (status & 0x7FFu) | ((color >> 31) << 10) | 0xE1000000u);
        w_u32(0x800CA2C4u, r_u32(rectangle));
        w_u32(0x800CA2C8u, r_u32(rectangle + 4u));
    }
    sub_80018794(0x800CA2B4u);
    return 0u;
}

/* Unverified draft from full original pseudocode */
uint32 sub_8001CC9C(void)
{
    uint32 pending, index, callback, timeout;
    uint32 status, mask, enabled, status_bits, mask_bits;
    if (!r_u16(0x8002E1D0u))
    {
        status = r_u32(0x8002F25Cu);
        moh_draft_missing_call(0x80015B28u, 0x80014B20u, (uint32)r_u16(status));
        return moh_native_irq_return();
    }
    status = r_u32(0x8002F25Cu);
    enabled = r_u16(0x8002E200u);
    w_u16(0x8002E1D2u, 1u);
    mask = r_u32(0x8002F260u);
    status_bits = r_u16(status);
    mask_bits = r_u16(mask);
    pending = status_bits & enabled & mask_bits;
    while (pending)
    {
        for (index = 0u; index < 11u && (pending & 0xFFFFu); ++index, pending >>= 1)
        {
            if (!(pending & 1u)) continue;
            status = r_u32(0x8002F25Cu);
            w_u16(status, ~(1u << index));
            callback = r_u32(0x8002E1D4u + 4u * index);
            if (callback) moh_draft_indirect_call(callback);
        }
        status = r_u32(0x8002F25Cu);
        enabled = r_u16(0x8002E200u);
        mask = r_u32(0x8002F260u);
        status_bits = r_u16(status);
        mask_bits = r_u16(mask);
        pending = status_bits & enabled & mask_bits;
    }
    status = r_u32(0x8002F25Cu);
    mask = r_u32(0x8002F260u);
    status_bits = r_u16(status);
    mask_bits = r_u16(mask);
    if (status_bits & mask_bits)
    {
        timeout = r_u32(0x8002F268u);
        w_u32(0x8002F268u, timeout + 1u);
        if ((sint32)timeout >= 2049)
        {
            status_bits = r_u16(status);
            mask_bits = r_u16(mask);
            moh_draft_missing_call(0x80015B28u, 0x80014B3Cu,
                status_bits, mask_bits);
            status = r_u32(0x8002F25Cu);
            w_u32(0x8002F268u, 0u);
            w_u16(status, 0u);
        }
    }
    else w_u32(0x8002F268u, 0u);
    w_u16(0x8002E1D2u, 0u);
    return moh_native_irq_return();
}

/* Unverified draft from full original pseudocode */
uint32 sub_80037A0C(uint32 first_sector, uint32 sector_count, uint32 filename)
{
    uint32 sector_index = 0u, offset, entry, extent, bytes, limit, cursor, count;
    uint32 byte2, byte3, byte4, byte5, byte10, byte11, byte12, byte13;
    char name[256];
    if ((sint32)r_u32(sector_count) <= 0) return 0u;
    do
    {
        sub_80037370(1u, r_u32(first_sector) + sector_index, 0x80037D90u);
        offset = 0u;
        if (r_u8(0x80037D90u))
        {
            do
            {
                entry = 0x80037D90u + offset;
                byte2 = r_u8(entry + 2u);
                byte3 = r_u8(entry + 3u);
                byte10 = r_u8(entry + 10u);
                byte4 = r_u8(entry + 4u);
                byte5 = r_u8(entry + 5u);
                byte11 = r_u8(entry + 11u);
                extent = byte2 + (byte3 << 8) + (byte4 << 16) + (byte5 << 24);
                byte12 = r_u8(entry + 12u);
                byte13 = r_u8(entry + 13u);
                limit = offset + r_u8(entry + 32u) + 32u;
                bytes = byte10 + (byte11 << 8) + (byte12 << 16) + (byte13 << 24);
                count = 0u;
                for (cursor = offset + 33u; (sint32)cursor <= (sint32)limit; ++cursor)
                    name[count++] = (char)r_u8(0x80037D90u + cursor);
                name[count] = 0;
                /* TODO Bind the addressable local string through the native adapter */
                if (!moh_draft_missing_call(0x8001ACC8u, moh_draft_local_address(name, sizeof(name)), filename))
                {
                    w_u32(first_sector, extent);
                    w_u32(sector_count, bytes >> 11);
                    return bytes;
                }
                offset += r_u8(entry);
            } while (r_u8(0x80037D90u + offset));
        }
        ++sector_index;
    } while ((sint32)sector_index < (sint32)r_u32(sector_count));
    return 0u;
}

/* Unverified draft from full original pseudocode */
uint32 sub_80016EA0(uint32 label, uint32 rectangle)
{
    uint32 level = r_u8(0x8002DF1Au), format;
    sint32 x, y, width, height, limit;
    if (level == 1u)
    {
        width = (sint16)r_u16(rectangle + 4u);
        limit = (sint16)r_u16(0x8002DF1Cu);
        if (limit < width) goto bad_rectangle;
        x = (sint16)r_u16(rectangle);
        if (limit < width + x) goto bad_rectangle;
        y = (sint16)r_u16(rectangle + 2u);
        limit = (sint16)r_u16(0x8002DF1Eu);
        if (limit < y) goto bad_rectangle;
        height = (sint16)r_u16(rectangle + 6u);
        if (limit < y + height || width <= 0 || x < 0 || y < 0 || height <= 0)
            goto bad_rectangle;
        return 0u;
    }
    if (level != 2u) return 2u;
    format = 0x8001488Cu;
    goto print_rectangle;
bad_rectangle:
    format = 0x8001486Cu;
print_rectangle:
    moh_draft_indirect_call(r_u32(0x8002DF14u), format, label);
    x = (sint16)r_u16(rectangle);
    y = (sint16)r_u16(rectangle + 2u);
    width = (sint16)r_u16(rectangle + 4u);
    height = (sint16)r_u16(rectangle + 6u);
    return moh_draft_indirect_call(r_u32(0x8002DF14u), 0x80014878u,
        (uint32)x, (uint32)y, (uint32)width, (uint32)height);
}

/* Unverified draft from full original pseudocode */
uint32 sub_8001ADD8(uint32 sector, uint32 output)
{
    sint32 frames = (sint32)(sector + 150u);
    sint32 seconds = frames / 75;
    sint32 minutes = seconds / 60;
    sint32 value = frames % 75;
    w_u8(output + 2u, (uint32)(16 * (value / 10) + value % 10));
    value = seconds % 60;
    w_u8(output + 1u, (uint32)(16 * (value / 10) + value % 10));
    w_u8(output, (uint32)(16 * (minutes / 10) + minutes % 10));
    return output;
}

/* Unverified draft from full original pseudocode */
uint32 sub_80027AEC(void)
{
    uint32 base = r_u32(0x8003531Cu);
    if (!r_u16(base + 440u) && !r_u16(base + 442u))
    {
        w_u16(base + 384u, 0x3FFFu);
        w_u16(base + 386u, 0x3FFFu);
        base = r_u32(0x8003531Cu);
    }
    w_u16(base + 432u, 0x3FFFu);
    w_u16(base + 434u, 0x3FFFu);
    w_u16(base + 426u, 0xC001u);
    w_u8(r_u32(0x80035308u), 2u);
    w_u8(r_u32(0x80035318u), 128u);
    w_u8(r_u32(0x8003530Cu), 0u);
    w_u8(r_u32(0x80035308u), 3u);
    w_u8(r_u32(0x80035314u), 128u);
    w_u8(r_u32(0x80035318u), 0u);
    w_u8(r_u32(0x8003530Cu), 32u);
    return 0u;
}

/* Unverified draft from full original pseudocode */
uint32 sub_80027DC8(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */


  char v4; 
  sint32 v5; 
  char v6; 
  sint32 v7; 
  sint32 v8; 
  sint32 v9; 
  sint32 result; 

  v4 = *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)))), 1u) & 3;
  while ( 1 )
  {
    v5 = sub_80026AD8();
    v6 = v5;
    v7 = v5 == 0;
    v8 = v5 & 4;
    if ( v7 )
      break;
    v7 = v8 == 0;
    v9 = v6 & 2;
    if ( !v7 )
    {
      if ( (*(sint32 *)moh_draft_guest_pointer(0x80035048u, sizeof(sint32))) )
        moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x80035048u, sizeof(sint32))), (unsigned char)(*(unsigned char *)moh_draft_guest_pointer(0x80035321u, sizeof(unsigned char))), -2146628308);
      v9 = v6 & 2;
    }
    if ( v9 )
    {
      if ( (*(sint32 *)moh_draft_guest_pointer(0x80035044u, sizeof(sint32))) )
        moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x80035044u, sizeof(sint32))), (unsigned char)(*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char))), -2146628316);
    }
  }
  result = (*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)));
  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)))), 1u) = v4;
  return result;
}

/* Unverified draft from full original pseudocode */
uint32 sub_80021388(void)
{
    uint32 base, status, count, callback;
    if (!r_u32(0x8003030Cu)) sub_800218D8();
    base = r_u32(0x800302BCu);
    w_u16(base + 426u, r_u16(base + 426u) & 0xFFCFu);
    status = r_u16(base + 426u) & 0x30u;
    for (count = 1u; status && count < 0xF01u; ++count)
        status = r_u16(base + 426u) & 0x30u;
    callback = r_u32(0x800302F4u);
    if (callback)
        return moh_draft_indirect_call(r_u32(0x800302F4u), 0xF0000000u);
    return moh_draft_missing_call(0x8002A8E0u, 0xF0000009u, 32u);
}

/* Unverified draft from full original pseudocode */
uint32 sub_800253B8(uint32 object)
{
    uint32 callback, buffer, mode = 0u, result, units;
    if (r_u32(0x800347B4u))
    {
        buffer = r_u32(object + 12u);
        callback = r_u32(0x8003474Cu);
        moh_draft_indirect_call(callback, buffer + 480u);
        buffer = r_u32(object + 12u);
        callback = r_u32(0x8003474Cu);
        moh_draft_indirect_call(callback, buffer + 720u);
    }
    if (!r_u8(object + 55u)) mode = r_u32(0x80034780u);
    result = sub_8002454C(object, mode);
    if ((sint32)result < 0) return result;
    units = 2u * (result & 15u);
    w_u32(0x800347B0u, units);
    if (!units) w_u32(0x800347B0u, 32u);
    return 0u;
}
