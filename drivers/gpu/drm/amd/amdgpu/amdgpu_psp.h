/*
 * Copyright 2016 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Author: Huang Rui
 *
 */
#ifndef __AMDGPU_PSP_H__
#define __AMDGPU_PSP_H__

#include "amdgpu.h"
#include "psp_gfx_if.h"
#include "ta_xgmi_if.h"
#include "ta_ras_if.h"

#define PSP_FENCE_BUFFER_SIZE	0x1000
#define PSP_CMD_BUFFER_SIZE	0x1000
#define PSP_XGMI_SHARED_MEM_SIZE 0x4000
#define PSP_RAS_SHARED_MEM_SIZE 0x4000
#define PSP_1_MEG		0x100000
#define PSP_TMR_SIZE	0x400000
#define PSP_HDCP_SHARED_MEM_SIZE	0x4000
#define PSP_DTM_SHARED_MEM_SIZE	0x4000
#define PSP_SHARED_MEM_SIZE		0x4000

struct psp_context;
struct psp_xgmi_node_info;
struct psp_xgmi_topology_info;

enum psp_bootloader_cmd {
	PSP_BL__LOAD_SYSDRV		= 0x10000,
	PSP_BL__LOAD_SOSDRV		= 0x20000,
	PSP_BL__LOAD_KEY_DATABASE	= 0x80000,
	PSP_BL__DRAM_LONG_TRAIN		= 0x100000,
	PSP_BL__DRAM_SHORT_TRAIN	= 0x200000,
	PSP_BL__LOAD_TOS_SPL_TABLE	= 0x10000000,
};

enum psp_ring_type
{
	PSP_RING_TYPE__INVALID = 0,
	/*
	 * These values map to the way the PSP kernel identifies the
	 * rings.
	 */
	PSP_RING_TYPE__UM = 1, /* User mode ring (formerly called RBI) */
	PSP_RING_TYPE__KM = 2  /* Kernel mode ring (formerly called GPCOM) */
};

struct psp_ring
{
	enum psp_ring_type		ring_type;
	struct psp_gfx_rb_frame		*ring_mem;
	uint64_t			ring_mem_mc_addr;
	void				*ring_mem_handle;
	uint32_t			ring_size;
};

/* More registers may will be supported */
enum psp_reg_prog_id {
	PSP_REG_IH_RB_CNTL        = 0,  /* register IH_RB_CNTL */
	PSP_REG_IH_RB_CNTL_RING1  = 1,  /* register IH_RB_CNTL_RING1 */
	PSP_REG_IH_RB_CNTL_RING2  = 2,  /* register IH_RB_CNTL_RING2 */
	PSP_REG_LAST
};

struct psp_funcs
{
	int (*init_microcode)(struct psp_context *psp);
	int (*bootloader_load_kdb)(struct psp_context *psp);
	int (*bootloader_load_spl)(struct psp_context *psp);
	int (*bootloader_load_sysdrv)(struct psp_context *psp);
	int (*bootloader_load_sos)(struct psp_context *psp);
	int (*ring_init)(struct psp_context *psp, enum psp_ring_type ring_type);
	int (*ring_create)(struct psp_context *psp,
			   enum psp_ring_type ring_type);
	int (*ring_stop)(struct psp_context *psp,
			    enum psp_ring_type ring_type);
	int (*ring_destroy)(struct psp_context *psp,
			    enum psp_ring_type ring_type);
	bool (*smu_reload_quirk)(struct psp_context *psp);
	int (*mode1_reset)(struct psp_context *psp);
	int (*mem_training)(struct psp_context *psp, uint32_t ops);
	uint32_t (*ring_get_wptr)(struct psp_context *psp);
	void (*ring_set_wptr)(struct psp_context *psp, uint32_t value);
	int (*load_usbc_pd_fw)(struct psp_context *psp, dma_addr_t dma_addr);
	int (*read_usbc_pd_fw)(struct psp_context *psp, uint32_t *fw_ver);
};

#define AMDGPU_XGMI_MAX_CONNECTED_NODES		64
struct psp_xgmi_node_info {
	uint64_t				node_id;
	uint8_t					num_hops;
	uint8_t					is_sharing_enabled;
	enum ta_xgmi_assigned_sdma_engine	sdma_engine;
};

struct psp_xgmi_topology_info {
	uint32_t			num_nodes;
	struct psp_xgmi_node_info	nodes[AMDGPU_XGMI_MAX_CONNECTED_NODES];
};

struct psp_asd_context {
	bool			asd_initialized;
	uint32_t		session_id;
};

struct psp_xgmi_context {
	uint8_t				initialized;
	uint32_t			session_id;
	struct amdgpu_bo                *xgmi_shared_bo;
	uint64_t                        xgmi_shared_mc_addr;
	void                            *xgmi_shared_buf;
	struct psp_xgmi_topology_info	top_info;
};

struct psp_ras_context {
	/*ras fw*/
	bool			ras_initialized;
	uint32_t		session_id;
	struct amdgpu_bo	*ras_shared_bo;
	uint64_t		ras_shared_mc_addr;
	void			*ras_shared_buf;
	struct amdgpu_ras	*ras;
};

struct psp_hdcp_context {
	bool			hdcp_initialized;
	uint32_t		session_id;
	struct amdgpu_bo	*hdcp_shared_bo;
	uint64_t		hdcp_shared_mc_addr;
	void			*hdcp_shared_buf;
	struct mutex		mutex;
};

struct psp_dtm_context {
	bool			dtm_initialized;
	uint32_t		session_id;
	struct amdgpu_bo	*dtm_shared_bo;
	uint64_t		dtm_shared_mc_addr;
	void			*dtm_shared_buf;
	struct mutex		mutex;
};

#define MEM_TRAIN_SYSTEM_SIGNATURE		0x54534942
#define GDDR6_MEM_TRAINING_DATA_SIZE_IN_BYTES	0x1000
#define GDDR6_MEM_TRAINING_OFFSET		0x8000
/*Define the VRAM size that will be encroached by BIST training.*/
#define GDDR6_MEM_TRAINING_ENCROACHED_SIZE	0x2000000

enum psp_memory_training_init_flag {
	PSP_MEM_TRAIN_NOT_SUPPORT	= 0x0,
	PSP_MEM_TRAIN_SUPPORT		= 0x1,
	PSP_MEM_TRAIN_INIT_FAILED	= 0x2,
	PSP_MEM_TRAIN_RESERVE_SUCCESS	= 0x4,
	PSP_MEM_TRAIN_INIT_SUCCESS	= 0x8,
};

enum psp_memory_training_ops {
	PSP_MEM_TRAIN_SEND_LONG_MSG	= 0x1,
	PSP_MEM_TRAIN_SAVE		= 0x2,
	PSP_MEM_TRAIN_RESTORE		= 0x4,
	PSP_MEM_TRAIN_SEND_SHORT_MSG	= 0x8,
	PSP_MEM_TRAIN_COLD_BOOT		= PSP_MEM_TRAIN_SEND_LONG_MSG,
	PSP_MEM_TRAIN_RESUME		= PSP_MEM_TRAIN_SEND_SHORT_MSG,
};

struct psp_memory_training_context {
	/*training data size*/
	u64 train_data_size;
	/*
	 * sys_cache
	 * cpu virtual address
	 * system memory buffer that used to store the training data.
	 */
	void *sys_cache;

	/*vram offset of the p2c training data*/
	u64 p2c_train_data_offset;

	/*vram offset of the c2p training data*/
	u64 c2p_train_data_offset;
	struct amdgpu_bo *c2p_bo;

	enum psp_memory_training_init_flag init;
	u32 training_cnt;
};

struct psp_context
{
	struct amdgpu_device            *adev;
	struct psp_ring                 km_ring;
	struct psp_gfx_cmd_resp		*cmd;

	const struct psp_funcs		*funcs;

	/* firmware buffer */
	struct amdgpu_bo		*fw_pri_bo;
	uint64_t			fw_pri_mc_addr;
	void				*fw_pri_buf;

	/* sos firmware */
	const struct firmware		*sos_fw;
	uint32_t			sos_fw_version;
	uint32_t			sos_feature_version;
	uint32_t			sys_bin_size;
	uint32_t			sos_bin_size;
	uint32_t			toc_bin_size;
	uint32_t			kdb_bin_size;
	uint32_t			spl_bin_size;
	uint8_t				*sys_start_addr;
	uint8_t				*sos_start_addr;
	uint8_t				*toc_start_addr;
	uint8_t				*kdb_start_addr;
	uint8_t				*spl_start_addr;

	/* tmr buffer */
	struct amdgpu_bo		*tmr_bo;
	uint64_t			tmr_mc_addr;

	/* asd firmware */
	const struct firmware		*asd_fw;
	uint32_t			asd_fw_version;
	uint32_t			asd_feature_version;
	uint32_t			asd_ucode_size;
	uint8_t				*asd_start_addr;

	/* fence buffer */
	struct amdgpu_bo		*fence_buf_bo;
	uint64_t			fence_buf_mc_addr;
	void				*fence_buf;

	/* cmd buffer */
	struct amdgpu_bo		*cmd_buf_bo;
	uint64_t			cmd_buf_mc_addr;
	struct psp_gfx_cmd_resp		*cmd_buf_mem;

	/* fence value associated with cmd buffer */
	atomic_t			fence_value;
	/* flag to mark whether gfx fw autoload is supported or not */
	bool				autoload_supported;
	/* flag to mark whether df cstate management centralized to PMFW */
	bool				pmfw_centralized_cstate_management;

	/* xgmi ta firmware and buffer */
	const struct firmware		*ta_fw;
	uint32_t			ta_fw_version;
	uint32_t			ta_xgmi_ucode_version;
	uint32_t			ta_xgmi_ucode_size;
	uint8_t				*ta_xgmi_start_addr;
	uint32_t			ta_ras_ucode_version;
	uint32_t			ta_ras_ucode_size;
	uint8_t				*ta_ras_start_addr;

	uint32_t			ta_hdcp_ucode_version;
	uint32_t			ta_hdcp_ucode_size;
	uint8_t				*ta_hdcp_start_addr;

	uint32_t			ta_dtm_ucode_version;
	uint32_t			ta_dtm_ucode_size;
	uint8_t				*ta_dtm_start_addr;

	struct psp_asd_context		asd_context;
	struct psp_xgmi_context		xgmi_context;
	struct psp_ras_context		ras;
	struct psp_hdcp_context 	hdcp_context;
	struct psp_dtm_context		dtm_context;
	struct mutex			mutex;
	struct psp_memory_training_context mem_train_ctx;
};

struct amdgpu_psp_funcs {
	bool (*check_fw_loading_status)(struct amdgpu_device *adev,
					enum AMDGPU_UCODE_ID);
};


#define psp_ring_init(psp, type) (psp)->funcs->ring_init((psp), (type))
#define psp_ring_create(psp, type) (psp)->funcs->ring_create((psp), (type))
#define psp_ring_stop(psp, type) (psp)->funcs->ring_stop((psp), (type))
#define psp_ring_destroy(psp, type) ((psp)->funcs->ring_destroy((psp), (type)))
#define psp_init_microcode(psp) \
		((psp)->funcs->init_microcode ? (psp)->funcs->init_microcode((psp)) : 0)
#define psp_bootloader_load_kdb(psp) \
		((psp)->funcs->bootloader_load_kdb ? (psp)->funcs->bootloader_load_kdb((psp)) : 0)
#define psp_bootloader_load_spl(psp) \
		((psp)->funcs->bootloader_load_spl ? (psp)->funcs->bootloader_load_spl((psp)) : 0)
#define psp_bootloader_load_sysdrv(psp) \
		((psp)->funcs->bootloader_load_sysdrv ? (psp)->funcs->bootloader_load_sysdrv((psp)) : 0)
#define psp_bootloader_load_sos(psp) \
		((psp)->funcs->bootloader_load_sos ? (psp)->funcs->bootloader_load_sos((psp)) : 0)
#define psp_smu_reload_quirk(psp) \
		((psp)->funcs->smu_reload_quirk ? (psp)->funcs->smu_reload_quirk((psp)) : false)
#define psp_mode1_reset(psp) \
		((psp)->funcs->mode1_reset ? (psp)->funcs->mode1_reset((psp)) : false)
#define psp_mem_training(psp, ops) \
	((psp)->funcs->mem_training ? (psp)->funcs->mem_training((psp), (ops)) : 0)

#define psp_ring_get_wptr(psp) (psp)->funcs->ring_get_wptr((psp))
#define psp_ring_set_wptr(psp, value) (psp)->funcs->ring_set_wptr((psp), (value))

#define psp_load_usbc_pd_fw(psp, dma_addr) \
	((psp)->funcs->load_usbc_pd_fw ? \
	(psp)->funcs->load_usbc_pd_fw((psp), (dma_addr)) : -EINVAL)

#define psp_read_usbc_pd_fw(psp, fw_ver) \
	((psp)->funcs->read_usbc_pd_fw ? \
	(psp)->funcs->read_usbc_pd_fw((psp), fw_ver) : -EINVAL)

extern const struct amd_ip_funcs psp_ip_funcs;

extern const struct amdgpu_ip_block_version psp_v3_1_ip_block;
extern int psp_wait_for(struct psp_context *psp, uint32_t reg_index,
			uint32_t field_val, uint32_t mask, bool check_changed);

extern const struct amdgpu_ip_block_version psp_v10_0_ip_block;
extern const struct amdgpu_ip_block_version psp_v12_0_ip_block;

int psp_gpu_reset(struct amdgpu_device *adev);
int psp_update_vcn_sram(struct amdgpu_device *adev, int inst_idx,
			uint64_t cmd_gpu_addr, int cmd_size);

int psp_xgmi_initialize(struct psp_context *psp);
int psp_xgmi_terminate(struct psp_context *psp);
int psp_xgmi_invoke(struct psp_context *psp, uint32_t ta_cmd_id);
int psp_xgmi_get_hive_id(struct psp_context *psp, uint64_t *hive_id);
int psp_xgmi_get_node_id(struct psp_context *psp, uint64_t *node_id);
int psp_xgmi_get_topology_info(struct psp_context *psp,
			       int number_devices,
			       struct psp_xgmi_topology_info *topology);
int psp_xgmi_set_topology_info(struct psp_context *psp,
			       int number_devices,
			       struct psp_xgmi_topology_info *topology);

int psp_ras_invoke(struct psp_context *psp, uint32_t ta_cmd_id);
int psp_ras_enable_features(struct psp_context *psp,
		union ta_ras_cmd_input *info, bool enable);
int psp_ras_trigger_error(struct psp_context *psp,
			  struct ta_ras_trigger_error_input *info);

int psp_hdcp_invoke(struct psp_context *psp, uint32_t ta_cmd_id);
int psp_dtm_invoke(struct psp_context *psp, uint32_t ta_cmd_id);

int psp_rlc_autoload_start(struct psp_context *psp);

extern const struct amdgpu_ip_block_version psp_v11_0_ip_block;
int psp_reg_program(struct psp_context *psp, enum psp_reg_prog_id reg,
		uint32_t value);
int psp_ring_cmd_submit(struct psp_context *psp,
			uint64_t cmd_buf_mc_addr,
			uint64_t fence_mc_addr,
			int index);
int psp_init_asd_microcode(struct psp_context *psp,
			   const char *chip_name);
int psp_init_sos_microcode(struct psp_context *psp,
			   const char *chip_name);
#endif
