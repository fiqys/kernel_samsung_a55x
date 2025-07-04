/*
 * linux/drivers/video/fbdev/exynos/panel/nt36672c_m33x_00/nt36672c_m33_00.h
 *
 * Header file for TFT_COMMON Dimming Driver
 *
 * Copyright (c) 2016 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __S6E3FC5_M56X_H__
#define __S6E3FC5_M56X_H__
#include "../panel.h"
#include "../panel_drv.h"

enum s6e3fc5_m56x_function {
    S6E3FC5_M56X_MAPTBL_GETIDX_HBM_TRANSITION,
    S6E3FC5_M56X_MAPTBL_COPY_LOCAL_HBM_COMP,
	MAX_S6E3FC5_M56X_FUNCTION,
};

extern struct pnobj_func s6e3fc5_m56x_function_table[MAX_S6E3FC5_M56X_FUNCTION];

#undef PANEL_FUNC
#define PANEL_FUNC(_index) (s6e3fc5_m56x_function_table[_index])

int s6e3fc5_m56x_flash_checksum(struct panel_device *panel, void *data, u32 len);

#endif /* __S6E3FC5_M56X_H__ */
