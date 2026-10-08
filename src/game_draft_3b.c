#include "game_draft_signatures.h"
#include <stdint.h>

/* Unverified draft from full original pseudocode */
uint32 sub_80026AD8(void)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */


  sint32 v4; 
  sint32 v5; 
  unsigned char *v6; 
  sint32 v7; 
  unsigned char *v8; 
  sint32 v9; 
  sint32 *v10; 
  sint32 k; 
  char v12; 
  sint32 result; 
  sint32 v14; 
  sint32 *v15; 
  sint32 m; 
  char v17; 
  sint32 v18; 
  sint32 *v19; 
  sint32 n; 
  char v21; 
  char v22; 
  sint32 v23; 
  sint32 *v24; 
  sint32 j; 
  char v26; 
  char v27; 
  sint32 v28; 
  sint32 *v29; 
  sint32 i; 
  char v31; 
  sint32 v32; 
  sint32 *v33; 
  sint32 ii; 
  char v35; 
  sint32 v36; 
  sint32 *v37; 
  sint32 jj; 
  char v39; 
  sint32 v40; 
  sint32 *v41; 
  sint32 kk; 
  char v43; 
  sint32 v44; 
  sint32 *v45; 
  sint32 mm; 
  char v47; 
  sint32 v50; 
  uint32 response[2];
#define v52 response[0]
#define v53 response[1] 
 

  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)))), 1u) = 1;
  (*(unsigned char *)&v50) = *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x8003530Cu, sizeof(sint32)))), 1u) & 7;
  v4 = 0;
  if ( !(unsigned char)v50 )
    return 0;
  while ( 1 )
  {
    v5 = 0;
    if ( (unsigned char)v50 == (*(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x8003530Cu, sizeof(sint32)))), 1u) & 7) )
      break;
    (*(unsigned char *)&v50) = *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x8003530Cu, sizeof(sint32)))), 1u) & 7;
  }
  do
  {
    v6 = (char *)&v52 + v5;
    if ( (*(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)))), 1u) & 0x20) == 0 )
      break;
    ++v5;
    *v6 = *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035314u, sizeof(sint32)))), 1u);
  }
  while ( v5 < 8 );
  v7 = v5;
  if ( v5 < 8 )
  {
    v8 = (char *)&v52 + v5;
    do
    {
      *v8 = 0;
      ++v7;
      v8 = (char *)&v52 + v7;
    }
    while ( v7 < 8 );
  }
  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)))), 1u) = 1;
  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x8003530Cu, sizeof(sint32)))), 1u) = 7;
  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035318u, sizeof(sint32)))), 1u) = 7;
  if ( (unsigned char)v50 != 3 || ((sint32 *)moh_draft_guest_pointer(0x80035208u, 1u))[(unsigned char)(*(unsigned char *)moh_draft_guest_pointer(0x80035061u, sizeof(unsigned char)))] )
  {
    if ( ((*(sint32 *)moh_draft_guest_pointer(0x80035050u, sizeof(sint32))) & 0x10) == 0 && (v52 & 0x10) != 0 )
      ++(*(sint32 *)moh_draft_guest_pointer(0x80035058u, sizeof(sint32)));
    v4 = v52 & 0x1D;
    (*(sint32 *)moh_draft_guest_pointer(0x80035050u, sizeof(sint32))) = (unsigned char)v52;
    (*(sint32 *)moh_draft_guest_pointer(0x80035054u, sizeof(sint32))) = (((uint32)v52 >> 8) & 255u);
  }
  if ( (unsigned char)v50 == 5 && (*(sint32 *)moh_draft_guest_pointer(0x8003504Cu, sizeof(sint32))) >= 3 )
  {
    moh_draft_missing_call(0x80015B28u, (sint32)0x800151A4u);
    if ( (*(sint32 *)moh_draft_guest_pointer(0x8003504Cu, sizeof(sint32))) >= 3 )
      moh_draft_missing_call(0x80015B28u, (sint32)0x800151B0u, (sint32)((uint32 *)moh_draft_guest_pointer(0x80035068u, 1u))[(unsigned char)(*(unsigned char *)moh_draft_guest_pointer(0x80035061u, sizeof(unsigned char)))], (*(sint32 *)moh_draft_guest_pointer(0x80035050u, sizeof(sint32))), (*(sint32 *)moh_draft_guest_pointer(0x80035054u, sizeof(sint32))));
  }
  switch ( (char)v50 )
  {
    case 1:
      if ( v4 && v5 == 1 )
        v4 = 0;
      v27 = 1;
      if ( v4 )
        v27 = 5;
      (*(unsigned char *)moh_draft_guest_pointer(0x80035321u, sizeof(unsigned char))) = v27;
      v28 = -2146628308;
      v29 = &v52;
      for ( i = 7; i != -1; --i )
      {
        v31 = *(unsigned char *)v29;
        v29 = (sint32 *)((char *)v29 + 1);
        *(unsigned char *)moh_draft_guest_pointer((uint32)(v28++), 1u) = v31;
      }
      *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)))), 1u) = 0;
      result = 4;
      *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x8003530Cu, sizeof(sint32)))), 1u) = 0;
      break;
    case 2:
      v22 = 2;
      if ( v4 )
        v22 = 5;
      (*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char))) = v22;
      v23 = -2146628316;
      v24 = &v52;
      for ( j = 7; j != -1; --j )
      {
        v26 = *(unsigned char *)v24;
        v24 = (sint32 *)((char *)v24 + 1);
        *(unsigned char *)moh_draft_guest_pointer((uint32)(v23++), 1u) = v26;
      }
      result = 2;
      break;
    case 3:
      if ( v4 )
      {
        (*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char))) = 5;
        v9 = -2146628316;
        v10 = &v52;
        for ( k = 7; k != -1; --k )
        {
          v12 = *(unsigned char *)v10;
          v10 = (sint32 *)((char *)v10 + 1);
          *(unsigned char *)moh_draft_guest_pointer((uint32)(v9++), 1u) = v12;
        }
        result = 2;
      }
      else if ( ((sint32 *)moh_draft_guest_pointer(0x80035108u, 1u))[(unsigned char)(*(unsigned char *)moh_draft_guest_pointer(0x80035061u, sizeof(unsigned char)))] )
      {
        (*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char))) = 3;
        v14 = -2146628316;
        v15 = &v52;
        for ( m = 7; m != -1; --m )
        {
          v17 = *(unsigned char *)v15;
          v15 = (sint32 *)((char *)v15 + 1);
          *(unsigned char *)v14++ = v17;
        }
        result = 1;
      }
      else
      {
        (*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char))) = 2;
        v18 = -2146628316;
        v19 = &v52;
        for ( n = 7; n != -1; --n )
        {
          v21 = *(unsigned char *)v19;
          v19 = (sint32 *)((char *)v19 + 1);
          *(unsigned char *)moh_draft_guest_pointer((uint32)(v18++), 1u) = v21;
        }
        result = 2;
      }
      break;
    case 4:
      v32 = -2146628300;
      (*(unsigned char *)moh_draft_guest_pointer(0x80035322u, sizeof(unsigned char))) = 4;
      v33 = &v52;
      (*(unsigned char *)moh_draft_guest_pointer(0x80035321u, sizeof(unsigned char))) = 4;
      for ( ii = 7; ii != -1; --ii )
      {
        v35 = *(unsigned char *)v33;
        v33 = (sint32 *)((char *)v33 + 1);
        *(unsigned char *)moh_draft_guest_pointer((uint32)(v32++), 1u) = v35;
      }
      v36 = -2146628308;
      v37 = &v52;
      for ( jj = 7; jj != -1; --jj )
      {
        v39 = *(unsigned char *)v37;
        v37 = (sint32 *)((char *)v37 + 1);
        *(unsigned char *)moh_draft_guest_pointer((uint32)(v36++), 1u) = v39;
      }
      result = 4;
      break;
    case 5:
      v40 = -2146628316;
      (*(unsigned char *)moh_draft_guest_pointer(0x80035321u, sizeof(unsigned char))) = 5;
      v41 = &v52;
      (*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char))) = 5;
      for ( kk = 7; kk != -1; --kk )
      {
        v43 = *(unsigned char *)v41;
        v41 = (sint32 *)((char *)v41 + 1);
        *(unsigned char *)moh_draft_guest_pointer((uint32)(v40++), 1u) = v43;
      }
      v44 = -2146628308;
      v45 = &v52;
      for ( mm = 7; mm != -1; --mm )
      {
        v47 = *(unsigned char *)v45;
        v45 = (sint32 *)((char *)v45 + 1);
        *(unsigned char *)moh_draft_guest_pointer((uint32)(v44++), 1u) = v47;
      }
      result = 6;
      break;
    default:
      moh_draft_missing_call(0x800224B0u, 0x800151CCu);
      moh_draft_missing_call(0x80015B28u, (sint32)0x800151E0u, (unsigned char)v50);
      return 0;
  }
  return result;
}
#undef v52
#undef v53

/* Unverified draft from full original pseudocode */
uint32 sub_80025528(uint32 a1)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */


  sint32 v2; 
  sint32 v3; 
  sint32 v4; 
  char v5; 
  sint32 result; 
  sint32 v7; 
  sint32 v8; 
  short v9; 
  sint32 v10; 
  sint32 v11; 
  sint32 v12; 
  sint32 v13; 
  sint32 *v14; 
  sint32 v15; 
  sint32 v16; 
  sint32 v17; 
  sint32 v18; 
  sint32 v19; 
  unsigned char v20; 
  sint32 v21; 
  sint32 v22; 
  short v23; 
  sint32 v24; 
  sint32 v25; 
  sint32 v26; 
  unsigned char v27; 
  sint32 v28; 
  sint32 v29; 
  short v30; 
  sint32 v31; 
  sint32 v32; 
  sint32 v33; 

  if ( (*(sint32 *)moh_draft_guest_pointer(0x800347B8u, sizeof(sint32))) )
    moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x80034740u, sizeof(sint32))), a1);
  v2 = (*(sint32 *)moh_draft_guest_pointer(0x800347B8u, sizeof(sint32)));
  v3 = -1;
  if ( !(*(sint32 *)moh_draft_guest_pointer(0x800347B8u, sizeof(sint32))) )
  {
LABEL_15:
    v12 = 0;
    v13 = (*(sint32 *)moh_draft_guest_pointer(0x80034774u, sizeof(sint32))) == 0;
    if ( (*(sint32 *)moh_draft_guest_pointer(0x800347B0u, sizeof(sint32))) < 2 )
    {
LABEL_36:
      --(*(sint32 *)moh_draft_guest_pointer(0x800347B0u, sizeof(sint32)));
      v26 = a1;
      if ( (*(sint32 *)moh_draft_guest_pointer(0x800347B0u, sizeof(sint32))) <= 0 )
      {
LABEL_46:
        while ( (*(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32))) + 4)), 1u) & 2) == 0 )
          ;
        v33 = *(unsigned char *)moh_draft_guest_pointer((uint32)((a1 + 68)), 1u);
        *(unsigned char *)moh_draft_guest_pointer((uint32)((a1 + 68)), 1u) = v33 + 1;
        *(unsigned char *)moh_draft_guest_pointer((uint32)((v33 + *(uint32 *)(a1 + 60))), 1u) = *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32)))), 1u);
        moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x80034734u, sizeof(sint32))), 0);
        return 0;
      }
      else
      {
        while ( 1 )
        {
          v27 = moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x8003473Cu, sizeof(sint32))), v26, v2);
          result = sub_80024328(a1, v27);
          if ( result < 0 )
            break;
          v28 = (*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32)));
          if ( *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32))) + 14)), 1u) != 34 )
          {
            v29 = (*(sint32 *)moh_draft_guest_pointer(0x1F801120u, 4u));
            *(uint32 *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x800347A8u, sizeof(sint32)))), 1u) = -129;
            v30 = *(unsigned short *)moh_draft_guest_pointer((uint32)((v28 + 4)), 1u);
            (*(sint32 *)moh_draft_guest_pointer(0x800D0A94u, 4u)) = 60;
            (*(sint32 *)moh_draft_guest_pointer(0x800D0A90u, 4u)) = v29;
            if ( (v30 & 0x80) != 0 )
            {
              while ( 1 )
              {
                v31 = sub_800251F8();
                v32 = 0;
                if ( v31 )
                  break;
                if ( (*(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32))) + 4)), 1u) & 0x80) == 0 )
                  goto LABEL_42;
              }
            }
            else
            {
LABEL_42:
              v32 = 1;
              *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32))) + 10)), 1u) |= 0x10u;
            }
            if ( !v32 )
              return -3;
          }
          --(*(sint32 *)moh_draft_guest_pointer(0x800347B0u, sizeof(sint32)));
          v26 = a1;
          if ( (*(sint32 *)moh_draft_guest_pointer(0x800347B0u, sizeof(sint32))) <= 0 )
            goto LABEL_46;
        }
      }
      return result;
    }
    v14 = (sint32 *)moh_draft_guest_pointer(r_u32(0x8003478Cu + 4u * (uint32)v13), sizeof(sint32));
    v15 = 240 * v13;
    while ( 1 )
    {
      v16 = *v14;
      if ( *v14 < 0 )
        goto LABEL_36;
      if ( v16 > 0 )
      {
        v12 = *(uint32 *)moh_draft_guest_pointer((uint32)((v15 + (*(sint32 *)moh_draft_guest_pointer(0x80034768u, sizeof(sint32))) + 12)), 1u) + 240 * v16 - 240;
        moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x80034754u, sizeof(sint32))), v12);
      }
      v17 = *v14;
      if ( *v14 == 3 )
        break;
      if ( v17 >= 4 )
      {
        v18 = a1;
        if ( v17 == 4 )
          *v14 = 3;
        goto LABEL_29;
      }
      v18 = a1;
      if ( v17 < 2 && v17 >= 0 )
      {
        v12 = (*(sint32 *)moh_draft_guest_pointer(0x80034768u, sizeof(sint32))) + v15;
        moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x80034754u, sizeof(sint32))), (*(sint32 *)moh_draft_guest_pointer(0x80034768u, sizeof(sint32))) + v15);
        moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x80034758u, sizeof(sint32))), v12);
        v19 = -1;
LABEL_28:
        *v14 = v19;
        v18 = a1;
      }
LABEL_29:
      v20 = moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x8003473Cu, sizeof(sint32))), v18, v2);
      result = sub_80024328(a1, v20);
      if ( result < 0 )
        return result;
      v21 = (*(sint32 *)moh_draft_guest_pointer(0x1F801120u, 4u));
      v22 = (*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32)));
      *(uint32 *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x800347A8u, sizeof(sint32)))), 1u) = -129;
      v23 = *(unsigned short *)moh_draft_guest_pointer((uint32)((v22 + 4)), 1u);
      (*(sint32 *)moh_draft_guest_pointer(0x800D0A94u, 4u)) = 60;
      (*(sint32 *)moh_draft_guest_pointer(0x800D0A90u, 4u)) = v21;
      if ( (v23 & 0x80) != 0 )
      {
        while ( 1 )
        {
          v24 = sub_800251F8();
          v25 = 0;
          if ( v24 )
            break;
          if ( (*(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32))) + 4)), 1u) & 0x80) == 0 )
            goto LABEL_33;
        }
      }
      else
      {
LABEL_33:
        v25 = 1;
        *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32))) + 10)), 1u) |= 0x10u;
      }
      result = -3;
      if ( !v25 )
        return result;
      if ( --(*(sint32 *)moh_draft_guest_pointer(0x800347B0u, sizeof(sint32))) < 2 )
        goto LABEL_36;
    }
    moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x80034754u, sizeof(sint32))), v12 - 240);
    v19 = 1;
    goto LABEL_28;
  }
  v4 = -240;
  while ( 1 )
  {
    if ( --(*(sint32 *)moh_draft_guest_pointer(0x800347B0u, sizeof(sint32))) <= 0 )
      goto LABEL_15;
    if ( v3 >= 0 )
      moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x80034740u, sizeof(sint32))), *(uint32 *)moh_draft_guest_pointer((uint32)((a1 + 12)), 1u) + v4);
    v5 = moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x8003473Cu, sizeof(sint32))), a1, 1);
    result = sub_8002454C(a1, v5);
    if ( result < 0 )
      return result;
    v7 = (*(sint32 *)moh_draft_guest_pointer(0x1F801120u, 4u));
    v8 = (*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32)));
    *(uint32 *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x800347A8u, sizeof(sint32)))), 1u) = -129;
    v9 = *(unsigned short *)moh_draft_guest_pointer((uint32)((v8 + 4)), 1u);
    (*(sint32 *)moh_draft_guest_pointer(0x800D0A94u, 4u)) = 60;
    (*(sint32 *)moh_draft_guest_pointer(0x800D0A90u, 4u)) = v7;
    if ( (v9 & 0x80) != 0 )
    {
      while ( 1 )
      {
        v10 = sub_800251F8();
        v11 = 0;
        if ( v10 )
          break;
        if ( (*(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32))) + 4)), 1u) & 0x80) == 0 )
          goto LABEL_12;
      }
    }
    else
    {
LABEL_12:
      v11 = 1;
      *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800347ACu, sizeof(sint32))) + 10)), 1u) |= 0x10u;
    }
    ++v3;
    if ( !v11 )
      return -3;
    v4 += 240;
    if ( v3 >= 4 )
      goto LABEL_15;
  }
}

/* Unverified draft from full original pseudocode */
uint32 sub_80027584(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */
    unsigned char *p_a2 = (unsigned char *)moh_draft_guest_pointer(a2, 1u);

  sint32 v8; 
  sint32 result; 
  sint32 v10; 
  sint32 v11; 
  char *v12; 
  sint32 *v13; 
  sint32 v14; 
  char *v15; 
  unsigned char *v16; 
  sint32 v17; 
  sint32 v18; 
  sint32 v19; 
  char v24; 
  sint32 v25; 
  char v26; 
  sint32 v27; 
  sint32 v28; 
  sint32 v29; 
  sint32 v30; 
  char v31; 

  if ( (*(sint32 *)moh_draft_guest_pointer(0x8003504Cu, sizeof(sint32))) >= 2 )
    moh_draft_missing_call(0x80015B28u, (sint32)0x80015214u, (sint32)((uint32 *)moh_draft_guest_pointer(0x80035068u, 1u))[a1]);
  v8 = a1;
  if ( !((sint32 *)moh_draft_guest_pointer(0x80035288u, 1u))[v8] || p_a2 )
  {
    sub_8002703C(0, 0);
    v10 = a1;
    if ( a1 == 2 )
    {
      v11 = 0;
      v12 = p_a2;
      do
      {
        ((unsigned char *)moh_draft_guest_pointer(0x8003505Cu, 1u))[v11++] = *v12;
        v12 = &p_a2[v11];
      }
      while ( v11 < 4 );
      v10 = a1;
    }
    if ( v10 == 14 )
      (*(unsigned char *)moh_draft_guest_pointer(0x80035060u, sizeof(unsigned char))) = *p_a2;
    (*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char))) = 0;
    if ( ((sint32 *)moh_draft_guest_pointer(0x80035188u, 1u))[v10] )
      (*(unsigned char *)moh_draft_guest_pointer(0x80035321u, sizeof(unsigned char))) = 0;
    *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)))), 1u) = 0;
    v13 = (sint32 *)moh_draft_guest_pointer(r_u32(0x80035288u + 4u * (uint32)v10), sizeof(sint32));
    v14 = 0;
    if ( *v13 > 0 )
    {
      v15 = p_a2;
      do
      {
        *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035318u, sizeof(sint32)))), 1u) = *v15;
        v15 = &p_a2[++v14];
      }
      while ( v14 < *v13 );
    }
    (*(unsigned char *)moh_draft_guest_pointer(0x80035061u, sizeof(unsigned char))) = a1;
    *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035314u, sizeof(sint32)))), 1u) = a1;
    result = 0;
    if ( !a4 )
    {
      (*(sint32 *)moh_draft_guest_pointer(0x800D0D3Cu, 4u)) = moh_draft_missing_call(0x80015DB8u, -1) + 960;
      (*(sint32 *)moh_draft_guest_pointer(0x800D0D40u, 4u)) = 0;
      (*(sint32 *)moh_draft_guest_pointer(0x800D0D44u, 4u)) = 0x8001522Cu;
      v16 = (unsigned char *)moh_draft_guest_pointer((uint32)(a3), 1u);
      if ( (*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char))) )
      {
LABEL_37:
        v29 = -2146628316;
        v30 = 7;
        if ( v16 )
        {
          do
          {
            v31 = *(unsigned char *)moh_draft_guest_pointer((uint32)(v29++), 1u);
            --v30;
            *v16++ = v31;
          }
          while ( v30 != -1 );
        }
        result = 0;
        if ( (*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char))) == 5 )
          return -1;
      }
      else
      {
        while ( 1 )
        {
          if ( (*(sint32 *)moh_draft_guest_pointer(0x800D0D3Cu, 4u)) >= (sint32)moh_draft_missing_call(0x80015DB8u, -1)
            && (v17 = (*(sint32 *)moh_draft_guest_pointer(0x800D0D40u, 4u)), ++(*(sint32 *)moh_draft_guest_pointer(0x800D0D40u, 4u)), v17 <= 3932160) )
          {
            v18 = 0;
          }
          else
          {
            moh_draft_missing_call(0x800224B0u, 0x80015178u);
            moh_draft_missing_call(0x80015B28u, (sint32)0x80015188u, (*(sint32 *)moh_draft_guest_pointer(0x800D0D44u, 4u)), (sint32)((uint32 *)moh_draft_guest_pointer(0x80035068u, 1u))[(unsigned char)(*(unsigned char *)moh_draft_guest_pointer(0x80035061u, sizeof(unsigned char)))], (sint32)((uint32 *)moh_draft_guest_pointer(0x800350E8u, 1u))[(unsigned char)(*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char)))], (sint32)((uint32 *)moh_draft_guest_pointer(0x800350E8u, 1u))[(unsigned char)(*(unsigned char *)moh_draft_guest_pointer(0x80035321u, sizeof(unsigned char)))]);
            sub_80027A18();
            v18 = -1;
          }
          v19 = v18 != 0;
          result = -1;
          if ( v19 )
            break;
          if ( sub_8001CB9C() )
          {
            v24 = *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)))), 1u) & 3;
            while ( 1 )
            {
              v25 = sub_80026AD8();
              v26 = v25;
              v19 = v25 == 0;
              v27 = v25 & 4;
              if ( v19 )
                break;
              v19 = v27 == 0;
              v28 = v26 & 2;
              if ( !v19 )
              {
                if ( (*(sint32 *)moh_draft_guest_pointer(0x80035048u, sizeof(sint32))) )
                  moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x80035048u, sizeof(sint32))), (unsigned char)(*(unsigned char *)moh_draft_guest_pointer(0x80035321u, sizeof(unsigned char))), -2146628308);
                v28 = v26 & 2;
              }
              if ( v28 && (*(sint32 *)moh_draft_guest_pointer(0x80035044u, sizeof(sint32))) )
                moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x80035044u, sizeof(sint32))), (unsigned char)(*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char))), -2146628316);
            }
            *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)))), 1u) = v24;
          }
          v16 = (unsigned char *)moh_draft_guest_pointer((uint32)(a3), 1u);
          if ( (*(unsigned char *)moh_draft_guest_pointer(0x80035320u, sizeof(unsigned char))) )
            goto LABEL_37;
        }
      }
    }
  }
  else
  {
    result = -2;
    if ( (*(sint32 *)moh_draft_guest_pointer(0x8003504Cu, sizeof(sint32))) > 0 )
    {
      moh_draft_missing_call(0x80015B28u, (sint32)0x8001521Cu, (sint32)((uint32 *)moh_draft_guest_pointer(0x80035068u, 1u))[v8]);
      return -2;
    }
  }
  return result;
}

/* Unverified draft from full original pseudocode */
uint32 sub_8002307C(void)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */


  sint32 v0; 
  sint32 result; 
  sint32 v2; 
  char v3; 
  char v4[8]; 

  v0 = (*(sint32 *)moh_draft_guest_pointer(0x80034618u, sizeof(sint32))) - 1;
  if ( (*(sint32 *)moh_draft_guest_pointer(0x80034618u, sizeof(sint32))) > 0 )
  {
    --(*(sint32 *)moh_draft_guest_pointer(0x80034618u, sizeof(sint32)));
    if ( !v0 )
      return moh_draft_missing_call(0x8002334Cu);
  }
  if ( (*(sint32 *)moh_draft_guest_pointer(0x80034614u, sizeof(sint32))) > 0 )
    --(*(sint32 *)moh_draft_guest_pointer(0x80034614u, sizeof(sint32)));
  if ( (*(sint32 *)moh_draft_guest_pointer(0x80034620u, sizeof(sint32))) > 0 )
    --(*(sint32 *)moh_draft_guest_pointer(0x80034620u, sizeof(sint32)));
  if ( ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] == 1 || ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] != 2 || (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) == 11 )
    goto LABEL_45;
  if ( (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) == 12 )
  {
    if ( (*(sint32 *)moh_draft_guest_pointer(0x80034728u, sizeof(sint32))) )
    {
      v4[0] = 0;
      if ( (*(sint32 *)moh_draft_guest_pointer(0x80034618u, sizeof(sint32))) <= 0 )
      {
        (*(sint32 *)moh_draft_guest_pointer(0x800345F0u, sizeof(sint32))) = 32;
        sub_80022EA4(14, (sint32)v4);
      }
      (*(sint32 *)moh_draft_guest_pointer(0x80034728u, sizeof(sint32))) = 0;
    }
    else
    {
      if ( (*(sint32 *)moh_draft_guest_pointer(0x80034618u, sizeof(sint32))) <= 0 )
      {
        (*(sint32 *)moh_draft_guest_pointer(0x800345F0u, sizeof(sint32))) = 32;
        sub_80022EA4(1, 0);
      }
      (*(sint32 *)moh_draft_guest_pointer(0x80034728u, sizeof(sint32))) = 1;
    }
    goto LABEL_45;
  }
  if ( (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) == 13 )
  {
    v2 = (*(sint32 *)moh_draft_guest_pointer(0x80034618u, sizeof(sint32)));
    (*(sint32 *)moh_draft_guest_pointer(0x80034728u, sizeof(sint32))) = 0;
    goto LABEL_42;
  }
  if ( (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) != 14 )
  {
    switch ( (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) )
    {
      case 15:
        if ( (*(sint32 *)moh_draft_guest_pointer(0x80034614u, sizeof(sint32))) )
          goto LABEL_45;
        ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] = 1;
        goto LABEL_40;
      case 16:
        if ( (unsigned char)(*(sint32 *)moh_draft_guest_pointer(0x80034608u, sizeof(sint32))) )
        {
          ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] = 1;
LABEL_40:
          (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) = 11;
          goto LABEL_45;
        }
        break;
      case 17:
        if ( MOH_BYTE2((*(sint32 *)moh_draft_guest_pointer(0x80034608u, sizeof(sint32)))) )
        {
          ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] = 1;
          goto LABEL_40;
        }
        break;
      default:
        goto LABEL_45;
    }
LABEL_41:
    v2 = (*(sint32 *)moh_draft_guest_pointer(0x80034618u, sizeof(sint32)));
LABEL_42:
    if ( v2 > 0 )
      goto LABEL_45;
    goto LABEL_43;
  }
  switch ( (*(sint32 *)moh_draft_guest_pointer(0x800345FCu, sizeof(sint32))) )
  {
    case 21:
      goto LABEL_41;
    case 22:
      ++(*(sint32 *)moh_draft_guest_pointer(0x8003460Cu, sizeof(sint32)));
      if ( (*(sint32 *)moh_draft_guest_pointer(0x80034618u, sizeof(sint32))) > 0 )
        break;
LABEL_43:
      (*(sint32 *)moh_draft_guest_pointer(0x800345F0u, sizeof(sint32))) = 32;
      v3 = 1;
LABEL_44:
      sub_80022EA4(v3, 0);
      break;
    case 23:
      if ( (*(sint32 *)moh_draft_guest_pointer(0x80034618u, sizeof(sint32))) > 0 )
        break;
      (*(sint32 *)moh_draft_guest_pointer(0x800345F0u, sizeof(sint32))) = 32;
      v3 = 19;
      goto LABEL_44;
    case 24:
      goto LABEL_41;
  }
LABEL_45:
  if ( (*(sint32 *)moh_draft_guest_pointer(0x800D0A70u, 4u)) && (*(sint32 *)moh_draft_guest_pointer(0x800345D4u, sizeof(sint32))) )
    moh_draft_indirect_call(r_u32(0x800D0A70u));
  result = 3;
  if ( ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] == 1 && (result = 3, !MOH_HIBYTE((*(sint32 *)moh_draft_guest_pointer(0x80034608u, sizeof(sint32))))) || ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] == 3 )
  {
    result = 33;
    if ( (*(sint32 *)moh_draft_guest_pointer(0x80034618u, sizeof(sint32))) <= 0 )
    {
      (*(sint32 *)moh_draft_guest_pointer(0x800345F0u, sizeof(sint32))) = 33;
      return sub_80022EA4(1, 0);
    }
  }
  return result;
}

/* Unverified draft from full original pseudocode */
uint32 sub_800236C8(uint32 a1, uint32 a2)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */


  sint32 v3; 
  sint32 result; 

  v3 = a1;
  if ( (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) == 12 )
  {
    (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) = 13;
    v3 = a1;
  }
  if ( v3 == 2 )
  {
    if ( (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) == 13 )
    {
      result = 14;
      if ( ((*(unsigned char *)moh_draft_guest_pointer(0x800345ECu, sizeof(unsigned char))) & 0x10) == 0 )
      {
        (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) = 14;
        ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] = 2;
        (*(sint32 *)moh_draft_guest_pointer(0x800345FCu, sizeof(sint32))) = 21;
        return ++(*(sint32 *)moh_draft_guest_pointer(0x80034610u, sizeof(sint32)));
      }
      return result;
    }
    if ( (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) == 14 )
    {
      if ( (*(sint32 *)moh_draft_guest_pointer(0x800345FCu, sizeof(sint32))) == 21 )
      {
        result = 22;
        if ( ((*(unsigned char *)moh_draft_guest_pointer(0x800345ECu, sizeof(unsigned char))) & 0x10) == 0 )
        {
          ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] = 2;
          (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) = 14;
          (*(sint32 *)moh_draft_guest_pointer(0x800345FCu, sizeof(sint32))) = 22;
          (*(sint32 *)moh_draft_guest_pointer(0x8003460Cu, sizeof(sint32))) = 0;
        }
        return result;
      }
      if ( (*(sint32 *)moh_draft_guest_pointer(0x800345FCu, sizeof(sint32))) == 22 )
      {
        result = 23;
        if ( ((*(unsigned char *)moh_draft_guest_pointer(0x800345ECu, sizeof(unsigned char))) & 2) == 0 )
        {
          result = (*(sint32 *)moh_draft_guest_pointer(0x8003460Cu, sizeof(sint32))) < 301;
          if ( (*(sint32 *)moh_draft_guest_pointer(0x8003460Cu, sizeof(sint32))) >= 301 )
          {
            result = 3;
            ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] = 3;
            if ( (*(sint32 *)moh_draft_guest_pointer(0x800D0A7Cu, 4u)) )
              return moh_draft_indirect_call(r_u32(0x800D0A7Cu), 5, a2);
          }
          return result;
        }
LABEL_17:
        ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] = 2;
        (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) = 14;
        (*(sint32 *)moh_draft_guest_pointer(0x800345FCu, sizeof(sint32))) = result;
        return result;
      }
      result = 24;
      if ( (*(sint32 *)moh_draft_guest_pointer(0x800345FCu, sizeof(sint32))) == 23 )
        goto LABEL_17;
      if ( (*(sint32 *)moh_draft_guest_pointer(0x800345FCu, sizeof(sint32))) == 24 )
      {
        result = (unsigned char)(*(unsigned char *)moh_draft_guest_pointer(0x800345ECu, sizeof(unsigned char)));
        if ( (*(unsigned char *)moh_draft_guest_pointer(0x800345ECu, sizeof(unsigned char))) == 2 )
        {
          ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] = 1;
          result = 11;
          (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) = 11;
          (*(sint32 *)moh_draft_guest_pointer(0x800345FCu, sizeof(sint32))) = 0;
          if ( (*(sint32 *)moh_draft_guest_pointer(0x800D0A7Cu, 4u)) )
            return moh_draft_indirect_call(r_u32(0x800D0A7Cu), 2, a2);
        }
      }
    }
    else
    {
      result = (uint32)((*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) - 16) < 2;
      if ( (uint32)((*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) - 16) < 2 )
      {
        result = (*(sint32 *)moh_draft_guest_pointer(0x80034620u, sizeof(sint32)));
        if ( !(*(sint32 *)moh_draft_guest_pointer(0x80034620u, sizeof(sint32))) )
        {
          result = (*(unsigned char *)moh_draft_guest_pointer(0x800345ECu, sizeof(unsigned char))) & 2;
          if ( ((*(unsigned char *)moh_draft_guest_pointer(0x800345ECu, sizeof(unsigned char))) & 2) == 0 )
          {
            ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] = 1;
            (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) = 11;
            if ( (*(sint32 *)moh_draft_guest_pointer(0x800D0A78u, 4u)) && (*(sint32 *)moh_draft_guest_pointer(0x800345D4u, sizeof(sint32))) )
              moh_draft_indirect_call(r_u32(0x800D0A78u), 5, a2);
            result = (sint32)(*(sint32 *)moh_draft_guest_pointer(0x800D0A74u, 4u));
            if ( (*(sint32 *)moh_draft_guest_pointer(0x800D0A74u, 4u)) )
            {
              result = (*(sint32 *)moh_draft_guest_pointer(0x800345D4u, sizeof(sint32)));
              if ( (*(sint32 *)moh_draft_guest_pointer(0x800345D4u, sizeof(sint32))) )
                return moh_draft_indirect_call(r_u32(0x800D0A74u), 5, a2);
            }
          }
        }
      }
    }
  }
  else
  {
    result = (*(unsigned char *)moh_draft_guest_pointer(0x800345ECu, sizeof(unsigned char))) & 0x10;
    if ( ((*(unsigned char *)moh_draft_guest_pointer(0x800345ECu, sizeof(unsigned char))) & 0x10) != 0 )
    {
      ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] = 2;
      (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) = 12;
    }
    else
    {
      result = 1;
      if ( (uint32)((*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) - 16) < 2 )
      {
        ((sint32 *)moh_draft_guest_pointer(0x800345F4u, 1u))[0] = 1;
        result = 11;
        (*(sint32 *)moh_draft_guest_pointer(0x800345F8u, sizeof(sint32))) = 11;
      }
    }
  }
  return result;
}

/* Unverified draft from full original pseudocode */
uint32 sub_80021444(uint32 a1, uint32 a2, uint32 a3)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */


  sint32 result; 
  sint32 v4; 
  uint32 v5; 
  sint32 v6; 
  uint32 v7; 
  short v8; 
  uint32 v9; 
  sint32 v10; 

  if ( a1 == 1 )
  {
    v4 = *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32))) + 422)), 1u);
    (*(sint32 *)moh_draft_guest_pointer(0x8003030Cu, sizeof(sint32))) = 0;
    if ( v4 == (unsigned short)(*(short *)moh_draft_guest_pointer(0x800302D4u, sizeof(short))) )
    {
LABEL_13:
      *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32))) + 426)), 1u) = *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32))) + 426)), 1u) & 0xFFCF | 0x20;
      return 0;
    }
    else
    {
      v5 = 1;
      while ( 1 )
      {
        result = -2;
        if ( v5 >= 0xF01 )
          break;
        ++v5;
        if ( *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32))) + 422)), 1u) == (unsigned short)(*(short *)moh_draft_guest_pointer(0x800302D4u, sizeof(short))) )
          goto LABEL_13;
      }
    }
  }
  else if ( a1 >= 2 )
  {
    if ( a1 == 2 )
    {
      (*(short *)moh_draft_guest_pointer(0x800302D4u, sizeof(short))) = a2 >> (*(sint32 *)moh_draft_guest_pointer(0x800302E4u, sizeof(sint32)));
      *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32))) + 422)), 1u) = (*(short *)moh_draft_guest_pointer(0x800302D4u, sizeof(short)));
      return 0;
    }
    else
    {
      result = 0;
      if ( a1 == 3 )
      {
        v8 = 32;
        if ( (*(sint32 *)moh_draft_guest_pointer(0x8003030Cu, sizeof(sint32))) == 1 )
          v8 = 48;
        v9 = 1;
        if ( (*(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32))) + 426)), 1u) & 0x30) == v8 )
        {
LABEL_24:
          if ( (*(sint32 *)moh_draft_guest_pointer(0x8003030Cu, sizeof(sint32))) == 1 )
            moh_draft_missing_call(0x800218B0u);
          else
            sub_80021888();
          (*(sint32 *)moh_draft_guest_pointer(0x80030310u, sizeof(sint32))) = a2;
          (*(sint32 *)moh_draft_guest_pointer(0x80030314u, sizeof(sint32))) = (a3 >> 6) + ((a3 & 0x3F) != 0);
          *(uint32 *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x800302C0u, sizeof(sint32)))), 1u) = a2;
          *(uint32 *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x800302C4u, sizeof(sint32)))), 1u) = ((*(sint32 *)moh_draft_guest_pointer(0x80030314u, sizeof(sint32))) << 16) | 0x10;
          v10 = 16777729;
          if ( (*(sint32 *)moh_draft_guest_pointer(0x8003030Cu, sizeof(sint32))) == 1 )
            v10 = 16777728;
          *(uint32 *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x800302C8u, sizeof(sint32)))), 1u) = v10;
          return 0;
        }
        else
        {
          while ( 1 )
          {
            result = -2;
            if ( v9 >= 0xF01 )
              break;
            ++v9;
            if ( (*(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32))) + 426)), 1u) & 0x30) == v8 )
              goto LABEL_24;
          }
        }
      }
    }
  }
  else
  {
    result = 0;
    if ( !a1 )
    {
      v6 = *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32))) + 422)), 1u);
      (*(sint32 *)moh_draft_guest_pointer(0x8003030Cu, sizeof(sint32))) = 1;
      if ( v6 == (unsigned short)(*(short *)moh_draft_guest_pointer(0x800302D4u, sizeof(short))) )
      {
LABEL_18:
        *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32))) + 426)), 1u) |= 0x30u;
        return 0;
      }
      else
      {
        v7 = 1;
        while ( 1 )
        {
          result = -2;
          if ( v7 >= 0xF01 )
            break;
          ++v7;
          if ( *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32))) + 422)), 1u) == (unsigned short)(*(short *)moh_draft_guest_pointer(0x800302D4u, sizeof(short))) )
            goto LABEL_18;
        }
      }
    }
  }
  return result;
}

/* Unverified draft from full original pseudocode */
uint32 sub_8001804C(uint32 a1, uint32 a2)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */
    unsigned short *p_a1 = (unsigned short *)moh_draft_guest_pointer(a1, 1u);

  short v3; 
  short v4; 
  sint32 v5; 
  short v6; 
  sint32 v7; 
  short v8; 
  sint32 v9; 
  sint32 v10; 
  sint32 v11; 
  sint32 v12; 

  v3 = p_a1[2];
  if ( v3 < 0 )
  {
    v4 = 0;
  }
  else
  {
    v4 = (*(short *)moh_draft_guest_pointer(0x8002DF1Cu, sizeof(short))) - 1;
    if ( (*(short *)moh_draft_guest_pointer(0x8002DF1Cu, sizeof(short))) - 1 >= v3 )
      v4 = v3;
  }
  v5 = (short)p_a1[3];
  v6 = p_a1[3];
  p_a1[2] = v4;
  if ( v5 < 0 )
  {
    v8 = 0;
  }
  else
  {
    v7 = (*(short *)moh_draft_guest_pointer(0x8002DF1Eu, sizeof(short))) - 1 < v5;
    v8 = (*(short *)moh_draft_guest_pointer(0x8002DF1Eu, sizeof(short))) - 1;
    if ( !v7 )
      v8 = v6;
  }
  v9 = *p_a1 & 0x3F;
  p_a1[3] = v8;
  if ( v9 || (p_a1[2] & 0x3F) != 0 )
  {
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2B4u, 4u)) = 135045852;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2C4u, 4u)) = -436207616;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2B8u, 4u)) = -486539264;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2BCu, 4u)) = -452984833;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2C0u, 4u)) = -452984832;
    v10 = *(uint32 *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x8002E020u, sizeof(sint32)))), 1u);
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2CCu, 4u)) = a2 & 0xFFFFFF | 0x60000000;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2C8u, 4u)) = v10 & 0x7FF | (a2 >> 31 << 10) | 0xE1000000;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2D0u, 4u)) = *(uint32 *)moh_draft_guest_pointer((uint32)(p_a1), 1u);
    v11 = *((uint32 *)moh_draft_guest_pointer((uint32)(p_a1), 1u) + 1);
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2DCu, 4u)) = 0x3FFFFFF;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2D4u, 4u)) = v11;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2E0u, 4u)) = sub_800187DC(3) | 0xE3000000;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2E4u, 4u)) = sub_800187DC(4) | 0xE4000000;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2E8u, 4u)) = sub_800187DC(5) | 0xE5000000;
  }
  else
  {
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2B4u, 4u)) = 100663295;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2B8u, 4u)) = -436207616;
    v12 = *(uint32 *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x8002E020u, sizeof(sint32)))), 1u);
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2C0u, 4u)) = a2 & 0xFFFFFF | 0x2000000;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2BCu, 4u)) = v12 & 0x7FF | (a2 >> 31 << 10) | 0xE1000000;
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2C4u, 4u)) = *(uint32 *)moh_draft_guest_pointer((uint32)(p_a1), 1u);
    (*(sint32 *)moh_draft_guest_pointer(0x800CA2C8u, 4u)) = *((uint32 *)moh_draft_guest_pointer((uint32)(p_a1), 1u) + 1);
  }
  sub_80018794(-2146655564);
  return 0;
}

/* Unverified draft from full original pseudocode */
uint32 sub_8001CC9C(void)
{
    uint32 pending, index, callback, timeout;
    uint32 status = r_u32(0x8002F25Cu);
    uint32 mask = r_u32(0x8002F260u);
    if (!r_u16(0x8002E1D0u))
    {
        moh_draft_missing_call(0x80015B28u, 0x80014B20u, (uint32)r_u16(status));
        return moh_native_irq_return();
    }
    w_u16(0x8002E1D2u, 1u);
    while ((pending = r_u16(mask) & r_u16(0x8002E200u) & r_u16(status)) != 0u)
    {
        for (index = 0u; index < 11u && (pending & 0xFFFFu); ++index, pending >>= 1)
        {
            if (!(pending & 1u)) continue;
            w_u16(status, ~(1u << index));
            callback = r_u32(0x8002E1D4u + 4u * index);
            if (callback) moh_draft_indirect_call(callback);
        }
    }
    if (r_u16(status) & r_u16(mask))
    {
        timeout = r_u32(0x8002F268u);
        w_u32(0x8002F268u, timeout + 1u);
        if ((sint32)timeout >= 2049)
        {
            moh_draft_missing_call(0x80015B28u, 0x80014B3Cu,
                (uint32)r_u16(status), (uint32)r_u16(mask));
            w_u32(0x8002F268u, 0u);
            w_u16(status, 0u);
        }
    }
    else w_u32(0x8002F268u, 0u);
    w_u16(0x8002E1D2u, 0u);
    return moh_native_irq_return();
}

/* Unverified draft from full original pseudocode */
uint32 sub_80037A0C(uint32 a1, uint32 a2, uint32 a3)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */
    sint32 *p_a1 = (sint32 *)moh_draft_guest_pointer(a1, 1u);
    sint32 *p_a2 = (sint32 *)moh_draft_guest_pointer(a2, 1u);
    unsigned char *p_a3 = (unsigned char *)moh_draft_guest_pointer(a3, 1u);

  sint32 v5; 
  sint32 v6; 
  sint32 v7; 
  sint32 v8; 
  uint32 v9; 
  sint32 v10; 
  sint32 i; 
  char *v12; 
  uint32 result; 
  char v14[16]; 

  v5 = 0;
  if ( *p_a2 <= 0 )
    return 0;
  while ( 1 )
  {
    sub_80037370(1, *p_a1 + v5, -2147254896);
    v6 = 0;
    if ( ((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[0] )
      break;
LABEL_9:
    if ( ++v5 >= *p_a2 )
      return 0;
  }
  v7 = 33;
  while ( 1 )
  {
    v8 = (unsigned char)((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[v6 + 2]
       + ((unsigned char)((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[v6 + 3] << 8)
       + ((unsigned char)((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[v6 + 4] << 16)
       + ((unsigned char)((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[v6 + 5] << 24);
    v9 = (unsigned char)((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[v6 + 10]
       + ((unsigned char)((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[v6 + 11] << 8)
       + ((unsigned char)((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[v6 + 12] << 16)
       + ((unsigned char)((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[v6 + 13] << 24);
    v10 = v6 + (unsigned char)((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[v6 + 32] + 32;
    for ( i = 0; v10 >= v7; ++i )
    {
      v12 = (unsigned char *)moh_draft_guest_pointer(0x80037D90u + (uint32)v7++, 1u);
      v14[i] = *v12;
    }
    v14[i] = 0;
    if ( !moh_draft_missing_call(0x8001ACC8u, moh_draft_local_address(v14, sizeof(v14)), a3) )
      break;
    v6 += (unsigned char)((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[v6];
    v7 = v6 + 33;
    if ( !((unsigned char *)moh_draft_guest_pointer(0x80037D90u, 1u))[v6] )
      goto LABEL_9;
  }
  result = v9;
  *p_a1 = v8;
  *p_a2 = v9 >> 11;
  return result;
}

/* Unverified draft from full original pseudocode */
uint32 sub_80016EA0(uint32 a1, uint32 a2)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */
    short *p_a2 = (short *)moh_draft_guest_pointer(a2, 1u);

  sint32 result; 
  sint32 v4; 
  sint32 v5; 
  sint32 v6; 
  sint32 v7; 

  if ( (*(unsigned char *)moh_draft_guest_pointer(0x8002DF1Au, sizeof(unsigned char))) != 1 )
  {
    result = 2;
    if ( (*(unsigned char *)moh_draft_guest_pointer(0x8002DF1Au, sizeof(unsigned char))) != 2 )
      return result;
    moh_draft_indirect_call((*(uint32 *)moh_draft_guest_pointer(0x8002DF14u, sizeof(uint32))), (const char *)(*(sint32 *)moh_draft_guest_pointer(0x8001488Cu, sizeof(sint32))), a1);
    return moh_draft_indirect_call((*(uint32 *)moh_draft_guest_pointer(0x8002DF14u, sizeof(uint32))), "(%d,%d)-(%d,%d)\n", *p_a2, p_a2[1], p_a2[2], p_a2[3]);
  }
  v4 = p_a2[2];
  if ( (*(short *)moh_draft_guest_pointer(0x8002DF1Cu, sizeof(short))) < v4
    || (v5 = *p_a2, (*(short *)moh_draft_guest_pointer(0x8002DF1Cu, sizeof(short))) < v4 + v5)
    || (v6 = p_a2[1], (*(short *)moh_draft_guest_pointer(0x8002DF1Eu, sizeof(short))) < v6)
    || (v7 = p_a2[3], (result = (*(short *)moh_draft_guest_pointer(0x8002DF1Eu, sizeof(short))) < v6 + v7) != 0)
    || v4 <= 0
    || v5 < 0
    || v6 < 0
    || v7 <= 0 )
  {
    moh_draft_indirect_call((*(uint32 *)moh_draft_guest_pointer(0x8002DF14u, sizeof(uint32))), "%s:bad RECT", a1);
    return moh_draft_indirect_call((*(uint32 *)moh_draft_guest_pointer(0x8002DF14u, sizeof(uint32))), "(%d,%d)-(%d,%d)\n", *p_a2, p_a2[1], p_a2[2], p_a2[3]);
  }
  return result;
}

/* Unverified draft from full original pseudocode */
uint32 sub_8001ADD8(uint32 a1, uint32 a2)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */
    unsigned char *p_a2 = (unsigned char *)moh_draft_guest_pointer(a2, 1u);

  sint32 v2; 
  unsigned char *result; 

  v2 = (sint32)(a1 + 150u);
  result = p_a2;
  p_a2[2] = 16 * (v2 % 75 / 10) + v2 % 75 % 10;
  p_a2[1] = 16 * (v2 / 75 % 60 / 10) + v2 / 75 % 60 % 10;
  *p_a2 = 16 * (v2 / 75 / 60 / 10) + v2 / 75 / 60 % 10;
  return a2;
}

/* Unverified draft from full original pseudocode */
uint32 sub_80027AEC(void)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */


  unsigned short *v0; 

  v0 = (unsigned short *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x8003531Cu, sizeof(sint32)))), 1u);
  if ( !*(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x8003531Cu, sizeof(sint32))) + 440)), 1u) && !*(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x8003531Cu, sizeof(sint32))) + 442)), 1u) )
  {
    *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x8003531Cu, sizeof(sint32))) + 384)), 1u) = 0x3FFF;
    v0[193] = 0x3FFF;
    v0 = (unsigned short *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x8003531Cu, sizeof(sint32)))), 1u);
  }
  v0[216] = 0x3FFF;
  v0[217] = 0x3FFF;
  v0[213] = -16383;
  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)))), 1u) = 2;
  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035318u, sizeof(sint32)))), 1u) = 0x80;
  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x8003530Cu, sizeof(sint32)))), 1u) = 0;
  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035308u, sizeof(sint32)))), 1u) = 3;
  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035314u, sizeof(sint32)))), 1u) = 0x80;
  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x80035318u, sizeof(sint32)))), 1u) = 0;
  *(unsigned char *)moh_draft_guest_pointer((uint32)((*(sint32 *)moh_draft_guest_pointer(0x8003530Cu, sizeof(sint32)))), 1u) = 32;
  return 0;
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
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */


  sint32 v0; 
  uint32 v1; 

  if ( !(*(sint32 *)moh_draft_guest_pointer(0x8003030Cu, sizeof(sint32))) )
    sub_800218D8();
  v0 = (*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32)));
  *(unsigned short *)moh_draft_guest_pointer((uint32)(((*(sint32 *)moh_draft_guest_pointer(0x800302BCu, sizeof(sint32))) + 426)), 1u) &= 0xFFCFu;
  if ( (*(unsigned short *)moh_draft_guest_pointer((uint32)((v0 + 426)), 1u) & 0x30) != 0 )
  {
    v1 = 1;
    do
    {
      if ( v1 >= 0xF01 )
        break;
      ++v1;
    }
    while ( (*(unsigned short *)moh_draft_guest_pointer((uint32)((v0 + 426)), 1u) & 0x30) != 0 );
  }
  if ( (*(sint32 *)moh_draft_guest_pointer(0x800302F4u, sizeof(sint32))) )
    return moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x800302F4u, sizeof(sint32))), -268435456);
  else
    return moh_draft_missing_call(0x8002A8E0u, -268435447, 32);
}

/* Unverified draft from full original pseudocode */
uint32 sub_800253B8(uint32 a1)
{
    /* TODO Memory adapter lifetime, pointer widths and unresolved volatile carriers */


  char v2; 
  sint32 result; 

  if ( (*(sint32 *)moh_draft_guest_pointer(0x800347B4u, sizeof(sint32))) )
  {
    moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x8003474Cu, sizeof(sint32))), *(uint32 *)moh_draft_guest_pointer((uint32)((a1 + 12)), 1u) + 480);
    moh_draft_indirect_call((*(sint32 *)moh_draft_guest_pointer(0x8003474Cu, sizeof(sint32))), *(uint32 *)moh_draft_guest_pointer((uint32)((a1 + 12)), 1u) + 720);
  }
  v2 = 0;
  if ( !*(unsigned char *)moh_draft_guest_pointer((uint32)((a1 + 55)), 1u) )
    v2 = (*(sint32 *)moh_draft_guest_pointer(0x80034780u, sizeof(sint32)));
  result = sub_8002454C(a1, v2);
  if ( result >= 0 )
  {
    (*(sint32 *)moh_draft_guest_pointer(0x800347B0u, sizeof(sint32))) = 2 * (result & 0xF);
    result = 0;
    if ( !(*(sint32 *)moh_draft_guest_pointer(0x800347B0u, sizeof(sint32))) )
    {
      (*(sint32 *)moh_draft_guest_pointer(0x800347B0u, sizeof(sint32))) = 32;
      return 0;
    }
  }
  return result;
}
