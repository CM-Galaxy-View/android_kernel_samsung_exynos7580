/*
 * Samsung Exynos5 SoC series Sensor driver
 *
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/gpio.h>
#include <linux/clk.h>
#include <linux/regulator/consumer.h>
#include <linux/videodev2.h>
#include <linux/videodev2_exynos_camera.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#ifdef CONFIG_OF
#include <linux/of_gpio.h>
#endif
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>
#include <mach/exynos-fimc-is-sensor.h>

#include "fimc-is-hw.h"
#include "fimc-is-core.h"
#include "fimc-is-device-sensor.h"
#include "fimc-is-resourcemgr.h"
#include "fimc-is-dt.h"
#include "fimc-is-device-5e3.h"

#define SENSOR_NAME "S5K5E3"
#define DEFAULT_SENSOR_WIDTH	184
#define DEFAULT_SENSOR_HEIGHT	104
#define SENSOR_MEMSIZE DEFAULT_SENSOR_WIDTH * DEFAULT_SENSOR_HEIGHT

#define SENSOR_REG_VIS_DURATION_MSB			(0x6026)
#define SENSOR_REG_VIS_DURATION_LSB			(0x6027)
#define SENSOR_REG_VIS_FRAME_LENGTH_LINE_ALV_MSB	(0x4340)
#define SENSOR_REG_VIS_FRAME_LENGTH_LINE_ALV_LSB	(0x4341)
#define SENSOR_REG_VIS_LINE_LENGTH_PCLK_ALV_MSB		(0x4342)
#define SENSOR_REG_VIS_LINE_LENGTH_PCLK_ALV_LSB		(0x4343)
#define SENSOR_REG_VIS_GAIN_RED				(0x6029)
#define SENSOR_REG_VIS_GAIN_GREEN			(0x602A)
#define SENSOR_REG_VIS_AE_TARGET			(0x600A)
#define SENSOR_REG_VIS_AE_SPEED				(0x5034)
#define SENSOR_REG_VIS_AE_NUMBER_OF_PIXEL_MSB		(0x5030)
#define SENSOR_REG_VIS_AE_NUMBER_OF_PIXEL_LSB		(0x5031)
#define SENSOR_REG_VIS_AE_WINDOW_WEIGHT_1x1_2		(0x6000)
#define SENSOR_REG_VIS_AE_WINDOW_WEIGHT_1x3_4		(0x6001)
#define SENSOR_REG_VIS_AE_WINDOW_WEIGHT_2x1_2		(0x6002)
#define SENSOR_REG_VIS_AE_WINDOW_WEIGHT_2x3_4		(0x6003)
#define SENSOR_REG_VIS_AE_WINDOW_WEIGHT_3x1_2		(0x6004)
#define SENSOR_REG_VIS_AE_WINDOW_WEIGHT_3x3_4		(0x6005)
#define SENSOR_REG_VIS_AE_WINDOW_WEIGHT_4x1_2		(0x6006)
#define SENSOR_REG_VIS_AE_WINDOW_WEIGHT_4x3_4		(0x6007)
#define SENSOR_REG_VIS_AE_MANUAL_EXP_MSB		(0x5039)
#define SENSOR_REG_VIS_AE_MANUAL_EXP_LSB		(0x503A)
#define SENSOR_REG_VIS_AE_MANUAL_ANG_MSB		(0x503B)
#define SENSOR_REG_VIS_AE_MANUAL_ANG_LSB		(0x503C)
#define SENSOR_REG_VIS_BIT_CONVERTING_MSB		(0x602B)
#define SENSOR_REG_VIS_BIT_CONVERTING_LSB		(0x7203)
#define SENSOR_REG_VIS_AE_OFF				(0x5000)

static struct fimc_is_sensor_cfg config_5e3[] = {
	/* 2576x1932@30fps */
	FIMC_IS_SENSOR_CFG(2576, 1932, 30, 19, 0),
	/* 2576x1932@24fps */
	FIMC_IS_SENSOR_CFG(2576, 1932, 24, 19, 1),
	/* 1280x960@15fps */
	FIMC_IS_SENSOR_CFG(1280, 960, 15, 19, 2),
	/* 1280x960@7fps */
	FIMC_IS_SENSOR_CFG(1280, 960, 7, 19, 3),
	/* 640x480@15fps */
	FIMC_IS_SENSOR_CFG(640, 480, 15, 19, 4),
	/* 1280x960@30fps */
	FIMC_IS_SENSOR_CFG(1280, 960, 30, 19, 5),
	/* 640x480@120fps */
	FIMC_IS_SENSOR_CFG(640, 480, 120, 19, 6),
};

static struct fimc_is_vci vci_5e3[] = {
	{
		.pixelformat = V4L2_PIX_FMT_SBGGR10,
		.config = {{0, HW_FORMAT_RAW10}, {1, HW_FORMAT_UNKNOWN}, {2, HW_FORMAT_UNKNOWN}, {3, 0}}
	}, {
		.pixelformat = V4L2_PIX_FMT_SBGGR12,
		.config = {{0, HW_FORMAT_RAW10}, {1, HW_FORMAT_UNKNOWN}, {2, HW_FORMAT_UNKNOWN}, {3, 0}}
	}, {
		.pixelformat = V4L2_PIX_FMT_SBGGR16,
		.config = {{0, HW_FORMAT_RAW10}, {1, HW_FORMAT_UNKNOWN}, {2, HW_FORMAT_UNKNOWN}, {3, 0}}
	}
};

static int sensor_5e3_open(struct v4l2_subdev *sd,
	struct v4l2_subdev_fh *fh)
{
	pr_info("%s\n", __func__);
	return 0;
}
static int sensor_5e3_close(struct v4l2_subdev *sd,
	struct v4l2_subdev_fh *fh)
{
	pr_info("%s\n", __func__);
	return 0;
}
static int sensor_5e3_registered(struct v4l2_subdev *sd)
{
	pr_info("%s\n", __func__);
	return 0;
}

static void sensor_5e3_unregistered(struct v4l2_subdev *sd)
{
	pr_info("%s\n", __func__);
}

static const struct v4l2_subdev_internal_ops internal_ops = {
	.open = sensor_5e3_open,
	.close = sensor_5e3_close,
	.registered = sensor_5e3_registered,
	.unregistered = sensor_5e3_unregistered,
};

static int sensor_5e3_init(struct v4l2_subdev *subdev, u32 val)
{
	int ret = 0;
	struct fimc_is_module_enum *module;
	struct fimc_is_module_5e3 *module_5e3;
	struct i2c_client *client;

	BUG_ON(!subdev);

	module = (struct fimc_is_module_enum *)v4l2_get_subdevdata(subdev);
	module_5e3 = module->private_data;
	client = module->client;

	module_5e3->system_clock = 146 * 1000 * 1000;
	module_5e3->line_length_pck = 146 * 1000 * 1000;

	pr_info("%s\n", __func__);
	/* sensor init */
	fimc_is_sensor_write8(client, 0x4200, 0x01);
	fimc_is_sensor_write8(client, 0x4201, 0x24);
	fimc_is_sensor_write8(client, 0x4305, 0x06);
	fimc_is_sensor_write8(client, 0x4307, 0xC0);
	fimc_is_sensor_write8(client, 0x4342, 0x03);

	fimc_is_sensor_write8(client, 0x4343, 0x90);
	fimc_is_sensor_write8(client, 0x4345, 0x08);
	fimc_is_sensor_write8(client, 0x4347, 0x08);
	fimc_is_sensor_write8(client, 0x4348, 0x07);
	fimc_is_sensor_write8(client, 0x4349, 0x83); /* 10 */

	fimc_is_sensor_write8(client, 0x434A, 0x04);
	fimc_is_sensor_write8(client, 0x434B, 0x3A);
	fimc_is_sensor_write8(client, 0x434C, 0x01);
	fimc_is_sensor_write8(client, 0x434D, 0x40);
	fimc_is_sensor_write8(client, 0x434E, 0x00);

	fimc_is_sensor_write8(client, 0x434F, 0xB4);
	fimc_is_sensor_write8(client, 0x4381, 0x01);
	fimc_is_sensor_write8(client, 0x4383, 0x05);
	fimc_is_sensor_write8(client, 0x4385, 0x06);
	fimc_is_sensor_write8(client, 0x4387, 0x06); /* 20 */

	fimc_is_sensor_write8(client, 0x5004, 0x01);
	fimc_is_sensor_write8(client, 0x5005, 0x1E);
#ifdef VISION_30FPS
	fimc_is_sensor_write8(client, 0x5014, 0x05);
	fimc_is_sensor_write8(client, 0x5015, 0x73);
#else
	fimc_is_sensor_write8(client, 0x5014, 0x13);
	fimc_is_sensor_write8(client, 0x5015, 0x33);
#endif
	fimc_is_sensor_write8(client, 0x5016, 0x00);

	fimc_is_sensor_write8(client, 0x5017, 0x02);
	fimc_is_sensor_write8(client, 0x5030, 0x0E);
	fimc_is_sensor_write8(client, 0x5031, 0x10);
	fimc_is_sensor_write8(client, 0x5034, 0x00);
	fimc_is_sensor_write8(client, 0x5035, 0x02); /* 30 */

	fimc_is_sensor_write8(client, 0x5036, 0x00);
	fimc_is_sensor_write8(client, 0x5037, 0x04);
	fimc_is_sensor_write8(client, 0x5038, 0xC0);
	fimc_is_sensor_write8(client, 0x503D, 0x20);
	fimc_is_sensor_write8(client, 0x503E, 0x70);

	fimc_is_sensor_write8(client, 0x503F, 0x02);
	fimc_is_sensor_write8(client, 0x600A, 0x3A);
	fimc_is_sensor_write8(client, 0x600E, 0x05);
	fimc_is_sensor_write8(client, 0x6014, 0x27);
	fimc_is_sensor_write8(client, 0x6015, 0x1D); /* 40 */

	fimc_is_sensor_write8(client, 0x6018, 0x01);
	fimc_is_sensor_write8(client, 0x6026, 0x00);
#ifdef VISION_30FPS
	fimc_is_sensor_write8(client, 0x6027, 0x1B);
#else
	fimc_is_sensor_write8(client, 0x6027, 0x52);
#endif
	fimc_is_sensor_write8(client, 0x6029, 0x08);
	fimc_is_sensor_write8(client, 0x602A, 0x08);

	fimc_is_sensor_write8(client, 0x602B, 0x00);
	fimc_is_sensor_write8(client, 0x602c, 0x00);
	fimc_is_sensor_write8(client, 0x6032, 0x63);
	fimc_is_sensor_write8(client, 0x6033, 0x94);
	fimc_is_sensor_write8(client, 0x7007, 0x18); /* 50 */

	fimc_is_sensor_write8(client, 0x7015, 0x28);
	fimc_is_sensor_write8(client, 0x7016, 0x2C);
	fimc_is_sensor_write8(client, 0x7027, 0x14);
	fimc_is_sensor_write8(client, 0x7028, 0x3C);
	fimc_is_sensor_write8(client, 0x7029, 0x02);

	fimc_is_sensor_write8(client, 0x702A, 0x02);
	fimc_is_sensor_write8(client, 0x703A, 0x04);
	fimc_is_sensor_write8(client, 0x703B, 0x36);
	fimc_is_sensor_write8(client, 0x7042, 0x04);
	fimc_is_sensor_write8(client, 0x7043, 0x36); /* 60 */

	fimc_is_sensor_write8(client, 0x7058, 0x6F);
	fimc_is_sensor_write8(client, 0x705A, 0x01);
	fimc_is_sensor_write8(client, 0x705C, 0x40);
	fimc_is_sensor_write8(client, 0x7060, 0x07);
	fimc_is_sensor_write8(client, 0x7061, 0x40);

	fimc_is_sensor_write8(client, 0x7064, 0x43);
	fimc_is_sensor_write8(client, 0x706D, 0x77);
	fimc_is_sensor_write8(client, 0x706E, 0xFA);
	fimc_is_sensor_write8(client, 0x7070, 0x0A);
	fimc_is_sensor_write8(client, 0x7073, 0x04); /* 70 */

	fimc_is_sensor_write8(client, 0x7087, 0x00);
	fimc_is_sensor_write8(client, 0x7090, 0x01);
	fimc_is_sensor_write8(client, 0x7115, 0x01);
	fimc_is_sensor_write8(client, 0x7209, 0xF5);
	fimc_is_sensor_write8(client, 0x720B, 0xF5);

	fimc_is_sensor_write8(client, 0x7245, 0xC4);
	fimc_is_sensor_write8(client, 0x7301, 0x02);
	fimc_is_sensor_write8(client, 0x7306, 0x02);
	fimc_is_sensor_write8(client, 0x7339, 0x03);
	fimc_is_sensor_write8(client, 0x7351, 0x00); /* 80 */

	fimc_is_sensor_write8(client, 0x7352, 0xC0);
	fimc_is_sensor_write8(client, 0x7405, 0x28);
	fimc_is_sensor_write8(client, 0x7406, 0x28);
	fimc_is_sensor_write8(client, 0x7407, 0xC0);
	fimc_is_sensor_write8(client, 0x740C, 0x60);

	fimc_is_sensor_write8(client, 0x740D, 0x00);
	fimc_is_sensor_write8(client, 0x7436, 0x03);
	fimc_is_sensor_write8(client, 0x7437, 0x03);
	fimc_is_sensor_write8(client, 0x7454, 0x01);
	fimc_is_sensor_write8(client, 0x7460, 0x00); /* 90 */

	fimc_is_sensor_write8(client, 0x7461, 0x01);
	fimc_is_sensor_write8(client, 0x7462, 0x68);
	fimc_is_sensor_write8(client, 0x7463, 0x1E);
	fimc_is_sensor_write8(client, 0x7464, 0x03);
	fimc_is_sensor_write8(client, 0x7465, 0x4B);

	fimc_is_sensor_write8(client, 0x7467, 0x20);
	fimc_is_sensor_write8(client, 0x7468, 0x20);
	fimc_is_sensor_write8(client, 0x7469, 0x20);
	fimc_is_sensor_write8(client, 0x746A, 0x20);
	fimc_is_sensor_write8(client, 0x746B, 0x20); /* 100 */

	fimc_is_sensor_write8(client, 0x746C, 0x20);
	fimc_is_sensor_write8(client, 0x746D, 0x02);
	fimc_is_sensor_write8(client, 0x746E, 0x80);
	fimc_is_sensor_write8(client, 0x746F, 0x01);
	fimc_is_sensor_write8(client, 0x4100, 0x01);

	pr_info("[MOD:D:%d] %s(%d)\n", module->sensor_id, __func__, val);

	return ret;
}

static const struct v4l2_subdev_core_ops core_ops = {
	.init = sensor_5e3_init
};

static int sensor_5e3_s_stream(struct v4l2_subdev *subdev, int enable)
{
	int ret = 0;
	struct fimc_is_module_enum *sensor;

	pr_info("%s\n", __func__);

	sensor = (struct fimc_is_module_enum *)v4l2_get_subdevdata(subdev);
	if (!sensor) {
		err("sensor is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	if (enable) {
		ret = CALL_MOPS(sensor, stream_on, subdev);
		if (ret) {
			err("s_duration is fail(%d)", ret);
			goto p_err;
		}
	} else {
		ret = CALL_MOPS(sensor, stream_off, subdev);
		if (ret) {
			err("s_duration is fail(%d)", ret);
			goto p_err;
		}
	}

p_err:
	return 0;
}

static int sensor_5e3_s_param(struct v4l2_subdev *subdev, struct v4l2_streamparm *param)
{
	int ret = 0;
	struct fimc_is_module_enum *sensor;
	struct v4l2_captureparm *cp;
	struct v4l2_fract *tpf;
	u64 duration;

	BUG_ON(!subdev);
	BUG_ON(!param);

	pr_info("%s\n", __func__);

	cp = &param->parm.capture;
	tpf = &cp->timeperframe;

	if (!tpf->denominator) {
		err("denominator is 0");
		ret = -EINVAL;
		goto p_err;
	}

	if (!tpf->numerator) {
		err("numerator is 0");
		ret = -EINVAL;
		goto p_err;
	}

	duration = (u64)(tpf->numerator * 1000 * 1000 * 1000) /
					(u64)(tpf->denominator);

	sensor = (struct fimc_is_module_enum *)v4l2_get_subdevdata(subdev);
	if (!sensor) {
		err("sensor is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	ret = CALL_MOPS(sensor, s_duration, subdev, duration);
	if (ret) {
		err("s_duration is fail(%d)", ret);
		goto p_err;
	}

p_err:
	return ret;
}

static int sensor_5e3_s_format(struct v4l2_subdev *subdev, struct v4l2_mbus_framefmt *fmt)
{
	/* TODO */
	return 0;
}

static const struct v4l2_subdev_video_ops video_ops = {
	.s_stream = sensor_5e3_s_stream,
	.s_parm = sensor_5e3_s_param,
	.s_mbus_fmt = sensor_5e3_s_format
};

static const struct v4l2_subdev_ops subdev_ops = {
	.core = &core_ops,
	.video = &video_ops
};

int sensor_5e3_stream_on(struct v4l2_subdev *subdev)
{
	int ret = 0;
	struct fimc_is_module_enum *sensor;
	struct i2c_client *client;

	BUG_ON(!subdev);

	sensor = (struct fimc_is_module_enum *)v4l2_get_subdevdata(subdev);
	if (unlikely(!sensor)) {
		err("sensor is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	client = sensor->client;
	if (unlikely(!client)) {
		err("client is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	ret = fimc_is_sensor_write8(client, 0x4100, 1);
	if (ret < 0) {
		err("fimc_is_sensor_write8 is fail(%d)", ret);
		goto p_err;
	}

p_err:
	return ret;
}

int sensor_5e3_stream_off(struct v4l2_subdev *subdev)
{
	int ret = 0;
	struct fimc_is_module_enum *sensor;
	struct i2c_client *client;

	BUG_ON(!subdev);

	sensor = (struct fimc_is_module_enum *)v4l2_get_subdevdata(subdev);
	if (unlikely(!sensor)) {
		err("sensor is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	client = sensor->client;
	if (unlikely(!client)) {
		err("client is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	ret = fimc_is_sensor_write8(client, 0x4100, 0);
	if (ret < 0) {
		err("fimc_is_sensor_write8 is fail(%d)", ret);
		goto p_err;
	}

p_err:
	return ret;
}

/*
 * @ brief
 * frame duration time
 * @ unit
 * nano second
 * @ remarks
 */
int sensor_5e3_s_duration(struct v4l2_subdev *subdev, u64 duration)
{
	int ret = 0;
	u8 value[2];
	struct fimc_is_module_enum *sensor;
	struct i2c_client *client;

	BUG_ON(!subdev);

	pr_info("%s\n", __func__);

	sensor = (struct fimc_is_module_enum *)v4l2_get_subdevdata(subdev);
	if (unlikely(!sensor)) {
		err("sensor is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	client = sensor->client;
	if (unlikely(!client)) {
		err("client is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	/*
	 * forcely set 10fps for 5e3,
	 */
	value[0] = 0x52;
	value[1] = 0x0;

	fimc_is_sensor_write8(client, SENSOR_REG_VIS_DURATION_MSB, value[1]);
	fimc_is_sensor_write8(client, SENSOR_REG_VIS_DURATION_LSB, value[0]);

p_err:
	return ret;
}

int sensor_5e3_g_min_duration(struct v4l2_subdev *subdev)
{
	int ret = 0;
	return ret;
}

int sensor_5e3_g_max_duration(struct v4l2_subdev *subdev)
{
	int ret = 0;
	return ret;
}

int sensor_5e3_s_exposure(struct v4l2_subdev *subdev, u64 exposure)
{
	int ret = 0;
	u8 value;
	struct fimc_is_module_enum *sensor;
	struct i2c_client *client;

	BUG_ON(!subdev);

	pr_info("%s(%d)\n", __func__, (u32)exposure);

	sensor = (struct fimc_is_module_enum *)v4l2_get_subdevdata(subdev);
	if (unlikely(!sensor)) {
		err("sensor is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	client = sensor->client;
	if (unlikely(!client)) {
		err("client is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	value = exposure & 0xFF;

	fimc_is_sensor_write8(client, SENSOR_REG_VIS_AE_TARGET, value);

p_err:
	return ret;
}

int sensor_5e3_g_min_exposure(struct v4l2_subdev *subdev)
{
	int ret = 0;
	return ret;
}

int sensor_5e3_g_max_exposure(struct v4l2_subdev *subdev)
{
	int ret = 0;
	return ret;
}

int sensor_5e3_s_again(struct v4l2_subdev *subdev, u64 sensitivity)
{
	int ret = 0;

	pr_info("%s\n", __func__);

	return ret;
}

int sensor_5e3_g_min_again(struct v4l2_subdev *subdev)
{
	int ret = 0;
	return ret;
}

int sensor_5e3_g_max_again(struct v4l2_subdev *subdev)
{
	int ret = 0;
	return ret;
}

int sensor_5e3_s_dgain(struct v4l2_subdev *subdev)
{
	int ret = 0;
	return ret;
}

int sensor_5e3_g_min_dgain(struct v4l2_subdev *subdev)
{
	int ret = 0;
	return ret;
}

int sensor_5e3_g_max_dgain(struct v4l2_subdev *subdev)
{
	int ret = 0;
	return ret;
}

struct fimc_is_sensor_ops module_5e3_ops = {
	.stream_on	= sensor_5e3_stream_on,
	.stream_off	= sensor_5e3_stream_off,
	.s_duration	= sensor_5e3_s_duration,
	.g_min_duration	= sensor_5e3_g_min_duration,
	.g_max_duration	= sensor_5e3_g_max_duration,
	.s_exposure	= sensor_5e3_s_exposure,
	.g_min_exposure	= sensor_5e3_g_min_exposure,
	.g_max_exposure	= sensor_5e3_g_max_exposure,
	.s_again	= sensor_5e3_s_again,
	.g_min_again	= sensor_5e3_g_min_again,
	.g_max_again	= sensor_5e3_g_max_again,
	.s_dgain	= sensor_5e3_s_dgain,
	.g_min_dgain	= sensor_5e3_g_min_dgain,
	.g_max_dgain	= sensor_5e3_g_max_dgain
};

#ifdef CONFIG_OF
static int sensor_5e3_power_setpin(struct platform_device *pdev,
	struct exynos_platform_fimc_is_module *pdata)
{
	struct device *dev;
	struct device_node *dnode;
	int gpio_reset = 0;
	int gpio_none = 0;
	int gpios_cam_en = 0;

	BUG_ON(!pdev);

	dev = &pdev->dev;
	dnode = dev->of_node;

	dev_info(dev, "%s E v4\n", __func__);

	gpio_reset = of_get_named_gpio(dnode, "gpio_reset", 0);
	if (!gpio_is_valid(gpio_reset)) {
		dev_err(dev, "failed to get PIN_RESET\n");
		return -EINVAL;
	} else {
		gpio_request_one(gpio_reset, GPIOF_OUT_INIT_LOW, "CAM_GPIO_OUTPUT_LOW");
		gpio_free(gpio_reset);
	}

	gpios_cam_en = of_get_named_gpio(dnode, "gpios_cam_en", 0);
	if (!gpio_is_valid(gpios_cam_en)) {
		err("failed to get main cam en gpio");
	} else {
		gpio_request_one(gpios_cam_en, GPIOF_OUT_INIT_LOW, "CAM_GPIO_OUTPUT_LOW");
		gpio_free(gpios_cam_en);
	}

	SET_PIN_INIT(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON);
	SET_PIN_INIT(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF);
	SET_PIN_INIT(pdata, SENSOR_SCENARIO_VISION, GPIO_SCENARIO_ON);
	SET_PIN_INIT(pdata, SENSOR_SCENARIO_VISION, GPIO_SCENARIO_OFF);
	SET_PIN_INIT(pdata, SENSOR_SCENARIO_READ_ROM, GPIO_SCENARIO_ON);
	SET_PIN_INIT(pdata, SENSOR_SCENARIO_READ_ROM, GPIO_SCENARIO_OFF);

#if defined(CONFIG_MACH_SMDK7580)
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_reset, "sen_rst low", PIN_OUTPUT, 0, 0);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_none, "pin", PIN_FUNCTION, 0, 0);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_reset, "sen_rst high", PIN_OUTPUT, 1, 0);
#elif defined(CONFIG_MACH_UNIVERSAL7580)
#if defined(CONFIG_CAMERA_A3X)
	/* Front Camera ON */
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_none, "VDD_CAM_SENSOR_A2P8", PIN_REGULATOR, 1, 1000);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_none, "VDD_VTCAM_1P2", PIN_REGULATOR, 1, 1000);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_none, "VDDIO_1.8V_CAM", PIN_REGULATOR, 1, 1000);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_reset, NULL, PIN_OUTPUT, 1, 0);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_none, "pin", PIN_FUNCTION, 0, 0);

	/* Front Camera OFF */
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF, gpio_none, "pin", PIN_FUNCTION, 1, 0);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF, gpio_reset, NULL, PIN_OUTPUT, 0, 0);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF, gpio_none, "VDDIO_1.8V_CAM", PIN_REGULATOR, 0, 1000);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF, gpio_none, "VDD_VTCAM_1P2", PIN_REGULATOR, 0, 1000);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF, gpio_none, "VDD_CAM_SENSOR_A2P8", PIN_REGULATOR, 0, 10);

	/* READ_ROM - POWER ON */
	SET_PIN(pdata, SENSOR_SCENARIO_READ_ROM, GPIO_SCENARIO_ON, gpio_none, "VDDIO_1.8V_CAM", PIN_REGULATOR, 1, 0);

	/* READ_ROM - POWER OFF */
	SET_PIN(pdata, SENSOR_SCENARIO_READ_ROM, GPIO_SCENARIO_OFF, gpio_none, "VDDIO_1.8V_CAM", PIN_REGULATOR, 0, 0);
#else
	/* Front Camera ON */
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_none, "VDD_VTCAM_SENSOR_A2P8", PIN_REGULATOR, 1, 1000);
	if (gpio_is_valid(gpios_cam_en)) {  /* VDD_VTCAM_1P2 */
		SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpios_cam_en, NULL, PIN_OUTPUT, 1, 0);
	}
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_none, "VDD_VTCAM_1P2", PIN_REGULATOR, 1, 1000);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_none, "VDDIO_1.8V_VT", PIN_REGULATOR, 1, 1000);

	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_reset, NULL, PIN_OUTPUT, 1, 0);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_ON, gpio_none, "pin", PIN_FUNCTION, 0, 0);

	/* Front Camera OFF */
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF, gpio_none, "pin", PIN_FUNCTION, 1, 0);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF, gpio_reset, NULL, PIN_OUTPUT, 0, 0);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF, gpio_none, "VDDIO_1.8V_VT", PIN_REGULATOR, 0, 1000);
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF, gpio_none, "VDD_VTCAM_1P2", PIN_REGULATOR, 0, 1000);
	if (gpio_is_valid(gpios_cam_en)) {  /* VDD_VTCAM_1P2 */
		SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF, gpios_cam_en, NULL, PIN_OUTPUT, 0, 0);
	}
	SET_PIN(pdata, SENSOR_SCENARIO_NORMAL, GPIO_SCENARIO_OFF, gpio_none, "VDD_VTCAM_SENSOR_A2P8", PIN_REGULATOR, 0, 10);

	/* READ_ROM - POWER ON */
	SET_PIN(pdata, SENSOR_SCENARIO_READ_ROM, GPIO_SCENARIO_ON, gpio_none, "VDDIO_1.8V_VT", PIN_REGULATOR, 1, 0);

	/* READ_ROM - POWER OFF */
	SET_PIN(pdata, SENSOR_SCENARIO_READ_ROM, GPIO_SCENARIO_OFF, gpio_none, "VDDIO_1.8V_VT", PIN_REGULATOR, 0, 0);
#endif
#endif
	/* BACK CAEMRA - POWER OFF */
	dev_info(dev, "%s X v4\n", __func__);

	return 0;
}
#endif /* CONFIG_OF */

int sensor_5e3_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct fimc_is_core *core;
	struct v4l2_subdev *subdev_module;
	struct fimc_is_module_enum *module;
	struct fimc_is_device_sensor *device;
	struct sensor_open_extended *ext;
	struct exynos_platform_fimc_is_module *pdata;
	struct device *dev;

	BUG_ON(!fimc_is_dev);


	core = (struct fimc_is_core *)dev_get_drvdata(fimc_is_dev);
	if (!core) {
		probe_err("core device is not yet probed");
		return -EPROBE_DEFER;
	}

	dev = &pdev->dev;

#ifdef CONFIG_OF
	fimc_is_sensor_module_parse_dt(pdev, sensor_5e3_power_setpin);
#endif

	pdata = dev_get_platdata(dev);
	device = &core->sensor[pdata->id];

	subdev_module = kzalloc(sizeof(struct v4l2_subdev), GFP_KERNEL);
	if (!subdev_module) {
		probe_err("subdev_module is NULL");
		ret = -ENOMEM;
		goto p_err;
	}

	/* S5K5E3 */
	module = &device->module_enum[atomic_read(&core->resourcemgr.rsccount_module)];
	atomic_inc(&core->resourcemgr.rsccount_module);
	clear_bit(FIMC_IS_MODULE_GPIO_ON, &module->state);
	module->pdata = pdata;
	module->pdev = pdev;
	module->sensor_id = SENSOR_S5K5E3_NAME;
	module->subdev = subdev_module;
	module->device = pdata->id;
	module->ops = &module_5e3_ops;
	module->client = NULL;
	module->active_width = 2576;
	module->active_height = 1932;
	module->margin_left = 0;
	module->margin_right = 0;
	module->margin_top = 0;
	module->margin_bottom = 0;
	module->pixel_width = module->active_width + 0;
	module->pixel_height = module->active_height + 0;
	module->max_framerate = 30;
	module->position = pdata->position;
	module->mode = CSI_MODE_DT_ONLY;
	module->lanes = CSI_DATA_LANES_2;
	module->vcis = ARRAY_SIZE(vci_5e3);
	module->vci = vci_5e3;
	module->sensor_maker = "SLSI";
	module->sensor_name = "S5K5E3";
	module->setfile_name = "setfile_5e3.bin";
	module->cfgs = ARRAY_SIZE(config_5e3);
	module->cfg = config_5e3;
	module->private_data = kzalloc(sizeof(struct fimc_is_module_5e3), GFP_KERNEL);
	if (!module->private_data) {
		probe_err("private_data is NULL");
		ret = -ENOMEM;
		kfree(subdev_module);
		goto p_err;
	}

	ext = &module->ext;
	ext->mipi_lane_num = module->lanes;
	ext->I2CSclk = I2C_L0;
	ext->sensor_con.product_name = SENSOR_NAME_S5K5E3;
	ext->sensor_con.peri_type = SE_I2C;
	ext->sensor_con.peri_setting.i2c.channel = pdata->sensor_i2c_ch;
	ext->sensor_con.peri_setting.i2c.slave_address = 0;
//	ext->sensor_con.peri_setting.i2c.slave_address = pdata->sensor_i2c_addr;
	ext->sensor_con.peri_setting.i2c.speed = 0;

	if (pdata->flash_product_name != FLADRV_NAME_NOTHING) {
		ext->flash_con.product_name = pdata->flash_product_name;
		ext->flash_con.peri_type = SE_GPIO;
		ext->flash_con.peri_setting.gpio.first_gpio_port_no = pdata->flash_first_gpio;
		ext->flash_con.peri_setting.gpio.second_gpio_port_no = pdata->flash_second_gpio;
	}

	ext->from_con.product_name = FROMDRV_NAME_NOTHING;

	ext->companion_con.product_name = COMPANION_NAME_NOTHING;

	v4l2_subdev_init(subdev_module, &subdev_ops);

	v4l2_set_subdevdata(subdev_module, module);
	v4l2_set_subdev_hostdata(subdev_module, device);
	snprintf(subdev_module->name, V4L2_SUBDEV_NAME_SIZE, "sensor-subdev.%d", module->sensor_id);

p_err:
	probe_info("%s(%d)\n", __func__, ret);
	return ret;
}

static int sensor_5e3_remove(struct platform_device *pdev)
{
	int ret = 0;
	return ret;
}

#ifdef CONFIG_OF
static const struct of_device_id exynos_fimc_is_sensor_5e3_match[] = {
	{
		.compatible = "samsung,exynos5-fimc-is-sensor-5e3",
	},
	{},
};
#endif

static struct platform_driver sensor_5e3_driver = {
	.driver = {
		.name	= SENSOR_NAME,
		.owner	= THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = exynos_fimc_is_sensor_5e3_match
#endif
	},
	.probe	= sensor_5e3_probe,
	.remove	= sensor_5e3_remove,
};

static int __init fimc_is_sensor_module_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&sensor_5e3_driver);
	if (ret)
		err("platform_driver_register failed: %d\n", ret);

	return ret;
}

static void __exit fimc_is_sensor_module_exit(void)
{
	platform_driver_unregister(&sensor_5e3_driver);
}
module_init(fimc_is_sensor_module_init);
module_exit(fimc_is_sensor_module_exit);

MODULE_AUTHOR("Gilyeon lim");
MODULE_DESCRIPTION("Sensor 5E3 driver");
MODULE_LICENSE("GPL v2");
