/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */

#ifndef __CACHE_H__
#define __CACHE_H__

#include "core_v5.h"

#define NDS_ISET			0x7
#define NDS_IWAY			0x38
#define NDS_ISIZE			0x1C0
#define NDS_DSET			0x7
#define NDS_DWAY			0x38
#define NDS_DSIZE			0x1C0

enum cache_t{ICACHE, DCACHE};

static inline unsigned long CACHE_SET(enum cache_t cache) {
	if(cache == ICACHE)
		return ((read_csr(NDS_MICM_CFG) & NDS_ISET) < 7) ? (unsigned long)(1 << ((read_csr(NDS_MICM_CFG) & NDS_ISET) + 6)) : 0;
	else
		return ((read_csr(NDS_MDCM_CFG) & NDS_DSET) < 7) ? (unsigned long)(1 << ((read_csr(NDS_MDCM_CFG) & NDS_DSET) + 6)) : 0;
}

static inline unsigned long CACHE_WAY(enum cache_t cache) {
	if(cache == ICACHE)
		return (unsigned long)(((read_csr(NDS_MICM_CFG) & NDS_IWAY) >> 3) + 1);
	else
		return (unsigned long)(((read_csr(NDS_MDCM_CFG) & NDS_DWAY) >> 3) + 1);
}

static inline unsigned long CACHE_LINE_SIZE(enum cache_t cache) {
	if(cache == ICACHE)
		return (((read_csr(NDS_MICM_CFG) & NDS_ISIZE) >> 6) && (((read_csr(NDS_MICM_CFG) & NDS_ISIZE) >> 6) <= 5)) ? (unsigned long)(1 << (((read_csr(NDS_MICM_CFG) & NDS_ISIZE) >> 6) + 2)) : 0;
	else
		return (((read_csr(NDS_MDCM_CFG) & NDS_DSIZE) >> 6) && (((read_csr(NDS_MDCM_CFG) & NDS_DSIZE) >> 6) <= 5)) ? (unsigned long)(1 << (((read_csr(NDS_MDCM_CFG) & NDS_DSIZE) >> 6) + 2)) : 0;
}


/* dcache ops */
extern void nds_dcache_invalidate(void);

extern void nds_dcache_writeback_range(unsigned long start, unsigned long end);
extern void nds_dcache_invalidate_range(unsigned long start, unsigned long end);
extern void nds_dcache_flush_range(unsigned long start, unsigned long end);

/* DMA-specific ops */
extern void nds_dma_writeback_range(unsigned long start, unsigned long end);
extern void nds_dma_invalidate_range(unsigned long start, unsigned long end);
extern void nds_dma_invalidate_boundary(unsigned long start, unsigned long end);

/* icache ops */
extern void nds_icache_invalidate(void);

/* V5 S-mode and U-mode CCTL ops */

#endif /* __CACHE_H__ */
