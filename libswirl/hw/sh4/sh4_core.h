/*
	This file is part of libswirl
*/
#include "license/bsd"


#pragma once
#include "types.h"
#include "sh4_if.h"


#define r Sh4cntx.r
#define r_bank Sh4cntx.r_bank
#define gbr Sh4cntx.gbr
#define ssr Sh4cntx.ssr
#define spc Sh4cntx.spc
#define sgr Sh4cntx.sgr
#define dbr Sh4cntx.dbr
#define vbr Sh4cntx.vbr
#define mac Sh4cntx.mac
#define pr Sh4cntx._pr
#define fpul Sh4cntx.fpul
#define next_pc Sh4cntx.pc
#define curr_pc (next_pc-2)
#define sr Sh4cntx.sr
#define fpscr Sh4cntx.fpscr
#define old_sr Sh4cntx.old_sr
#define old_fpscr Sh4cntx.old_fpscr
#define fr (&Sh4cntx.xffr[16])
#define xf Sh4cntx.xffr
#define fr_hex ((u32*)fr)
#define xf_hex ((u32*)xf)
#define dr_hex ((u64*)fr)
#define xd_hex ((u64*)xf)
#define sh4_int_bCpuRun Sh4cntx.CpuRunning



void UpdateFPSCR();
bool UpdateSR();

union DoubleReg
{
	f64 dbl;
	f32 sgl[2];
};

static INLINE f64 GetDR(u32 n)
{
#ifdef TRACE
	if (n>7)
		printf("DR_r INDEX OVERRUN %d >7",n);
#endif
	DoubleReg t;

	t.sgl[1]=fr[(n<<1) + 0];
	t.sgl[0]=fr[(n<<1) + 1];

	return t.dbl;
}

static INLINE f64 GetXD(u32 n)
{
#ifdef TRACE
	if (n>7)
		printf("XD_r INDEX OVERRUN %d >7",n);
#endif
	DoubleReg t;

	t.sgl[1]=xf[(n<<1) + 0];
	t.sgl[0]=xf[(n<<1) + 1];

	return t.dbl;
}

static INLINE void SetDR(u32 n,f64 val)
{
#ifdef TRACE
	if (n>7)
		printf("DR_w INDEX OVERRUN %d >7",n);
#endif
	DoubleReg t;
	t.dbl=val;


	fr[(n<<1) | 1]=t.sgl[0];
	fr[(n<<1) | 0]=t.sgl[1];
}

static INLINE void SetXD(u32 n,f64 val)
{
#ifdef TRACE
	if (n>7)
		printf("XD_w INDEX OVERRUN %d >7",n);
#endif

	DoubleReg t;
	t.dbl=val;

	xf[(n<<1) | 1]=t.sgl[0];
	xf[(n<<1) | 0]=t.sgl[1];
}
//needs to be removed
u32* Sh4_int_GetRegisterPtr(Sh4RegType reg);
//needs to be made portable
void SetFloatStatusReg();


bool Do_Interrupt(u32 intEvn);
bool Do_Exception(u32 epc, u32 expEvn, u32 CallVect);

struct SH4ThrownException {
	u32 epc;
	u32 expEvn;
	u32 callVect;
};

static INLINE void RaiseFPUDisableException()
{
#if !defined(NO_MMU)
	if (settings.dreamcast.FullMMU)
	{
		SH4ThrownException ex = { next_pc - 2, 0x800, 0x100 };
		throw ex;
	}
#else
	msgboxf("Full MMU support needed", MBX_ICONERROR);
#endif
}

// The SH4 sets the signaling bit to 0 for qNaN (unlike all recent CPUs). Some games relies on this.
static INLINE f32 fixNaN(f32 f)
{
//	u32& hex = *(u32 *)&f;
//	// no fast-math
//	if (f != f)
//		hex = 0x7fbfffff;
//	// fast-math
//	if ((hex & 0x7fffffff) > 0x7f800000)
//		hex = 0x7fbfffff;
	return f;
}

static INLINE f64 fixNaN64(f64 f)
{
	// no fast-math
//	return f == f ? f : 0x7ff7ffffffffffffll;
	// fast-math
//	return (*(u64 *)&f & 0x7fffffffffffffffll) <= 0x7f80000000000000ll ? f : 0x7ff7ffffffffffffll;
	return f;
}
