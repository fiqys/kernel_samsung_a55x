/*
 * linux/drivers/video/fbdev/exynos/panel/tft_common/tft_common.c
 *
 * TFT_COMMON Dimming Driver
 *
 * Copyright (c) 2016 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/of_gpio.h>
#include <video/mipi_display.h>
#include "../panel_debug.h"
#include "s6e3fc5_m56x.h"
#include "s6e3fc5_m56x_panel.h"
#include "s6e3fc5_m56x_ezop.h"

int s6e3fc5_m56x_flash_checksum(struct panel_device *panel, void *data, u32 len)
{
	int ret, state;
	struct dim_flash_result *result;
	char read_buf[16] = { 0, };

	result = (struct dim_flash_result *)data;

	if (!panel)
		return -EINVAL;

	if (!result)
		return -ENODATA;

	if (atomic_cmpxchg(&result->running, 0, 1) != 0) {
		panel_info("already running\n");
		return -EBUSY;
	}

	memset(result->result, 0, ARRAY_SIZE(result->result));
	result->exist = 0;
	result->state = state = GAMMA_FLASH_PROGRESS;

	ret = panel_resource_copy(panel, read_buf, "flash_loaded");
	if (ret < 0) {
		panel_err("flash_loaded copy failed\n");
		state = GAMMA_FLASH_ERROR_READ_FAIL;
		goto out;
	}

	result->exist = 1;
	state = panel_is_dump_status_success(panel, "flash_loaded") ?
		GAMMA_FLASH_SUCCESS : GAMMA_FLASH_ERROR_CHECKSUM_MISMATCH;

out:
	snprintf(result->result, ARRAY_SIZE(result->result), "1\n%d %02X%02X%02X%02X",
		state, read_buf[0], read_buf[1], 0x00, 0x00);

	result->state = state;
	atomic_xchg(&result->running, 0);

	return ret;
}



int s6e3fc5_m56x_maptbl_getidx_hbm_transition(struct maptbl *tbl)
{
	int layer, row, col;
	struct panel_bl_device *panel_bl;
	struct panel_device *panel = (struct panel_device *)tbl->pdata;

	if (panel == NULL) {
		panel_err("panel is null\n");
		return -EINVAL;
	}

	panel_bl = &panel->panel_bl;

	layer = is_hbm_brightness(panel_bl, panel_bl->props.brightness);
	row = panel_bl->props.smooth_transition;
	col = panel_bl->props.local_hbm;

	panel_dbg("brightness %d, layer %d, row %d, col %d\n", panel_bl->props.brightness, layer, row, col);

	return maptbl_4d_index(tbl, layer, row, col, 0);
}

void s6e3fc5_m56x_maptbl_copy_local_hbm(struct maptbl *tbl, u8 *dst)
{
	struct panel_device *panel;
	struct panel_info *panel_data;
	u8 comp[S6E3FC5_LOCAL_HBM_COMP_LEN] = { 0, };
	int ret;

	if (!tbl) {
		panel_err("tbl is null\n");
		return;
	}

	if (!dst) {
		panel_err("dst is null\n");
		return;
	}

	panel = (struct panel_device *)tbl->pdata;
	if (unlikely(!panel))
		return;

	panel_data = &panel->panel_data;

	ret = panel_resource_copy(panel, comp, "local_hbm_comp");
	if (ret < 0) {
		panel_err("failed to copy local hbm comp resource %d\n", ret);
		return;
	}
	memcpy(dst, comp, sizeof(comp));
}

struct pnobj_func s6e3fc5_m56x_function_table[MAX_S6E3FC5_M56X_FUNCTION] = {
	[S6E3FC5_M56X_MAPTBL_GETIDX_HBM_TRANSITION] = __PNOBJ_FUNC_INITIALIZER(S6E3FC5_M56X_MAPTBL_GETIDX_HBM_TRANSITION, s6e3fc5_m56x_maptbl_getidx_hbm_transition),
	[S6E3FC5_M56X_MAPTBL_COPY_LOCAL_HBM_COMP] = __PNOBJ_FUNC_INITIALIZER(S6E3FC5_M56X_MAPTBL_COPY_LOCAL_HBM_COMP, s6e3fc5_m56x_maptbl_copy_local_hbm),
};

__visible_for_testing int __init s6e3fc5_m56x_panel_init(void)
{
	struct common_panel_info *cpi = &s6e3fc5_m56x_panel_info;
	int ret;

	s6e3fc5_init(cpi);
	cpi->ezop_json = EZOP_JSON_BUFFER;
	register_common_panel(cpi);

	ret = panel_function_insert_array(s6e3fc5_m56x_function_table,
			ARRAY_SIZE(s6e3fc5_m56x_function_table));
	if (ret < 0)
		panel_err("failed to insert s6e3fc5_m56x_function_table\n");

	panel_vote_up_to_probe(NULL);

	return 0;
}

__visible_for_testing void __exit s6e3fc5_m56x_panel_exit(void)
{
	deregister_common_panel(&s6e3fc5_m56x_panel_info);
}

module_init(s6e3fc5_m56x_panel_init)
module_exit(s6e3fc5_m56x_panel_exit)

MODULE_DESCRIPTION("Samsung Mobile Panel Driver");
MODULE_LICENSE("GPL");
