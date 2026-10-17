#include "game_level2p_signatures.h"
#include "xport_trace.h"

typedef struct
{
    uint32 kind;
    uint32 other_owner;
    uint32 other_instance;
    uint32 category;
    uint32 handler_index;
} MOH_CALLBACK_FILTER;

static uint32 moh_callback_object(uint32 handle)
{
    return handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
}

uint32 sub_8001552C(void)
{
    uint32 state;
    FUNCTION_MARKER(0x8001552C, "SLUS_009.74");
    state = r_u32(0x800CA2A4) * 0x41C64E6D + 0x3039;
    w_u32(0x800CA2A4, state);
    return (state >> 16) & 0x7FFF;
}

void sub_8001C6D8(uint32 distance, uint32 divisor)
{
    uint32 numerator = 0 - ((distance * 5) << 6);
    FUNCTION_MARKER(0x8001C6D8, "SLUS_009.74");
    if (!divisor)
    {
        moh_draft_fail_fast("Original fog division BREAK 0x1C00", 0x8001C6F4);
        return;
    }
    if (divisor == 0xFFFFFFFF && numerator == 0x80000000)
    {
        moh_draft_fail_fast("Original fog division BREAK 0x1800", 0x8001C70C);
        return;
    }
    sub_8001C714((uint32)((sint32)numerator / (sint32)divisor));
}

uint32 level2p_sub_80061A70(uint32 event, uint32 owner, uint32 instance, uint32 filter, uint32 other_owner, uint32 other_instance, uint32 category, uint32 code, uint32 handler_id)
{
    uint32 data, entry, matches, current_filter, table, next;
    FUNCTION_MARKER(0x80061A70, "LEVEL2P.BIN");
    data = r_u32(0x8009938C);
    if (data ? (sint32)event >= r_s32(data) : (sint32)event >= 0)
    {
        moh_draft_missing_call(0x80015B28, 0x8003C8B0, event);
        return 0;
    }
    if (!filter) level2p_sub_80061D58(event, owner, instance, 0, 0, 0, 0);
    else
    {
        entry = r_u32(r_u32(0x80099390) + (event << 2));
        while (entry)
        {
            matches = 0;
            /* TODO Addressable owner arguments use the permitted missing local adapter */
            if (owner) matches = level2p_sub_80068988(entry, moh_draft_local_address(&owner, sizeof(owner)));
            if (!matches && instance) matches = instance == r_u32(entry + 4);
            if (matches)
            {
                current_filter = r_u32(entry + 8);
                if (!current_filter)
                {
                    w_u32(entry + 8, filter);
                    level2p_sub_80068A04(entry + 0xC, other_owner);
                    w_u32(entry + 0x10, other_instance);
                    w_u32(entry + 0x14, category);
                    w_u32(entry + 0x18, code);
                    w_u32(entry + 0x1C, handler_id);
                    return 1;
                }
                if (current_filter == filter)
                {
                    switch (filter)
                    {
                    case 0: break;
                    case 1:
                    case 2:
                        if (r_u32(entry + 0x14) == category) return 1;
                        break;
                    case 3:
                    case 4:
                        if (other_owner && level2p_sub_80068988(entry + 0xC,
                            moh_draft_local_address(&other_owner, sizeof(other_owner)))) return 1;
                        if (other_instance && other_instance == r_u32(entry + 0x10)) return 1;
                        break;
                    case 5:
                        if (r_u32(entry + 0x14) != category) break;
                        if (other_owner && level2p_sub_80068988(entry + 0xC,
                            moh_draft_local_address(&other_owner, sizeof(other_owner)))) return 1;
                        if (other_instance && other_instance == r_u32(entry + 0x10)) return 1;
                        break;
                    default:
                        moh_draft_missing_call(0x80015B28, 0x8003C8DC, filter);
                        return 0;
                    }
                }
            }
            entry = r_u32(entry + 0x20);
        }
    }
    entry = level2p_sub_800619D8();
    if (!entry)
    {
        moh_draft_missing_call(0x80015B28, 0x8003C918);
        return 0;
    }
    level2p_sub_80068A04(entry, owner);
    w_u32(entry + 4, instance);
    w_u32(entry + 8, filter);
    level2p_sub_80068A04(entry + 0xC, other_owner);
    table = r_u32(0x80099390);
    w_u32(entry + 0x10, other_instance);
    w_u32(entry + 0x14, category);
    w_u32(entry + 0x18, code);
    w_u32(entry + 0x1C, handler_id);
    next = r_u32(table + (event << 2));
    w_u32(entry + 0x20, next);
    w_u32(table + (event << 2), entry);
    return 1;
}

uint32 level2p_sub_80061D58(uint32 event, uint32 owner, uint32 instance, uint32 filter, uint32 other_owner, uint32 other_instance, uint32 category)
{
    uint32 data, entry, next, previous = 0, removed = 0, matches, erase;
    FUNCTION_MARKER(0x80061D58, "LEVEL2P.BIN");
    data = r_u32(0x8009938C);
    if (data ? (sint32)event >= r_s32(data) : (sint32)event >= 0)
    {
        moh_draft_missing_call(0x80015B28, 0x8003C968, event);
        return 0;
    }
    entry = r_u32(r_u32(0x80099390) + (event << 2));
    while (entry)
    {
        next = r_u32(entry + 0x20);
        matches = 0;
        if (owner) matches = level2p_sub_80068988(entry, moh_draft_local_address(&owner, sizeof(owner)));
        if (!matches && instance) matches = instance == r_u32(entry + 4);
        erase = 0;
        if (matches)
        {
            if (!filter) erase = 1;
            else if (r_u32(entry + 8) == filter)
            {
                switch (filter)
                {
                case 0: break;
                case 1:
                case 2: erase = r_u32(entry + 0x14) == category; break;
                case 3:
                case 4:
                    if (other_owner) erase = level2p_sub_80068988(entry + 0xC,
                        moh_draft_local_address(&other_owner, sizeof(other_owner)));
                    if (!erase && other_instance) erase = other_instance == r_u32(entry + 0x10);
                    break;
                case 5:
                    if (r_u32(entry + 0x14) != category) break;
                    if (other_owner) erase = level2p_sub_80068988(entry + 0xC,
                        moh_draft_local_address(&other_owner, sizeof(other_owner)));
                    if (!erase && other_instance) erase = other_instance == r_u32(entry + 0x10);
                    break;
                default:
                    moh_draft_missing_call(0x80015B28, 0x8003C994, filter);
                    return 0;
                }
            }
        }
        if (erase)
        {
            removed = 1;
            if (!previous) w_u32(r_u32(0x80099390) + (event << 2), next);
            else w_u32(previous + 0x20, next);
            level2p_sub_80061A5C(entry);
            if (other_owner || other_instance) return 1;
        }
        else previous = entry;
        entry = next;
    }
    return removed;
}

static uint32 moh_callback_value(uint32 index)
{
    return r_u32(0x800ABAFC + (index << 3));
}

static void moh_callback_push_value(uint32 value)
{
    uint32 index = r_u32(0x80099388) + 1;
    w_u32(0x80099388, index);
    w_u32(0x800ABAFC + (index << 3), value);
}

static uint32 moh_callback_filter(MOH_CALLBACK_FILTER *filter, uint32 diagnostic)
{
    uint32 index = r_u32(0x80099388), remaining;
    remaining = index - 2;
    w_u32(0x80099388, index - 1);
    w_u32(0x80099388, remaining);
    filter->kind = moh_callback_value(index - 1);
    filter->handler_index = moh_callback_value(index);
    filter->other_owner = 0;
    filter->other_instance = 0;
    filter->category = 0xFFFFFFFF;
    switch (filter->kind)
    {
    case 0: return 1;
    case 1:
    case 2:
        remaining = r_u32(0x80099388);
        filter->category = moh_callback_value(remaining);
        w_u32(0x80099388, remaining - 1);
        return 1;
    case 3:
    case 4:
        remaining = r_u32(0x80099388);
        filter->other_instance = moh_callback_value(remaining);
        filter->other_owner = moh_callback_value(remaining - 1);
        w_u32(0x80099388, remaining - 1);
        w_u32(0x80099388, remaining - 2);
        return 1;
    case 5:
        remaining = r_u32(0x80099388);
        filter->other_instance = moh_callback_value(remaining);
        filter->other_owner = moh_callback_value(remaining - 1);
        filter->category = moh_callback_value(remaining - 2);
        w_u32(0x80099388, remaining - 1);
        w_u32(0x80099388, remaining - 2);
        w_u32(0x80099388, remaining - 3);
        return 1;
    default:
        moh_draft_missing_call(0x80015B28, diagnostic, filter->kind);
        return 0;
    }
}

void level2p_sub_80064658(uint32 context, uint32 handlers, uint32 handler_id)
{
    MOH_CALLBACK_FILTER filter;
    uint32 record, owner, event, code, instance;
    FUNCTION_MARKER(0x80064658, "LEVEL2P.BIN");
    if (!moh_callback_filter(&filter, 0x8003CC70)) return;
    record = level2p_sub_8006460C(filter.handler_index, handlers);
    if (!record) return;
    owner = moh_callback_object(r_u32(context));
    event = r_u16(record);
    code = handlers + r_u16(record + 2);
    instance = r_u32(context + 0xC);
    level2p_sub_80061A70(event, owner, instance, filter.kind, filter.other_owner,
        filter.other_instance, filter.category, code, handler_id);
}

void level2p_sub_80064858(uint32 context, uint32 handlers, uint32 handler_id)
{
    MOH_CALLBACK_FILTER filter;
    uint32 record, owner, event, instance;
    FUNCTION_MARKER(0x80064858, "LEVEL2P.BIN");
    if (!moh_callback_filter(&filter, 0x8003CCC8)) return;
    record = level2p_sub_8006460C(filter.handler_index, handlers);
    if (!record) return;
    owner = moh_callback_object(r_u32(context));
    event = r_u16(record);
    instance = r_u32(context + 0xC);
    level2p_sub_80061D58(event, owner, instance, filter.kind, filter.other_owner,
        filter.other_instance, filter.category);
}

void level2p_sub_80064A34(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, handle, event, owner, instance;
    FUNCTION_MARKER(0x80064A34, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 1);
    handle = r_u32(context);
    event = moh_callback_value(index);
    owner = moh_callback_object(handle);
    instance = r_u32(context + 0xC);
    level2p_sub_80061FE4(event, owner, instance, 0, 0);
}

void level2p_sub_80064AAC(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, selected, event, receiver = 0, owner, instance;
    FUNCTION_MARKER(0x80064AAC, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 2);
    w_u32(0x80099388, index - 3);
    selected = moh_callback_value(index - 2);
    event = moh_callback_value(index);
    if (selected) receiver = r_u32(selected + 0x1C);
    owner = moh_callback_object(r_u32(context));
    instance = r_u32(context + 0xC);
    level2p_sub_80061FE4(event, owner, instance, 0, receiver);
}

void level2p_sub_80064B4C(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, handle, flags, owner;
    FUNCTION_MARKER(0x80064B4C, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 1);
    handle = r_u32(context);
    flags = moh_callback_value(index);
    owner = moh_callback_object(handle);
    level2p_sub_8006123C(flags, owner);
}

void level2p_sub_80064BB8(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, first, second;
    FUNCTION_MARKER(0x80064BB8, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 1);
    w_u32(0x80099388, index - 2);
    first = moh_callback_value(index);
    second = moh_callback_value(index - 1);
    level2p_sub_800529C8(first, second);
}

void level2p_sub_80064C10(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, owner;
    FUNCTION_MARKER(0x80064C10, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    owner = moh_callback_value(index);
    w_u32(0x80099388, index - 1);
    if (owner) level2p_sub_800529FC(owner);
}

void level2p_sub_80064C58(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, handle, command, owner, instance;
    FUNCTION_MARKER(0x80064C58, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 1);
    handle = r_u32(context);
    command = moh_callback_value(index);
    owner = moh_callback_object(handle);
    instance = r_u32(context + 0xC);
    level2p_sub_80052A94(owner, instance, command);
}

void level2p_sub_80064CCC(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, values[7], part, handle, owner, instance, result;
    FUNCTION_MARKER(0x80064CCC, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    for (part = 1; part <= 7; ++part) w_u32(0x80099388, index - part);
    for (part = 0; part < 6; ++part) values[part] = moh_callback_value(index - part);
    handle = r_u32(context);
    values[6] = moh_callback_value(index - 6);
    owner = moh_callback_object(handle);
    instance = r_u32(context + 0xC);
    result = level2p_sub_80052F54(owner, instance, values[0], values[1], values[2],
        values[3], values[4], values[5], values[6]);
    if (values[0] == 3) moh_callback_push_value(result);
}

void level2p_sub_80064E08(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, channel;
    FUNCTION_MARKER(0x80064E08, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    channel = moh_callback_value(index);
    w_u32(0x80099388, index - 1);
    level2p_sub_80052FC4(channel);
}

void level2p_sub_80064E48(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, command, value;
    FUNCTION_MARKER(0x80064E48, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 1);
    w_u32(0x80099388, index - 2);
    command = moh_callback_value(index - 1);
    value = moh_callback_value(index);
    level2p_sub_80052A1C(command, value);
}

void level2p_sub_80064EA0(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, first, second, selector, result;
    FUNCTION_MARKER(0x80064EA0, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 1);
    w_u32(0x80099388, index - 2);
    first = moh_callback_value(index - 2);
    second = moh_callback_value(index - 1);
    selector = moh_callback_value(index);
    w_u32(0x80099388, index - 3);
    result = level2p_sub_80053274(first, second, selector);
    moh_callback_push_value(result);
}

void level2p_sub_80064F30(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, id, text_id;
    FUNCTION_MARKER(0x80064F30, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 1);
    w_u32(0x80099388, index - 2);
    id = moh_callback_value(index);
    text_id = moh_callback_value(index - 1);
    level2p_sub_800532AC(id, text_id);
}

void level2p_sub_80064F88(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, id;
    FUNCTION_MARKER(0x80064F88, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    id = moh_callback_value(index);
    w_u32(0x80099388, index - 1);
    level2p_sub_8005338C(id);
}

void level2p_sub_80064FC8(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, id;
    FUNCTION_MARKER(0x80064FC8, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    id = moh_callback_value(index);
    w_u32(0x80099388, index - 1);
    level2p_sub_80053430(id);
}

void level2p_sub_80065008(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, id, result;
    FUNCTION_MARKER(0x80065008, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    id = moh_callback_value(index);
    w_u32(0x80099388, index - 1);
    result = level2p_sub_800534CC(id);
    moh_callback_push_value(result);
}

void level2p_sub_80065070(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, id;
    FUNCTION_MARKER(0x80065070, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    id = moh_callback_value(index);
    w_u32(0x80099388, index - 1);
    level2p_sub_8005348C(id);
}

void level2p_sub_800650B0(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, first, second;
    FUNCTION_MARKER(0x800650B0, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 1);
    w_u32(0x80099388, index - 2);
    first = moh_callback_value(index - 1);
    second = moh_callback_value(index);
    level2p_sub_800534AC(first, second);
}

void level2p_sub_80065108(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, handle, text_id, owner, instance;
    FUNCTION_MARKER(0x80065108, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 1);
    handle = r_u32(context);
    text_id = moh_callback_value(index);
    owner = moh_callback_object(handle);
    instance = r_u32(context + 0xC);
    level2p_sub_80052FE4(instance, owner, text_id);
}

void level2p_sub_8006517C(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, owner, instance, category, result;
    FUNCTION_MARKER(0x8006517C, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 1);
    w_u32(0x80099388, index - 2);
    owner = moh_callback_value(index - 2);
    instance = moh_callback_value(index - 1);
    category = moh_callback_value(index);
    w_u32(0x80099388, index - 3);
    result = level2p_sub_8005309C(owner, instance, category);
    moh_callback_push_value(result);
}

void level2p_sub_8006520C(uint32 context, uint32 handlers, uint32 handler_id)
{
    uint32 index, category, left_instance, left_owner, right_instance, right_owner, result;
    FUNCTION_MARKER(0x8006520C, "LEVEL2P.BIN");
    index = r_u32(0x80099388);
    w_u32(0x80099388, index - 1);
    w_u32(0x80099388, index - 2);
    w_u32(0x80099388, index - 3);
    w_u32(0x80099388, index - 4);
    category = moh_callback_value(index);
    left_instance = moh_callback_value(index - 1);
    left_owner = moh_callback_value(index - 2);
    right_instance = moh_callback_value(index - 3);
    right_owner = moh_callback_value(index - 4);
    w_u32(0x80099388, index - 5);
    result = level2p_sub_8005311C(left_owner, left_instance, right_owner, right_instance, category);
    moh_callback_push_value(result);
}

void level2p_sub_800529C8(uint32 instance, uint32 unused)
{
    uint32 flags;
    FUNCTION_MARKER(0x800529C8, "LEVEL2P.BIN");
    flags = r_u32(instance);
    if (!(flags & 1))
    {
        w_u32(instance, flags | 1);
        level2p_sub_8006F390(instance);
    }
}

void level2p_sub_800529FC(uint32 owner)
{
    FUNCTION_MARKER(0x800529FC, "LEVEL2P.BIN");
    level2p_sub_80066C64(owner);
}

void level2p_sub_80052A1C(uint32 command, uint32 value)
{
    FUNCTION_MARKER(0x80052A1C, "LEVEL2P.BIN");
    if (!command)
    {
        w_u16(r_u32(0x80098FB0) + 0xCC, (uint16)value);
        sub_8001C6D8((uint32)(sint32)(sint16)value, 400);
    }
    else if (command == 2) w_u32(0x80094904, value);
    else if (command == 3) w_u32(0x80098B8C, r_u32(0x80098B8C) | 0x24);
}

void level2p_sub_80052A94(uint32 owner, uint32 instance, uint32 command)
{
    uint32 position, axis, count, variation, random, adjusted, result, x, y, z;
    FUNCTION_MARKER(0x80052A94, "LEVEL2P.BIN");
    if (owner) position = owner + 0x20C;
    else
    {
        position = 0x800AADB0;
        for (axis = 0; axis < 3; ++axis)
            w_u32(position + axis * 4,
                (uint32)(sint32)r_s16(r_u32(instance + 4) + 0x10 + axis * 2) << 16);
    }
    switch (command)
    {
    case 0:
    case 5:
        level2p_sub_8006CD4C(owner, 4, 4, 60, 0x10000, 0x1000, 0x40000, 7, r_u32(instance + 4));
        random = sub_8001552C();
        count = (uint32)((sint32)(random * 5) >> 15) + 15;
        level2p_sub_8006DF10(position, 5, count, 40, 25, 80, 30, 0x3FF, 0x3F,
            0x3E0, 255, 1, 0x17FFF, 0, 0);
        if (!command) level2p_sub_800431A0(owner);
        break;
    case 1:
    case 6:
        random = sub_8001552C();
        count = (uint32)((sint32)(random * 3) >> 15) + 2;
        random = sub_8001552C();
        variation = (uint32)((sint32)(random << 2) >> 15) + 6;
        level2p_sub_8006DF10(position, 1, count, 30, 25, 15, 100, 0x3F4, 0xE0,
            0x3E0, 254, 15, 0x10000, 1, variation);
        if (command == 1)
        {
            random = sub_8001552C();
            if (((sint32)(random * 100) >> 15) < 50)
            {
                random = sub_8001552C();
                adjusted = (sint32)random < 0 ? random + 3 : random;
                variation = random - ((uint32)((sint32)adjusted >> 2) << 2);
                level2p_sub_800420C4(0, variation, owner, 0xCCCC, 0, 0x8000, 0x2000, 0, 0, 0, 0);
            }
        }
        break;
    case 2:
        result = level2p_sub_8006EDF8();
        x = r_u32(position);
        y = r_u32(position + 4);
        z = r_u32(position + 8);
        w_u32(result + 0x20C, x);
        w_u32(result + 0x210, y);
        w_u32(result + 0x214, z);
        level2p_sub_80066BA8(result);
        break;
    case 3:
        random = sub_8001552C();
        count = (uint32)((sint32)(random * 5) >> 15) + 15;
        level2p_sub_8006DF10(position, 5, count, 40, 25, 15, 100, 0x3F4, 0xE0,
            0x3E0, 254, 15, 0x17FFF, 0, 0);
        break;
    case 4:
        result = level2p_sub_8006CD4C(owner, 18, 30, 800, 0x40000, 0x5000, 0x140000, 3, r_u32(instance + 4));
        if (result) w_u32(result + 0x14, 0x88004450);
        level2p_sub_800431A0(owner);
        level2p_sub_80050C78(1, 1, 2, 2, 192, 192, 192, 128, 128, 128);
        break;
    case 7:
    case 8:
        level2p_sub_8006CD4C(owner, 6, 10, 15, 0x3000, 0x200, 0x20000, 5, r_u32(instance + 4));
        random = sub_8001552C();
        count = (uint32)((sint32)(random * 3) >> 15) + 2;
        random = sub_8001552C();
        variation = (uint32)((sint32)(random << 2) >> 15) + 6;
        level2p_sub_8006DF10(position, 1, count, 30, 25, 15, 100, 0x3F4, 0xE0,
            0x3E0, 254, 15, 0x10000, 1, variation);
        if (command == 7) level2p_sub_800431A0(owner);
        break;
    }
}

uint32 level2p_sub_80052F54(uint32 owner, uint32 unused, uint32 command, uint32 choice, uint32 parameter5, uint32 parameter6, uint32 parameter7, uint32 parameter8, uint32 parameter9)
{
    FUNCTION_MARKER(0x80052F54, "LEVEL2P.BIN");
    if (choice) --choice;
    return level2p_sub_800420C4((uint32)(sint32)(sint16)choice, parameter5, owner,
        parameter8, parameter9, parameter6, parameter7, 0, command == 3, 0, 0);
}

void level2p_sub_80052FC4(uint32 channel)
{
    FUNCTION_MARKER(0x80052FC4, "LEVEL2P.BIN");
    level2p_sub_80042714(channel);
}

void level2p_sub_80052FE4(uint32 unused_instance, uint32 unused_owner, uint32 text_id)
{
    uint32 y, text;
    FUNCTION_MARKER(0x80052FE4, "LEVEL2P.BIN");
    if (r_s32(0x80096198) >= 50) return;
    if (text_id != r_u32(0x8009490C))
    {
        y = r_u32(0x80094908);
        w_u32(0x8009490C, text_id);
        y += 12;
        w_u32(0x80094908, y);
        if ((sint32)y > 156) w_u32(0x80094908, 120);
    }
    text = r_u32(0x80094468 + (text_id << 2));
    y = r_u32(0x80094908);
    level2p_sub_8005ABFC(text, 20, y, 128, 128, 128, 1, 100, 255, 3, 0);
}

uint32 level2p_sub_80053274(uint32 owner, uint32 instance, uint32 selector)
{
    uint32 entry, callback;
    FUNCTION_MARKER(0x80053274, "LEVEL2P.BIN");
    entry = 0x80094910 + (selector << 2);
    callback = r_u32(entry);
    return moh_draft_indirect_call(callback, owner, instance, entry);
}

void level2p_sub_800532AC(uint32 id, uint32 text_id)
{
    uint32 count, index, offset, text;
    FUNCTION_MARKER(0x800532AC, "LEVEL2P.BIN");
    count = r_u32(0x80094900);
    if ((sint32)count > 0)
    {
        for (index = 0; (sint32)index < (sint32)count; ++index)
        {
            offset = index << 2;
            if (r_u32(0x80094C0C + offset) != id) continue;
            w_u32(0x80094BBC + offset, 0);
            text = r_u32(0x80094468 + (text_id << 2));
            w_u32(0x80094BE4 + offset, text);
            w_u32(0x80094C0C + offset, id);
            return;
        }
    }
    count = r_u32(0x80094900);
    offset = count << 2;
    w_u32(0x80094900, count + 1);
    w_u32(0x80094BBC + offset, 0);
    text = r_u32(0x80094468 + (text_id << 2));
    w_u32(0x80094BE4 + offset, text);
    w_u32(0x80094C0C + offset, id);
}

void level2p_sub_8005338C(uint32 id)
{
    uint32 count, index = 0, last;
    FUNCTION_MARKER(0x8005338C, "LEVEL2P.BIN");
    count = r_u32(0x80094900);
    if ((sint32)count > 0)
        for (; (sint32)index < (sint32)count; ++index)
            if (r_u32(0x80094C0C + (index << 2)) == id) break;
    /* TODO Original removes the last record even when the requested id is absent */
    last = r_u32(0x80094900) - 1;
    w_u32(0x80094900, last);
    w_u32(0x80094BBC + (index << 2), r_u32(0x80094BBC + (last << 2)));
    w_u32(0x80094BE4 + (index << 2), r_u32(0x80094BE4 + (last << 2)));
    w_u32(0x80094C0C + (index << 2), r_u32(0x80094C0C + (last << 2)));
}

void level2p_sub_80053430(uint32 id)
{
    uint32 count, index;
    FUNCTION_MARKER(0x80053430, "LEVEL2P.BIN");
    count = r_u32(0x80094900);
    if ((sint32)count <= 0) return;
    for (index = 0; (sint32)index < (sint32)count; ++index)
    {
        if (r_u32(0x80094C0C + (index << 2)) == id)
        {
            w_u32(0x80094BBC + (index << 2), 1);
            return;
        }
    }
}

void level2p_sub_8005348C(uint32 id)
{
    FUNCTION_MARKER(0x8005348C, "LEVEL2P.BIN");
    level2p_sub_80054198(id);
}

void level2p_sub_800534AC(uint32 first, uint32 second)
{
    FUNCTION_MARKER(0x800534AC, "LEVEL2P.BIN");
    level2p_sub_800543E8(first, second);
}

uint32 level2p_sub_800534CC(uint32 id)
{
    uint32 count, index;
    FUNCTION_MARKER(0x800534CC, "LEVEL2P.BIN");
    count = r_u32(0x80094900);
    if ((sint32)count <= 0) return 1;
    for (index = 0; (sint32)index < (sint32)count; ++index)
        if (r_u32(0x80094C0C + (index << 2)) == id) return r_u32(0x80094BBC + (index << 2));
    return 1;
}

uint32 level2p_sub_80053528(uint32 owner, uint32 unused_instance, uint32 unused_entry)
{
    FUNCTION_MARKER(0x80053528, "LEVEL2P.BIN");
    return r_u32(owner + 0xC) & 1;
}

uint32 level2p_sub_80053534(uint32 owner, uint32 unused_instance, uint32 unused_entry)
{
    FUNCTION_MARKER(0x80053534, "LEVEL2P.BIN");
    return (r_u32(owner + 0xC) & 0x20000000) != 0;
}


void level2p_sub_8006F390(uint32 instance)
{
    uint32 object, kind, result = 0, data;
    FUNCTION_MARKER(0x8006F390, "LEVEL2P.BIN");
    object = r_u32(instance + 4);
    kind = r_u32(object + 0xC);
    if (kind == 2 || kind == 6)
    {
        data = r_u32(object + 0x60);
        result = level2p_sub_8006EFEC(r_u32(data + 4), instance);
    }
    else if (kind == 3)
    {
        data = r_u32(object + 0x74);
        result = level2p_sub_8006EFEC(r_u32(data + 4), instance);
    }
    else if (kind == 4) result = level2p_sub_8006F080(r_u32(object + 0x60), instance);
    if (result) level2p_sub_8006F21C(instance);
}

void level2p_sub_80066C64(uint32 owner)
{
    uint32 flags, instance, object, kind, flag, callback, value;
    FUNCTION_MARKER(0x80066C64, "LEVEL2P.BIN");
    if (!owner) return;
    flags = r_u32(owner + 0xC);
    if (flags & 0x80000) return;
    instance = r_u32(owner + 0x1C);
    w_u32(owner + 0xC, flags | 0x80000);
    if (instance)
    {
        object = r_u32(instance + 4);
        kind = r_u32(object + 0xC);
        flag = (flags >> 29) & 1;
        if (kind == 2 || kind == 3 || kind == 6)
        {
            if (r_u32(object + 0x30) & 8) flag = 0;
        }
        else flag = 0;
        if (flag)
        {
            instance = r_u32(owner + 0x1C);
            value = r_u32(instance);
            w_u32(instance, value | 2);
        }
        instance = r_u32(owner + 0x1C);
        value = r_u32(instance);
        w_u32(instance, value & 0xFFFFFFFE);
    }
    level2p_sub_80042674(owner);
    level2p_sub_80066C10(owner);
    level2p_sub_80068C08(owner);
    callback = r_u32(owner + 0x384);
    if (callback) moh_draft_indirect_call(callback, owner);
}

void level2p_sub_80042714(uint32 channel)
{
    uint32 record;
    FUNCTION_MARKER(0x80042714, "LEVEL2P.BIN");
    if ((sint32)channel <= 0) return;
    moh_draft_missing_call(0x80015C08, channel, 0, 0);
    moh_draft_missing_call(0x800157BC, channel, 0);
    moh_draft_missing_call(0x800158DC, 0, 1u << (channel & 31));
    record = 0x8008D440 + channel * 52;
    w_u8(0x8008D3B4 + channel, 0);
    w_u8(record + 0x12, 0);
    w_u8(record + 0xE, 0);
    level2p_sub_80068A04(record + 4, 0);
    w_u16(record + 0x28, 0);
}

uint32 level2p_sub_8006EDF8(void)
{
    uint32 owner;
    FUNCTION_MARKER(0x8006EDF8, "LEVEL2P.BIN");
    owner = level2p_sub_8006EC30(15);
    if (owner)
    {
        w_u32(owner + 0x36C, 0x8006EDD0);
        level2p_sub_8005A1B8(owner, 0, 0);
        level2p_sub_8005A1B8(owner, 2, 0);
        level2p_sub_8005A1B8(owner, 3, 0);
    }
    return owner;
}

void level2p_sub_80066BA8(uint32 owner)
{
    uint32 head, next;
    FUNCTION_MARKER(0x80066BA8, "LEVEL2P.BIN");
    head = r_u32(0x8009941C);
    next = r_u32(head + 4);
    w_u32(owner + 4, next);
    w_u32(head + 4, owner);
}

void level2p_sub_80054198(uint32 id)
{
    uint32 group = 0, base = 0x80095378, index, entry, descriptor, mode, value, limit, mapped_id;
    FUNCTION_MARKER(0x80054198, "LEVEL2P.BIN");
    while ((sint32)group < r_s32(0x80094C64))
    {
        if (group != r_u32(0x800A94C8) && r_u32(base + 0x284))
        {
            entry = base;
            index = 0;
            while ((sint32)index < r_s32(base + 0x280))
            {
                descriptor = r_u32(entry);
                if (r_u32(descriptor + 0xC) == id)
                {
                    mode = r_u32(descriptor + 8);
                    switch (mode)
                    {
                    case 0:
                        value = r_u32(entry + 0xC) + r_u32(entry + 0x10);
                        w_u32(entry + 0xC, value);
                        limit = r_u16(descriptor + 0x10);
                        if ((sint32)value >= (sint32)limit)
                        {
                            w_u32(entry + 0xC, limit - 1);
                            w_u32(entry + 0x10, 0);
                        }
                        break;
                    case 1:
                        value = r_u32(entry + 0xC) + 23;
                        w_u32(entry + 0xC, value);
                        limit = r_u16(descriptor + 0x10);
                        if (!limit)
                        {
                            moh_draft_fail_fast("Original animation division BREAK 0x1C00", 0x800542E0);
                            return;
                        }
                        w_u32(entry + 0xC, (uint32)((sint32)value % (sint32)limit));
                        break;
                    case 2:
                        value = r_u32(entry + 0xC) + 1;
                        w_u32(entry + 0xC, value);
                        limit = r_u16(descriptor + 0x10);
                        if ((sint32)value >= (sint32)limit) w_u32(entry + 0xC, 0);
                        break;
                    case 3:
                        value = r_u32(entry + 0xC) + r_u32(entry + 0x10);
                        w_u32(entry + 0xC, value);
                        if ((sint32)value < 0)
                        {
                            w_u32(entry + 0xC, 1);
                            w_u32(entry + 0x10, 1);
                        }
                        else
                        {
                            limit = r_u16(descriptor + 0x10);
                            if ((sint32)value >= (sint32)limit)
                            {
                                w_u32(entry + 0xC, limit - 2);
                                w_u32(entry + 0x10, 0xFFFFFFFF);
                            }
                        }
                        break;
                    }
                    mapped_id = r_u32(descriptor + 0xC);
                    if (mapped_id < 99) w_u8(0x80095D98 + mapped_id, r_u8(entry + 0xC));
                    level2p_sub_80053EF0(entry, group);
                }
                entry += 20;
                index++;
            }
        }
        base += 0x288;
        group++;
    }
}

void level2p_sub_800543E8(uint32 first, uint32 second)
{
    uint32 group = 0, base = 0x80095378, index, entry, descriptor, mapped_id;
    FUNCTION_MARKER(0x800543E8, "LEVEL2P.BIN");
    while ((sint32)group < r_s32(0x80094C64))
    {
        if (group != r_u32(0x800A94C8) && r_u32(base + 0x284))
        {
            index = 0;
            entry = base;
            while ((sint32)index < r_s32(base + 0x280))
            {
                descriptor = r_u32(entry);
                if (r_u32(descriptor + 0xC) == first)
                {
                    w_u32(entry + 0xC, second);
                    mapped_id = r_u32(descriptor + 0xC);
                    if (mapped_id < 99) w_u8(0x80095D98 + mapped_id, r_u8(entry + 0xC));
                    level2p_sub_80053EF0(entry, group);
                }
                index++;
                entry += 20;
            }
        }
        base += 0x288;
        group++;
    }
}

void level2p_sub_800431A0(uint32 owner)
{
    uint32 kind, random, choice, volume = 0x10000;
    FUNCTION_MARKER(0x800431A0, "LEVEL2P.BIN");
    kind = r_u32(owner + 8);
    if (kind == 0x13F1 || kind == 0x13F3)
    {
        random = sub_8001552C();
        choice = (uint32)(sint32)(sint16)(((sint32)(random << 1) >> 15) + 9);
        level2p_sub_800420C4(0, choice, owner, 0x40000, 0, 0x10000, 0x40AC, 0, 0, 3, 0);
        if (kind == 0x13F1) volume = 0x7FFF;
    }
    random = sub_8001552C();
    choice = (uint32)(sint32)(sint16)(((sint32)(random << 1) >> 15) + 9);
    level2p_sub_800420C4(0, choice, owner, 0x40000, 0, volume, 0x40AC, 0, 0, 3, 0);
    level2p_sub_800420C4(0, choice, owner, 0x40000, 0, 0x10000, 0x40AC, 0, 0, 3, 0);
}

uint32 level2p_sub_8005ABFC(uint32 text, uint32 x, uint32 y, uint32 red, uint32 green, uint32 blue, uint32 parameter7, uint32 parameter8, uint32 parameter9, uint32 parameter10, uint32 output)
{
    uint32 index, record, scan, generated, current, count;
    FUNCTION_MARKER(0x8005ABFC, "LEVEL2P.BIN");
    index = r_u32(0x80096198);
    if ((sint32)index >= 50) return 0xFFFFFFFF;
    record = 0x80096214 + (index << 5);
    w_u32(record, text);
    w_u16(record + 4, x);
    w_u16(record + 6, y);
    w_u8(record + 8, red);
    w_u8(record + 9, green);
    w_u8(record + 0xA, blue);
    w_u8(record + 0xB, parameter7);
    w_u32(record + 0xC, parameter8);
    if ((parameter9 & 255) == 0xFE)
    {
        scan = 0;
        generated = (sub_8001552C() & 0x7F) + 120;
        while ((sint32)scan < r_s32(0x80096198))
        {
            current = r_u8(0x80096214 + (scan << 5) + 0x1F);
            scan++;
            if (current == (generated & 255))
            {
                scan = 0;
                generated = (sub_8001552C() & 0x7F) + 120;
            }
        }
        w_u8(output, generated);
        /* TODO Original stores at the final scan index rather than the allocated index */
        w_u8(0x80096214 + (scan << 5) + 0x1F, generated);
    }
    else w_u8(record + 0x1F, parameter9);
    count = r_u32(0x80096198);
    w_u32(record + 0x10, 0xFFFFFFFE);
    w_u32(record + 0x18, parameter10);
    w_u32(0x80096198, count + 1);
    return index;
}

uint32 level2p_sub_800420C4(uint32 bank, uint32 choice, uint32 owner, uint32 parameter4, uint32 parameter5, uint32 parameter6, uint32 parameter7, uint32 parameter8, uint32 parameter9, uint32 parameter10, uint32 parameter11)
{
    uint32 channel, record, scaled, spread, divisor, random, left_address, right_address, value;
    sint16 left = 0, right = 0;
    FUNCTION_MARKER(0x800420C4, "LEVEL2P.BIN");
    bank &= 0xFFFF;
    if (bank >= 8 || (sint32)choice < 0) return 0xFFFFFFFF;
    if (!r_u32(0x8008D40C + (bank << 2))) return 0xFFFFFFFF;
    channel = level2p_sub_80041E7C();
    if (channel == 0xFFFFFFFF) return channel;
    record = 0x8008D440 + channel * 52;
    w_u16(record + 0xC, bank);
    w_u32(record + 8, choice);
    scaled = parameter4 << 7;
    w_u32(record + 0x14, scaled);
    if ((sint32)scaled > 0xFF0000) w_u32(record + 0x14, 0xFF0000);
    w_u32(record + 0x18, parameter4);
    w_u32(record + 0x1C, parameter5);
    w_u32(record + 0x20, parameter6);
    w_u8(record + 0xE, parameter9);
    w_u16(record + 0x28, 0);
    w_u32(record + 0x24, parameter7);
    w_u8(record + 0x13, parameter10);
    if (parameter5)
    {
        random = sub_8001552C();
        scaled = r_u32(record + 0x14);
        spread = scaled * parameter5;
        divisor = spread << 1;
        if (!divisor)
        {
            moh_draft_fail_fast("Original sound division BREAK 0x1C00", 0x80042218);
            return channel;
        }
        if (divisor == 0xFFFFFFFF && random == 0x80000000)
        {
            moh_draft_fail_fast("Original sound division BREAK 0x1800", 0x80042230);
            return channel;
        }
        w_u32(record + 0x14, scaled + spread - (uint32)((sint32)random % (sint32)divisor));
    }
    w_u8(record + 0xF, parameter8);
    w_u8(record + 0x12, 0);
    w_u8(record + 0x10, 0);
    level2p_sub_80068A04(record + 4, owner);
    w_u32(record, parameter11);
    if (!owner || (r_u32(owner + 0xC) & 0x40000000))
    {
        w_u16(record + 0x2C, 0);
        w_u16(record + 0x2E, 0);
        w_u16(record + 0x30, 0);
        scaled = (uint32)(r_s32(record + 0x14) >> 10);
        value = r_u8(0x8008FCDE);
        w_u32(record, r_u32(record) | 1);
        left = right = (sint16)((sint32)(scaled * value) >> 8);
    }
    else
    {
        w_u16(record + 0x2C, r_u16(owner + 0x20E));
        w_u16(record + 0x2E, r_u16(owner + 0x212));
        w_u16(record + 0x30, r_u16(owner + 0x216));
        /* TODO Addressable output words need the permitted local-view adapter */
        left_address = moh_draft_local_address(&left, sizeof(left));
        right_address = moh_draft_local_address(&right, sizeof(right));
        level2p_sub_80042E9C(channel, left_address, right_address, parameter10);
        if (!left && !right) left = right = 1;
    }
    level2p_sub_80041EE0((uint32)(sint32)(sint16)bank, choice, channel,
        (uint32)(sint32)left, (uint32)(sint32)right, parameter6, parameter7);
    return channel;
}

uint32 level2p_sub_80050C78(uint32 parameter1, uint32 parameter2, uint32 parameter3, uint32 parameter4, uint32 parameter5, uint32 parameter6, uint32 parameter7, uint32 parameter8, uint32 parameter9, uint32 parameter10)
{
    uint32 owner, data, page;
    sint32 duration, red_step, green_step, blue_step;
    FUNCTION_MARKER(0x80050C78, "LEVEL2P.BIN");
    owner = level2p_sub_80066934(0, 100, 0xFFFFFFFF, 0, 0, 0, 0, 0);
    if (!owner) return owner;
    duration = (sint16)parameter3;
    if (!duration)
    {
        moh_draft_fail_fast("Original color division BREAK 0x1C00", 0x80050D2C);
        return owner;
    }
    red_step = ((sint32)(parameter8 & 255) - (sint32)(parameter5 & 255)) / duration;
    green_step = ((sint32)(parameter9 & 255) - (sint32)(parameter6 & 255)) / duration;
    data = r_u32(owner + 0x388);
    blue_step = ((sint32)(parameter10 & 255) - (sint32)(parameter7 & 255)) / duration;
    w_u32(owner + 8, 0);
    w_u32(owner + 0x14, 0);
    w_u32(owner + 0x374, 0);
    w_u32(owner + 0x378, 0);
    w_u32(owner + 0xC, 0x20000);
    w_u32(owner + 0x10, 0x203);
    w_u32(owner + 0x36C, 0x80050A08);
    w_u32(owner + 0x370, 0x80050B9C);
    w_u32(owner + 0x384, 0x800653D8);
    w_u32(data, parameter1);
    w_u32(data + 4, parameter2);
    w_u32(data + 8, parameter3);
    w_u32(data + 0xC, parameter4);
    w_u8(data + 0x18, parameter5);
    w_u8(data + 0x19, parameter6);
    w_u8(data + 0x1A, parameter7);
    w_u16(data + 0x10, (uint32)red_step);
    w_u16(data + 0x12, (uint32)green_step);
    w_u16(data + 0x14, (uint32)blue_step);
    moh_draft_missing_call(0x8001B9A8, data + 0x2C);
    moh_draft_missing_call(0x8001B9A8, data + 0x44);
    moh_draft_missing_call(0x8001BA38, data + 0x2C, 1);
    moh_draft_missing_call(0x8001BA38, data + 0x44, 1);
    w_u8(data + 0x30, parameter5);
    w_u8(data + 0x31, parameter6);
    w_u8(data + 0x32, parameter7);
    w_u8(data + 0x48, parameter5);
    w_u8(data + 0x49, parameter6);
    w_u8(data + 0x4A, parameter7);
    w_u16(data + 0x34, 0);
    w_u16(data + 0x36, 0);
    page = r_u16(0x80098B74);
    w_u16(data + 0x3A, 0);
    w_u16(data + 0x3C, 0);
    w_u16(data + 0x38, page - 1);
    w_u16(data + 0x3E, r_u16(0x80098B78) - 1);
    w_u16(data + 0x40, r_u16(0x80098B74) - 1);
    page = r_u16(0x80098B78);
    w_u16(data + 0x4C, 0);
    w_u16(data + 0x4E, 0);
    w_u16(data + 0x42, page - 1);
    page = r_u16(0x80098B74);
    w_u16(data + 0x52, 0);
    w_u16(data + 0x54, 0);
    w_u16(data + 0x50, page - 1);
    w_u16(data + 0x56, r_u16(0x80098B78) - 1);
    w_u16(data + 0x58, r_u16(0x80098B74) - 1);
    w_u16(data + 0x5A, r_u16(0x80098B78) - 1);
    page = moh_draft_missing_call(0x8001B968, 2, 1, 0, 0) & 0xFFFF;
    moh_draft_missing_call(0x8001B9C8, data + 0x1C, r_u8(0x80096993), r_u8(0x80096992), page);
    page = moh_draft_missing_call(0x8001B968, 2, 1, 0, 0) & 0xFFFF;
    moh_draft_missing_call(0x8001B9C8, data + 0x24, r_u8(0x80096993), r_u8(0x80096992), page);
    level2p_sub_80066BA8(owner);
    return owner;
}


uint32 level2p_sub_8006CD4C(uint32 owner, uint32 kind, uint32 count, uint32 duration, uint32 parameter5, uint32 parameter6, uint32 parameter7, uint32 parameter8, uint32 object)
{
    uint32 result, data, item, index, axis, position, offset, random, value, angle = 0;
    uint32 x, y, z, first, second, direction = 1, sequence = 0, descriptor, allocation, flags;
    FUNCTION_MARKER(0x8006CD4C, "LEVEL2P.BIN");
    result = level2p_sub_80066934(0, (kind + count + 1) * 60 + 100, 0xFFFFFFFF, 0, 1, 0, 0, 0);
    if (!result) return 0;
    w_u32(result + 8, 0x31);
    w_u32(result + 0x294, 0xB4);
    w_u32(result + 0x290, 0xB4);
    w_u32(result + 0x36C, 0x8006C7EC);
    w_u32(result + 0x370, 0x8006C13C);
    w_u32(result + 0x374, 0x8004CC0C);
    w_u32(result + 0x14, 0x88004451);
    flags = r_u32(result + 0x10);
    w_u32(result + 0x38, 0);
    w_u32(result + 0xC, 0x1228000);
    w_u32(result + 0x378, 0);
    w_u32(result + 0x384, 0x8006CCB8);
    w_u32(result + 0x284, 0);
    w_u32(result + 0x10, flags | 0x201);
    data = r_u32(result + 0x388);
    if (owner)
    {
        x = r_u32(owner + 0x20C);
        y = r_u32(owner + 0x210);
        z = r_u32(owner + 0x214);
        w_u32(result + 0x20C, x);
        w_u32(result + 0x210, y);
        w_u32(result + 0x214, z);
        w_u16(result + 0x3CE, r_u16(owner + 0x3CE));
        flags = r_u32(owner + 0x10);
        w_u32(result + 0x10, r_u32(result + 0x10) | (flags & 0x100000));
        if (!r_u32(owner + 0x364) && !r_u16(owner + 0x368))
            angle = (uint32)(sint32)r_s16(owner + 0x22E) + 0x800;
        else
        {
            first = r_u32(owner + 0x364);
            second = r_u32(owner + 0x368);
            w_u32(result + 0x364, first);
            w_u32(result + 0x368, second);
            direction = 0;
        }
    }
    else
    {
        if (!object)
        {
            sub_80036E44(result);
            return 0;
        }
        w_u32(result + 0x20C, (uint32)(sint32)r_s16(object + 0x10) << 16);
        w_u32(result + 0x210, (uint32)(sint32)r_s16(object + 0x12) << 16);
        value = (uint32)(sint32)r_s16(object + 0x14);
        w_u16(result + 0x3CE, 0xFFFF);
        w_u32(result + 0x214, value << 16);
        angle = (uint32)(sint32)r_s16(object + 0x1A) + 0x800;
    }
    if (direction)
    {
        first = moh_draft_missing_call(0x8001C9A8, angle) << 16;
        if ((sint32)first < 0) first += 0xFFF;
        first = (uint32)((sint32)first >> 12) * 40;
        second = moh_draft_missing_call(0x8001C908, angle) << 16;
        if ((sint32)second < 0) second += 0xFFF;
        second = (uint32)((sint32)second >> 12) * 5;
        w_u16(result + 0x364, (uint32)((sint32)first >> 16));
        w_u16(result + 0x366, 0);
        w_u16(result + 0x368, (uint32)((sint32)second >> 13));
    }
    x = r_u32(result + 0x20C);
    y = r_u32(result + 0x210);
    z = r_u32(result + 0x214);
    w_u32(result + 0x258, x);
    w_u32(result + 0x25C, y);
    w_u32(result + 0x260, z);
    level2p_sub_8005A1B8(result, 0, parameter6);
    item = data + 100;
    w_u32(data, 0);
    w_u32(data + 4, 0);
    w_u16(data + 0x10, kind);
    w_u32(data + 0x34, item);
    w_u16(data + 0x12, kind);
    w_u32(data + 0xC, 1);
    w_u32(data + 0x1C, 0xF0000);
    w_u32(data + 0x14, 1);
    w_u32(data + 0x20, 0xFFFFFE00);
    w_u16(data + 0x26, 0x20);
    w_u16(data + 0x2A, 0xFF);
    w_u16(data + 0x2C, 0x3F);
    w_u16(data + 0x2E, 0x3F);
    w_u16(data + 0x30, 0x10);
    w_u16(data + 0x24, 0x3E0);
    w_u16(data + 0x28, 0x3E0);
    w_u16(data + 0x32, 2);
    w_u32(data + 0x18, parameter5);
    for (index = 0; (sint32)index < (sint32)kind; index++, item += 60)
    {
        w_u8(item, 3);
        w_u32(item + 0x20, 0x50000);
        for (axis = 0; axis < 3; axis++)
        {
            position = r_u32(result + 0x20C + axis * 4);
            offset = (0 - duration) << 16;
            if (duration << 1)
            {
                random = sub_8001552C();
                offset = (0 - duration + (uint32)((sint32)(random * (duration << 1)) >> 15)) << 16;
            }
            w_u32(item + 0x24 + axis * 4, position + offset);
        }
        w_u32(item + 0x30, 0);
        w_u32(item + 0x34, 0);
        w_u32(item + 0x38, 0);
        random = sub_8001552C();
        w_u16(item + 8, (uint32)((sint32)(random * 50) >> 15) - 25);
        random = sub_8001552C();
        w_u16(item + 0xA, random >> 3);
        w_u32(item + 0x10, 0x140000);
        w_u32(item + 0x18, 0x400000);
        w_u32(item + 0x14, 0x400000);
        w_u8(item + 2, 5);
        w_u8(item + 1, 5);
        random = sub_8001552C();
        w_u32(item + 0x1C, ((uint32)((sint32)(random << 1) >> 15) + 3) << 16);
        random = sub_8001552C();
        w_u16(item + 0xC, ((uint32)((sint32)(random * 100) >> 15) + 35) << 1);
        random = sub_8001552C();
        w_u16(item + 0xE, ((uint32)((sint32)(random * 100) >> 15) + 35) << 1);
        w_u16(item + 6, sequence);
        random = sub_8001552C();
        sequence += (uint32)((sint32)random >> 15);
        w_u8(item + 3, 0);
        w_u16(item + 4, 1);
    }
    w_u16(data + 0x3C, count);
    w_u32(data + 0x60, item);
    w_u16(data + 0x3E, count);
    w_u32(data + 0x38, 1);
    w_u32(data + 0x44, parameter5 << 1);
    w_u32(data + 0x48, 0x1E0000);
    w_u32(data + 0x4C, 0xFFFFE000);
    w_u16(data + 0x50, 1000);
    w_u16(data + 0x54, 0x3E0);
    w_u16(data + 0x56, 0xFA);
    w_u16(data + 0x58, 0x1F);
    w_u16(data + 0x5A, 0x1F);
    w_u16(data + 0x5C, 8);
    w_u16(data + 0x52, 0);
    w_u16(data + 0x5E, 2);
    for (index = 0; (sint32)index < (sint32)count; index++, item += 60)
    {
        w_u8(item, 3);
        w_u32(item + 0x20, 0);
        for (axis = 0; axis < 3; axis++)
        {
            position = r_u32(result + 0x20C + axis * 4);
            offset = (0 - duration) << 16;
            if (duration << 1)
            {
                random = sub_8001552C();
                offset = (0 - duration + (uint32)((sint32)(random * (duration << 1)) >> 15)) << 16;
            }
            w_u32(item + 0x24 + axis * 4, position + offset);
        }
        w_u32(item + 0x30, 0);
        w_u32(item + 0x34, 0);
        w_u32(item + 0x38, 0);
        random = sub_8001552C();
        w_u16(item + 8, (random * 3) >> 13);
        random = sub_8001552C();
        w_u16(item + 0xA, random >> 3);
        w_u32(item + 0x10, 0x50000);
        random = sub_8001552C();
        value = ((uint32)((sint32)(random * 10) >> 15) + 20) << 16;
        w_u8(item + 2, 45);
        w_u8(item + 1, 45);
        w_u32(item + 0x18, value);
        w_u32(item + 0x14, value);
        w_u32(item + 0x1C, 0x50000);
        w_u16(item + 0xC, 120);
        w_u16(item + 0xE, 120);
        random = sub_8001552C();
        w_u16(item + 6, (uint32)((sint32)(random * 20) >> 15) + 8);
        w_u8(item + 3, 0);
        w_u16(item + 4, 2);
    }
    descriptor = r_u32(result + 0x304);
    w_u16(result + 0x2E8, duration * 5);
    w_u16(result + 0x36, 1);
    w_u32(descriptor, 0);
    w_u32(r_u32(result + 0x304) + 0x18, 0);
    w_u32(r_u32(result + 0x304) + 8, 5);
    w_u32(r_u32(result + 0x304) + 0xC, 5);
    w_u32(r_u32(result + 0x304) + 0x10, 0x640000);
    descriptor = r_u32(result + 0x304);
    w_u32(result + 0xF8, 0x640000);
    w_u32(descriptor + 0x20, (uint32)(sint32)r_s16(result + 0x20E));
    descriptor = r_u32(result + 0x304);
    w_u32(descriptor + 0x24, (uint32)(sint32)r_s16(result + 0x212));
    descriptor = r_u32(result + 0x304);
    w_u32(descriptor + 0x28, (uint32)(sint32)r_s16(result + 0x216));
    w_u32(r_u32(result + 0x304) + 0x34, duration * parameter8);
    descriptor = r_u32(result + 0x304);
    w_u32(descriptor + 0x30, r_u32(descriptor + 0x34));
    descriptor = r_u32(result + 0x304);
    value = r_u32(descriptor + 0x34);
    w_u32(descriptor + 0x38, value * value);
    if ((sint32)level2p_sub_80065BF8(0x42A7A5EF) >= 0)
    {
        allocation = sub_80036BF0(0xB0);
        w_u32(r_u32(result + 0x304) + 0x3C, allocation);
        if (allocation)
        {
            descriptor = r_u32(result + 0x304);
            if (!level2p_sub_8006654C(r_u32(descriptor + 0x3C), 0x42A7A5EF, 0x16, 0))
            {
                sub_80036E44(r_u32(r_u32(result + 0x304) + 0x3C));
                /* TODO Original retains the second particle loop index in this unusual clear */
                w_u32(r_u32(result + 0x304) + index * 72 + 0x3C, 0);
            }
        }
    }
    level2p_sub_8004D8D4(result);
    level2p_sub_80066BA8(result);
    w_u8(0x800A94CC, 0x50);
    w_u8(0x800A94CD, 0x28);
    w_u8(0x800A94CE, 10);
    w_u16(0x800A94D0, 0x96);
    w_u32(0x800A94D4, 0x57E4);
    w_u32(0x80099438, result);
    w_u8(0x8009A060, 0);
    w_u32(0x800A94A8, 1);
    if (owner)
    {
        flags = r_u32(owner + 0x10);
        if (flags & 0x1000) w_u32(result + 0x10, r_u32(result + 0x10) | 0x1000);
        else if (flags & 0x2000) w_u32(result + 0x10, r_u32(result + 0x10) | 0x2000);
    }
    level2p_sub_8006591C(result);
    level2p_sub_8004DFBC(result);
    return result;
}

uint32 level2p_sub_8006DF10(uint32 position, uint32 kind, uint32 count, uint32 duration, uint32 parameter5, uint32 parameter6, uint32 parameter7, uint32 parameter8, uint32 parameter9, uint32 parameter10, uint32 parameter11, uint32 parameter12, uint32 parameter13, uint32 parameter14, uint32 parameter15)
{
    uint32 result, size, data, item, index, axis, base, offset, random, value, x, y, z;
    FUNCTION_MARKER(0x8006DF10, "LEVEL2P.BIN");
    if (r_s32(0x8009A064) >= 5) return 0;
    size = count * 52 + 100;
    if (parameter14) size += parameter15 * 52;
    result = level2p_sub_80066934(0, size, 0xFFFFFFFF, 0, 0, 0, 0, 0);
    if (!result) return 0;
    w_u32(result + 8, 0x31);
    w_u32(result + 0x294, 0xB4);
    w_u32(result + 0x290, 0xB4);
    w_u32(result + 0x36C, 0x8006DE44);
    w_u32(result + 0x370, 0x8006D634);
    data = r_u32(result + 0x388);
    w_u32(result + 0x38, 0);
    w_u32(result + 0xC, 0x1028000);
    w_u32(result + 0x374, 0);
    w_u32(result + 0x378, 0);
    w_u32(result + 0x384, 0x8006DEE4);
    value = r_u32(result + 0x10);
    w_u32(result + 0x284, 0);
    w_u32(result + 0x10, value | 0x203);
    x = r_u32(position);
    y = r_u32(position + 4);
    z = r_u32(position + 8);
    w_u32(result + 0x20C, x);
    w_u32(result + 0x210, y);
    w_u32(result + 0x214, z);
    x = r_u32(result + 0x20C);
    y = r_u32(result + 0x210);
    z = r_u32(result + 0x214);
    w_u32(result + 0x258, x);
    w_u32(result + 0x25C, y);
    w_u32(result + 0x260, z);
    item = data + 100;
    w_u32(data, 0);
    w_u32(data + 4, 0);
    w_u32(data + 0x34, item);
    w_u32(data + 0x10, count);
    w_u32(data + 0xC, 1);
    w_u32(data + 0x18, 0x20000);
    w_u32(data + 0x1C, 0x20000);
    w_u32(data + 0x20, 0x4B0000);
    w_u32(data + 0x14, parameter13);
    w_u16(data + 0x24, parameter8);
    w_u16(data + 0x26, parameter9);
    w_u16(data + 0x28, parameter10);
    w_u16(data + 0x2A, parameter11);
    w_u16(data + 0x2C, parameter12);
    w_u16(data + 0x2E, parameter12);
    w_u16(data + 0x30, 8);
    w_u16(data + 0x32, 0);
    for (index = 0; (sint32)index < (sint32)count; index++, item += 52)
    {
        w_u8(item, 1);
        w_u8(item + 1, 0);
        random = sub_8001552C();
        w_u8(item + 2, (uint32)((sint32)(random * 10) >> 15) + 25);
        for (axis = 0; axis < 3; axis++)
        {
            base = r_u32(result + 0x20C + axis * 4);
            offset = (0 - kind) << 16;
            if (kind << 1)
            {
                random = sub_8001552C();
                offset = (0 - kind + (uint32)((sint32)(random * (kind << 1)) >> 15)) << 16;
            }
            w_u32(item + 4 + axis * 4, base + offset);
        }
        for (axis = 0; axis < 3; axis++)
        {
            offset = (0 - duration) << 16;
            if (parameter5 << 1)
            {
                random = sub_8001552C();
                offset = (0 - duration + (uint32)((sint32)(random * (parameter5 << 1)) >> 15)) << 16;
            }
            w_u32(item + 0x10 + axis * 4, offset);
        }
        random = sub_8001552C();
        w_u16(item + 0x1C, random >> 3);
        random = sub_8001552C();
        w_u16(item + 0x1E, random >> 3);
        random = sub_8001552C();
        w_u16(item + 0x20, random >> 3);
        w_u16(item + 0x24, 150);
        w_u16(item + 0x26, 100);
        w_u16(item + 0x28, 200);
        value = parameter6 & 255;
        if (parameter7 & 255)
        {
            random = sub_8001552C();
            value += (uint32)((sint32)(random * (parameter7 & 255)) >> 15);
        }
        w_u8(item + 0x2E, value);
        w_u8(item + 0x2D, value);
        w_u8(item + 0x2C, value);
        random = sub_8001552C();
        w_u8(item + 3, ((sint32)(random * 100) >> 15) >= 51);
        random = sub_8001552C();
        w_u16(item + 0x30, (uint32)((sint32)(random * 7) >> 15) + 5);
        random = sub_8001552C();
        w_u16(item + 0x32, (uint32)((sint32)(random * 7) >> 15) + 5);
    }
    if (!parameter14)
    {
        w_u32(data + 0x3C, 0);
        w_u32(data + 0x60, 0);
        w_u32(data + 0x38, 2);
    }
    else
    {
        w_u32(data + 0x3C, parameter15);
        w_u32(data + 0x60, item);
        w_u32(data + 0x38, 1);
        w_u32(data + 0x40, parameter13);
        w_u32(data + 0x48, 0xFFFFFFFC);
        w_u32(data + 0x4C, 0x960000);
        w_u16(data + 0x50, 1000);
        w_u16(data + 0x54, 0x3E0);
        w_u16(data + 0x56, 0xFA);
        w_u16(data + 0x58, 0x1F);
        w_u16(data + 0x5A, 0x1F);
        w_u32(data + 0x44, 2);
        w_u16(data + 0x52, 0);
        w_u16(data + 0x5C, 8);
        w_u16(data + 0x5E, 2);
        for (index = 0; (sint32)index < (sint32)parameter15; index++, item += 52)
        {
            w_u8(item, 1);
            w_u8(item + 1, index);
            random = sub_8001552C();
            w_u8(item + 2, (uint32)((sint32)(random << 1) >> 15) + 20);
            w_u32(item + 4, r_u32(result + 0x20C));
            w_u32(item + 8, r_u32(result + 0x210));
            value = r_u32(result + 0x214);
            w_u32(item + 0x10, 0);
            w_u32(item + 0xC, value);
            random = sub_8001552C();
            value = ((uint32)((sint32)(0 - (random << 1)) >> 15) - 2) << 16;
            w_u32(item + 0x14, value);
            w_u16(item + 0x30, 5);
            w_u32(item + 0x18, 0);
            w_u16(item + 0x1C, 0);
            w_u16(item + 0x1E, 0);
            w_u16(item + 0x20, 0);
            w_u16(item + 0x24, 0);
            w_u16(item + 0x26, 0);
            w_u16(item + 0x28, 0);
            w_u16(item + 0x32, 6);
        }
    }
    level2p_sub_80066BA8(result);
    w_u32(0x8009A064, r_u32(0x8009A064) + 1);
    return result;
}


uint32 level2p_sub_8006EFEC(uint32 table, uint32 instance)
{
    uint32 index, data, owner;
    FUNCTION_MARKER(0x8006EFEC, "LEVEL2P.BIN");
    index = level2p_sub_80065BF8(table);
    if ((sint32)index < 0) level2p_sub_800885B8(2, 0x18);
    data = level2p_sub_80065BB8(index);
    owner = level2p_sub_800688DC(data, instance);
    if (!owner) return 1;
    level2p_sub_80066BA8(owner);
    w_u32(instance, r_u32(instance) | 1);
    w_u32(owner + 0x1C, instance);
    level2p_sub_80061700(owner, 0);
    level2p_sub_8006EE70(owner, instance);
    return 0;
}

uint32 level2p_sub_8006F080(uint32 table, uint32 instance)
{
    uint32 owner;
    FUNCTION_MARKER(0x8006F080, "LEVEL2P.BIN");
    owner = level2p_sub_800828C0(instance, table, 0xFFFFFFFF);
    if (owner)
    {
        level2p_sub_8006EE70(owner, instance);
        level2p_sub_80066BA8(owner);
        w_u32(instance, r_u32(instance) | 1);
        w_u32(owner + 0x1C, instance);
        level2p_sub_80061700(owner, 0);
    }
    return 0;
}

void level2p_sub_8006F21C(uint32 instance)
{
    uint32 head, count, tail;
    FUNCTION_MARKER(0x8006F21C, "LEVEL2P.BIN");
    if (r_u32(0x8009A078) >= 64)
    {
        level2p_sub_800885B8(4, 0x28);
        head = r_u32(0x8009A070);
        w_u32(0x8009A084 + (head << 2), 0);
        count = r_u32(0x8009A078);
        w_u32(0x8009A070, head + 1);
        w_u32(0x8009A078, count - 1);
    }
    count = r_u32(0x8009A078);
    tail = r_u32(0x8009A074);
    w_u32(0x8009A078, count + 1);
    w_u32(0x8009A084 + (tail << 2), instance);
    w_u32(0x8009A074, tail + 1);
    if (tail + 1 >= 64) w_u32(0x8009A074, tail - 63);
}

void level2p_sub_80042674(uint32 owner)
{
    uint32 channel, record = 0x8008D474, owner_address, current;
    FUNCTION_MARKER(0x80042674, "LEVEL2P.BIN");
    /* TODO Addressable owner word needs the permitted local-view adapter */
    owner_address = moh_draft_local_address(&owner, sizeof(owner));
    for (channel = 1; channel < 24; channel++, record += 52)
    {
        if (level2p_sub_80068988(record + 4, owner_address))
        {
            current = owner;
            w_u16(record + 0x2C, r_u16(current + 0x20E));
            w_u16(record + 0x2E, r_u16(current + 0x212));
            w_u16(record + 0x30, r_u16(current + 0x216));
            level2p_sub_80068A04(record + 4, 0);
        }
    }
}

void level2p_sub_80066C10(uint32 owner)
{
    uint32 current, next;
    FUNCTION_MARKER(0x80066C10, "LEVEL2P.BIN");
    if (!owner) return;
    current = r_u32(0x80099434);
    if (current == owner)
    {
        w_u32(0x80099434, r_u32(owner + 4));
        return;
    }
    while (current)
    {
        next = r_u32(current + 4);
        if (next == owner)
        {
            w_u32(current + 4, r_u32(next + 4));
            return;
        }
        current = next;
    }
}

void level2p_sub_80068C08(uint32 owner)
{
    uint32 index;
    FUNCTION_MARKER(0x80068C08, "LEVEL2P.BIN");
    index = level2p_sub_80068A50(owner);
    w_u32(0x80099C60 + (index << 2), 1);
}

uint32 level2p_sub_80068A50(uint32 owner)
{
    FUNCTION_MARKER(0x80068A50, "LEVEL2P.BIN");
    return r_u16(owner);
}

uint32 level2p_sub_80065BB8(uint32 index)
{
    uint32 count;
    FUNCTION_MARKER(0x80065BB8, "LEVEL2P.BIN");
    count = r_u32(0x8009B790);
    if ((sint32)index >= (sint32)count) return 0x8009B79C + ((index - count) << 8);
    return r_u32(0x8009B794) + (index << 8);
}

uint32 level2p_sub_8006EC30(uint32 kind)
{
    uint32 owner, data, signed_kind;
    FUNCTION_MARKER(0x8006EC30, "LEVEL2P.BIN");
    owner = level2p_sub_80066934(0, 0xB64, 0xFFFFFFFF, 0, 2, 0, 0, 0);
    if (!owner) return 0;
    w_u32(owner + 8, 0x1644);
    w_u32(owner + 0x36C, 0x8006EBF4);
    w_u32(owner + 0x370, 0x8006EB64);
    w_u32(owner + 0x384, 0x800653D8);
    data = r_u32(owner + 0x388);
    w_u32(owner + 0x374, 0);
    w_u32(owner + 0x378, 0);
    w_u32(data + 0xB48, 0);
    signed_kind = (uint32)(sint32)(sint16)kind;
    level2p_sub_80069AF0(data, 0x42, 0x7FFF, 0, 0, 1, 0, 0xFFFFFFCE, 0xFFFFFF9C,
        0xFFFFFFCE, 100, 20, 100, signed_kind, 0, signed_kind, 0, 0x70000, 20,
        0, 0, 0xFFFFF800, 0, 0x18000, 0xF0000, 0, 0, 4, 5, 40);
    level2p_sub_8006B91C(data, 0x360, 0x1C0, 0x1F, 0x1F, 0x370, 0x1FD, 2);
    w_u32(owner + 0x10, r_u32(owner + 0x10) | 0x200);
    return owner;
}

void level2p_sub_8005A1B8(uint32 owner, uint32 field, uint32 value)
{
    uint32 data, source, destination, x, y, z, address, bit, flags;
    FUNCTION_MARKER(0x8005A1B8, "LEVEL2P.BIN");
    if (field - 0x60 < 0x20)
    {
        if (field != 0x6A) return;
        data = r_u32(owner + 0x38C);
        if (!data) return;
        source = value ? data + 0xE0 : owner + 0x20C;
        destination = value ? owner + 0x20C : data + 0xE0;
        x = r_u32(source);
        y = r_u32(source + 4);
        z = r_u32(source + 8);
        w_u32(destination, x);
        w_u32(destination + 4, y);
        w_u32(destination + 8, z);
        return;
    }
    if (!(field & 0x1C0))
    {
        w_u32(owner + (field << 2) + 0x4C, value);
        return;
    }
    if (field & 0x100)
    {
        field &= 0xFFFFFEFF;
        address = owner + ((field >> 5) << 2) + 0xEC;
        bit = 1u << (field & 31);
    }
    else if (field & 0x40)
    {
        address = owner + 0xC;
        bit = 1u << ((field - 0x40) & 31);
    }
    else if (field & 0x80)
    {
        address = owner + 0x10;
        bit = 1u << ((field - 0x80) & 31);
    }
    else return;
    flags = r_u32(address) | bit;
    w_u32(address, flags);
    if (!value) flags ^= bit;
    w_u32(address, flags);
}

void level2p_sub_800885B8(uint32 category, uint32 code)
{
    unsigned char first_draw[92], second_draw[92], first_display[20], second_display[20];
    unsigned char packet[40], ordering[8];
    uint32 draw1, draw2, display1, display2, primitive, table;
    FUNCTION_MARKER(0x800885B8, "LEVEL2P.BIN");
    moh_draft_missing_call(0x80016CD4, 0);
    moh_draft_missing_call(0x8001CB08, 0);
    moh_draft_missing_call(0x80016D40, 0);
    /* TODO Native packet and environment buffers need the permitted local-view adapter */
    draw1 = moh_draft_local_address(first_draw, sizeof(first_draw));
    draw2 = moh_draft_local_address(second_draw, sizeof(second_draw));
    display1 = moh_draft_local_address(first_display, sizeof(first_display));
    display2 = moh_draft_local_address(second_display, sizeof(second_display));
    primitive = moh_draft_local_address(packet, sizeof(packet));
    table = moh_draft_local_address(ordering, sizeof(ordering));
    moh_draft_missing_call(0x8001BB28, draw1, 0, 0, 320, 240);
    moh_draft_missing_call(0x8001BB28, draw2, 0, 240, 320, 240);
    moh_draft_missing_call(0x8001BBE8, display1, 0, 240, 320, 240);
    moh_draft_missing_call(0x8001BBE8, display2, 0, 0, 320, 240);
    w_u8(draw1 + 25, 128);
    w_u8(draw1 + 26, 128);
    w_u8(draw1 + 27, 128);
    w_u8(draw2 + 25, 128);
    w_u8(draw2 + 26, 128);
    w_u8(draw2 + 27, 128);
    w_u8(primitive + 3, 8);
    w_u8(draw2 + 24, 0);
    w_u8(draw1 + 24, 0);
    w_u8(primitive + 7, 0x38);
    moh_draft_missing_call(0x8001BA38, primitive, 0);
    w_u8(primitive + 22, 0 - ((code >> 5) & 1));
    w_u8(primitive + 12, 0 - ((code >> 6) & 1));
    w_u8(primitive + 13, 0 - ((code >> 7) & 1));
    w_u8(primitive + 14, 0 - ((code >> 8) & 1));
    w_u8(primitive + 4, 0 - (category & 1));
    w_u8(primitive + 5, 0 - ((category >> 1) & 1));
    w_u8(primitive + 6, 0 - ((category >> 2) & 1));
    w_u8(primitive + 28, 0 - (code & 1));
    w_u8(primitive + 29, 0 - ((code >> 1) & 1));
    w_u8(primitive + 30, 0 - ((code >> 2) & 1));
    w_u8(primitive + 20, 0 - ((code >> 3) & 1));
    w_u8(primitive + 21, 0 - ((code >> 4) & 1));
    w_u16(primitive + 8, 0);
    w_u16(primitive + 10, 0);
    w_u16(primitive + 16, 512);
    w_u16(primitive + 18, 0);
    w_u16(primitive + 24, 0);
    w_u16(primitive + 26, 240);
    w_u16(primitive + 32, 512);
    w_u16(primitive + 34, 240);
    moh_draft_missing_call(0x80017164, table, 1);
    moh_draft_missing_call(0x8001DD10, table, primitive);
    for (;;)
    {
        moh_draft_missing_call(0x80016E38, 0);
        moh_draft_missing_call(0x80015DB8, 0);
        moh_draft_missing_call(0x80017408, display1);
        moh_draft_missing_call(0x80017348, draw1);
        moh_draft_missing_call(0x800172D8, table);
        moh_draft_missing_call(0x80016E38, 0);
        moh_draft_missing_call(0x80015DB8, 0);
        moh_draft_missing_call(0x80017408, display2);
        moh_draft_missing_call(0x80017348, draw2);
        moh_draft_missing_call(0x800172D8, table);
    }
}


void level2p_sub_8006EE70(uint32 owner, uint32 instance)
{
    uint32 object, x, y, z;
    FUNCTION_MARKER(0x8006EE70, "LEVEL2P.BIN");
    if (instance)
    {
        object = r_u32(instance + 4);
        w_u32(owner + 0x228, (uint32)(sint32)r_s16(object + 0x18) << 16);
        w_u32(owner + 0x22C, (uint32)(sint32)r_s16(object + 0x1A) << 16);
        w_u32(owner + 0x230, (uint32)(sint32)r_s16(object + 0x1C) << 16);
        w_u32(owner + 0x20C, (uint32)(sint32)r_s16(object + 0x10) << 16);
        w_u32(owner + 0x210, (uint32)(sint32)r_s16(object + 0x12) << 16);
        w_u32(owner + 0x214, (uint32)(sint32)r_s16(object + 0x14) << 16);
        x = r_u32(owner + 0x20C);
        y = r_u32(owner + 0x210);
        z = r_u32(owner + 0x214);
        w_u32(owner + 0x258, x);
        w_u32(owner + 0x25C, y);
        w_u32(owner + 0x260, z);
    }
    else
    {
        w_u32(owner + 0x214, 0);
        w_u32(owner + 0x210, 0);
        w_u32(owner + 0x20C, 0);
        w_u32(owner + 0x230, 0);
        w_u32(owner + 0x22C, 0);
        w_u32(owner + 0x228, 0);
    }
    x = r_u32(owner + 0x20C);
    y = r_u32(owner + 0x210);
    z = r_u32(owner + 0x214);
    w_u32(owner + 0x3C0, x);
    w_u32(owner + 0x3C4, y);
    w_u32(owner + 0x3C8, z);
}

uint32 level2p_sub_80065BF8(uint32 id)
{
    uint32 count, index, entry, first_count;
    FUNCTION_MARKER(0x80065BF8, "LEVEL2P.BIN");
    if (id == 0xFFFFFFFF) return 0xFFFFFFFF;
    count = r_u32(0x8009B790);
    if (count)
    {
        entry = r_u32(0x8009B794);
        for (index = 0; index < count; index++, entry += 256)
            if (r_u32(entry) == id) return index;
    }
    count = r_u32(0x8009B798);
    if (count)
    {
        first_count = r_u32(0x8009B790);
        entry = 0x8009B79C;
        for (index = 0; index < count; index++, entry += 256)
            if (r_u32(entry) == id) return index + first_count;
    }
    return 0xFFFFFFFF;
}

uint32 level2p_sub_80041E7C(void)
{
    uint32 channel, record = 0x8008D474, state;
    FUNCTION_MARKER(0x80041E7C, "LEVEL2P.BIN");
    for (channel = 1; channel < 24; channel++, record += 52)
    {
        if (r_u8(record + 0xE)) continue;
        state = r_u8(0x8008D3B4 + channel);
        if (!state || state == 3) return channel;
    }
    return 0xFFFFFFFF;
}

void level2p_sub_8006B91C(uint32 data, uint32 parameter2, uint32 parameter3, uint32 parameter4, uint32 parameter5, uint32 parameter6, uint32 parameter7, uint32 parameter8)
{
    uint32 current = data, offset = 0x508, packet, second, left, right, top, bottom, page, mode;
    FUNCTION_MARKER(0x8006B91C, "LEVEL2P.BIN");
    left = (parameter2 & 63) << 2;
    right = left + parameter4;
    top = parameter3;
    bottom = top + parameter5;
    w_u16(data + 0xB50, parameter2);
    w_u16(data + 0xB52, parameter3);
    w_u16(data + 0xB4C, parameter4);
    w_u16(data + 0xB5E, 0);
    w_u16(data + 0xB4E, parameter5);
    do
    {
        packet = data + offset;
        second = packet + 0x28;
        moh_draft_missing_call(0x8001B9F8, packet);
        moh_draft_missing_call(0x8001B9F8, second);
        w_u8(current + 0x50C, 128);
        w_u8(current + 0x50D, 128);
        w_u8(current + 0x50E, 128);
        w_u8(current + 0x534, 128);
        w_u8(current + 0x535, 128);
        w_u8(current + 0x536, 128);
        w_u8(current + 0x514, left);
        w_u8(current + 0x515, top);
        w_u8(current + 0x51C, right);
        w_u8(current + 0x51D, top);
        w_u8(current + 0x524, left);
        w_u8(current + 0x525, bottom);
        w_u8(current + 0x52C, right);
        w_u8(current + 0x52D, bottom);
        w_u8(current + 0x53C, left);
        w_u8(current + 0x53D, top);
        w_u8(current + 0x544, right);
        w_u8(current + 0x545, top);
        w_u8(current + 0x54C, left);
        w_u8(current + 0x54D, bottom);
        w_u8(current + 0x554, right);
        w_u8(current + 0x555, bottom);
        mode = parameter8 & 0xFFFF;
        page = (((mode ? mode - 1 : 1) & 3) << 5) | ((parameter3 & 0x100) >> 4)
            | ((parameter2 & 0x3C0) >> 6) | ((parameter3 & 0x200) << 2);
        w_u16(current + 0x51E, page);
        w_u16(current + 0x546, page);
        if (mode)
        {
            moh_draft_missing_call(0x8001BA38, packet, 1);
            moh_draft_missing_call(0x8001BA38, second, 1);
        }
        page = moh_draft_missing_call(0x8001BA18, parameter6 & 0xFFFF, parameter7 & 0xFFFF);
        w_u16(current + 0x516, page);
        page = moh_draft_missing_call(0x8001BA18, parameter6 & 0xFFFF, parameter7 & 0xFFFF);
        offset += 0x50;
        w_u16(current + 0x53E, page);
        current += 0x50;
    } while ((sint32)current < (sint32)(data + 0x640));
}
