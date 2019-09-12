/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include "cache.h"

/* AndeStar_V5_SPA_UMxxx_V0.1.18-20180525.pdf, page 167 */
#define NDS_MMSC_CFG				0xFC2	//Machine CSRs
#define NDS_MCACHE_CTL				0x7CA	//Machine CSRs
#define NDS_MCCTLBEGINADDR			0x7CB	//Machine CSRs
#define NDS_MCCTLCOMMAND			0x7CC	//Machine CSRs
#define NDS_MCCTLDATA				0x7CD	//Machine CSRs
#define NDS_UCCTLBEGINADDR			0x80B	//User CSRs
#define NDS_UCCTLCOMMAND			0x80C	//User CSRs
#define NDS_SCCTLDATA				0x9CD	//Supervisor CSRs

/* AndeStar CCTL Register Bit Field */
#define CCTL_SUEN_MSK				(1ULL << (8))		//NDS_MCACHE_CTL
#define CCTLCSR_MSK				(1ULL << (16))		//NDS_MMSC_CFG

/* AndeStar CCTL Command Machine mode */
#define CCTL_L1D_VA_INVAL			0	//Allow S/U mode
#define CCTL_L1D_VA_WB				1	//Allow S/U mode
#define CCTL_L1D_VA_WBINVAL			2	//Allow S/U mode
#define CCTL_L1D_VA_LOCK			3
#define CCTL_L1D_VA_UNLOCK			4
#define CCTL_L1D_WBINVAL_ALL			6
#define CCTL_L1D_WB_ALL				7
#define CCTL_L1I_VA_INVAL			8	//Allow S/U mode
#define CCTL_L1I_VA_LOCK			11
#define CCTL_L1I_VA_UNLOCK			12
#define CCTL_L1D_IX_INVAL			16
#define CCTL_L1D_IX_WB				17
#define CCTL_L1D_IX_WBINVAL			18
#define CCTL_L1D_IX_RTAG			19
#define CCTL_L1D_IX_RDATA			20
#define CCTL_L1D_IX_WTAG			21
#define CCTL_L1D_IX_WDATA			22
#define CCTL_L1D_INVAL_ALL			23
#define CCTL_L1I_IX_INVAL			24
#define CCTL_L1I_IX_RTAG			27
#define CCTL_L1I_IX_RDATA			28
#define CCTL_L1I_IX_WTAG			29
#define CCTL_L1I_IX_WDATA			30

#define MEMSET(s, c, n) __builtin_memset ((s), (c), (n))

static inline void GIE_SAVE(unsigned long *var) {
	*var = read_csr(NDS_MSTATUS);
	/* Disable global interrupt for core */
	clear_csr(NDS_MSTATUS, MSTATUS_MIE);
}

static inline void GIE_RESTORE(unsigned long var) {
	if (var & MSTATUS_MIE) {
		/* Enable global interrupt for core */
		set_csr(NDS_MSTATUS, MSTATUS_MIE);
	}
}

void nds_dcache_invalidate(void) {
	unsigned long saved_gie;
	GIE_SAVE(&saved_gie);

	write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_INVAL_ALL);

	GIE_RESTORE(saved_gie);
}

void nds_dcache_writeback_range(unsigned long start, unsigned long end) {
	unsigned long saved_gie;
	unsigned long line_size;

	line_size = CACHE_LINE_SIZE(DCACHE);
	end = (end + line_size - 1) & (~(line_size-1));

	GIE_SAVE(&saved_gie);

	while (end > start) {
		write_csr(NDS_MCCTLBEGINADDR, start);
		write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_VA_WB);
		start += line_size;
	}

	GIE_RESTORE(saved_gie);
}

/*
 * nds_dcache_invalidate_range(start, end)
 *
 * throw away all D-cached data in specified region without an obligation
 * to write them back.
 *
 * Note: however that we must clean the D-cached entries around the
 * boundaries if the start and/or end address are not cache aligned.
 */
void nds_dcache_invalidate_range(unsigned long start, unsigned long end) {
	unsigned long saved_gie;
	unsigned long line_size;

	line_size = CACHE_LINE_SIZE(DCACHE);
	end = (end + line_size - 1) & (~(line_size-1));

	GIE_SAVE(&saved_gie);

	while (end > start) {
		write_csr(NDS_MCCTLBEGINADDR, start);
		write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_VA_INVAL);
		start += line_size;
	}

	GIE_RESTORE(saved_gie);
}

void nds_dcache_flush_range(unsigned long start, unsigned long end) {
	unsigned long saved_gie;
	unsigned long line_size;

	line_size = CACHE_LINE_SIZE(DCACHE);
	end = (end + line_size - 1) & (~(line_size-1));

	GIE_SAVE(&saved_gie);

	while (end > start) {
		write_csr(NDS_MCCTLBEGINADDR, start);
		write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_VA_WBINVAL);
		start += line_size;
	}

	GIE_RESTORE(saved_gie);
}

static inline __attribute__((always_inline)) void nds_dcache_invalidate_line(unsigned long addr) {
	unsigned long saved_gie;

	GIE_SAVE(&saved_gie);

	write_csr(NDS_MCCTLBEGINADDR, addr);
	write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_VA_INVAL);

	GIE_RESTORE(saved_gie);
}

static inline __attribute__((always_inline)) void nds_dcache_flush_line(unsigned long addr) {
	unsigned long saved_gie;

	GIE_SAVE(&saved_gie);

	write_csr(NDS_MCCTLBEGINADDR, addr);
	write_csr(NDS_MCCTLCOMMAND, CCTL_L1D_VA_WBINVAL);

	GIE_RESTORE(saved_gie);
}

static inline __attribute__((always_inline)) void unaligned_cache_line_move(unsigned char* src, unsigned char* dst, unsigned long len) {
	int i;
	unsigned char* src_p = src;
	unsigned char* dst_p = dst;

	for (i = 0; i < len; ++i)
		*(dst_p+i)=*(src_p+i);
}

static void nds_dcache_invalidate_partial_line(unsigned long start, unsigned long end) {
	unsigned long line_size;
	line_size = CACHE_LINE_SIZE(DCACHE);
	unsigned char buf[line_size];

	unsigned long aligned_start = start & (~(line_size-1));
	unsigned long aligned_end   = (end + line_size - 1) & (~(line_size-1));
	unsigned long end_offset    = end & (line_size-1);

	/* handle cache line unaligned */
	if (aligned_start < start) {
		unaligned_cache_line_move((unsigned char*)aligned_start, buf, start - aligned_start);
	}
	if (end < aligned_end) {
		unaligned_cache_line_move((unsigned char*)end, buf + end_offset, aligned_end - end);
	}

	nds_dcache_invalidate_line(start);

	/* handle cache line unaligned */
	if (aligned_start < start) {
		unaligned_cache_line_move(buf, (unsigned char*)aligned_start, start - aligned_start);
	}
	if (end < aligned_end) {
		unaligned_cache_line_move(buf + end_offset, (unsigned char*)end, aligned_end - end);
	}
}

void nds_dma_writeback_range(unsigned long start, unsigned long end) {
	unsigned long line_size;
	line_size = CACHE_LINE_SIZE(DCACHE);
	start = start & (~(line_size-1));
	end = (end + line_size - 1) & (~(line_size-1));
	if (start == end)
		return;

	nds_dcache_writeback_range(start, end);
}

void nds_dma_invalidate_range(unsigned long start, unsigned long end) {
	unsigned long line_size = CACHE_LINE_SIZE(DCACHE);

	unsigned long aligned_start = (start + line_size - 1) & (~(line_size - 1));
	unsigned long aligned_end   = end & (~(line_size - 1));

	if (aligned_start > aligned_end) {
		nds_dcache_flush_line(start);
	}
	else {
		if (start < aligned_start) {
			nds_dcache_flush_line(start);
		}
		if (aligned_start < aligned_end) {
			nds_dcache_invalidate_range(aligned_start, aligned_end-1);
		}
		if (aligned_end < end) {
			nds_dcache_flush_line(end);
		}
	}
}

void nds_dma_invalidate_boundary(unsigned long start, unsigned long end) {
	unsigned long line_size = CACHE_LINE_SIZE(DCACHE);

	unsigned long aligned_start = (start + line_size - 1) & (~(line_size - 1));
	unsigned long aligned_end   = end & (~(line_size - 1));

	if (aligned_start > aligned_end) {
		nds_dcache_invalidate_partial_line(start, end);
	}
	else {
		if (start < aligned_start) {
			nds_dcache_invalidate_partial_line(start, aligned_start);
		}
		if (aligned_end < end) {
			nds_dcache_invalidate_partial_line(aligned_end, end);
		}
	}
}

/*
 * nds_icache_invalidate(void)
 *
 * invalidate all I-cached data.
 *
 * Note: CCTL doesn't support icache invalidate all operation,
 * so this function emulates "Invalidate all" operation by invalidating 
 * each cache line of cache (index-based CCTL).
 */
void nds_icache_invalidate(void) {
	unsigned long saved_gie;
	unsigned long line_size;
	unsigned long end;
	int i;

	line_size = CACHE_LINE_SIZE(ICACHE);
	end = CACHE_WAY(ICACHE) * CACHE_SET(ICACHE) * line_size;

	GIE_SAVE(&saved_gie);

	for (i = 0; i < end; i += line_size) {
		write_csr(NDS_MCCTLBEGINADDR, i);
		write_csr(NDS_MCCTLCOMMAND, CCTL_L1I_IX_INVAL);
	}

	GIE_RESTORE(saved_gie);
}
