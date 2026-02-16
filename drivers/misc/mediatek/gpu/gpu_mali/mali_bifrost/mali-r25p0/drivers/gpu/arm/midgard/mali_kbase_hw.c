// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
/*
 *
 * (C) COPYRIGHT 2012-2024 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-2.0.html.
 *
 */

/*
 * Run-time work-arounds helpers
 */

#include <mali_base_hwconfig_features.h>
#include <mali_base_hwconfig_issues.h>
#include <hw_access/mali_kbase_hw_access_regmap.h>
#include "mali_kbase.h"
#include "mali_kbase_hw.h"

void kbase_hw_set_features_mask(struct kbase_device *kbdev)
{
	const enum base_hw_feature *features;

	if (true) {
		features = base_hw_features_tGOx;
	}

	for (; *features != BASE_HW_FEATURE_END; features++)
		set_bit(*features, &kbdev->hw_features_mask[0]);
}

/**
 * kbase_hw_get_issues_for_new_id - Get the hardware issues for a new GPU ID
 * @kbdev: Device pointer
 *
 * Return: pointer to an array of hardware issues, terminated by
 * BASE_HW_ISSUE_END.
 *
 * In debugging versions of the driver, unknown versions of a known GPU will
 * be treated as the most recent known version not later than the actual
 * version. In such circumstances, the GPU ID in @kbdev will also be replaced
 * with the most recent known version.
 *
 * Note: The GPU configuration must have been read by kbase_gpuprops_get_props()
 * before calling this function.
 */
static const enum base_hw_issue *kbase_hw_get_issues_for_new_id(struct kbase_device *kbdev)
{
	const enum base_hw_issue *issues = NULL;

	struct base_hw_product {
		u32 product_model;
		struct {
			u32 version;
			const enum base_hw_issue *issues;
		} map[7];
	};

	static const struct base_hw_product base_hw_products[] = {
		{ GPU_ID_PRODUCT_TMIX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 1), base_hw_issues_tMIx_r0p0_05dev0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 2), base_hw_issues_tMIx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tMIx_r0p1 },
		    { U32_MAX /* sentinel value */, NULL } } },

		{ GPU_ID_PRODUCT_THEX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tHEx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 1), base_hw_issues_tHEx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tHEx_r0p1 },
		    { GPU_ID_VERSION_MAKE(0, 1, 1), base_hw_issues_tHEx_r0p1 },
		    { GPU_ID_VERSION_MAKE(0, 2, 0), base_hw_issues_tHEx_r0p2 },
		    { GPU_ID_VERSION_MAKE(0, 3, 0), base_hw_issues_tHEx_r0p3 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TSIX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tSIx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 1), base_hw_issues_tSIx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tSIx_r0p1 },
		    { GPU_ID_VERSION_MAKE(1, 0, 0), base_hw_issues_tSIx_r1p0 },
		    { GPU_ID_VERSION_MAKE(1, 1, 0), base_hw_issues_tSIx_r1p1 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TDVX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tDVx_r0p0 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TNOX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tNOx_r0p0 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TGOX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tGOx_r0p0 },
		    { GPU_ID_VERSION_MAKE(1, 0, 0), base_hw_issues_tGOx_r1p0 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TTRX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tTRx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 3), base_hw_issues_tTRx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tTRx_r0p1 },
		    { GPU_ID_VERSION_MAKE(0, 1, 1), base_hw_issues_tTRx_r0p1 },
		    { GPU_ID_VERSION_MAKE(0, 2, 0), base_hw_issues_tTRx_r0p2 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TNAX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tNAx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 3), base_hw_issues_tNAx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 4), base_hw_issues_tNAx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 5), base_hw_issues_tNAx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tNAx_r0p1 },
		    { GPU_ID_VERSION_MAKE(0, 1, 1), base_hw_issues_tNAx_r0p1 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_LBEX,
		  { { GPU_ID_VERSION_MAKE(1, 0, 0), base_hw_issues_lBEx_r1p0 },
		    { GPU_ID_VERSION_MAKE(1, 1, 0), base_hw_issues_lBEx_r1p1 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TBEX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tBEx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 3), base_hw_issues_tBEx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tBEx_r0p1 },
		    { GPU_ID_VERSION_MAKE(1, 0, 0), base_hw_issues_tBEx_r1p0 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TBAX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tBAx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 1), base_hw_issues_tBAx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 2), base_hw_issues_tBAx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tBAx_r0p1 },
		    { GPU_ID_VERSION_MAKE(0, 2, 0), base_hw_issues_tBAx_r0p2 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TODX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tODx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 4), base_hw_issues_tODx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 5), base_hw_issues_tODx_r0p0 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_LODX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tODx_r0p0 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TGRX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tGRx_r0p0 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TVAX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tVAx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 0, 5), base_hw_issues_tVAx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tVAx_r0p1 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TTUX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tTUx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tTUx_r0p1 },
		    { GPU_ID_VERSION_MAKE(1, 0, 0), base_hw_issues_tTUx_r1p0 },
		    { GPU_ID_VERSION_MAKE(1, 1, 0), base_hw_issues_tTUx_r1p1 },
		    { GPU_ID_VERSION_MAKE(1, 2, 0), base_hw_issues_tTUx_r1p2 },
		    { GPU_ID_VERSION_MAKE(1, 3, 0), base_hw_issues_tTUx_r1p3 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_LTUX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tTUx_r0p0 },
		    { GPU_ID_VERSION_MAKE(1, 0, 0), base_hw_issues_tTUx_r1p0 },
		    { GPU_ID_VERSION_MAKE(1, 1, 0), base_hw_issues_tTUx_r1p1 },
		    { GPU_ID_VERSION_MAKE(1, 2, 0), base_hw_issues_tTUx_r1p2 },
		    { GPU_ID_VERSION_MAKE(1, 3, 0), base_hw_issues_tTUx_r1p3 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TTIX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tTIx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tTIx_r0p1 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_LTIX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tTIx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tTIx_r0p1 },
		    { U32_MAX, NULL } } },

		{ GPU_ID_PRODUCT_TKRX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tKRx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tKRx_r0p1 },
		    { U32_MAX, NULL } } },
		{ GPU_ID_PRODUCT_LKRX,
		  { { GPU_ID_VERSION_MAKE(0, 0, 0), base_hw_issues_tKRx_r0p0 },
		    { GPU_ID_VERSION_MAKE(0, 1, 0), base_hw_issues_tKRx_r0p1 },
		    { U32_MAX, NULL } } },
	};

	struct kbase_gpu_id_props *gpu_id = &kbdev->gpu_props.gpu_id;
	const struct base_hw_product *product = NULL;
	size_t p;

	/* Stop when we reach the end of the products array. */
	for (p = 0; p < ARRAY_SIZE(base_hw_products); ++p) {
		if (gpu_id->product_model == GPU_ID_PRODUCT_TGOX) {
			product = &base_hw_products[p];
			break;
		}
	}

	if (product != NULL) {
		/* Found a matching product. */
		const u32 version = gpu_id->version_id;
		u32 fallback_version = 0;
		const enum base_hw_issue *fallback_issues = NULL;
		size_t v;

		/* Stop when we reach the end of the map. */
		for (v = 0; product->map[v].version != U32_MAX; ++v) {
			if (version == product->map[v].version) {
				/* Exact match so stop. */
				issues = product->map[v].issues;
				break;
			}

			/* Check whether this is a candidate for most recent
			 * known version not later than the actual version.
			 */
			if ((version > product->map[v].version) &&
			    (product->map[v].version >= fallback_version)) {
#if MALI_CUSTOMER_RELEASE
				/* Match on version's major and minor fields */
				if (GPU_ID_VERSION_ID_MAJOR_MINOR_GET(version ^
								      product->map[v].version) == 0)
#endif
				{
					fallback_version = product->map[v].version;
					fallback_issues = product->map[v].issues;
				}
			}
		}

		if ((issues == NULL) && (fallback_issues != NULL)) {
			u16 fallback_version_major = GPU_ID_VERSION_ID_MAJOR_GET(fallback_version);
			u16 fallback_version_minor = GPU_ID_VERSION_ID_MINOR_GET(fallback_version);
			u16 fallback_version_status =
				GPU_ID_VERSION_ID_STATUS_GET(fallback_version);

			/* Fall back to the issue set of the most recent known
			 * version not later than the actual version.
			 */
			issues = fallback_issues;

			dev_notice(kbdev->dev, "r%dp%d status %d not found in HW issues table;\n",
				   gpu_id->version_major, gpu_id->version_minor,
				   gpu_id->version_status);
			dev_notice(kbdev->dev, "falling back to closest match: r%dp%d status %d\n",
				   fallback_version_major, fallback_version_minor,
				   fallback_version_status);
			dev_notice(kbdev->dev,
				   "Execution proceeding normally with fallback match\n");

			gpu_id->version_major = fallback_version_major;
			gpu_id->version_minor = fallback_version_minor;
			gpu_id->version_status = fallback_version_status;
			gpu_id->version_id = fallback_version;
		}
	}


	return issues;
}

int kbase_hw_set_issues_mask(struct kbase_device *kbdev)
{
	const enum base_hw_issue *issues;
	struct kbase_gpu_id_props *gpu_id;
	u32 impl_tech;

	gpu_id = &kbdev->gpu_props.gpu_id;
	impl_tech = kbdev->gpu_props.impl_tech;

	if (true) {
		/* Software model */
		if (true) {
			issues = base_hw_issues_model_tGOx;
		}
	}

	dev_info(kbdev->dev, "GPU identified as 0x%x arch %d.%d.%d r%dp%d status %d",
		 2, 7, gpu_id->arch_minor, gpu_id->arch_rev,
		 gpu_id->version_major, gpu_id->version_minor, gpu_id->version_status);

	for (; *issues != BASE_HW_ISSUE_END; issues++)
		set_bit(*issues, &kbdev->hw_issues_mask[0]);

	return 0;
}
